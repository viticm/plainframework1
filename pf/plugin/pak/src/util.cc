#include "pak/util.h"

int32_t g_pakerror;

namespace pak {

namespace util {

void set_lasterror(int32_t error) {
  g_pakerror = error;
}

int32_t get_lasterror() {
  return g_pakerror;
}

const char *get_last_errorstr(int32_t error) {
  const char *errorstr;
  switch (error) {
    case PAK_ERROR_INVALID_FUNCTION:
      errorstr = "function not implemented";
      break;
    case PAK_ERROR_FILE_NOT_FOUND:
      errorstr = "file not found";
      break;
    case PAK_ERROR_ACCESS_DENIED:
      errorstr = "access denied";
      break;
    case PAK_ERROR_NOT_ENOUGH_MEMORY:
      errorstr = "not enough memory";
      break;
    case PAK_ERROR_BAD_FORMAT:
      errorstr = "bad format";
      break;
    case PAK_ERROR_NO_MORE_FILES:
      errorstr = "no more files";
      break;
    case PAK_ERROR_HANDLE_EOF:
      errorstr = "access beyound EOF";
      break;
    case PAK_ERROR_HANDLE_DISK_FULL:
      errorstr = "no space left on device";
      break;
    case PAK_ERROR_INVALID_PARAMETER:
      errorstr = "invalid parameter";
      break;
    case PAK_ERROR_DISK_FULL:
      errorstr = "no space left on device";
      break;
    case PAK_ERROR_ALREADY_EXISTS:
      errorstr = "file exists";
      break;
    case PAK_ERROR_CAN_NOT_COMPLETE:
      errorstr = "operation cannot be completed";
      break;
    default:
      errorstr = "unkown error";
      break;
  }
  return errorstr;
}

void freearchive(archive_t *&archive) {
  __ENTER_FUNCTION
    
  __LEAVE_FUNCTION
}

bool isvalid_archivepointer(archive_t *archive);
void freefile(file_t *&file);
bool isvalid_filepointer(file_t *file);
void freeattributes(attribute_t *attributes);
int32_t save_listfile(archive_t *archive, bool internal);
int32_t save_listfile(archive_t *archive);
int32_t savetables(archive_t *archive);
uint64_t save_attributefile(archive_t *archive, bool internal);
uint64_t save_attributefile(archive_t *archive);
int32_t addfile_toarchive(archive_t *archive, 
                          void *fp, 
                          const char *archivedname, 
                          uint64_t flag, 
                          uint64_t quality, 
                          int32_t filetype, 
                          bool *replace);
int32_t addfile_toarchive(archive_t *archive, 
                          void *fp, 
                          const char *filename, 
                          uint64_t filelength,
                          const char *archivedname,
                          uint64_t flag, 
                          uint64_t quality, 
                          int32_t filetype, 
                          bool *replace);
int32_t internal_updatefile(archive_t *archive,
                            void *fp, 
                            file_t *file,
                            const char *archivedname,
                            uint64_t flag, 
                            uint64_t quality, 
                            int32_t filetype,
                            bool *replace);

} //namespace util

} //namespace pak

