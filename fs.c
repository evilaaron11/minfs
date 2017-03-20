#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "fs.h"
#include "verbose.h"

int verbose, part, subpart, iNodeMapSize, zoneMapSize;
char *imageName, *pathName;
/*
void usageMessage() {
   printf("usage: minls [-v] [-p num [-s num] ] imagefile [path]\n");
   printf("Options: \n");
   printf("-p part    --- select partition for filesystem (default: none)\n");
   printf("-s sub     --- select partition for filesystem (default: none)\n");
   printf("-h help    --- print usage information and exit\n");
   printf("-v verbose --- increase verbosity level\n");
}
*/
struct superblock getSB(FILE *image, uint32_t lFirst) {
   int offset = BOOT_BLOCK + (lFirst * SECTOR_SIZE);
   if (fseek(image, offset, SEEK_SET) != 0){
      exit(EXIT_FAILURE);
   }

   struct superblock sb;
   fread(&sb, sizeof(struct superblock), 1, image);
   return sb;
}

struct inode getRoot(FILE *image, uint16_t blocksize, uint32_t lFirst) {
   struct inode root;
   int offset = (2 * blocksize) + iNodeMapSize + zoneMapSize
      + (lFirst * SECTOR_SIZE);
   if (fseek(image, offset, SEEK_SET) != 0) {
      exit(EXIT_FAILURE);
   }
   fread(&root, sizeof(struct inode), 1, image);
   return root;
}

uint32_t getZoneSize(uint16_t blocksize, int16_t log_zone_size) {
   return blocksize << log_zone_size;
}
/*
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
*/
/*
void verboseSB(struct superblock *sb) { */
   /* Verbose superblock output */
   /* Magic numbers used to offset col size to a total length of 20 */
 /*  printf("Superblock Contents:\n");
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
*/

