#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

char g_basedir_parentpath[FILENAME_MAX] = {0};

void loopdir(const char *dirname, int depth) {
  DIR *dp = NULL;
  struct dirent *entry = NULL;
  struct stat statbuf;
  if (NULL == (dp = opendir(dirname))) {
    printf("can't open dir %s\n", dirname);
    return;
  }
  chdir(dirname);
  char curdir[FILENAME_MAX] = {0};
  getcwd(curdir, sizeof(curdir));
  while ((entry = readdir(dp)) != NULL) {
    lstat(entry->d_name, &statbuf);
    if (S_ISDIR(statbuf.st_mode)) {
      if (0 == strcmp(".", entry->d_name) || 0 == strcmp("..", entry->d_name)) {
        continue;
      }
      printf("%d, %s, %s\n", depth, "", entry->d_name);
      loopdir(entry->d_name, depth + 1);
    } else {
      printf("%d, %s, %s curdir: %s\n", depth, "", entry->d_name, curdir);
    }
  }
  chdir("..");
  closedir(dp);
}

int main(int argc, char *argv[]) {
  char current_dir[FILENAME_MAX] = {0};
  getcwd(current_dir, sizeof(current_dir));
  printf("argv[0]: %s\n", argv[0]);
  //loopdir("./", 0);
  return 0;
}
