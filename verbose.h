#ifndef __VERBOSE_H
#define __VERBOSE_H
#include "fs.h"

void usageMessage(); /* for minls right now */
void getPermissions(uint16_t);
void verboseSB(struct superblock *sb);
void verboseComputedFields(struct superblock *sb);
void verboseiNode(struct inode *in);
void verbosePartTable(struct part parts[]);

#endif
