#ifndef __FS_H
#define __FS_H

#include <stdint.h>

#define DIRECT_ZONES 7
#define SECTOR_SIZE 512
#define BOOT_BLOCK 1024
#define SUPER_BLOCK 1024
#define TRUE 1
#define FALSE 0
#define NONE -1
#define MINIX_MAGIC 0X4D5A
#define PART_OFFSET 0X1BE
#define DIR_SIZE 64


struct superblock {
	uint32_t ninodes;		/* number of inodes in this filesystem */
	uint16_t pad1;			/* make things line up properly */
	int16_t i_blocks;		/* # of blocks used by inode bit map */
	int16_t z_blocks;		/* # of blocks used by zone bit map */
	uint16_t firstdata;		/* number of first data zone */
	int16_t log_zone_size;	/* log2 of blocks per zone */
	int16_t pad2;			/* make things line up again */
	uint32_t max_file;		/* maximum file size */
	uint32_t zones;			/* number of sones on disks */
	int16_t magic;			/* magic number */
	int16_t pad3;			/* make things line up again */
	uint16_t blocksize;		/* block size in bytes */
	uint8_t subversion;		/* filesystem sub-version */
};

struct inode {
	uint16_t mode;					/* file type and rwx bits */
	uint16_t links;					/* number of links */
	uint16_t uid;					/* identifies user who owns files */
	uint16_t gid;					/* owner's group */
	uint32_t size;					/* # of bytes in file */
	int32_t atime;					/* access time */
	int32_t mtime;					/* modification time */
	int32_t ctime;					/* status change time */
	uint32_t zone[DIRECT_ZONES];	/* zone numbers for frist seven data zones */
	uint32_t indirect;				/* used for files larger than 7 zones */
	uint32_t two_indirect;			/* used for files larger than 7 zones */
	uint32_t unused;				/* could be used for triple indirect zone */
};

struct dir {
	uint32_t inode;
	unsigned char name[60];
};

/* Partition entry */
struct part {
   uint8_t bootind;
   uint8_t start_head;
   uint8_t start_sec;
   uint8_t start_cyl;
   uint8_t type;
   uint8_t end_head;
   uint8_t end_sec;
   uint8_t end_cyl;
   uint32_t lFirst;
   uint32_t size;
};

extern int verbose;
extern int part;
extern int subpart;
extern int zoneMapSize;
extern int iNodeMapSize;
extern char* imageName;
extern char* pathName;
#endif /*__FS_H*/
