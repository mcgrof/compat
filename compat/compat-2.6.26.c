/*
 * Copyright 2007-2010	Luis R. Rodriguez <mcgrof@winlab.rutgers.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Compatibility file for Linux wireless for kernels 2.6.26.
 *
 * Copyright holders from ported work:
 *
 * Copyright (c) 2002-2003 Patrick Mochel <mochel@osdl.org>
 * Copyright (c) 2006-2007 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (c) 2006-2007 Novell Inc.
 */

#include <net/compat.h>

/* All things not in 2.6.25 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))


/* 2.6.24 does not have the struct kobject with a name */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25))

/**
 * kobject_set_name_vargs - Set the name of an kobject
 * @kobj: struct kobject to set the name of
 * @fmt: format string used to build the name
 * @vargs: vargs to format the string.
 */
static
int kobject_set_name_vargs(struct kobject *kobj, const char *fmt,
				  va_list vargs)
{
	const char *old_name = kobj->name;
	char *s;

	if (kobj->name && !fmt)
		return 0;

	kobj->name = kvasprintf(GFP_KERNEL, fmt, vargs);
	if (!kobj->name)
		return -ENOMEM;

	/* ewww... some of these buggers have '/' in the name ... */
	while ((s = strchr(kobj->name, '/')))
		s[0] = '!';

	kfree(old_name);
	return 0;
}
#else
static
int kobject_set_name_vargs(struct kobject *kobj, const char *fmt,
				  va_list vargs)
{
	struct device *dev;
	unsigned int len;
	va_list aq;

	dev = container_of(kobj, struct device, kobj);

	va_copy(aq, vargs);
	len = vsnprintf(NULL, 0, fmt, aq);
	va_end(aq);

	len = len < BUS_ID_SIZE ? (len + 1) : BUS_ID_SIZE;

	vsnprintf(dev->bus_id, len, fmt, vargs);
	return 0;
}
#endif

/**
 * dev_set_name - set a device name
 * @dev: device
 * @fmt: format string for the device's name
 */
int dev_set_name(struct device *dev, const char *fmt, ...)
{
	va_list vargs;
	int err;

	va_start(vargs, fmt);
	err = kobject_set_name_vargs(&dev->kobj, fmt, vargs);
	va_end(vargs);
	return err;
}
EXPORT_SYMBOL_GPL(dev_set_name);

#if 0
/*
 * Below was an attempt to backport debugfs_remove_recursive() but as
 * can be seen if enabled it cannot be backported since it relies on
 * two static variables from fs/debugfs/inode.c . If you need backport
 * this consider instead just disabling debugfs for drivers that use
 * this for kernels < 2.6.26
 */
static inline int debugfs_positive(struct dentry *dentry)
{
	return dentry->d_inode && !d_unhashed(dentry);
}

static void __debugfs_remove(struct dentry *dentry, struct dentry *parent)
{
	int ret = 0;

	if (debugfs_positive(dentry)) {
		if (dentry->d_inode) {
			dget(dentry);
			switch (dentry->d_inode->i_mode & S_IFMT) {
			case S_IFDIR:
				ret = simple_rmdir(parent->d_inode, dentry);
				break;
			case S_IFLNK:
				kfree(dentry->d_inode->i_private);
				/* fall through */
			default:
				simple_unlink(parent->d_inode, dentry);
				break;
			}
			if (!ret)
				d_delete(dentry);
			dput(dentry);
		}
	}
}

/**
 * debugfs_remove_recursive - recursively removes a directory
 * @dentry: a pointer to a the dentry of the directory to be removed.
 *
 * This function recursively removes a directory tree in debugfs that
 * was previously created with a call to another debugfs function
 * (like debugfs_create_file() or variants thereof.)
 *
 * This function is required to be called in order for the file to be
 * removed, no automatic cleanup of files will happen when a module is
 * removed, you are responsible here.
 */
void debugfs_remove_recursive(struct dentry *dentry)
{
	struct dentry *child;
	struct dentry *parent;

	if (!dentry)
		return;

	parent = dentry->d_parent;
	if (!parent || !parent->d_inode)
		return;

	parent = dentry;
	mutex_lock(&parent->d_inode->i_mutex);

	while (1) {
		/*
		 * When all dentries under "parent" has been removed,
		 * walk up the tree until we reach our starting point.
		 */
		if (list_empty(&parent->d_subdirs)) {
			mutex_unlock(&parent->d_inode->i_mutex);
			if (parent == dentry)
				break;
			parent = parent->d_parent;
			mutex_lock(&parent->d_inode->i_mutex);
		}
		child = list_entry(parent->d_subdirs.next, struct dentry,
				d_u.d_child);

		/*
		 * If "child" isn't empty, walk down the tree and
		 * remove all its descendants first.
		 */
		if (!list_empty(&child->d_subdirs)) {
			mutex_unlock(&parent->d_inode->i_mutex);
			parent = child;
			mutex_lock(&parent->d_inode->i_mutex);
			continue;
		}
		__debugfs_remove(child, parent);
		if (parent->d_subdirs.next == &child->d_u.d_child) {
			/*
			 * Avoid infinite loop if we fail to remove
			 * one dentry.
			 */
			mutex_unlock(&parent->d_inode->i_mutex);
			break;
		}
		simple_release_fs(&debugfs_mount, &debugfs_mount_count);
	}

	parent = dentry->d_parent;
	mutex_lock(&parent->d_inode->i_mutex);
	__debugfs_remove(dentry, parent);
	mutex_unlock(&parent->d_inode->i_mutex);
	simple_release_fs(&debugfs_mount, &debugfs_mount_count);
 }
EXPORT_SYMBOL_GPL(debugfs_remove_recursive);
#endif

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) */

