/**
 * @file main.cpp
 * @brief File containing Server main function and thread that handles
 * interaction with clients.
 * 
 * A Distributed Memory Server will wait for incoming connections by clients.
 * Once a client has connected it can perform operations with a simple protocol:
 * - Map request: message <MAP, ID>
 * - Unmap request: message <UNMAP, ID>
 * - Update request: message <UPDATE, ID>
 * - Write request: message <WRITE, ID, data>
 * - Wait request: message <WAIT, ID>
 *
 * Server can then reply:
 * - Map reply: message <OK, data>
 * - Unmap reply: message <OK>
 * - Update replies: message <OK, data> or message <UPDATED>
 * - Write reply: message <OK>
 * - Generic error reply: message <ERROR>
 * - Write error reply: message <ERROR, INVALID> or message <ERROR, UNMAPPED>
 *
 * All messages are defined in msg.h file.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "dm.h"
#include "msg.h"
#include "utility.h"

#include <stdio.h> // only for printf

#define BACKLOG 10

/**
 * Distributed memory object. Declared global because it must be accessed by all
 * threads.
 */
DM mem;

/**
 * Thread that handles all communications with one client, and all client's
 * operations on distributed memory object mem.
 * @param[in]	in Client socket descriptor.
 */
void *thread (void *in)
{
	intptr_t sd = (intptr_t) in;
	int ret, type, id, resp;
	char req[2 * sizeof(int)];

	while (1) {
		ret = recv_msg (sd, &req[0], 2 * sizeof(int));
		if (ret == -1)
			break;

		memcpy (&type, req, sizeof(int));
		memcpy (&id, req + sizeof(int), sizeof(int));
		type = ntohl (type);
		id = ntohl (id);

		if (type == MAP) {
			// map request
			char buf[DIMBLOCK]; 
			ret = mem.map_client (sd, id, buf);
			char res[sizeof(int)];
			if (ret == -1) {
				build_resphdr (res, ERROR);
				ret = send_msg (sd, res, sizeof(int));
				if (ret == -1)
					break;

				continue;
			}
			build_resphdr (res, OK);
			ret = send_msg (sd, res, sizeof(int));
			if (ret == -1)
				break;

			ret = send_msg (sd, buf, DIMBLOCK);
			if (ret == -1)
				break;

			continue;
		} else if (type == UNMAP) {
			// unmap request
			char buf[sizeof(int)];
			ret = mem.unmap_client (sd, id);
			if (ret == 0)
				build_resphdr (buf, OK);
			else
				build_resphdr (buf, ERROR);
			ret = send_msg (sd, buf, sizeof(int));
			if (ret == -1)
				break;

			continue;
		} else if (type == UPDATE) {
			// update request
			char buf[DIMBLOCK];
			char res[sizeof(int)];
			ret = mem.update_block (sd, id, buf);
			if (ret == 0) {
				build_resphdr (res, OK);
				ret = send_msg (sd, res, sizeof(int));
				if (ret == -1)
					break;

				ret = send_msg (sd, buf, DIMBLOCK);
				if (ret == -1)
					break;
			} else if (ret == 1) {
				build_resphdr (res, UPDATED);
				ret = send_msg (sd, res, sizeof(int));
				if (ret == -1)
					break;
			} else {
				build_resphdr (res, ERROR);
				ret = send_msg (sd, res, sizeof(int));
				if (ret == -1)
					break;
			}
			continue;
		} else if (type == WRITE) {
			// write request
			char buf[DIMBLOCK];
			ret = recv_msg (sd, buf, DIMBLOCK);
			if (ret == -1)
				break;

			char res[sizeof(int)];
			ret = mem.write_block (sd, id, buf);
			if (ret == 0) {
				build_resphdr (res, OK);
				ret = send_msg (sd, res, sizeof(int));
				if (ret == -1)
					break;
			} else if (ret == -1) {
				build_resphdr (res, ERROR);
				ret = send_msg (sd, res, sizeof(int));
				if (ret == -1)
					break;

				build_resphdr (res, UNMAPPED);
				ret = send_msg (sd, res, sizeof(int));
				if (ret == -1)
					break;
			} else if (ret == -2) {
				build_resphdr (res, ERROR);
				ret = send_msg (sd, res, sizeof(int));
				if (ret == -1)
					break;

				build_resphdr (res, INVALID);
				ret = send_msg (sd, res, sizeof(int));
				if (ret == -1)
					break;
			}
			continue;
		} else if (type == WAIT) {
			// wait request
			char buf[sizeof(int)];
			ret = mem.wait_block (sd, id);
			if (ret == 0)
				build_resphdr (buf, OK);
			else
				build_resphdr (buf, ERROR);
			ret = send_msg (sd, buf, sizeof(int));
			if (ret == -1)
				break;

			continue;
		} else {
			// error: unrecognizable msg
			break;
		}
	}

	// cleaning operation always done. a client can crash or disconnect
	// without errors but we are note sure that all client's blocks are
	// unmapped
	mem.clean (sd);
	close (sd);
}

/**
 * Server main function, input parameters are:
 * @param[in]	argv[0] Server port.
 * @param[in]	argv[1]	First block id.
 * @param[in]	argv[2] Last block id.
 */
int main (int argc, char *argv[])
{
	if (argc != 4) {
		printf ("Server: Bad arguments\n");
		exit (1);
	}

	// initialization
	int port = atoi (argv[1]);
	int first = atoi (argv[2]);
	int last = atoi (argv[3]);
	pthread_t tid;

	mem.init (first, last);

	// create listening socket
	sockaddr_in s_addr;
	memset ((void *) &s_addr, 0, sizeof(sockaddr_in));
	int sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		printf ("Server: Unable to create listening socket\n");
		exit (1);
	}

	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = INADDR_ANY;
	s_addr.sin_port = htons (port);

	int opt = 1;
	int ret = setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	if (ret == -1) {
		printf ("Server: Unable to set socket options\n");
		exit (1);
	}
	ret = bind (sd, (sockaddr *) &s_addr, sizeof(sockaddr_in));
	if (ret == -1) {
		printf ("Server: Unable to bind socket address\n");
		exit (1);
	}
	ret = listen (sd, BACKLOG);
	if (ret == -1) {
		printf ("Server: Unable to listen on socket\n");
		exit (1);
	}

	while (1) {
		sockaddr_in c_addr;
		int len = sizeof(sockaddr_in);
		int csd = accept (sd, (sockaddr *) &c_addr, (socklen_t *) &len);
		if (csd == -1) {
			printf ("Server: Unable to accept connection\n");
			exit (1);
		}
		// interaction with clients are handled by separate threads.
		pthread_create (&tid, 0, thread, (void *) csd);
	}
}
