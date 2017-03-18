#include <stdio.h>
#include <time.h>
#include "fs.h"
#include "verbose.h"

void usageMessage() {
   printf("usage: minls [-v] [-p num [-s num] ] imagefile [path]\n");
   printf("Options: \n");
   printf("-p part    --- select partition for filesystem (default: none)\n");
   printf("-s sub     --- select partition for filesystem (default: none)\n");
   printf("-h help    --- print usage information and exit\n");
   printf("-v verbose --- increase verbosity level\n");
}

void getPermissions(uint16_t mode)
{
   char permissions[] = "----------";
   permissions[0] = (mode & DIRECT) ? 'd' : '-';
   permissions[1] = (mode & O_READ) ? 'r' : '-';
   permissions[2] = (mode & O_WRITE) ? 'w' : '-';
   permissions[3] = (mode & O_EXEC) ? 'x' : '-';
   permissions[4] = (mode & G_READ) ? 'r' : '-';
   permissions[5] = (mode & G_WRITE) ? 'w' : '-';
   permissions[6] = (mode & G_EXEC) ? 'x' : '-';
   permissions[7] = (mode & OTHER_READ) ? 'r' : '-';
   permissions[8] = (mode & OTHER_WRITE) ? 'w' : '-';
   permissions[9] = (mode & OTHER_EXEC) ? 'x' : '-';
   printf(permissions);
} 

void verboseSB(struct superblock *sb) {
   /* Verbose superblock output */
   /* Magic numbers used to offset col size to a total length of 20 */
   printf("Superblock Contents:\n");
   printf("Stored Fields:\n");
   printf("  ninodes%13d\n", sb->ninodes);
   printf("  i_blocks%12d\n", sb->i_blocks);
   printf("  z_blocks%12d\n", sb->z_blocks);
   printf("  firstdata%11d\n", sb->firstdata);
   printf("  log_zone_size%7d (zone size: %d)\n",
         sb->log_zone_size, sb->blocksize);
   printf("  tmax_file%11u\n", sb->max_file);
   printf("  magic%15x\n", sb->magic);
   printf("  zones%15d\n", sb->zones);
   printf("  blocksize%11d\n", sb->blocksize);
   printf("  subversion%10d\n", sb->subversion);
}

void verboseComputedFields(struct superblock *sb) {
   printf("Computed Fields:\n");
   printf("  version%13i\n", 0);
   printf("  firstImap%11i\n", 0);
   printf("  firstZmap%11i\n", 0);
   printf("  firstIblock%9i\n", 0);
   printf("  zonesize%12i\n", 0);
   printf("  ptrs_per_zone%7i\n", 0);
   printf("  ino_per_zone%8i\n", 0);
   printf("  wrongended%10i\n", 0);
   printf("  fileent_size%8i\n", 0);
   printf("  max_filename%8i\n", 0);
   printf("  ent_per_zone%8i\n", 0);
   printf("\n");
}

void verboseiNode(struct inode *in) {
   time_t a = (time_t)in->atime;
   time_t m = (time_t)in->mtime;
   time_t c = (time_t)in->ctime;

   /* File inode output */
   printf("File inode:\n");
   printf("  unsigned short mode%18x ", in->mode);
   printf("(");
   getPermissions(in->mode);
   printf(")\n");
   printf("  unsigned short links%17d\n", in->links);
   printf("  unsigned short uid%19d\n", in->uid);
   printf("  unsigned short gid%19d\n", in->gid);
   printf("  uint32_t size%13d\n", in->size);
   printf("  uint32_t atime%12d --- %s",
         in->atime, ctime(&a));
   printf("  uint32_t mtime%12d --- %s",
         in->mtime, ctime(&m));
   printf("  uint32_t ctime%12d --- %s",
         in->ctime, ctime(&c));
   printf("\n");

   /* Direct zone info */
   printf("  Direct zones:\n");
   printf("\t\tzone[0] = %13i\n", in->zone[0]);
   printf("\t\tzone[1] = %13i\n", in->zone[1]);
   printf("\t\tzone[2] = %13i\n", in->zone[2]);
   printf("\t\tzone[3] = %13i\n", in->zone[3]);
   printf("\t\tzone[4] = %13i\n", in->zone[4]);
   printf("\t\tzone[5] = %13i\n", in->zone[5]);
   printf("\t\tzone[6] = %13i\n", in->zone[6]);
   printf(" uint32_t      indirect = %13i\n", in->indirect);
   printf(" uint32_t \t double = %13i\n", in->two_indirect);
}

void verbosePartTable(struct part parts[]) {
   int i;
   printf("Partition table:\n");
   printf("  Boot head  sec  cyl Type head  sec  cyl   First    Size\n");
   for (i = 0; i < NUM_POSS_PARTS; i++) {
      printf("  0x%x", parts[i].bootind);
      printf(" %4d", parts[i].start_head);
      printf(" %4d", parts[i].start_sec);
      printf(" %4d", parts[i].start_cyl);
      printf(" 0x%x", parts[i].type);
      printf(" %4d", parts[i].end_head);
      printf(" %4d", parts[i].end_sec);
      printf(" %4d", parts[i].end_cyl);
      printf(" %5d", parts[i].lFirst);
      printf("      %5d\n", parts[i].size);
   }
}

