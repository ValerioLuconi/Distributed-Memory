/**
 * @file block.cpp
 * @brief File containing Block class definitions.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#include "block.h"

Block::Block ()
{
	data = new char[DIMBLOCK];
	memset (data, 0, DIMBLOCK);
	curr_version = 0;
	pthread_mutex_init (&mutex, 0);
	pthread_cond_init (&waitcond, 0);
	blockwait = 0;
}

Block::~Block ()
{
	delete[] data;
}

int Block::bmap (int sd, char *buf)
{
	pthread_mutex_lock (&mutex);

	if (cmap.find (sd) != cmap.end ()) {
		// if block is not mapped to client sd: error
		pthread_mutex_unlock (&mutex);
		return -1;
	}

	cmap[sd] = curr_version;
	memcpy (buf, data, DIMBLOCK);

	pthread_mutex_unlock (&mutex);

	return 0;
}

int Block::unmap (int sd)
{
	pthread_mutex_lock (&mutex);

	if (cmap.find (sd) == cmap.end ()) {
		// if block is not mapped to client sd: error
		pthread_mutex_unlock (&mutex);
		return -1;
	}

	cmap.erase (sd);

	pthread_mutex_unlock (&mutex);

	return 0;
}

int Block::write (int sd, char *buf)
{
	pthread_mutex_lock (&mutex);

	if (cmap.find (sd) == cmap.end ()) {
		// if block is not mapped to client sd: error
		pthread_mutex_unlock (&mutex);
		return -1;
	}

	if (cmap[sd] != curr_version) {
		// if block is invalid for client sd
		pthread_mutex_unlock (&mutex);
		return -2;
	}

	curr_version++;
	cmap[sd]++;
	memcpy (data, buf, DIMBLOCK);

	if (blockwait != 0)
		// all clients waiting for their copies to become invalid must
		// be waken up. all their local copies are invalidated
		pthread_cond_broadcast (&waitcond);

	pthread_mutex_unlock (&mutex);

	return 0;
}

int Block::update (int sd, char *buf)
{
	pthread_mutex_lock (&mutex);

	if (cmap.find (sd) == cmap.end ()) {
		// if block is not mapped to client sd: error
		pthread_mutex_unlock (&mutex);
		return -1;
	}

	if (cmap[sd] == curr_version) {
		// block is already up to date for client sd
		pthread_mutex_unlock (&mutex);
		return 1;
	}

	cmap[sd] = curr_version;
	memcpy (buf, data, DIMBLOCK);

	pthread_mutex_unlock (&mutex);

	return 0;
}

int Block::wait (int sd)
{
	pthread_mutex_lock (&mutex);

	if (cmap.find (sd) == cmap.end ()) {
		// if block is not mapped to client sd: error
		pthread_mutex_unlock (&mutex);
		return -1;
	}

	// if is enough, because once curr_version has been incremented it
	// cannot be decremented.
	if (cmap[sd] == curr_version) {
		blockwait++;
		pthread_cond_wait (&waitcond, &mutex);
		blockwait--;
	}

	pthread_mutex_unlock (&mutex);

	return 0;
}

void Block::clean (int sd)
{
	pthread_mutex_lock (&mutex);

	if (cmap.find (sd) != cmap.end ())
		cmap.erase (sd);

	pthread_mutex_unlock (&mutex);
}
