/*
 * Copyright 2007 Mintec, Inc.
 * All Rights Reserved.
 *
 * Minesight: ci.h
 *
 * Description: 
 *    header for the libmsci utility funcitons
 */
#ifndef CI_HEADER
#define CI_HEADER

#ifdef CILIBS_AS_STATIC
#define LIBMSCI_API
#else
#ifdef  LIBMSCI_EXPORTS
#define LIBMSCI_API __declspec(dllexport)
#else
#ifdef  LIBMSCI8_EXPORTS
#define LIBMSCI_API __declspec(dllexport)
#else
#define LIBMSCI_API __declspec(dllimport)
#endif
#endif
#endif

#include <msextern_c.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <io.h>
#include <float.h>
#include <fcntl.h>
#include <stdarg.h>
#include <math.h>
#include <windows.h>

#include <libcstl.h>

msBEGIN_EXTERN_C

#define chrDOT         '.'
#define chrNULLT       '\0'
#define chrSLASH       "\\"

#define cigStrIs3D(s)       ((s) && strncmp((s),  chr3D, strlen( chr3D)) == 0)
#define cigStrIs2D(s)       ((s) && strncmp((s), chrPEQ, strlen(chrPEQ)) == 0)
#define cigSTREQ(a,b)       ((a) && (b) ? strcmp((a), (b))==0 : !(a) && !(b))

#define BYTESPERWORD   4
#define SIXBITBLANK    17043521
#define nullT          '\0'
#define ERROR_LEN      1024
#define RADIANS        0.01745329251994329577
#define PI             3.14159265358974
#define DEGREES        57.29577951308232088
#define BYTESPERWORD   4
#define BYTE           unsigned char
#define FLAG           int
#define TRUE           1
#define FALSE          0

#define ciOPEN_READ_ONLY          "rb"
#define ciOPEN_READ_WRITE         "rb+"
#define ciMAX_STRING              256

typedef enum {
   ci64OPEN_READ,
   ci64OPEN_WRITE
} ci64;

#define ciFILE        FILE
#define CHRSTR(w)     ciGetCharStringFromSstring((w))  
#define TF(v)         ((v) ? "TRUE" : "FALSE")

#define ciDict        Sdict
#define ciDictIter    SdictIter
#define ciDictLoad    sdictLoad

static char ciErrorStr[ERROR_LEN] = {'\0'};

/* yes these two unions could be combined into the same one.

   I made them different so at no time would anyone be confused as
   to their content.

   1) equivWord is only used internally to retrieve a word from
      a file. In general, the word contains multiple packed values.

   2) EQUIV is used by the application.  It contains exactly one
      unpacked value.
*/

typedef union {   /* 1) used to decode packed word */
   int     i;
   float   f;
   char    c[4];
} equivWord;

typedef union {   /* 2) used to store one unpacked item value */
   int          i;
   float        f;
   const Sname *n;
} EQUIV;

/* MISCELLANEOUS UTILITIES */


LIBMSCI_API ciFILE      *ciOpenFile(char *path, char *mode);
LIBMSCI_API int          ciCloseFile(ciFILE *fp);
LIBMSCI_API void         ciFileWriteString(ciFILE *fp, const char *str);
LIBMSCI_API size_t       cpFileRead(void *ptr, size_t size, size_t nobj, ciFILE *fp);
LIBMSCI_API size_t       cpFileWrite(const void *ptr, size_t size, size_t nobj, ciFILE *fp);
LIBMSCI_API int          ciSeekFromStart(ciFILE *fp, int skipbytes);
LIBMSCI_API int          ciSeekFromCurrent(ciFILE *fp, int skipbytes);
LIBMSCI_API int          ciOpenFile64(const char *path, int mode);
LIBMSCI_API int          ciCloseFile64(int handle);
LIBMSCI_API _int64       ciMulti64(int num, ...);
LIBMSCI_API Sstring     *ci6ToSstring(int six);
LIBMSCI_API int          ciSstringTo6(Sstring *str);
LIBMSCI_API double       ciRound(double x, int ndec);
LIBMSCI_API double       ciLimitAngle(double ang, double min, double max);
LIBMSCI_API int          ciBitSet(int flag, int bitstart, int bits, int value);
LIBMSCI_API int          ciBitGet(int flag, int bitstart, int bits);
            int          ciSetFlag(int word, int flag, int isOn);
            int          ciIsFlagOn(int word, int flag);
