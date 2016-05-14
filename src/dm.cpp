/**
 * @file dm.cpp
 * @brief File containing DM class definitions.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#include "dm.h"

DM::DM () {}

DM::~DM ()
{
	for (int i = first; i <= last; i++)
		delete dm_map[i];
}

void DM::init (int f, int l)
{
	first = f;
	last = l;
	for (int i = first; i <= last; i++)
		dm_map[i] = new Block();
}

int DM::map_client (int sd, int ID, char *buf)
{
	if (dm_map.find (ID) == dm_map.end ())
		return -1;

	int ret = dm_map[ID]->bmap (sd, buf);
	return ret;
}

int DM::unmap_client (int sd, int ID)
{
	if (dm_map.find (ID) == dm_map.end ())
		return -1;

	int ret = dm_map[ID]->unmap (sd);
	return ret;
}

int DM::write_block (int sd, int ID, char *buf)
{
	if (dm_map.find (ID) == dm_map.end ())
		return -1;

	int ret = dm_map[ID]->write (sd, buf);
	return ret;
}

int DM::update_block (int sd, int ID, char *buf)
{
	if (dm_map.find (ID) == dm_map.end ())
		return -1;

	int ret = dm_map[ID]->update (sd, buf);
	return ret;
}

int DM::wait_block (int sd, int ID)
{
	if (dm_map.find (ID) == dm_map.end ())
		return -1;

	int ret = dm_map[ID]->wait (sd);
	return ret;
}

void DM::clean (int sd)
{
	for (int i = first; i <= last; i++)
		dm_map[i]->clean (sd);
}
