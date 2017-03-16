#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include "fs.h"

int verbose, part, subpart, iNodeMapSize, zoneMapSize;
char *imageName, *pathName;

void usageMessage() {
   printf("usage: minls [-v] [-p num [-s num] ] imagefile [path]\n");
   printf("Options: \n");
   printf("-p part --- select partition for filesystem (default: none)\n");
   printf("-s sub --- select partition for filesystem (default: none)\n");
   printf("-h help --- print usage information and exit\n");
   printf("-v verbose --- increase verbosity level\n");
}

/* get filename */
void getFileStats(FILE *image, struct stat fileStat) {
   printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
   printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
   printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
   printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
   printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
   printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
   printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
   printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
   printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
   printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
   printf(" %i", fileStat.st_size);
   /* print filename */
}

struct superblock getSB(FILE *image) {
   int offset = BOOT_BLOCK;
   if (fseek(image, offset, SEEK_SET) != 0){
      exit(EXIT_FAILURE);
   }

   struct superblock sb;
   fread(&sb, sizeof(struct superblock), 1, image);
   return sb;
}

struct inode getRoot(FILE *image, uint16_t blocksize) {
   struct inode root;
   int offset = 2 * blocksize + iNodeMapSize + zoneMapSize;
   //int offset = 4096 * 16;
   printf("%d\n", offset);
   if (fseek(image, offset, SEEK_SET) != 0) {
      exit(EXIT_FAILURE);
   }
   fread(&root, sizeof(struct inode), 1, image);
   return root;
}

void verboseSB(struct superblock *sb) {
   /* Verbose superblock output */
   /* Magic numbers used to offset col size to a total length of 20 */
   printf("Superblock Contents:\n");
   printf("Stored Fields:\n");
   printf("\tninodes%13d\n", sb->ninodes);
   printf("\ti_blocks%12d\n", sb->i_blocks);
   printf("\tz_blocks%12d\n", sb->z_blocks);
   printf("\tfirstdata%11d\n", sb->firstdata);
   printf("\tlog_zone_size%7d (zone size: %d)\n",
         sb->log_zone_size, sb->blocksize);
   printf("\tmax_file%12u\n", sb->max_file);
   printf("\tmagic%15x\n", sb->magic);
   printf("\tzones%15d\n", sb->zones);
   printf("\tblocksize%11d\n", sb->blocksize);
   printf("\tsubversion%10d\n", sb->subversion);
   printf("\n");
}

