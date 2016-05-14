/**
 * @file utility.cpp
 * @brief Header file containing utility functions definitions.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#include "utility.h"

void build_reqhdr (void *buffer, int type, int id)
{
	char *buf = (char *) buffer;
	type = htonl (type);
	int p = sizeof(int);
	memcpy (buf, &type, p);
	id = htonl (id);
	memcpy (buf + p , &id, p);
}

void build_resphdr (void *buffer, int type)
{
	char *buf = (char *) buffer;
	type = htonl (type);
	int p = sizeof(int);
	memcpy (buf, &type, p);
}

int send_msg (int sd, void *buffer, int size)
{
	char *buf = (char *) buffer;
	int ret;
	for (int wr = 0; wr != size; wr += ret) {
		ret = write (sd, &buf[wr], size - wr);
		if (ret == -1)
			return -1;
	}
	return 0;
}

int recv_msg (int sd, void *buffer, int size)
{
	char *buf = (char *) buffer;
	int ret;
	for (int rd = 0; rd != size; rd += ret) {
		ret = read (sd, &buf[rd], size - rd);
		if (ret == -1 || ret == 0)
			return -1;
	}
	return 0;
}
