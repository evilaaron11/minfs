#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
   //t printf("%d\n", offset);
   if (fseek(image, offset, SEEK_SET) != 0) {
      exit(EXIT_FAILURE);
   }
   fread(&root, sizeof(struct inode), 1, image);
   return root;
}

void getPermissions(uint16_t mode)
{
   //char *permissions = (char *) malloc(sizeof(char) * 5);
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
   //return permissions;
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

struct inode getiNode(FILE *image, int blocksize, int inodeNum) {
   struct inode root;
   int offset = 2 * blocksize + iNodeMapSize + zoneMapSize;
   int offset2 = (inodeNum - 1) * sizeof(struct inode);
   if (fseek(image, offset + offset2, SEEK_SET) != 0) {
      exit(EXIT_FAILURE);
   }
   fread(&root, sizeof(struct inode), 1, image);
   return root;
}

void displayNames(struct dir *filenames, 
	uint16_t blocksize, int numFiles, FILE *image) {
   struct inode in;
   int i = 0;
   for (i = 0; i < numFiles; i++) {
      in = getiNode(image, blocksize, filenames->inode);
      getPermissions(in.mode);
      printf("%10i %s\n", in.size,
          filenames->name);
      filenames++;
   }
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

   if (verbose) {
      verboseSB(&sb);
      verboseComputedFields(&sb);
      verboseiNode(&in);   
   }
   if (in.mode & DIRECT) {
      printf("/:\n");
   }
   displayNames(files, sb.blocksize, numFiles, image);
   fclose(image);

   return 0;
}