LIBMSCI_API void         ciResetError(void);
LIBMSCI_API void         ciSetError(const char *);
LIBMSCI_API const char  *ciGetError(void);
            const char  *ciGetChar(EQUIV data);
            double       ciGetDouble(EQUIV data);
            float        ciGetFloat(EQUIV data);
            int          ciGetInt(EQUIV data);
            void         ciSetChar(Snametable *nt, EQUIV data, const char *chr);
            void         ciSetDouble(EQUIV data, double v);
            void         ciSetFloat(EQUIV data, float v);
            void         ciSetInt(EQUIV data, float v);
LIBMSCI_API const Sname *ciCharToSname(Snametable *nt, const char *chr);

LIBMSCI_API const char  *ciGetCharStringFromSstring(const Sstring *sstr);
LIBMSCI_API const char  *ciGetCharStringFromSname(const Sname *sname);

LIBMSCI_API void         ciGetDirAndFilename(Sstring *file, Sstring **dirptr, Sstring **filenameptr);
LIBMSCI_API const Sname *ciGetAbsPathSnameFromCharString(Snametable *nt, const char *cpath);
LIBMSCI_API Sstring     *ciGetAbsPathSstringFromCharString(const char *inputpath);
LIBMSCI_API void         ciGetPrefExt(Sstring *file, Sstring **prefptr, Sstring **extptr);

/* DICTIONARY FUNCTIONS */

LIBMSCI_API ciDictIter  *ciDictIterCreate(ciDict *dict);
LIBMSCI_API int          ciDictIterGetBegin(ciDictIter *diter);
LIBMSCI_API int          ciDictIterIsValid(ciDictIter *diter);
LIBMSCI_API int          ciDictIterGetNext(ciDictIter *diter);
LIBMSCI_API const void  *ciDictIterGetKey(ciDictIter *diter);
LIBMSCI_API void        *ciDictIterGetValue(ciDictIter *diter);
LIBMSCI_API void         ciDictIterDestroy(ciDictIter *diter);

#define ciSetFlag(w, f, o) ((o) ? (w) | (f) : (w) & ~(f))
#define ciIsFlagOn(w, f)   ((w) & (f) ? TRUE : FALSE)

#define     ciGetChar(d)      (CHRSTR((d).n))
#define     ciGetDouble(d)    ((double) (d).f)
#define     ciGetFloat(d)     ((d).f)
#define     ciGetInt(d)       ((int) ((d).f + .49)

#define     ciSetChar(t, d, v) ((d).n = ciCharToSname((t), (v)))
#define     ciSetDouble(d, v)  ((d).f = (float) (v))
#define     ciSetFloat(d, v)   ((d).f = (float) (v))
#define     ciSetInt(d, v)     ((d).f = (float) (v))

/* Error messages.
 */

#define ciNO_ERROR                     "no error"
#define ciERROR_NO_MEMORY              "No memory."
#define ciINTERNAL_ERROR_INVALID_ARGS  "Internal Error: invalid arguments encountered."

#define ciResetError() (ciSetError(ciNO_ERROR))

int ciItemNameCompare_Raw(const char *itemName1, const char *itemName2);

/* For debuggin' those pesky PYD files.
 *
 * Set USE_LOG to a non-zero value to have the log macros
 * activate.
 *
 * --IMB Thursday May 27, 2004 (12:20)
 */
#define USE_LOG 0


#if USE_LOG
#  define ciLOG()                     printf("%s:%d\n", __FILE__, __LINE__);
#  define ciLOG_FRMT_1(frmt, a1)      printf("%s:%d -- "##frmt##"\n", __FILE__, __LINE__, a1);
#  define ciLOG_FRMT_2(frmt, a1, a2)  printf("%s:%d -- "##frmt##"\n", __FILE__, __LINE__, a1, a2);
#else
#  define ciLOG()                     ((void)0)
#  define ciLOG_FRMT_1(frmt, a1)      ((void)0)
#  define ciLOG_FRMT_2(frmt, a1, a2)  ((void)0)
#endif

msEND_EXTERN_C
#endif
