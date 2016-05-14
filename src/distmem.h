/**
 * @file distmem.h
 * @brief Header file containing DM_client class declaration, library file for
 * 	  client.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#ifndef DISTMEM_H
#define DISTMEM_H

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include "msg.h"
#include "utility.h"
using namespace std;

/**
 * @struct server distmem.h "distmem.h"
 * @brief Identifies a distributed memory Server in DM_client class.
 */
struct server {
	/**
	 * Socket descriptor used for communication with server.
	 */
	int sd;

	/**
	 * Structure containing server's address and port.
	 */
	sockaddr_in address;
};

/**
 * @class DM_client distmem.h "distmem.h"
 * @brief Provides a library for client's distributed memory operations and
 * 	  synchronization with other clients.
 * 
 * Usage: A client needs to allocate only one DM_client object. Two DM_client
 * objects will be considered by server as two separate clients.
 * First of all declare a DM_client object and initialize it:
 *
 * DM_client dm;
 *
 * dm.dm_init(config_file);
 *
 * It is now possible to perform operations on distributed memory. Remember that
 * if DM_client object has not been initialized all operations will return an
 * error (-3 value), all functions except dm_block_wait() have timeout set to 60
 * seconds. If server does not reply until timeout the function will return
 * error (-1).
 */
class DM_client {
	/**
	 * Distributed memory block dimension. Read from configuration file.
	 */
	int dim;

	/**
	 * Distributed memory map. For each block indicates where to locate it
	 * (server address) and how to reach it (server socket). In order to
	 * minimize memory allocation, server structure is allocated only once.
	 * Blocks owned by the same server point to the same server structure.
	 */
	map<int, server *> DM;

	/**
	 * Local memory map. For each block mapped by the client indicates the
	 * local address. Contains only mapped blocks.
	 */
	map<int, char *> LM;
public:
	/**
	 * DM_client Constructor. Initializes dim to 0.
	 */
	DM_client ();

	/**
	 * DM_client Destructor. Closes server sockets and deallocates server
	 * structures.
	 */
	~DM_client ();

	/**
	 * Initializes distributed memory map DM with the content of a
	 * configuration file. Opens connections with distributed memory
	 * servers. This operation must be done before all other operations
	 * otherwise they will return error.
	 * @param[in]	config_file Path to configuration file.
	 * @return	0 on success, -1 on error.
	 */
	int dm_init (char *config_file);

	/**
	 * Maps block identified by ID in local memory at a given address.
	 * @param[in]	ID Block id.
	 * @param[out]	address Local memory address in which block data will be
	 *		stored. This should be an address of a memory block
	 *		already allocated, with correct size. Otherwise
	 * 		behaviour is undefined.
	 * @return	0 on success, -1 on error, -2 if block is already
	 *		mapped, -3 if DM_client has not been initialized.
	 */
	int dm_block_map (int ID, void *address);

	/**
	 * Unmaps block identified by ID.
	 * @param[in]	ID Block id.
	 * @return	0 on success, -1 on error, -3 if DM_client has not been
	 * 		initialized.
	 */
	int dm_block_unmap (int ID);

	/**
	 * Updates content of block identified by ID.
	 * @param[in]	ID Block id.
	 * @return	0 on success, -1 on error, -3 if DM_client has not been
	 * 		initialized.
	 */
	int dm_block_update (int ID);

	/**
	 * Writes data in local block identified by ID to distributed memory.
	 * @param[in]	ID Block id.
	 * @return	0 on success, -2 if block is invalid, -1 on error, -3 if
	 * 		DM_client has not been initialized.
	 */
	int dm_block_write (int ID);

	/**
	 * Waits for block identified by ID to become invalid.
	 * @param[in]	ID Block id.
	 * @return	0 on success, -1 on error, -3 if DM_client has not been
	 * 		initialized.
	 */
	int dm_block_wait (int ID);

	/**
	 * Returns block dimension.
	 * @return	Block dimension or 0 if DM_client has not been
	 * 		initialized.
	 */
	int dm_block_dim ();
};

#endif // DISTMEM_H
