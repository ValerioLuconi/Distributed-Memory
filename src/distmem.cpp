/**
 * @file distmem.cpp
 * @brief File containing DM_client class definitions.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#include "distmem.h"

DM_client::DM_client ()
{
	dim = 0;
}

DM_client::~DM_client () 
{
	for (int i = 0; DM.find (i) != DM.end (); i++) {
		close (DM[i]->sd);
		delete DM[i];
	}
}

int DM_client::dm_init (char *config_file)
{
	FILE *fp = fopen (config_file, "r");
	if (fp == NULL)
		return -1;

	char s[81];
	char tmp[81];
	int port;
	char *address;
	char *ret;

	// configuration file parsing
	while (feof (fp) == 0) {
		ret = fgets (s, 80, fp);
		if (ret == NULL)
			continue;
		if (s[0] == '#')
			continue;
		if (strstr (s, "DIMBLOCK=") != 	NULL) {
			strcpy (tmp, &s[9]);
			dim = atoi (tmp);
		} else if (strstr (s, "Address=") != NULL) {
			strcpy (tmp, &s[8]);
			address = &tmp[0];
		} else if (strstr (s, "Port=") != NULL) {
			strcpy (tmp, &s[5]);
			port = atoi (tmp);
		} else if (strstr (s, "ID=") != NULL) {
			strcpy (tmp, &s[3]);
			char *p = strtok (tmp, "-");
			char *q = strtok (NULL, "-");
			int first = atoi (p);
			int last = atoi (q);
			server *srv = new server;
			memset ((void *) &srv->address, 0, sizeof(sockaddr_in));
			srv->sd = socket (AF_INET, SOCK_STREAM, 0);
			if (srv->sd == -1)
				return -1;

			srv->address.sin_family = AF_INET;
			srv->address.sin_port = htons (port);
			inet_pton (AF_INET, address, &srv->address.sin_addr);

			int ret = connect (srv->sd, (sockaddr *) &srv->address,
					   sizeof(sockaddr_in));
			if (ret == -1)
				return -1;
			timeval t;
			t.tv_sec = 60;
			t.tv_usec = 0;
			ret = setsockopt (srv->sd, SOL_SOCKET, SO_RCVTIMEO, &t,
					  sizeof(timeval));
			if (ret == -1)
				return -1;

			for (int i = first; i <= last; i++) {
				DM[i] = srv;
			}
		}
	}

	fclose (fp);
	return 0;
}

int DM_client::dm_block_map (int ID, void *address)
{
	// DM_client not initialized
	if (DM.empty ())
		return -3;

	// ID not in server list
	if (DM.find (ID) == DM.end ())
		return -1;

	// ID already mapped
	if (LM.find (ID) != LM.end ())
		return -2;

	//address = new char[dim];
	LM[ID] = (char *) address;

	int sd = DM[ID]->sd;

	// construct buffer to send
	int size = 2 * sizeof(int);
	char buf[size];
	build_reqhdr (buf, MAP, ID);

	// send request to server
	int ret = send_msg (sd, buf, size);
	if (ret == -1)
		return -1;

	// receive response from server
	int resp;
	size = sizeof(int);
	ret = recv_msg (sd, &resp, size);
	if (ret == -1)
		return -1;
	resp = ntohl (resp);
	if (resp != OK)
		return -1;
	ret = recv_msg (sd, address, dim);
	if (ret == -1)
		return -1;

	return 0;
}

int DM_client::dm_block_unmap (int ID)
{
	// DM_client not initialized
	if (DM.empty ())
		return -3;
	if (LM.find (ID) == LM.end ())
		return -1;
	if (DM.find (ID) == DM.end ())
		return -1;

	int sd = DM[ID]->sd;

	// construct buffer to send
	int size = 2 * sizeof(int);
	char buf[size];
	build_reqhdr (buf, UNMAP, ID);

	// send request to server
	int ret = send_msg (sd, buf, size);
	if (ret == -1)
		return -1;

	// receive response from server
	int resp;
	size = sizeof(int);
	ret = recv_msg (sd, &resp, size);
	if (ret == -1)
		return -1;
	resp = ntohl (resp);
	if (resp != OK)
		return -1;

	//delete[] (char *) LM[ID];
	LM.erase (ID);

	return 0;
}

int DM_client::dm_block_update (int ID)
{
	// DM_client not initialized
	if (DM.empty ())
		return -3;
	if (LM.find (ID) == LM.end ())
		return -1;
	if (DM.find (ID) == DM.end ())
		return -1;

	int sd = DM[ID]->sd;

	// construct buffer to send
	int size = 2 * sizeof(int);
	char buf[size];
	build_reqhdr (buf, UPDATE, ID);

	// send request to server
	int ret = send_msg (sd, buf, size);
	if (ret == -1)
		return -1;

	// receive response from server
	int resp;
	size = sizeof(int);
	ret = recv_msg (sd, &resp, size);
	if (ret == -1)
		return -1;
	resp = ntohl (resp);
	if (resp != OK && resp != UPDATED)
		return -1;
	if (resp == UPDATED)
		return 0;

	ret = recv_msg (sd, LM[ID], dim);

	return 0;
}

int DM_client::dm_block_write (int ID)
{
	// DM_client not initialized
	if (DM.empty ())
		return -3;
	if (LM.find (ID) == LM.end ())
		return -1;
	if (DM.find (ID) == DM.end ())
		return -1;

	int sd = DM[ID]->sd;

	// construct buffer to send
	int size = 2 * sizeof(int);
	char buf[size];
	build_reqhdr (buf, WRITE, ID);

	// send first request to server
	int ret = send_msg (sd, buf, size);
	if (ret == -1)
		return -1;
	// send block
	ret = send_msg (sd, LM[ID], dim);
	if (ret == -1)
		return -1;

	// receive response from server
	int resp;
	size = sizeof(int);
	ret = recv_msg (sd, &resp, size);
	if (ret == -1)
		return -1;
	resp = ntohl (resp);
	if (resp != OK) {
		int why;
		ret = recv_msg (sd, &why, size);
		if (ret == -1)
			return -1;
		why = ntohl (why);
		if (why == UNMAPPED)
			return -1;
		if (why == INVALID)
			return -2;
	}

	return 0;
}

int DM_client::dm_block_wait (int ID)
{
	// DM_client not initialized
	if (DM.empty ())
		return -3;
	if (LM.find (ID) == LM.end ())
		return -1;
	if (DM.find (ID) == DM.end ())
		return -1;

	int sd = DM[ID]->sd;

	// set timeout to 0, otherwise client would wait only 60 seconds
	timeval t;
	t.tv_sec = 0;
	t.tv_usec = 0;
	int ret = setsockopt (sd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));
	if (ret == -1)
		return -1;

	// construct buffer to send
	int size = 2 * sizeof(int);
	char buf[size];
	build_reqhdr (buf, WAIT, ID);

	// send request to server
	ret = send_msg (sd, buf, size);
	if (ret == -1)
		return -1;

	// receive response from server
	int resp;
	size = sizeof(int);
	ret = recv_msg (sd, &resp, size);
	if (ret == -1)
		return -1;

	// re-set timeout to 60 seconds
	t.tv_sec = 60;
	t.tv_usec = 0;
	ret = setsockopt (sd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));
	if (ret == -1)
		return -1;

	resp = ntohl (resp);
	if (resp != OK)
		return -1;
	return 0;
}

int DM_client::dm_block_dim ()
{
	return dim;
}
