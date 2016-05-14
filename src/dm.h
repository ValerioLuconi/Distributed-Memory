/**
 * @file dm.h
 * @brief Header file containing DM class declaration.
 *
 * A DM object handles operations on Distributed Memory server side.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#ifndef DM_H
#define DM_H

#include <map>
#include "block.h"
using namespace std;

/**
 * @class DM dm.h "dm.h"
 * @brief Manages operations on distributed memory.
 *
 * A DM (Distributed Memory) object handles all possible operations on
 * distributed memory and stores all data in blocks.
 * Blocks are objects of class Block and they are identified by a unique integer
 * between all distributed memory servers.
 */
class DM {

	/**
	 * First id in distributed memory.
	 */
	int first;

	/**
	 * Last id in distributed memory.
	 */
	int last;

	/**
	 * Maps blocks with their id. Blocks are identified through a unique
	 * integer between all distributed memory servers.
	 */
	map<int, Block *> dm_map;
public:
	/**
	 * Distributed memory constructor. No operations.
	 * @return	No value is returned.
	 */
	DM ();

	/**
	 * Distributed memory destructor. Frees distributed memory.
	 * @return	No value is returned.
	 */
	~DM ();

	/**
	 * Initializes all blocks.
	 * @param[in]	f First id in memory.
	 * @param[in]	l Last id in memory.
	 * @return	No value is returned.
	 */
	void init (int f, int l);

	/**
	 * Maps ID block to client identified by socket descriptor sd. If
	 * no error occurs buf is filled with block data.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @param[in]	ID Block ID.
	 * @param[out]	buf In it is stored block data.
	 * @return	0 on success, -1 on error (if block is already mapped to
	 *		that client or if block id doesn't exist).
	 */
	int map_client (int sd, int ID, char *buf);

	/**
	 * Unmaps ID block from client identified by socket descriptor sd.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @param[in]	ID Block ID.
	 * @return	0 on success, -1 on error (if block is already mapped to
	 *		that client or if block id doesn't exist).
	 */
	int unmap_client (int sd, int ID);

	/**
	 * Writes data in block.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @param[in]	ID Block ID.
	 * @param[in]	buf Contains data to be stored.
	 * @return	0 on success. On error -1 is returned if block isn't
	 *		mapped to that client or if block id doesn't exist. -2
	 *		is returned if version associated to that client is
	 *		different from current version (invalid block).
	 */
	int write_block (int sd, int ID, char *buf);

	/**
	 * Updates client's local block.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @param[in]	ID Block ID.
	 * @param[out]	buf Filled with updated data.
	 * @return	1 if block is already up to date (current version and
	 * 		client's version are the same). 0 on success, and data
	 *		is stored in buf. On error -1 is returned if block isn't
	 *		mapped to that client or if block id doesn't exist.
	 */
	int update_block (int sd, int ID, char *buf);

	/**
	 * Waits for block data to become invalid.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @param[in]	ID Block ID.
	 * @return	0 on success, when block becomes invalid. On error -1 is
	 *		returned if block isn't mapped to that client or if
	 *		block id doesn't exist.
	 */
	int wait_block (int sd, int ID);

	/**
	 * Unmaps all memory blocks from client identified by sd. Used if
	 * client disconnects or crashes without unmapping blocks.
	 * @param[in]	sd Client's socket descriptor used for identification.
	 * @return	No value is returned.
	 */
	void clean (int sd);
};

#endif // DM_H
