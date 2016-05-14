/**
 * @file block.h
 * @brief Header file containing Block class declaration.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#ifndef BLOCK_H
#define BLOCK_H

#include <map>
#include <pthread.h>
#include <string.h>
using namespace std;

/**
 * @def DIMBLOCK
 * Block dimension in bytes.
 */
#define DIMBLOCK 128

/**
 * @class Block block.h "block.h"
 * @brief Manages operations on a distributed memory block.
 *
 * All operations on a Block must be performed in mutual exclusion, because they
 * can be performed by several threads at the same time. So a mutex semaphore is
 * provided. A condition variable is also provided, to ensure that a client
 * blocks while performing a wait operation until block is modified.
 */
class Block {

	/**
	 * Data stored in memory block.
	 */
	char *data;

	/**
	 * Maps a client identification with the version stored in client's
	 * local memory (which may be different from current block version).
	 * Clients are identified through their socket number, which doesn't
	 * change while connection is up (connection with client is persistent).
	 */
	map<int, int> cmap;

	/**
	 * Current block version. Identifies valid or invalid client blocks, if
	 * client's version (stored in cmap) differs from curr_version, client's
	 * block local copy is invalid.
	 */
	int curr_version;

	/**
	 * Mutual exclusion semaphore.
	 */
	pthread_mutex_t mutex;

	/**
	 * Condition variable. Clients block on this variable while waiting
	 * their local memory block to become invalid.
	 */
	pthread_cond_t waitcond;

	/**
	 * Number of clients (or threads) blocked on condition variable
	 */
	int blockwait;
public:
	/**
	 * Block constructor. Initializes Block data structures. Data in block
	 * are set to zero, and so is current version.
	 * @return	No value is returned.
	 */
	Block ();

	/**
	 * Block destructor. Frees dynamic memory for data.
	 * @return	No value is returned.
	 */
	~Block ();

	/**
	 * Maps client to block.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @param[out]	buf In it is stored block data.
	 * @return	0 on success, -1 on error (if block is already mapped to
	 *		that client).
	 */
	int bmap (int sd, char *buf);

	/**
	 * Unmaps client from block.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @return	0 on success, -1 on error (if block is not mapped to
	 *		that client).
	 */
	int unmap (int sd);

	/**
	 * Writes data in block.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @param[in]	buf Contains data to be stored.
	 * @return	0 on success. On error -1 is returned if block isn't
	 *		mapped to that client. -2 is returned if version
	 *		associated to that client is different from current
	 *		version (invalid block).
	 */
	int write (int sd, char *buf);

	/**
	 * Updates client's local block.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @param[out]	buf Filled with updated data.
	 * @return	1 if block is already up to date (current version and
	 * 		client's version are the same). 0 on success, and data
	 *		is stored in buf. On error -1 is returned if block isn't
	 *		mapped to that client.
	 */
	int update (int sd, char *buf);

	/**
	 * Waits for data to become invalid.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @return	0 on success, when block becomes invalid. On error -1 is
	 *		returned if block isn't mapped to that client.
	 */
	int wait (int sd);

	/**
	 * Removes client's entry, identified by socket sd, from cmap. Used if
	 * client disconnects or crashes without unmapping blocks.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @return	No value is returned.
	 */
	void clean (int sd);
};

#endif // BLOCK_H