/*
void verboseiNode(struct inode *in) {
   time_t a = (time_t)in->atime;
   time_t m = (time_t)in->mtime;
   time_t c = (time_t)in->ctime;
*/
   /* File inode output */
 /*  printf("File inode:\n");
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
*/
   /* Direct zone info */
 /*  printf("  Direct zones:\n");
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
*/
/*void verbosePartTable(struct part parts[]) {
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
*/
void parseArgs(char **argv, int argc) {
   /* Parse arguments */
   int cmd, firstPass = TRUE;

   while ((cmd = getopt(argc, argv, "p:s:vh")) != -1) {
      switch(cmd) {
      case 'p':
         part = atoi(optarg);
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
   imageName = argv[optind];
   if (optind++ < argc) {
      pathName = argv[optind];
      /*printf("path: %s\n", pathName);*/
   }
}

int findMapSize(int blocks, int blockSize) {
   int currSize = blockSize;
   while (blocks > (currSize * 8))
      currSize <<= 1;
   return currSize;
}

/* Seek set and return inode and zone map
 * This function can be generalized for a
 * partition or subpartition hence why only
 * one struct is needed */
void getMaps(void *inodeMap, void *zoneMap, uint16_t blocksize,
      uint32_t lFirst, FILE *image) {
   int offset = 2 * blocksize + lFirst * SECTOR_SIZE;
   fseek(image, offset, SEEK_SET);
   fread(inodeMap, iNodeMapSize, 1, image);
   fread(zoneMap, zoneMapSize, 1, image);
}

/* Displays filenam at an inode.
 * Returns size left to grab once zone size is reached */
int fileNames(int zoneNum, uint16_t zonesize, uint16_t size,
      FILE *image, struct dir **files, uint32_t lFirst) {
   int i, offset = zonesize * zoneNum + lFirst * SECTOR_SIZE;
   int numFiles = size / DIR_SIZE, currSize = 0;
   struct dir *temp = *files;
<<<<<<< HEAD
   /* Meeds to expand to multiple zone support */
   fseek(image, offset, SEEK_SET);
   for (i = 0; i < numFiles && currSize < zonesize; i++) {
      fread(temp + i, DIR_SIZE, 1, image);
      printf("%s\n", (temp + i)->name);
=======
   /* Needs to expand to multiple zone support */
   fseek(image, offset, SEEK_SET);
   for (i = 0; i < numFiles && currSize < zonesize; i++) {
      fread(temp + i, DIR_SIZE, 1, image);
      /*printf("%s\n", (temp + i)->name);*/
>>>>>>> 7e121bfeeccec627ba471393c6422e7675975658
      currSize += DIR_SIZE;
   }

   return size - currSize;
}

struct inode getiNode(FILE *image, int blocksize, uint32_t lFirst, 
      int inodeNum) {
   struct inode root;
   int offset = (2 * blocksize) + iNodeMapSize + zoneMapSize
      + (lFirst * SECTOR_SIZE);
   offset += (inodeNum - 1) * sizeof(struct inode);
   if (fseek(image, offset, SEEK_SET) != 0) {
      exit(EXIT_FAILURE);
   }
   fread(&root, sizeof(struct inode), 1, image);
   return root;
}

void displayNames(struct dir *filenames,
      uint16_t blocksize, int numFiles, uint32_t lFirst, FILE *image) {
   struct inode in;
   struct dir *temp = filenames;
   int i = 0;
   for (i = 0; i < numFiles; i++) {
<<<<<<< HEAD
      printf("inside display names\n");
=======
>>>>>>> 7e121bfeeccec627ba471393c6422e7675975658
      in = getiNode(image, blocksize, lFirst, temp->inode);
      if (temp->inode) {
         getPermissions(in.mode);
         printf("%10i %s\n", in.size, temp->name);
      }
      temp++;
   }
}

int getInodeFromPath(struct dir *filenames,
      uint16_t blocksize, int numFiles, uint32_t lFirst, FILE *image,
      char *folder) {
   struct inode in;
   int i = 0;
   for (i = 0; i < numFiles; i++) {
      if (strcmp(folder, filenames->name) == 0) {
         printf("Found and inode is %d\n", filenames->inode);
         return filenames->inode;
      }
      filenames++;
   }

   return 0;
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
      fread(&parts[i].bootind, sizeof(uint8_t), 1, image);
      fread(&parts[i].start_head, sizeof(uint8_t), 1, image);
      fread(&parts[i].start_sec, sizeof(uint8_t), 1, image);
      fread(&parts[i].start_cyl, sizeof(uint8_t), 1, image);
      fread(&parts[i].type, sizeof(uint8_t), 1, image);
      fread(&parts[i].end_head, sizeof(uint8_t), 1, image);
      fread(&parts[i].end_sec, sizeof(uint8_t), 1, image);
      fread(&parts[i].end_cyl, sizeof(uint8_t), 1, image);
      fread(&parts[i].lFirst, sizeof(uint32_t), 1, image);
      fread(&parts[i].size, sizeof(uint32_t), 1, image);
   }
}

void getSubParts(FILE *image, struct part curr, struct part subParts[]) {
   /* Do code to get subpartitions */
   int i, offset = curr.lFirst * SECTOR_SIZE + PART_OFFSET;

   fseek(image, offset, SEEK_SET);
   for (i = 0; i < NUM_POSS_PARTS; i++) {
      fread(&subParts[i], sizeof(struct part), 1, image);
   }
}

/* Goes through all inodes and gets names */
void getAllFiles(struct inode in, uint16_t zonesize,
      FILE *image, struct dir **files, uint32_t lFirst) {
   int i, sizeLeft = in.size, currFile = 0;
   struct dir *temp;
   for (i = 0; sizeLeft != 0 && i < DIRECT_ZONES; i++) {
<<<<<<< HEAD
      printf("In getAllFiles\n");
      //temp += numPerZone;
      temp = *files + currFile;
      sizeLeft = fileNames(in.zone[i], zonesize, sizeLeft, image,
            &temp, lFirst);    
      printf("sizeleft is %d\n", sizeLeft);
      currFile = (in.size - sizeLeft) / DIR_SIZE;
      printf("currFileNum is %d\n", currFile);

=======
      temp = *files + currFile;
      sizeLeft = fileNames(in.zone[i], zonesize, sizeLeft, image,
            &temp, lFirst);    
      currFile = (in.size - sizeLeft) / DIR_SIZE;
>>>>>>> 7e121bfeeccec627ba471393c6422e7675975658
   }
}

struct inode parsePath(char *path, uint16_t zonesize, FILE *image,
      struct dir **files, uint32_t lFirst, int *numF, struct inode root,
      uint16_t blocksize) {
   char *curr;
   int inodeNum, numFiles;
   struct dir *temp;

   curr = strtok(path, "/");

   while (curr != NULL) {
      numFiles = root.size / DIR_SIZE;
      temp = malloc(sizeof(struct dir) * numFiles);
      printf("Q %s\n", curr);
      getAllFiles(root, zonesize, image, &temp, lFirst);
<<<<<<< HEAD
      printf("Above inode nums\n");
=======
>>>>>>> 7e121bfeeccec627ba471393c6422e7675975658
      inodeNum = getInodeFromPath(temp, blocksize, numFiles,
            lFirst, image, curr);
      root = getiNode(image, blocksize, lFirst, inodeNum);
      curr = strtok(NULL, "/");                    
      free(temp);
   }

   numFiles = root.size / DIR_SIZE;
<<<<<<< HEAD
   printf("Num files are %d\n", numFiles);
   temp = malloc(sizeof(struct dir) * numFiles);
   getAllFiles(root, zonesize, image, &temp, lFirst);
   printf("\nShould display filenames\n");
=======
   temp = malloc(sizeof(struct dir) * numFiles);
   getAllFiles(root, zonesize, image, &temp, lFirst);
>>>>>>> 7e121bfeeccec627ba471393c6422e7675975658
   displayNames(temp, blocksize, numFiles, lFirst, image);
   printf("\n\n");
   *files = temp;
   *numF = numFiles;

   return root;
}

int main (int argc, char **argv) {
   FILE *image;
   void *inodeMap, *zoneMap;
   struct superblock sb;
   struct inode in;
   struct dir *files;
   struct part partition[NUM_POSS_PARTS];
   struct part subPartition[NUM_POSS_PARTS];
   int numFiles, sizeLeft, i;
   uint32_t firstSector = 0;
   verbose = FALSE, part = NONE, subpart = NONE;
   part = subpart = -1;
   parseArgs(argv, argc);
   image = fopen(imageName, "rb");
   if (part >= 0) {
      getParts(image, partition);
      firstSector = partition[part].lFirst;
      if (subpart >= 0) {
         getSubParts(image, partition[part], subPartition);
         firstSector = subPartition[subpart].lFirst;
      }
   }
   sb = getSB(image, firstSector);
   /*if (testMagicNum(sb) != 0) {
     exit(EXIT_FAILURE);
     } */

   zoneMapSize = findMapSize(sb.z_blocks, sb.blocksize);
   iNodeMapSize = findMapSize(sb.i_blocks, sb.blocksize);

   inodeMap = malloc(iNodeMapSize);
   zoneMap = malloc(zoneMapSize);
   /* Read iNode bitmap */
   getMaps(inodeMap, zoneMap, sb.blocksize, firstSector, image);
   in = getRoot(image, sb.blocksize, firstSector);

   numFiles = in.size / DIR_SIZE;
   if (pathName == NULL) {
      files = malloc(numFiles * DIR_SIZE);
      /*for (i = 0; sizeLeft != 0; i++) {
         sizeLeft = fileNames(in.zone[i], getZoneSize(sb.blocksize,
                  sb.log_zone_size), in.size, image, &files, firstSector);
      } */
      getAllFiles(in, getZoneSize(sb.blocksize, sb.log_zone_size), image,
            &files, firstSector);
   } else {
      in = parsePath(pathName, getZoneSize(sb.blocksize,sb.log_zone_size),
            image, &files, firstSector, &numFiles, in, sb.blocksize);
<<<<<<< HEAD
      printf("Verbose returned inode\n");
=======
>>>>>>> 7e121bfeeccec627ba471393c6422e7675975658
      verboseiNode(&in);
   }

   if (verbose) {
      verboseSB(&sb);
      verboseComputedFields(&sb);
      verboseiNode(&in);
      if (part >= 0)
         verbosePartTable(partition);
      if (subpart >= 0)
         verbosePartTable(subPartition);
   }
   if (in.mode & DIRECT) {
      if (pathName) {
         printf("%s:\n", pathName);
      }
      else {   
         printf("/:\n");
      }
   }
   printf("The size of the dirs are %d\n", sizeof(files));
   displayNames(files, sb.blocksize, numFiles, firstSector, image);
   fclose(image);

   return 0;
}