void verboseiNode(struct inode *in) {
   time_t a = (time_t)in->atime;
   time_t m = (time_t)in->mtime;
   time_t c = (time_t)in->ctime;

   /* File inode output */
   printf("File inode:\n");
   printf("Stored Fields:\n");
   printf("\tunsigned short mode%13x (%s)\n", in->mode, "insert permissions");
   printf("\tunsigned short links%13d\n", in->links);
   printf("\tunsigned short uid%13d\n", in->uid);
   printf("\tunsigned short gid%13d\n", in->gid);
   printf("\tuint32_t size%13d\n", in->size);
   printf("\tuint32_t atime%13d --- %s\n",
         in->atime, ctime(&a));
   printf("\tuint32_t mtime%13d --- %s\n",
         in->mtime, ctime(&m));
   printf("\tuint32_t ctime%13d --- %s\n",
         in->ctime, ctime(&c));
   printf("\n");

   /* Direct zone info */
   printf("Direct zones:\n");
   printf("zone[0] = %13i\n", in->zone[0]);
   printf("zone[1] = %13i\n", in->zone[1]);
   printf("zone[2] = %13i\n", in->zone[2]);
   printf("zone[3] = %13i\n", in->zone[3]);
   printf("zone[4] = %13i\n", in->zone[4]);
   printf("zone[5] = %13i\n", in->zone[5]);
   printf("zone[6] = %13i\n", in->zone[6]);
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

void printPermissions(FILE *image)
{

}

/* Print items in the directory */
void printNames(struct inode currDir, FILE *image) {
   /* Just printing files in root for now */
   struct dir mydir;

}
void parseArgs(char **argv, int argc) {
   /* Parse arguments */
   int cmd, firstPass = TRUE;
   while (optind < argc) {
      while ((cmd = getopt(argc, argv, "p:s:vh")) != -1) {
         switch(cmd) {
         case 'p':
            part = atoi(optarg);
            printf("part here is %d\n", part);
            break;
         case 's':
            subpart = atoi(optarg);
            break;
         case 'h':
            usageMessage();
            break;
         case 'v':
            verbose = TRUE;
            break;
         default:
            break;
         }
      }
      if (firstPass) {
         imageName = argv[optind];
         firstPass = FALSE;
      } else {
         pathName = argv[optind];
      }
      optind++;
   }
}
int findMapSize(int blocks, int blockSize) {
   int currSize = blockSize;
   while (blocks > (currSize * 8))
      currSize <<= 1;
   return currSize;
}

/* Seek set and return inode and zone map */
void getMaps(void *inodeMap, void *zoneMap, uint16_t blocksize, FILE *image) {
   fseek(image, 2 * blocksize, SEEK_SET);
   fread(inodeMap, iNodeMapSize, 1, image);
   fread(zoneMap, zoneMapSize, 1, image);
}

void fileNames(int zoneNum, uint16_t blocksize, uint16_t size,
      FILE *image, struct dir **files) {
   int i, offset = blocksize * zoneNum, numFiles = size / DIR_SIZE;
   *files = malloc(numFiles);
   fseek(image, offset, SEEK_SET);
   for (i = 0; i < numFiles; i++) {
      fread(*files + i, DIR_SIZE, 1, image);

   }
}

void displayNames(struct dir *filenames, int numFiles) {
   int i = 0, currZone;
   for (i = 0; i < numFiles; i++) {
      if (filenames->inode) 
         printf("%s at inode %d\n", filenames->name, filenames->inode);
      filenames++;
   }
}

int testMagicNum(struct superblock sb) {
   if (sb.magic != MINIX_MAGIC) {
      printf("Bad magic number. (0x%.4x)\n", sb.magic);
      printf("This doesn't look like a MINIX filesystem.\n");
      return -1;
   }
   return 0;
}

void testPartTable(FILE *image, int offset) {
   int sig;
   fseek(image, offset, SEEK_SET);
   fread(&sig, sizeof(int), 1, image);
   printf("0x%x\n", sig);

}

void getParts(FILE *image, struct part parts[]) {
   int i;
   fseek(image, PART_OFFSET, SEEK_SET);
   for (i = 0; i < NUM_POSS_PARTS; i++) {
      fread(&parts[i], sizeof(struct part), 1, image);
      //testPartTable(image, PART_SIG_OFFSET);

   }
}

void getSubParts(FILE *image, struct part curr, struct part subParts[]) {
   /* Do code to get subpartitions */
   int i, offset = curr.lFirst * SECTOR_SIZE + PART_OFFSET; 
   
   fseek(image, offset, SEEK_SET);
   for (i = 0; i < NUM_POSS_PARTS; i++) {
      fread(&subParts[i], sizeof(struct part), 1, image);
      //testPartTable(image, offset + PART_SIG_OFFSET);

   }
}

int main (int argc, char **argv) {
   FILE *image;
   void *inodeMap, *zoneMap;
   struct superblock sb;
   struct inode in;
   struct dir *files;
   struct part partition[NUM_POSS_PARTS];
   struct part subPartition[NUM_POSS_PARTS];
   //struct part test;
   int numFiles;
   //int num;
   verbose = FALSE, part = NONE, subpart = NONE;
   part = subpart = -1;
   parseArgs(argv, argc);
   image = fopen(imageName, "rb");
   printf("%d\n", sizeof(struct part));
   /* Test code */
   /*fseek(image, PART_OFFSET, SEEK_SET);
     fread(&test, sizeof(struct part),  1, image);
     printf("0x%x\n", test.bootind);
     printf("%d\n", test.start_head);
     printf("%d\n", test.start_sec);
     printf("%d\n", test.start_cyl);
     printf("0x%x\n", test.type);
     printf("%d\n", test.end_head);
     printf("%d\n", test.end_sec);
     printf("%d\n", test.end_cyl);
     printf("%d\n", test.lFirst);
     printf("%d\n", test.size);
     testPartTable(image); */
   /* End test code */
   printf("part is %d\n", part);
   if (part >= 0) {
      getParts(image, partition);
      if (subpart >= 0)
         getSubParts(image, partition[part], subPartition);
   }
   sb = getSB(image);
   if (verbose) {
      verboseSB(&sb);
      verboseiNode(&in);
      if (part >= 0)
         verbosePartTable(partition);
      if (subpart >= 0)
         verbosePartTable(subPartition);
   }
   if (testMagicNum(sb) != 0) {
      exit(EXIT_FAILURE);
   }

   zoneMapSize = findMapSize(sb.z_blocks, sb.blocksize);
   iNodeMapSize = findMapSize(sb.i_blocks, sb.blocksize);

   inodeMap = malloc(iNodeMapSize);
   zoneMap = malloc(zoneMapSize);
   /* Read iNode bitmap */
   getMaps(inodeMap, zoneMap, sb.blocksize, image);
   in = getRoot(image, sb.blocksize);
   if (pathName == NULL) {
      numFiles = in.size / DIR_SIZE;
      fileNames(in.zone[0], sb.blocksize, in.size, image, &files);
      displayNames(files, numFiles);
   }
   fclose(image);
   return 0;

}
