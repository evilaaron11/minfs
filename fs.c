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

void parseArgs (char **argv, int argc) {
   /* Parse arguments */
   int cmd, firstPass = TRUE;
   while (optind < argc) {
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

/* Should return permission string */

char * getPermissions(uint16_t mode)
{
   char *permissions = (char *) malloc(sizeof(char) * 10);
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
   return permissions;
}

/* can get inode numbers from dirent, but not inode mode */
int getMode(FILE *image, struct dir *filenames, 
	uint16_t blocksize, int offset) {
   fseek(image, offset, SEEK_SET);
   return filenames->inode;
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

void displayNames(struct inode node, struct dir *filenames, 
	uint16_t blocksize, int numFiles, FILE *image) {
   struct inode in;
   int i = 0;
   for (i = 0; i < numFiles; i++) {
      node = getiNode(image, blocksize, filenames->inode);
      printf("%s %9i %s\n", getPermissions(node.mode), node.size,
          filenames->name);
      filenames++;
   }
}

int main (int argc, char **argv) {
   FILE *image;
   void *inodeMap, *zoneMap;
   struct superblock sb;
   struct inode in;
   struct dir *files;
   int numFiles;
   //int num;
   verbose = FALSE, part = NONE, subpart = NONE;

   parseArgs(argv, argc);
   image = fopen(imageName, "rb");
   sb = getSB(image);
   zoneMapSize = findMapSize(sb.z_blocks, sb.blocksize);
   iNodeMapSize = findMapSize(sb.i_blocks, sb.blocksize);

   inodeMap = malloc(iNodeMapSize);
   zoneMap = malloc(zoneMapSize);
   /* Read iNode bitmap */
   getMaps(inodeMap, zoneMap, sb.blocksize, image);
   in = getRoot(image, sb.blocksize);
   numFiles = in.size / DIR_SIZE;
   fileNames(in.zone[0], sb.blocksize, in.size, image, &files);
   //displayNames(files, numFiles);

   if (verbose) {
      verboseSB(&sb);
      verboseiNode(&in);   
   }

   displayNames(in, files, sb.blocksize, numFiles, image);
   fclose(image);

   return 0;
}
