# Miaou FileSystem

## Header

The disk is sliced in chunks
A chunk is 4kb.

A section is a set of 0x8000 chunks.
The last chunk of a section contains a bitmap of the previous 0x7fff chunks.

Chunks 0-9 (including the VBR) are reserved. Chunk 10 contains the header.

| name           | size |  offset | description                   |
|----------------|------|---------|-------------------------------|
| magic          | 0x8  | 0x00    | Must be "MeowMeow"            |
| version        | 0x4  | 0x08    | M.m.pp                        |
| z0             | 0x4  | 0x0c    | Must be zero                  |
| size           | 0x8  | 0x10    | The size of the disk in bytes |
| root           | 0x8  | 0x18    | The root inode                |

## Inode data

| name           | size |  offset | description                   |
|----------------|------|---------|-------------------------------|
| mode           | 0x4  | 0x0     | The UNIX mode                 |
| uid            | 0x4  | 0x4     | The UID                       |
| gid            | 0x4  | 0x8     | The GID                       |
| link           | 0x4  | 0xc     | The link count                |
| btime          | 0x8  | 0x10    | The birth time                |
| mtime          | 0x8  | 0x18    | The modify time               |
| ctime          | 0x8  | 0x20    | The change time               |
| atime          | 0x8  | 0x28    | The access time               |
| acl            | 0x8  | 0x30    | The ACL pointer               |
| size           | 0x8  | 0x38    | The file size in bytes        |
| data[12]       | 0x8  | 0x40    | The direct pointers           |
| idata          | 0x8  | 0x100   | The indirect pointer          |
| idata2         | 0x8  | 0x108   | The second indirect pointer   |
| idata3         | 0x8  | 0x110   | The third indirect pointer    |
| idata4         | 0x8  | 0x118   | Because we can                |
| rdata          | 0xee0| 0x120   | Raw inode data (unused)       |

## Directories

Each entry is

* Two bytes for length
* The UTF8 name
* A 64 bit inode number
