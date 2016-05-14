/**
 * @file countword.cpp
 * @brief Simple test program. Reads a file from distributed memory, counts
 * words in file, reads spaces count from distributed memory, then outputs
 * results on screen.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#include "../src/distmem.h"

/**
 * Countword main function.
 * @param[in]	argv[1] A valid Distributed Memory configuration file.
 */
int main (int argc, char *argv[])
{
	if (argc != 2)
		exit (1);

	char *config_file = argv[1];

	DM_client dm;
	int ret = dm.dm_init (config_file);
	int size = dm.dm_block_dim ();
	char blocks[512][size];

	// map dm on lm
	for (int i = 0; i < 512; i++) {
		ret = dm.dm_block_map (i, blocks[i]);
		if (ret == -1) {
			printf ("Countword: Error while mapping DM on LM\n");
			exit (1);
		}
	}

	// wait for countspace to write file on DM
	ret = dm.dm_block_wait (0);
	if (ret == -1) {
		printf ("Countword: Error while waiting block 0\n");
		exit (1);
	}
	ret = dm.dm_block_update (0);
	if (ret < 0) {
		printf ("Countword: Error while updating block 0\n");
		exit (1);
	}

	// read number of blocks and total bytes of file from block 0
	int n, tot;
	memcpy (&n, blocks[0], sizeof(int));
	memcpy (&tot, blocks[0] + sizeof(int), sizeof(int));

	// read file
	for (int i = 1; i <= n; i++) {
		ret = dm.dm_block_wait (i);
		if (ret < 0) {
			printf ("Countword: Error while waiting block %d\n",
				i);
			exit (1);
		}
		ret = dm.dm_block_update (i);
		if (ret < 0) {
			printf ("Countword: Error while updating block %d\n",
				i);
			exit (1);
		}
	}

	// copy all file blocks in one block
	char file[tot + 1];
	int copy = size;
	char *p = file;
	int t = tot;

	for (int i = 1; i <= n ; i++) {
		memcpy (p, blocks[i], copy);
		p += copy;
		t -= copy;
		if (t < copy)
			copy = t;
	}
		
	file[tot] = '\0';

	// count words in file
	int words = 0;
	char *w;
	w = strtok (file, " \t\r\n");
	if (w != NULL) {
		words++;
		while (1) {
			w = strtok (NULL, " \t\r\n");
			if (w == NULL)
				break;
			words++;
		}
	}

	// write words count on block n + 1
	memcpy (blocks[n + 1] + sizeof(int), &words, sizeof (int));

	ret = dm.dm_block_write (n + 1);
	if (ret == -1) {
		printf ("Countword: Error while writing count on DM\n");
		exit (1);
	}
	if (ret == -2) {
		// invalid block
		ret = dm.dm_block_update (n + 1);
		// write words count on block n + 1
		memcpy (blocks[n + 1] + sizeof(int), &words, sizeof (int));
		if (ret < 0) {
			printf ("Countword: Error while updating block %d\n",
			       n + 1);
			exit (1);
		}
		ret = dm.dm_block_write (n + 1);
		if (ret == -1) {
			printf ("Countword: Error while writing count on DM\n"
				);
			exit (1);
		}
	} else {
		// wait for countspace to write spaces number on dm then read it
		ret = dm.dm_block_wait (n + 1);
		if (ret == -1) {
			printf ("Countword: Error while waiting\n");
			exit (1);
		}
		ret = dm.dm_block_update (n + 1);
		if (ret < 0) {
			printf ("Countword: Error while updating block %d\n",
				n + 1);
			exit (1);
		}
	}

	// read spaces number from local memory
	int count;
	memcpy (&count, blocks[n + 1], sizeof(int));

	printf ("Spaces Number: %d\n\tWords Number: %d\n", count,
		words);

	for (int i = 0; i < 512; i++) {
		ret = dm.dm_block_unmap (i);
	}

	exit (0);
}
