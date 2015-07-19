#include <limits>
#include <iconv.h>
#include "pf/base/base64.hpp"
#include "pf/base/string.h"

namespace pf_base {

namespace string {

/***
  * 快速字符串转换成整数模板通用函数
  * @str: 需要被转换的字符串
  * @result: 存储转换后的结果
  * @max_length: 该整数类型对应的字符串的最多字符个数，不包括结尾符
  * @converted_length: 需要转换的字符串长度，如果为0则表示转换整个字符串
  * @ignored_zero: 是否忽略开头的0
  * @return: 如果转换成功返回true, 否则返回false
  */
template <typename IntType>
static bool fast_toint(const char *str,
                       IntType &result, 
                       uint8_t max_length, 
                       uint8_t converted_length, 
                       bool ignored_zero) {
  __ENTER_FUNCTION
    bool negative = false;
    const char *tmp_str = str;
    if (NULL == str) return false;

    // 处理负数
    if ('-' == tmp_str[0]) {
      // 负数
      negative = true;
      ++tmp_str;
    }

    // 处理空字符串
    if ('\0' == tmp_str[0]) return false;

    // 处理0打头的
    if ('0' == tmp_str[0]) {
      // 如果是0开头，则只能有一位数字
      if (('\0' == tmp_str[1]) || (1 == converted_length)) {
        result = 0;
        return true;
      } else {
        if (!ignored_zero) return false;
        for (;;) {
          ++tmp_str;
          if (tmp_str - str > max_length-1) return false;
          if (*tmp_str != '0') break;
        }
        if ('\0' == *tmp_str) {
          result = 0;
          return true;
        }
      }
    }

    // 检查第一个字符
    if ((*tmp_str < '0') || (*tmp_str > '9')) return false;
    result = (*tmp_str - '0');

    while ((0 == converted_length) || (tmp_str - str < converted_length-1)) {
      ++tmp_str;
      if ('\0' == *tmp_str) break;
      if (tmp_str - str > max_length-1) return false;

      if ((*tmp_str < '0') || (*tmp_str > '9')) return false;

      result = result * 10;
      result += (*tmp_str - '0');
    }

    if (negative) result = -result;
    return true;
  __LEAVE_FUNCTION
    return false;
}

void replace_all(std::string& str, 
                 const std::string source, 
                 const std::string destination) {
  __ENTER_FUNCTION
    size_t position = str.find(source, 0);
    while (position != std::string::npos) {
      str.replace(position, source.length(), destination);
      position = str.find(source, position);
    }
  __LEAVE_FUNCTION
}

bool toint32(const char *source, 
             int32_t &result, 
             uint8_t converted_length, 
             bool ignored_zero) {
  __ENTER_FUNCTION
    if (NULL == source) return false;

    long value;
    if (!fast_toint<long>(
          source, 
          value, 
          sizeof("-2147483648") - 1, 
          converted_length, 
          ignored_zero)) 
      return false;
#undef max
#undef min
    int32_t _max = std::numeric_limits<int32_t>::max();
    int32_t _min = std::numeric_limits<int32_t>::min();
    if (value < _min || value > _max)  return false;
    result = static_cast<int32_t>(value);
    return true;
  __LEAVE_FUNCTION
    return false;
}

bool toint16(const char *source, 
             int16_t &result, 
             uint8_t converted_length, 
             bool ignored_zero) {
  __ENTER_FUNCTION
    int32_t value = 0;
    if (!toint32(source, value, converted_length, ignored_zero)) 
      return false;
    if (value < std::numeric_limits<int16_t>::min() ||
        value > std::numeric_limits<int16_t>::max()) return false;
    result = static_cast<int16_t>(value);
    return true;
  __LEAVE_FUNCTION
    return false;
}

char get_base64char(int index) {
  const char str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghi"
                     "jklmnopqrstuvwxyz0123456789+/";
  if ((index >= 0) && (index < 64)) {
    return str[index];
  }
  return '=';
}

void encrypt(const char *in, char *out, int32_t out_length) {
  __ENTER_FUNCTION
    int insize = static_cast<int>(strlen(in));
    if (insize <= 0) return;
    int middle = 0 == insize % 2 ? insize / 2 : (insize + 1) / 2;
    int length = insize + 2 + 3 + 1;
    char *src = new char[length];
    char *temp = new char[length + length / 3 + 10]; //enough output size
    int i, j, index;
    srand(static_cast<unsigned int>(time(NULL)));
    i = j = 0;
    for (; i < length; ++i) {
      index = rand() % 100;
      if (i < 2 || (middle <= i && middle + 3 > i) || i == length - 1) {
        src[i] = get_base64char(index);
        continue;
      }
      src[i] = in[j++];
    }
    base64encode(temp, src, length);
    strncpy(out, temp, out_length);
    out[out_length - 1] = '\0';
    SAFE_DELETE_ARRAY(temp);
    SAFE_DELETE_ARRAY(src);
  __LEAVE_FUNCTION
}

void decrypt(const char *in, char *out, int32_t out_length) {
  __ENTER_FUNCTION
    int insize = static_cast<int>(strlen(in));
    if (insize <= 0) return;
    char *temp = new char[insize - insize / 3 + 10]; // enough buffer size
    base64decode(temp, in, insize);
    int length = static_cast<int>(strlen(temp));
    int right_length = length - 2 - 3 - 1;
    char *_temp = new char[right_length + 1];
    int middle = //用正确的长度算出中间值
      0 == right_length % 2 ? right_length / 2 : (right_length + 1) / 2;
    int i, j;
    i = j = 0;
    for (; i < length; ++i) {
      if (i < 2 || (middle <= i && middle + 3 > i) || i == length - 1) {
        continue;
      }
      _temp[j++] = temp[i];
    }
    strncpy(out, _temp, out_length);
    out[out_length - 1] = '\0';
    SAFE_DELETE_ARRAY(_temp);
    SAFE_DELETE_ARRAY(temp);
  __LEAVE_FUNCTION
}

uint32_t crc(const char *str) {
  __ENTER_FUNCTION
    if (NULL == str|| 0 == str[0]) return 0;
    uint32_t crc32 = 0xFFFFFFFF;
    int32_t size = static_cast<int32_t>(strlen(str));
    uint16_t i;
    for (i = 0; i < size; ++i) {
      crc32 = crc32 * 33 + static_cast<unsigned char>(str[i]);
    }
    return crc32;
  __LEAVE_FUNCTION
    return 0;
}

char *safecopy(char *dest, const char *src, size_t size) {
  __ENTER_FUNCTION
    Assert(dest && src);
    strncpy(dest, src, size);
    dest[size - 1] = '\0';
    return dest;
  __LEAVE_FUNCTION
    return NULL;
}

/**
 * @desc this function can convert charset with diffrent
 * @param from source charset(example: utf8)
 * @param to destination charset
 * @param save want save string
 * @param savelen want save string length
 * @param src want convert string
 * @param srclen want convert string length
 */
int32_t charset_convert(const char *from, 
                        const char *to, 
                        char *save, 
                        int32_t save_length, 
                        const char *src, 
                        int32_t src_length) {
  __ENTER_FUNCTION
    int32_t status = 0;
    iconv_t cd;
    const char *inbuf  = src;
    char *outbuf       = save;
    size_t outbufsize  = save_length;
    size_t savesize    = 0;
    size_t inbufsize   = src_length;
    const char *inptr  = inbuf;
    size_t insize      = inbufsize;
    char *outptr       = outbuf;
    size_t outsize     = outbufsize;

    cd = iconv_open(to, from);
    iconv(cd, NULL, NULL, NULL, NULL);
    if (0 == inbufsize) {
      status = -1;
      goto done;
    }
    while (0 < insize) {
      size_t res = iconv(cd, (const char**)&inptr, &insize, &outptr, &outsize);
      if (outptr != outbuf) {
        char *_outbuf = new char[outsize + 1];
        memset(_outbuf, 0, outsize + 1);
        strncpy(_outbuf, outbuf, outsize);
        int32_t saved_errno = errno;
        int32_t outsize = static_cast<int32_t>(outptr - outbuf);
        strncpy(save + savesize, _outbuf, outsize);
        SAFE_DELETE_ARRAY(_outbuf);
        errno = saved_errno;
      }
      if ((size_t)(-1) == res) {
        if (EILSEQ == errno) {
          int one = 1 ;
          iconvctl(cd, ICONV_SET_DISCARD_ILSEQ, &one);
          status = -3;
        } else if (EINVAL == errno) {
          if (0 == inbufsize) {
            status = -4;
            goto done;
          } 
          else {
            break;
          }
        } else if (E2BIG == errno) {
          status = -5;
          goto done;
        } else {
          status = -6;
          goto done;
        }
      }
    }
    status = static_cast<int32_t>(strlen(save));
    done:
    iconv_close(cd);
    return status;
  __LEAVE_FUNCTION
    return -1;
}

bool get_escapechar(char in, char& out) {
  __ENTER_FUNCTION
    const char char_array[][2] = {
      {0, '0'},
      {'\n', 'n'},
      {'\r', 'r'},
      {0x1a, 'Z'},
      {'\"', '\"'},
      {'\'', '\''},
      {'\\', '\\'}
    };
    for (uint8_t i = 0; i < sizeof(char_array)/sizeof(char_array[0]); ++i) {
      if (char_array[i][0] == in) {
        out = char_array[i][1];
        return true;
      }
    }
    return false;
  __LEAVE_FUNCTION
    return false;
}

bool getescape(const char *in, size_t insize, char *out, size_t outsize) {
  __ENTER_FUNCTION
    size_t index1 = 0, index2 = 0;
    for ( ; index1 < insize && index2 < outsize; ++index1) {
      char _char;
      if (get_escapechar(in[index1], _char)) {
        out[index2] = '\\';
        ++index2;
        out[index2] = _char;
        ++index2;
      } else {
        out[index2] = in[index1];
        ++index2;
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

int64_t toint64(const char *str) {
  __ENTER_FUNCTION
    char *endpointer = NULL;
    int64_t result = strtoint64(str, &endpointer, 10);
    return result;
  __LEAVE_FUNCTION
    return -1;
}

uint64_t touint64(const char *str) {
  __ENTER_FUNCTION
    char *endpointer = NULL;
    uint64_t result = strtouint64(str, &endpointer, 10);
    return result;
  __LEAVE_FUNCTION
    return 0;
}

int32_t explode(const char *source,
                std::vector<std::string> &result,
                const char *key,
                bool one_key,
                bool ignore_empty) {
  __ENTER_FUNCTION
    result.clear();
    std::string str = source; //use stanard string class to source
    if (str.empty()) return 0;
    std::string::size_type left = 0;
    std::string::size_type right;
    if (one_key) {
      right = str.find_first_of(key);
    } else {
      right = str.find(key);
    }

    if (std::string::npos == right) right = str.length();
    for(;;) {
      std::string item = str.substr(left, right - left);

      if (item.length() > 0 || !ignore_empty) result.push_back(item);
      if (right == str.length()) break;
      left = right + (one_key ? 1 : strlen(key));
      if (one_key) {

        std::string temp = str.substr(left);
        right = temp.find_first_of(key);
        if (right != std::string::npos) right += left;
      } else {
        right = str.find(key, left);
      }
      if (std::string::npos == right) right = str.length();
    }
    int32_t _result = static_cast<int32_t>(result.size());
    return _result;
  __LEAVE_FUNCTION
    return 0;
}

bool checkstr(const char *in, uint32_t size) {
  __ENTER_FUNCTION
    if (0 == size) return false;
    for (uint32_t i = 0; i < size; ++i) {
      switch (in[i]) {
        case '\0':
          return true;
        case '\'':
          return false;
        case '\"':
          return false;
        case '(':
          return false;
        case ')':
          return false;
        case '=':
          return false;
        default:
          break;
      }
    }
    return true;
  __LEAVE_FUNCTION
    return false;
}

} //namespace string

} //namespace pf_base
