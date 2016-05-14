/**
 * @file countspace.cpp
 * @brief Simple test program. Stores a file in distributed memory, counts
 * spaces in file, reads words count from distributed memory, then outputs
 * results on screen.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#include "../src/distmem.h"

/**
 * Countspace main function.
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
			printf ("Countspace: Error while mapping DM on LM\n");
			exit (1);
		}
	}

	FILE *fp = fopen ("divina", "r");

	// read file and store it on local memory
	// n = number of blocks
	// tot = number of bytes
	int n = 0;
	int bytes, tot = 0;
	while (feof (fp) == 0) {
		bytes = fread (blocks[n + 1], 1, size, fp);
		tot += bytes;
		n++;
	}

	// write number of blocks and total bytes of file on block 0
	memcpy (blocks[0], &n, sizeof(int));
	memcpy (blocks[0] + sizeof(int), &tot, sizeof(int));

	for (int i = 0; i <= n; i++) {
		ret = dm.dm_block_write (i);
		if (ret < 0) {
			printf ("Countspace: Error while writing file on DM\n");
			exit (1);
		}
	}

	// count spaces in file
	int count = 0;
	for (int i = 1; i <= n; i++) {
		for (int j = 0; j < size; j++) {
			if (blocks[i][j] == ' ')
				count++;
		}
	}

	// write spaces count on block n + 1
	memcpy (blocks[n + 1], &count, sizeof (int));

	ret = dm.dm_block_write (n + 1);
	if (ret == -1) {
		printf ("Countspace: Error while writing count on DM\n");
		exit (1);
	}
	if (ret == -2) {
		ret = dm.dm_block_update (n + 1);
		// write spaces count on block n + 1
		memcpy (blocks[n + 1], &count, sizeof (int));
		if (ret < 0) {
			printf ("Countspace: Error while updating block %d\n",
			       n + 1);
			exit (1);
		}
		ret = dm.dm_block_write (n + 1);
		if (ret == -1) {
			printf ("Countspace: Error while writing count on DM\n"
				);
			exit (1);
		}
	} else {
		// waits for countword to write words in distributed memory.
		ret = dm.dm_block_wait (n + 1);
		if (ret == -1) {
			printf ("Countspace: Error while waiting\n");
			exit (1);
		}
		ret = dm.dm_block_update (n + 1);
		if (ret < 0) {
			printf ("Countspace: Error while updating block %d\n",
				n + 1);
			exit (1);
		}
	}

	// reads words number from local memory
	int words;
	memcpy (&words, blocks[n + 1] + sizeof(int), sizeof(int));

	printf ("Spaces Number: %d\n\tWords Number: %d\n", count, words);

	for (int i = 0; i < 512; i++) {
		ret = dm.dm_block_unmap (i);
	}

	exit (0);
}
