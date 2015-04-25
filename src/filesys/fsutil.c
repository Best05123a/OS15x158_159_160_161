#include "filesys/fsutil.h"
#include <debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ustar.h>
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
<<<<<<< HEAD:src/filesys/fsutil.c
#include "devices/disk.h"
=======
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"

/* List files in the root directory. */
void
fsutil_ls (char **argv UNUSED) 
{
  struct dir *dir;
  char name[NAME_MAX + 1];
  
  printf ("Files in the root directory:\n");
  dir = dir_open_root ();
  if (dir == NULL)
    PANIC ("root dir open failed");
  while (dir_readdir (dir, name))
    printf ("%s\n", name);
  printf ("End of listing.\n");
}

/* Prints the contents of file ARGV[1] to the system console as
   hex and ASCII. */
void
fsutil_cat (char **argv)
{
  const char *file_name = argv[1];
  
  struct file *file;
  char *buffer;

  printf ("Printing '%s' to the console...\n", file_name);
  file = filesys_open (file_name);
  if (file == NULL)
    PANIC ("%s: open failed", file_name);
  buffer = palloc_get_page (PAL_ASSERT);
  for (;;) 
    {
      off_t pos = file_tell (file);
      off_t n = file_read (file, buffer, PGSIZE);
      if (n == 0)
        break;

      hex_dump (pos, buffer, n, true); 
    }
  palloc_free_page (buffer);
  file_close (file);
}

/* Deletes file ARGV[1]. */
void
fsutil_rm (char **argv) 
{
  const char *file_name = argv[1];
  
  printf ("Deleting '%s'...\n", file_name);
  if (!filesys_remove (file_name))
    PANIC ("%s: delete failed\n", file_name);
}

<<<<<<< HEAD:src/filesys/fsutil.c
/* Extracts a ustar-format tar archive from the scratch disk, hdc
   or hd1:0, into the Pintos file system. */
