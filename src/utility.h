/**
 * @file utility.h
 * @brief Header file containing utility functions.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

/**
 * Build a request header. Header format is <Type, Id>
 * Type can be:
 * - MAP
 * - UNMAP
 * - UPDATE
 * - WRITE
 * - WAIT
 *
 * @param[out]	buffer Buffer ready for sending.
 * @param[in]	type Request type.
 * @param[in]	id Requested block's id.
 * @return	No value is returned.
 */
void build_reqhdr (void *buffer, int type, int id);

/**
 * Build a response header. Header format is <Type>
 * Type can be:
 * - OK
 * - UPDATED
 * - ERROR
 *
 * @param[out]	buffer Buffer ready for sending.
 * @param[in]	type Response type.
 * @return	No value is returned.
 */
void build_resphdr (void *buffer, int type);

/**
 * Sends message through socket.
 * @param[in]	sd A valid socket descriptor.
 * @param[in]	buffer Buffer to send.
 * @param[in]	size Number of bytes of buffer to send.
 * @return	0 on success, -1 on error.
 */
int send_msg (int sd, void *buffer, int size);

/**
 * Receives message through socket.
 * @param[in]	sd A valid socket descriptor.
 * @param[in]	buffer Buffer in which data is stored.
 * @param[in]	size Number of bytes to receive.
 * @return	0 on success, -1 on error or on connection teardown by peer.
 */
int recv_msg (int sd, void *buffer, int size);

#endif // UTILITY_H
