/*
 * Copyright (c) 2021 - 2022, the pranaOS Developers & Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "poll.h"
#include <ak/atomic.h>
#include <libc/fcntl.h>
#include <ak/list.h>
#include <ak/types.h>
#include <locking/semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <system/time.h>

#define MS_NOUSER (1 << 31)

/**
 * @brief FMODE
 * 
 */
#define FMODE_READ 0x1
#define FMODE_WRITE 0x2
#define FMODE_CAN_READ 0x20000
#define FMODE_CAN_WRITE 0x40000
#define OPEN_FMODE(flag) ((flag + 1) & O_ACCMODE)

struct vm_area_struct;
struct vfs_superblock;

/**
 * @brief address space
 * 
 */
struct address_space {
	struct vm_area_struct *i_mmap;
	struct list_head pages;
	uint32_t npages;
};

/**
 * @brief dirent
 * 
 */
struct dirent {
	ino_t d_ino;			
	off_t d_off;			 
	unsigned short d_reclen; 
	unsigned short d_type;	 
                             
	char d_name[];			 
};

struct kstat {
	dev_t st_dev;		  
	ino_t st_ino;		  
	mode_t st_mode;		  
	nlink_t st_nlink;	  
	uid_t st_uid;		  
	gid_t st_gid;		  
	dev_t st_rdev;		  
	off_t st_size;		  
	blksize_t st_blksize; 
	blkcnt_t st_blocks;	  

	struct timespec st_atim; 
	struct timespec st_mtim; 
	struct timespec st_ctim; 
};

#define ATTR_MODE 1
#define ATTR_UID 2
#define ATTR_GID 4
#define ATTR_SIZE 8
#define ATTR_ATIME 16
#define ATTR_MTIME 32
#define ATTR_CTIME 64
#define ATTR_ATIME_SET 128
#define ATTR_MTIME_SET 256
#define ATTR_FORCE 512 
#define ATTR_ATTR_FLAG 1024
#define ATTR_KILL_SUID 2048
#define ATTR_KILL_SGID 4096

struct iattr {
	unsigned int ia_valid;
	umode_t ia_mode;
	uid_t ia_uid;
	gid_t ia_gid;
	loff_t ia_size;
	struct timespec ia_atime;
	struct timespec ia_mtime;
	struct timespec ia_ctime;
	unsigned int ia_attr_flags;
};

struct vfs_file_system_type {
	const char *name;
	struct vfs_mount *(*mount)(struct vfs_file_system_type *, char *, char *);
	void (*unmount)(struct vfs_superblock *);
	struct vfs_file_system_type *next;
};

struct vfs_mount {
	struct vfs_dentry *mnt_root;
	struct vfs_dentry *mnt_mountpoint;
	struct vfs_superblock *mnt_sb;
	char *mnt_devname;
	struct list_head sibling;
};

struct vfs_superblock {
	unsigned long s_blocksize;
	dev_t s_dev;
	unsigned char s_blocksize_bits;
	struct vfs_file_system_type *s_type;
	struct vfs_super_operations *s_op;
	unsigned long s_magic;
	struct vfs_dentry *s_root;
	char *mnt_devname;
	void *s_fs_info;
};

struct vfs_super_operations {
	struct vfs_inode *(*alloc_inode)(struct vfs_superblock *sb);
	void (*read_inode)(struct vfs_inode *);
	void (*write_inode)(struct vfs_inode *);
	void (*write_super)(struct vfs_superblock *);
};

struct vfs_inode {
	unsigned long i_ino;
	umode_t i_mode;
	unsigned int i_nlink;
	uid_t i_uid;
	gid_t i_gid;
	dev_t i_rdev;
	atomic_t i_count;
	struct timespec i_atime;
	struct timespec i_mtime;
	struct timespec i_ctime;
	uint32_t i_blocks;
	unsigned long i_blksize;
	uint32_t i_flags;
	uint32_t i_size;
	struct semaphore i_sem;
	struct pipe *i_pipe;
	struct address_space i_data;
	struct vfs_inode_operations *i_op;
	struct vfs_file_operations *i_fop;
	struct vfs_superblock *i_sb;
	void *i_fs_info;
};

struct vfs_inode_operations {
	struct vfs_inode *(*create)(struct vfs_inode *dir, struct vfs_dentry *dentry, mode_t mode);
	struct vfs_inode *(*lookup)(struct vfs_inode *dir, struct vfs_dentry *dentry);
	int (*mkdir)(struct vfs_inode *, char *, int);
	int (*rename)(struct vfs_inode *old_dir, struct vfs_dentry *old_dentry,
				  struct vfs_inode *new_dir, struct vfs_dentry *new_dentry);
	int (*unlink)(struct vfs_inode *dir, struct vfs_dentry *dentry);
	int (*mknod)(struct vfs_inode *, struct vfs_dentry *, int, dev_t);
	void (*truncate)(struct vfs_inode *);
	int (*setattr)(struct vfs_dentry *, struct iattr *);
	int (*getattr)(struct vfs_mount *mnt, struct vfs_dentry *, struct kstat *);
};