void
fsutil_extract (char **argv UNUSED) 
{
  static disk_sector_t sector = 0;

  struct disk *src;
  void *header, *data;

  /* Allocate buffers. */
  header = malloc (DISK_SECTOR_SIZE);
  data = malloc (DISK_SECTOR_SIZE);
  if (header == NULL || data == NULL)
    PANIC ("couldn't allocate buffers");

  /* Open source disk. */
  src = disk_get (1, 0);
  if (src == NULL)
    PANIC ("couldn't open scratch disk (hdc or hd1:0)");

  printf ("Extracting ustar archive from scratch disk into file system...\n");
=======
/* Extracts a ustar-format tar archive from the scratch block
   device into the Pintos file system. */
void
fsutil_extract (char **argv UNUSED) 
{
  static block_sector_t sector = 0;

  struct block *src;
  void *header, *data;

  /* Allocate buffers. */
  header = malloc (BLOCK_SECTOR_SIZE);
  data = malloc (BLOCK_SECTOR_SIZE);
  if (header == NULL || data == NULL)
    PANIC ("couldn't allocate buffers");

  /* Open source block device. */
  src = block_get_role (BLOCK_SCRATCH);
  if (src == NULL)
    PANIC ("couldn't open scratch device");

  printf ("Extracting ustar archive from scratch device "
          "into file system...\n");
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c

  for (;;)
    {
      const char *file_name;
      const char *error;
      enum ustar_type type;
      int size;

      /* Read and parse ustar header. */
<<<<<<< HEAD:src/filesys/fsutil.c
      disk_read (src, sector++, header);
=======
      block_read (src, sector++, header);
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c
      error = ustar_parse_header (header, &file_name, &type, &size);
      if (error != NULL)
        PANIC ("bad ustar header in sector %"PRDSNu" (%s)", sector - 1, error);

      if (type == USTAR_EOF)
        {
          /* End of archive. */
          break;
        }
      else if (type == USTAR_DIRECTORY)
        printf ("ignoring directory %s\n", file_name);
      else if (type == USTAR_REGULAR)
        {
          struct file *dst;

          printf ("Putting '%s' into the file system...\n", file_name);

          /* Create destination file. */
          if (!filesys_create (file_name, size))
            PANIC ("%s: create failed", file_name);
          dst = filesys_open (file_name);
          if (dst == NULL)
            PANIC ("%s: open failed", file_name);

          /* Do copy. */
          while (size > 0)
            {
<<<<<<< HEAD:src/filesys/fsutil.c
              int chunk_size = (size > DISK_SECTOR_SIZE
                                ? DISK_SECTOR_SIZE
                                : size);
              disk_read (src, sector++, data);
=======
              int chunk_size = (size > BLOCK_SECTOR_SIZE
                                ? BLOCK_SECTOR_SIZE
                                : size);
              block_read (src, sector++, data);
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c
              if (file_write (dst, data, chunk_size) != chunk_size)
                PANIC ("%s: write failed with %d bytes unwritten",
                       file_name, size);
              size -= chunk_size;
            }

          /* Finish up. */
          file_close (dst);
        }
    }

<<<<<<< HEAD:src/filesys/fsutil.c
  /* Erase the ustar header from the start of the disk, so that
     the extraction operation is idempotent.  We erase two blocks
     because two blocks of zeros are the ustar end-of-archive
     marker. */
  printf ("Erasing ustar archive...\n");
  memset (header, 0, DISK_SECTOR_SIZE);
  disk_write (src, 0, header);
  disk_write (src, 1, header);
=======
  /* Erase the ustar header from the start of the block device,
     so that the extraction operation is idempotent.  We erase
     two blocks because two blocks of zeros are the ustar
     end-of-archive marker. */
  printf ("Erasing ustar archive...\n");
  memset (header, 0, BLOCK_SECTOR_SIZE);
  block_write (src, 0, header);
  block_write (src, 1, header);
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c

  free (data);
  free (header);
}

/* Copies file FILE_NAME from the file system to the scratch
<<<<<<< HEAD:src/filesys/fsutil.c
   disk, in ustar format.

   The first call to this function will write starting at the
   beginning of the scratch disk.  Later calls advance across the
   disk.  This position is independent of that used for
=======
   device, in ustar format.

   The first call to this function will write starting at the
   beginning of the scratch device.  Later calls advance across
   the device.  This position is independent of that used for
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c
   fsutil_extract(), so `extract' should precede all
   `append's. */
void
fsutil_append (char **argv)
{
<<<<<<< HEAD:src/filesys/fsutil.c
  static disk_sector_t sector = 0;
=======
  static block_sector_t sector = 0;
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c

  const char *file_name = argv[1];
  void *buffer;
  struct file *src;
<<<<<<< HEAD:src/filesys/fsutil.c
  struct disk *dst;
  off_t size;

  printf ("Appending '%s' to ustar archive on scratch disk...\n", file_name);

  /* Allocate buffer. */
  buffer = malloc (DISK_SECTOR_SIZE);
=======
  struct block *dst;
  off_t size;

  printf ("Appending '%s' to ustar archive on scratch device...\n", file_name);

  /* Allocate buffer. */
  buffer = malloc (BLOCK_SECTOR_SIZE);
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c
  if (buffer == NULL)
    PANIC ("couldn't allocate buffer");

  /* Open source file. */
  src = filesys_open (file_name);
  if (src == NULL)
    PANIC ("%s: open failed", file_name);
  size = file_length (src);

<<<<<<< HEAD:src/filesys/fsutil.c
  /* Open target disk. */
  dst = disk_get (1, 0);
  if (dst == NULL)
    PANIC ("couldn't open target disk (hdc or hd1:0)");
=======
  /* Open target block device. */
  dst = block_get_role (BLOCK_SCRATCH);
  if (dst == NULL)
    PANIC ("couldn't open scratch device");
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c
  
  /* Write ustar header to first sector. */
  if (!ustar_make_header (file_name, USTAR_REGULAR, size, buffer))
    PANIC ("%s: name too long for ustar format", file_name);
<<<<<<< HEAD:src/filesys/fsutil.c
  disk_write (dst, sector++, buffer);
=======
  block_write (dst, sector++, buffer);
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c

  /* Do copy. */
  while (size > 0) 
    {
<<<<<<< HEAD:src/filesys/fsutil.c
      int chunk_size = size > DISK_SECTOR_SIZE ? DISK_SECTOR_SIZE : size;
      if (sector >= disk_size (dst))
        PANIC ("%s: out of space on scratch disk", file_name);
      if (file_read (src, buffer, chunk_size) != chunk_size)
        PANIC ("%s: read failed with %"PROTd" bytes unread", file_name, size);
      memset (buffer + chunk_size, 0, DISK_SECTOR_SIZE - chunk_size);
      disk_write (dst, sector++, buffer);
=======
      int chunk_size = size > BLOCK_SECTOR_SIZE ? BLOCK_SECTOR_SIZE : size;
      if (sector >= block_size (dst))
        PANIC ("%s: out of space on scratch device", file_name);
      if (file_read (src, buffer, chunk_size) != chunk_size)
        PANIC ("%s: read failed with %"PROTd" bytes unread", file_name, size);
      memset (buffer + chunk_size, 0, BLOCK_SECTOR_SIZE - chunk_size);
      block_write (dst, sector++, buffer);
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c
      size -= chunk_size;
    }

  /* Write ustar end-of-archive marker, which is two consecutive
     sectors full of zeros.  Don't advance our position past
     them, though, in case we have more files to append. */
<<<<<<< HEAD:src/filesys/fsutil.c
  memset (buffer, 0, DISK_SECTOR_SIZE);
  disk_write (dst, sector, buffer);
  disk_write (dst, sector, buffer + 1);
=======
  memset (buffer, 0, BLOCK_SECTOR_SIZE);
  block_write (dst, sector, buffer);
  block_write (dst, sector, buffer + 1);
>>>>>>> fbc51e42e63d1e953a126d05260e7d06f75ecc2a:src/filesys/fsutil.c

  /* Finish up. */
  file_close (src);
  free (buffer);
}
