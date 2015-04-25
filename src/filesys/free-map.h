#ifndef FILESYS_FREE_MAP_H
#define FILESYS_FREE_MAP_H

#include <stdbool.h>
#include <stddef.h>
<<<<<<< HEAD:src/filesys/free-map.h
#include "devices/disk.h"
=======
#include "devices/block.h"
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/free-map.h

void free_map_init (void);
void free_map_read (void);
void free_map_create (void);
void free_map_open (void);
void free_map_close (void);

<<<<<<< HEAD:src/filesys/free-map.h
bool free_map_allocate (size_t, disk_sector_t *);
void free_map_release (disk_sector_t, size_t);
=======
bool free_map_allocate (size_t, block_sector_t *);
void free_map_release (block_sector_t, size_t);
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/free-map.h

#endif /* filesys/free-map.h */
