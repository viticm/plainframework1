#include "pak/compress.h"

#include "pklib/pklib.h"

#ifndef __SYS_ZLIB
#include "zlib/zlib.h"
#else
#include <zlib.h>
#endif

#include "huffman/huff.h"
#include "wave/wave.h"

#ifndef __SYS_BZLIB
#include "bzip2/bzlib.h"
#else
#include <bzlib.h>
#endif

#include "pak/util.h"

namespace pak {

namespace compress {

typedef struct datainfo_struct {
  const char *in;
  int32_t inposition;
  int32_t insize;
  char *out;
  int32_t outposition;
  int32_t outsize;
} datainfo_t;

typedef int32_t (*compress_function)(
    char *, int32_t *, char *, int32_t, int32_t *, int32_t);
typedef struct compress_table_struct {
  uint64_t mask;
  compress_function compress;
} compress_table_t;

typedef int32_t (*decompress_function)(char *, int32_t *, char *, int32_t);
typedef struct decompress_table_struct {
  uint64_t mask;
  decompress_function decompress;
} decompress_table_t;

static uint32_t read_inputdata(char *buffer, uint32_t *size, void *param) {
  __ENTER_FUNCTION
    datainfo_t *datainfo = reinterpret_cast<datainfo_t *>(param);
    uint32_t availmax = datainfo->insize - datainfo->inposition;
    uint32_t toread = *size;
    if (toread > availmax) toread = availmax;
    memcpy(buffer, datainfo->in + datainfo->inposition, toread);
    return toread;
  __LEAVE_FUNCTION
    return 0;
}

static void write_outputdata(char *buffer, uint32_t *size, void *param) {
  __ENTER_FUNCTION
    datainfo_t *datainfo = reinterpret_cast<datainfo_t *>(param);
    uint32_t writemax = datainfo->outsize - datainfo->outposition;
    uint32_t towrite = *size;
    if (towrite > writemax) towrite = writemax;
    memcpy(datainfo->out + datainfo->outposition, buffer, towrite);
    datainfo->outposition += towrite;
  __LEAVE_FUNCTION
}

int32_t pklib(char *out, 
              int32_t *outsize, 
              char *in, 
              int32_t insize, 
              int32_t *type, 
              int32_t level) {
  __ENTER_FUNCTION
    USE_PARAM(level);
    datainfo_t datainfo;
    char *workbuffer = 
      reinterpret_cast<char *>(malloc(sizeof(char) * CMP_BUFFER_SIZE));
    Assert(workbuffer);
    memset(workbuffer, 0, CMP_BUFFER_SIZE);
    uint32_t dictionarysize;
    uint32_t _type;
    datainfo.in = in;
    datainfo.insize = insize;
    datainfo.inposition = 0;
    datainfo.out = out;
    datainfo.outsize = *outsize;
    datainfo.outposition = 0;
    _type = 2 == *type ? CMP_ASCII : CMP_BINARY;
    if (insize < 0x600) {
      dictionarysize = 0x400;
    } else if (insize >= 0x600 && insize < 0xC00) {
      dictionarysize = 0x800;
    } else {
      dictionarysize = 0x1000;
    }
    //do compression
    implode(read_inputdata, 
            write_outputdata, 
            workbuffer, 
            &datainfo, 
            &_type, 
            &dictionarysize);
    *outsize = datainfo.outposition;
    SAFE_FREE(workbuffer);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_pklib(char *out, int32_t *outsize, char *in, int32_t insize) {
  __ENTER_FUNCTION
    datainfo_t datainfo;
    char *workbuffer = 
      reinterpret_cast<char *>(malloc(sizeof(char) * CMP_BUFFER_SIZE));
    Assert(workbuffer);
    memset(workbuffer, 0, CMP_BUFFER_SIZE);
    //uint32_t dictionarysize;
    datainfo.in = in;
    datainfo.insize = insize;
    datainfo.inposition = 0;
    datainfo.out = out;
    datainfo.outsize = *outsize;
    datainfo.outposition = 0;
    //do the decompression
    explode(read_inputdata, write_outputdata, workbuffer, &datainfo);
    if (0 == datainfo.outposition) {
      datainfo.outposition = min(*outsize, insize);
      memcpy(out, in, datainfo.outposition); 
    }
    *outsize = datainfo.outposition;
    SAFE_FREE(workbuffer);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t wave_mono(char *out, 
                  int32_t *outsize, 
                  char *in, 
                  int32_t insize, 
                  int32_t *type, 
                  int32_t level) {
  __ENTER_FUNCTION
    if (level > 0 && level <= 2) {
      level = 4;
      *type = 6;
    } else if (3 == level) {
      level = 6;
      *type = 8;
    } else {
      level = 5;
      *type = 7;
    }
    *outsize = CompressWave(reinterpret_cast<unsigned char *>(out),
                            *outsize,
                            (short *)in,
                            insize,
                            1,
                            level);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_wave_mono(char *out, 
                     int32_t *outsize, 
                     char *in, 
                     int32_t insize) {
  __ENTER_FUNCTION
    DecompressWave((unsigned char *)out, 
                   *outsize, 
                   (unsigned char *)in, 
                   insize, 
                   1);
    return 1;
  __LEAVE_FUNCTION
    return -1;
}

int32_t wave_stereo(char *out, 
                    int32_t *outsize, 
                    char *in, 
                    int32_t insize, 
                    int32_t *type, 
                    int32_t level) {
  __ENTER_FUNCTION
    if (level > 0 && level <= 2) {
      level = 4;
      *type = 6;
    } else if (3 == level) {
      level = 6;
      *type = 8;
    } else {
      level = 5;
      *type = 7;
    }
    *outsize = CompressWave(reinterpret_cast<unsigned char *>(out),
                            *outsize,
                            (short *)in,
                            insize,
                            2,
                            level);    
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_wave_stereo(char *out, 
                       int32_t *outsize, 
                       char *in, 
                       int32_t insize) {
  __ENTER_FUNCTION
    DecompressWave((unsigned char *)out, 
                   *outsize, 
                   (unsigned char *)in, 
                   insize, 
                   2);
    return 1;
  __LEAVE_FUNCTION
    return -1;
}

int32_t huff(char *out, 
             int32_t *outsize, 
             char *in, 
             int32_t insize, 
             int32_t *type, 
             int32_t level) {
  __ENTER_FUNCTION
    USE_PARAM(level);
    THuffmannTree huffmann_tree;
    TOutputStream outputstream;
    //Initialize output stream
    outputstream.pbOutBuffer = (unsigned char *)out;
    outputstream.dwOutSize = *outsize;
    outputstream.pbOutPos = (unsigned char *)out;
    outputstream.dwBitBuff = 0;
    outputstream.nBits = 0;
    huffmann_tree.InitTree(true);
    *outsize = huffmann_tree.DoCompression(
        &outputstream, (unsigned char *)in, insize, *type);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_huff(char *out, int32_t *outsize, char *in, int32_t insize) {
  __ENTER_FUNCTION
    USE_PARAM(insize);
    THuffmannTree huffmann_tree;
    TInputStream inputstream;
    //why not use uint64_t ? open source is unsigned long
    inputstream.dwBitBuff = (*(unsigned long *)in); 
    in += sizeof(unsigned long);
    inputstream.pbInBuffer = (unsigned char *)in;
    inputstream.nBits = 32;
    huffmann_tree.InitTree(false);
    *outsize = huffmann_tree.DoDecompression(
        (unsigned char *)out, *outsize, &inputstream);
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t zlib(char *out, 
             int32_t *outsize, 
             char *in, 
             int32_t insize, 
             int32_t *type, 
             int32_t level) {
  __ENTER_FUNCTION
    USE_PARAM(type);
    USE_PARAM(level);
    z_stream z;
    int32_t result = 0;
    z.next_in = (Bytef *)in;
    z.avail_in = static_cast<uInt>(insize);
    z.total_in = insize;
    z.next_out = (Bytef *)out;
    z.avail_out = *outsize; //contorl the outsize, 
                            //if the size more than max size, 
                            //then outsize will changed to max size
    z.total_out = 0;
    z.zalloc = NULL;
    z.zfree = NULL;
    //Initialize the compression structure. Storm.dll uses zlib version 1.1.3
    if (0 == (result = deflateInit(&z, Z_DEFAULT_COMPRESSION))) {
      // Call zlib to compress the data
      result = deflate(&z, Z_FINISH);
      if (Z_OK == result || Z_STREAM_END == result)
        *outsize = z.total_out;
      deflateEnd(&z);
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_zlib(char *out, int32_t *outsize, char *in, int32_t insize) {
  __ENTER_FUNCTION
    int32_t result = 0;
    z_stream z;
    //Fill the stream structure for zlib
    z.next_in = (Bytef *)in;
    z.avail_in = (uInt)insize;
    z.total_in = insize;
    z.next_out = (Bytef *)out;
    z.avail_out = *outsize;
    z.total_out = 0;
    z.zalloc = NULL;
    z.zfree = NULL;
    if (0 == (result = inflateInit(&z))) {
      result = inflate(&z, Z_FINISH);
      *outsize = z.total_out;
      inflateEnd(&z);
    }
    return result;
  __LEAVE_FUNCTION
    return -1;
}

int32_t bzip2(char *out, 
              int32_t *outsize, 
              char *in, 
              int32_t insize, 
              int32_t *type, 
              int32_t level) {
  __ENTER_FUNCTION
    USE_PARAM(level);
    bz_stream stream;
    int32_t blocksize_100k;
    int32_t workfactor = 30;
    stream.bzalloc = NULL;
    stream.bzfree = NULL;
    //Adjust the block size
    blocksize_100k = *type;
    if (blocksize_100k < 1 || blocksize_100k > 9) {
      blocksize_100k = 9;
    }
    if (0 == BZ2_bzCompressInit(&stream, blocksize_100k, 0, workfactor)) {
      stream.next_in = in;
      stream.avail_in = insize;
      stream.next_out = out;
      stream.avail_out = *outsize;
      while (BZ2_bzCompress(
              &stream, 
              (stream.avail_in != 0) ? BZ_RUN : BZ_FINISH) != BZ_STREAM_END);
      BZ2_bzCompressEnd(&stream);
      *outsize = stream.total_out_lo32;
    } else {
      *outsize = 0;
    }
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

int32_t de_bzip2(char *out, int32_t *outsize, char *in, int32_t insize) {
  __ENTER_FUNCTION
    bz_stream stream;
    //Initialize the BZLIB decompression
    stream.bzalloc = NULL;
    stream.bzfree = NULL;
    if (0 == BZ2_bzDecompressInit(&stream, 0, 0)) {
      stream.next_in = in;
      stream.avail_in = insize;
      stream.next_out = out;
      stream.avail_out = *outsize;
      //perform the decompression
      while (BZ2_bzDecompress(&stream) != BZ_STREAM_END);
      //Put the stream into idle state
      BZ2_bzDecompressEnd(&stream);
      *outsize = stream.total_out_lo32;
    } else {
      outsize = 0;
    }
    return 0;
  __LEAVE_FUNCTION
    return -1;
}

static compress_table_t compress_table[] = {
  {PAK_COMPRESSION_WAVE_MONO, wave_mono},
  {PAK_COMPRESSION_WAVE_STEREO, wave_stereo},
  {PAK_COMPRESSION_HUFFMANN, huff},
  {PAK_COMPRESSION_ZLIB, zlib},
  {PAK_COMPRESSION_PKWARE, pklib},
  {PAK_COMPRESSION_BZIP2, bzip2}
};

int32_t smart(char *out, 
              int32_t *outsize, 
              char *in, 
              int32_t insize, 
              int32_t compressions,
              int32_t type, 
              int32_t level) {
  __ENTER_FUNCTION
    char *tempbuffer = NULL;
    char *output = out;
    char *input = NULL;
    int32_t compressions2;
    int32_t compresscount = 0;
    int32_t downcount = 0;
    int32_t _outsize = 0;
    int32_t _insize = insize;
    int32_t entries = (sizeof(compress_table) / sizeof(compress_table_t));
    int32_t result = 1;
    int32_t i;
    if (!outsize || *outsize < insize || !out || !in) {
      util::set_lasterror(PAK_ERROR_INVALID_PARAMETER);
      return 0;
    }
    for (i = 0, compressions2 = compressions; i < entries; ++i) {
      if (compressions & compress_table[i].mask) ++compresscount;
      compressions2 &= ~compress_table[i].mask;
    }
    if (compressions2 != 0) {
      util::set_lasterror(PAK_ERROR_INVALID_PARAMETER);
      return 0;
    }
    //If more that one compression, allocate intermediate buffer 
    if (compresscount >= 2) {
      tempbuffer = (char *)malloc(sizeof(char) * (*outsize + 1));
      Assert(tempbuffer);
      if (tempbuffer) {
        memset(tempbuffer, 0, *outsize + 1);
      } else {
        return 0;
      }
    }
    //Perform the compressions
    input = in;
    _insize = insize;
    for (i = 0, compressions2 = compressions; i < entries; ++i) {
      if (compressions2 & compress_table[i].mask) {
        --compresscount;
        output = (compresscount & 1) ? tempbuffer : out;
        _outsize = *outsize - 1;
        compress_table[i].compress(output + 1, 
                                   &_outsize, 
                                   input, 
                                   _insize, 
                                   &type, 
                                   level);
        if (0 == _outsize) {
          util::set_lasterror(PAK_ERROR_GEN_FAILURE);
          *outsize = 0;
          result = 0;
          break;
        }
        //If the compression failed, copy the block instead
        if (_outsize >= _insize - 1) {
          if (downcount > 0) ++output;
          memcpy(output, input, _insize);
          input = output;
          compressions &= ~compress_table[i].mask;
          _outsize = _insize;
        } else {
          input = output + 1;
          _insize = _outsize;
          ++downcount;
        }
      }
    }
    //Copy the compressed data to the correct output buffer
    if (result != 0) {
      if (compressions && (_insize + 1) < *outsize) {
        if (output != out  && output != output + 1)
          memcpy(out, output, _insize);
        *out = static_cast<char>(compressions);
        *outsize = _insize + 1;
      } else {
        memmove(out, in, _insize);
        *outsize = _insize;
      }
    }
    if (tempbuffer != NULL) SAFE_FREE(tempbuffer);
    return result;
  __LEAVE_FUNCTION
    return -1;
}

static decompress_table_t decompress_table[] = {
  {PAK_COMPRESSION_BZIP2, de_bzip2},
  {PAK_COMPRESSION_PKWARE, de_pklib},
  {PAK_COMPRESSION_ZLIB, de_zlib},
  {PAK_COMPRESSION_HUFFMANN, de_huff},
  {PAK_COMPRESSION_WAVE_STEREO, de_wave_stereo},
  {PAK_COMPRESSION_WAVE_MONO, de_wave_mono},
};

int32_t de_smart(char *out, 
                 int32_t *outsize, 
                 char *in, 
                 int32_t insize) {
  __ENTER_FUNCTION
    char *tempbuffer = NULL;
    char *workbuffer = NULL;
    int32_t _outsize = *outsize;
    uint8_t decompressions1 = 0;
    uint8_t decompressions2 = 0;
    int32_t count = 0;
    int32_t entries = (sizeof(decompress_table) / sizeof(decompress_table_t));
    int32_t result = 1;
    int32_t i;
    //If the input length is the same as output, do nothing.
    if (insize == _outsize) {
      if (in == out) return 1;
      memcpy(out, in, insize);
      *outsize = insize;
      return 1;
    }
    decompressions1 = decompressions2 = (uint8_t)*in++;
    --insize;
    for (i = 0; i < entries; ++i) {
      if (decompressions1 & decompress_table[i].mask) ++count;
      decompressions2 &= ~decompress_table[i].mask;
    }
    if (decompressions2 != 0) {
      util::set_lasterror(PAK_ERROR_INVALID_PARAMETER);
      return 0;
    }
    if (count >= 2) {
      tempbuffer = reinterpret_cast<char *>(malloc(sizeof(char) * _outsize));
      Assert(tempbuffer);
      memset(tempbuffer, 0, _outsize);
    }
    //Apply all decompressions
    for (i = 0, count = 0; i < entries; ++i) {
      if (decompressions1 & decompress_table[i].mask) {
        //If odd case, use target buffer for output, otherwise use allocated tempbuffer
        workbuffer = (count++ & 1) ? tempbuffer : out;
        _outsize = *outsize;
        //Decompress buffer using corresponding function
        decompress_table[i].decompress(workbuffer, &_outsize, in, insize);
        if (0 == _outsize) {
          util::set_lasterror(PAK_ERROR_GEN_FAILURE);
          result = 0;
          break;
        }
        //Move output length to src length for next compression
        insize = _outsize;
        in = workbuffer;
      }
    }
    //If output buffer is not the same like target buffer, we have to copy data 
    if (result != 0) {
      if (workbuffer != out) memcpy(out, in, _outsize);
    }
    //Delete temporary buffer, if necessary
    SAFE_FREE(tempbuffer);
    *outsize = _outsize;
    return result;
  __LEAVE_FUNCTION
    return -1;
}

} //namespace compress

} //namespace pak