struct vfs_dentry {
	struct vfs_inode *d_inode;
	struct vfs_dentry *d_parent;
	char *d_name;
	struct vfs_superblock *d_sb;
	struct list_head d_subdirs;
	struct list_head d_sibling;
};

struct vfs_file {
	struct vfs_dentry *f_dentry;
	struct vfs_mount *f_vfsmnt;
	struct vfs_file_operations *f_op;
	atomic_t f_count;
	size_t f_maxcount;
	unsigned int f_flags;
	void *private_data;
	fmode_t f_mode;
	loff_t f_pos;
};

struct vfs_file_operations {
	loff_t (*llseek)(struct vfs_file *file, loff_t ppos, int);
	ssize_t (*read)(struct vfs_file *file, char *buf, size_t count, loff_t ppos);
	ssize_t (*write)(struct vfs_file *file, const char *buf, size_t count, loff_t ppos);
	int (*readdir)(struct vfs_file *dir, struct dirent *dirent, unsigned int count);
	unsigned int (*poll)(struct vfs_file *file, struct poll_table *pt);
	int (*ioctl)(struct vfs_inode *inode, struct vfs_file *file, unsigned int cmd, unsigned long arg);
	int (*mmap)(struct vfs_file *file, struct vm_area_struct *vm);
	int (*open)(struct vfs_inode *inode, struct vfs_file *file);
	int (*release)(struct vfs_inode *inode, struct vfs_file *file);
};

struct nameidata {
	struct vfs_dentry *dentry;
	struct vfs_mount *mnt;
};

/**
 * @brief register filesystem
 * 
 * @param fs 
 * @return int 
 */
int register_filesystem(struct vfs_file_system_type *fs);

/**
 * @brief unregister filesystem
 * 
 * @param fs 
 * @return int 
 */
int unregister_filesystem(struct vfs_file_system_type *fs);

/**
 * @brief find unused fd_slot
 * 
 * @param lowerlimit 
 * @return int 
 */
int find_unused_fd_slot(int lowerlimit);

/**
 * @brief vfs mount
 * 
 * @param d 
 * @return struct vfs_mount* 
 */
struct vfs_mount *lookup_mnt(struct vfs_dentry *d);

/**
 * @brief vfs init 
 * 
 * @param fs 
 * @param dev_name 
 */
void vfs_init(struct vfs_file_system_type *fs, char *dev_name);

/**
 * @brief vfs inode
 * 
 * @return struct vfs_inode* 
 */
struct vfs_inode *init_inode();

/**
 * @brief init special inode
 * 
 * @param inode 
 * @param mode 
 * @param dev 
 */
void init_special_inode(struct vfs_inode *inode, umode_t mode, dev_t dev);
struct vfs_mount *do_mount(const char *fstype, int flags, const char *name);

struct vfs_dentry *alloc_dentry(struct vfs_dentry *parent, char *name);
int32_t vfs_open(const char *path, int32_t flags, ...);
int32_t vfs_close(int32_t fd);
int vfs_stat(const char *path, struct kstat *stat);
int vfs_fstat(int32_t fd, struct kstat *stat);
int vfs_mknod(const char *path, int mode, dev_t dev);
int path_walk(struct nameidata *nd, const char *path, int32_t flags, mode_t mode);
int vfs_truncate(const char *path, int32_t length);
int vfs_ftruncate(int32_t fd, int32_t length);
struct vfs_file *get_empty_filp();
int generic_memory_readdir(struct vfs_file *file, struct dirent *dirent, unsigned int count);
int vfs_setattr(struct vfs_dentry *d, struct iattr *attrs);
int vfs_access(const char *path, int amode);

char *vfs_read(const char *path);
ssize_t vfs_fread(int32_t fd, char *buf, size_t count);
ssize_t vfs_fwrite(int32_t fd, const char *buf, size_t count);
loff_t generic_file_llseek(struct vfs_file *file, loff_t offset, int whence);
loff_t vfs_flseek(int32_t fd, loff_t offset, int whence);

int do_fcntl(int fd, int cmd, unsigned long arg);

void vfs_build_path_backward(struct vfs_dentry *dentry, char *path);
int vfs_unlink(const char *path, int flag);
int vfs_rename(const char *oldpath, const char *newpath);
int generic_memory_rename(struct vfs_inode *old_dir, struct vfs_dentry *old_dentry,
						  struct vfs_inode *new_dir, struct vfs_dentry *new_dentry);
int vfs_mkdir(const char *path, mode_t mode);