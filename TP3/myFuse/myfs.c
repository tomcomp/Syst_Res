#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "tosfs.h"
#include <fuse/fuse_lowlevel.h>

struct file_map {
    void *data;
    size_t size;
};

static struct file_map mmap_file(const char *filepath)
{
    struct file_map map = { .data = NULL, .size = 0 };
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        perror(filepath);
        return map;
    }
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return map;
    }
    if (st.st_size == 0) {
        fprintf(stderr, "%s: empty file\n", filepath);
        close(fd);
        return map;
    }
    void *m = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (m == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return map;
    }
    close(fd);
    map.data = m;
    map.size = (size_t)st.st_size;
    return map;
}

static void print_superblock(struct tosfs_superblock *sb) {
    if (!sb) return;
    printf("TOSFS Superblock:\n");
    printf("  magic: %x\n", sb->magic);
    printf("  block_bitmap: " PRINTF_BINARY_PATTERN_INT32 "\n", PRINTF_BYTE_TO_BINARY_INT32(sb->block_bitmap));
    printf("  inode_bitmap: " PRINTF_BINARY_PATTERN_INT32 "\n", PRINTF_BYTE_TO_BINARY_INT32(sb->inode_bitmap));
    printf("  block_size: %u\n", sb->block_size);
    printf("  blocks: %u\n", sb->blocks);
    printf("  inodes: %u\n", sb->inodes);
    printf("  root_inode: %u\n", sb->root_inode);
}

static void print_inode(struct tosfs_inode *ino, int index) {
    if (!ino) return;

    struct tosfs_inode *to_print = ino + index;
    printf("%p\n", (void *)to_print);

    printf("Inode %d:\n", index);
    printf("  block_no: %u\n", to_print->block_no);
    printf("  uid: %u gid: %u\n", to_print->uid, to_print->gid);
    printf("  mode: %u perm: %u\n", to_print->mode, to_print->perm);
    printf("  size: %u nlink: %u\n", to_print->size, to_print->nlink);
}

static void print_dentry(struct tosfs_dentry *dentry) {
    if (!dentry) return;
    printf("Dentry:\n");
    printf("  inode: %u\n", dentry->inode);
    printf("  name: %s\n", dentry->name);
}

int main(int argc, char *argv[])
{

    const char *path = "test_tosfs_files";
    if (argc > 1 && argv[1]) path = argv[1];

    struct file_map map = mmap_file(path);
    if (!map.data) {
        fprintf(stderr, "failed to mmap '%s'\n", path);
        return 1;
    }

    if (map.size < sizeof(struct tosfs_superblock)) {
        fprintf(stderr, "%s: file too small for superblock\n", path);
        munmap(map.data, map.size);
        return 1;
    }

    struct tosfs_superblock *sb = (struct tosfs_superblock *)map.data;
    print_superblock(sb);


    struct tosfs_inode *ino = (struct tosfs_inode *)((void *)sb + TOSFS_BLOCK_SIZE);

    for (unsigned i = 0; i < sb->inodes; ++i) {
        print_inode(ino, i+1);
    }

    for (unsigned i = 0; i < sb->inodes; i++) {
        off_t cur_off = TOSFS_BLOCK_SIZE + (off_t)i * TOSFS_INODE_SIZE;
        
        struct tosfs_inode *cur = (struct tosfs_inode *)((void *)sb + cur_off);
        if (S_ISDIR(cur->mode)) {
            off_t dentry_block_offset = (off_t)cur->block_no * sb->block_size;
            
            struct tosfs_dentry *dblock = (struct tosfs_dentry *)((char *)map.data + dentry_block_offset);
            int entries = sb->block_size / sizeof(struct tosfs_dentry);
            for (int j = 0; j < entries; ++j) {
                struct tosfs_dentry *de = &dblock[j];
                if (de->inode == 0)
                    break;
                print_dentry(de);
            }
        }
    }

    munmap(map.data, map.size);

    return 0;
}
