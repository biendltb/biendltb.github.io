---
layout: single
title: "Notes and Tips for working in Linux environments"
date: 2021-01-02 12:00:00 +0700
categories: tech/randoms
classes:
  - dark-theme
tags:
  - linux
  - bash
published: false
---

## Useful Linux commands
#### Check sizes of folders in a directory

```bash
du -sh /home/* 2> /dev/null
```
  
#### Copy files in/out from a server

```bash
scp -P 2222 -r biendltb@dynim.ddns.net:/path/to/source/ /path/to/destination/
```

#### Copy a specific types of file and/or with specific pattern of name
* Case 1: depth level known
```bash
cp /src/**/gnss.csv --parents dest/
# Note: this will result in de directory /dest/src/abc/gnss.csv so make appropriate `cd` inadvance if necessary
```
* Case 2: copy files at any depth
```bash
find /path/to/files -name '*.csv' | cpio -pdm /target
# Note: it will find all of the file in /path/to/files and below and copy them to /target/path/to/files and below.
```

#### Safely move directories
```bash
# Copy from A to B by 'rsync', P--> showing progress
sudo rsync -aXSP /src_dir/ /dst_dir/

# check if the copy successfully
sudo diff -r /src_dir/ /dst_dir/

# delete the src_dir
rm -rf /src_dir/

```
  
#### Find some files
* Normal find

```bash
find /search/dir/ -name *libtiff*.so*
```

* Ignore error messages (use <code> grep -v </code> to select non-matching lines)

```bash 
find /search/dir/ -name *libtiff*.so* 2>&1 | grep -v "Permission denied"
```

#### Uncompress/compress .tar with tar
1. Gzip is a compression tool used to reduce the size of a file
2. Tar is an archiver used to to combine multiple files into one
3. Gzip and Tar are usually used in tandem to create Tarballs that are compressed significantly
4. Extracting an individual file can take a lot longer with zipped tarballs than with other formats

* `-x`: extract
* `-c`: Create an archive.
* `-z`: Compress the archive with gzip.
* `-v`: Display progress in the terminal while creating the archive, also known as “verbose” mode. The v is always optional in these commands, but it’s helpful.
* `-f`: Allows you to specify the filename of the archive.

Read more: Difference Between GZIP and TAR | Difference Between http://www.differencebetween.net/technology/difference-between-gzip-and-tar/#ixzz6JTqzTyxm
```bash
# uncompress file
tar -xvzf compressed_file.tar.gz -C /path/to/extraction/dir

# compress file
tar -cvzf arbitrary_name.tar.gz /to/be/compressed/dir/
```

#### Mount an external hard drive  
```bash
# Create a folder to mount the HDD to
>> mkdir ~/HDD

# Show all available drives to find the right HDD port (e.g. `/dev/sdb1`)
>> lsblk

# Mount
>> sudo mount /dev/sdb1 ~/HDD

# Do the work ...

# Unmount after use
>> sudo umount ~/HDD
```
