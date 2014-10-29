#include "pf/base/string.h"
#include "pf/base/util.h"
#include "pak/interface.h"
#if __LINUX__
#include <sys/stat.h>
#include <dirent.h>
#endif //__LINUX__
#include "main.h"

char g_basedir_parentpath[FILENAME_MAX] = {0};
int32_t g_addfilecount = 0;

void loopdir(const char *dirname, int depth, pak::archive_t *archive = NULL) {
  if (NULL == archive) return;
#if __LINUX__
  DIR *dp = NULL;
  struct dirent *entry = NULL;
  struct stat statbuf;
  if (NULL == (dp = opendir(dirname))) {
    ERRORPRINTF("can't open dir %s", dirname);
    return;
  }
  chdir(dirname);
  char curdir[FILENAME_MAX] = {0};
  getcwd(curdir, sizeof(curdir));
  char filename[FILENAME_MAX] = {0};
  std::string _filename = curdir;
  while ((entry = readdir(dp)) != NULL) {
    lstat(entry->d_name, &statbuf);
    if (S_ISDIR(statbuf.st_mode)) {
      if (0 == strcmp(".", entry->d_name) || 0 == strcmp("..", entry->d_name)) {
        continue;
      }
      loopdir(entry->d_name, depth + 1, archive);
    } else {
      pf_base::string::replace_all(_filename, g_basedir_parentpath, "");
      pf_base::string::safecopy(filename, _filename.c_str(), sizeof(filename));
      pf_base::util::path_towindows(filename, strlen(filename));
      char *filepath = filename;
      while ('\\' == filepath[0]) ++filepath;
      strncat(filename, "\\", sizeof(filename));
      strncat(filename, entry->d_name, sizeof(filename));
      ++g_addfilecount;
      bool result = pak::fileadd(
        archive, 
        entry->d_name, 
        filepath,
        PAK_FILE_ENCRYPTED | PAK_FILE_COMPRESS | PAK_FILE_REPLACEEXISTING, 
        0, 
        pak::kFileTypeData);
      if (!result)  {
        ERRORPRINTF("file: %s add failed", filepath);
      } else {
        DEBUGPRINTF("file: %s add success", filepath);
      }
    }
  }
  chdir("..");
  closedir(dp);
#endif
}

int32_t main(int32_t argc, char *argv[]) {
  uint64_t result = 0;
  if (argc < 4) return -1;
  if (strcmp(argv[1], "create") && strcmp(argv[1], "add")) return -1;
  pak::archive_t *archive = NULL;
  bool iscreate = 0 == strcmp(argv[1], "create");
  if (iscreate && argc < 5) return -1;
  if (iscreate) {
    uint64_t hashtable_size = pf_base::string::touint64(argv[3]);
    uint64_t paktype = PAK_TYPE_PATCH;
    if (0 == strcmp(argv[4], "normal")) paktype = PAK_TYPE_NORMAL;
    archive = pak::archivecreate(argv[2],
                                 result,
                                 hashtable_size,
                                 paktype);
  } else {
    archive = pak::archiveopen(argv[2], result);
#if __LINUX__  
    char current_dir[FILENAME_MAX] = {0};
    getcwd(current_dir, sizeof(current_dir));
    if (-1 == chdir(argv[3])) {
      ERRORPRINTF("path: %s not exits", argv[2]);
      return -1;
    }
    chdir("..");
    getcwd(g_basedir_parentpath, sizeof(g_basedir_parentpath));
    chdir(current_dir);
#endif  
  }
  if (!archive) return -1;
  if (!iscreate) {
    loopdir(argv[3], 0, archive);
    DEBUGPRINTF("add file count: %d", g_addfilecount);
  }
  pak::archiveclose(archive);
  return 0;
}
