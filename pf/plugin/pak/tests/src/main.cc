#include "pak/interface.h"
#include "pak/file.h"
#include "pak/util.h"
#include "main.h"

int32_t main(int32_t argc, char * argv[]) {
  uint64_t result = 0;
  /**
  pak::archive_t *archive = pak::archivecreate("test.pak", 
                                               result, 
                                               0x10000, 
                                               PAK_TYPE_PATCH);
  if (NULL == archive) return -1;
  pak::fileadd(archive, 
               "files\\filelist.txt", 
               "files\\filelist.txt",
               PAK_FILE_ENCRYPTED | PAK_FILE_COMPRESS, 
               0, 
               pak::kFileTypeData);
  pak::fileadd(archive, 
               "files\\filelist.txt", 
               "files\\filelist.txt",
               PAK_FILE_ENCRYPTED | PAK_FILE_COMPRESS | PAK_FILE_REPLACEEXISTING, 
               0, 
               pak::kFileTypeData);
  ERRORPRINTF("pak::fileadd end 1.file");
  pak::fileadd(archive, 
               "files\\global.txt", 
               "2.file",
               PAK_FILE_ENCRYPTED | PAK_FILE_COMPRESS, 
               0, 
               pak::kFileTypeData);
  ERRORPRINTF("pak::fileadd end 2.file");
  pak::archive_t *clonearchive = archive->createclone(result);
  pak::file_t *file = pak::fileopen(clonearchive, "files\\filelist.txt", result);
  DEBUGPRINTF("result: %d", result);
  if (!file) return -1;
  char *buffer = new char[1024 * 1024];
  if (!buffer) return -1;
  memset(buffer, 0, 1024 * 1024);
  uint64_t readed, size;
  size = pak::filesize(file);
  ERRORPRINTF("size: %d", size);
  pak::fileread(file, buffer, size, &readed);
  DEBUGPRINTF("buffer: %s, readed: %d", buffer, readed);
  pak::fileeof(file);
  pak::fileclose(file);
  archive->destoryclone(clonearchive);
  pak::archiveclose(archive);
  DEBUGPRINTF("1 archiveclose");
  **/
  uint64_t size, readed;
  char *buffer = new char[1024 * 1024];
  if (!buffer) return -1;
  memset(buffer, 0, 1024 * 1024);
  pak::archive_t *archive = pak::archiveopen("test.pak", result, true);
  DEBUGPRINTF("result: %d", result);
  if (!archive) return -1;
  DEBUGPRINTF("archive not NULL");
  pak::file_t *file = pak::fileopen(archive, "files\\filelist.txt", result);
  if (!file) return -1;
  size = pak::filesize(file);
  pak::fileread(file, buffer, size, &readed);
  DEBUGPRINTF("1 buffer: %s, readed: %d", buffer, readed);
  pak::fileeof(file);
  pak::fileclose(file);
  pak::fileadd(archive,
               "files\\task.txt",
               "2.file",
               PAK_FILE_ENCRYPTED | PAK_FILE_COMPRESS | PAK_FILE_REPLACEEXISTING,
               0,
               pak::kFileTypeData);
  file = pak::fileopen(archive, "2.file", result);
  if (!file) return -1;
  size = pak::filesize(file);
  pak::fileread(file, buffer, size, &readed);
  DEBUGPRINTF("1 buffer: %s, readed: %d", buffer, readed);
  pak::fileeof(file);
  pak::fileclose(file);
  pak::archiveclose(archive);
  archive = pak::archiveopen("test.pak", result, true);
  DEBUGPRINTF("result: %d", result);
  if (!archive) return -1;
  DEBUGPRINTF("archive not NULL");
  file = pak::fileopen(archive, "2.file", result);
  if (!file) return -1;
  size = pak::filesize(file);
  pak::fileread(file, buffer, size, &readed);
  DEBUGPRINTF("1 buffer: %s, readed: %d", buffer, readed);
  pak::fileclose(file);
  pak::fileeof(file);
  pak::archiveclose(archive);
#if __WINDOWS__
  system("pause");
#endif
  return 0;
}
