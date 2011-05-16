#pragma once
/********************************************************************
 *
 * FILE:       libcSTL
 * AUTHOR:     Robbert de Groot
 * COPYRIGHT:  2003-2006, Mintec, Inc.
 * 
 * DESC:
 * LibcSTL
 *
 * STL data structures dropped down to be 'C' accessable. If you use
 * STL in C code use these functions ONLY.  If you are using STL in
 * C++ code then used STL directly and don't bother with this code 
 * unless you are passed information that was generated from these 
 * routines.
 * 
 * STL considers the data types a 'containers' and to remain 
 * consistent with STL we'll use the same names.
 *******************************************************************/

#if !defined(LIBCSTLH)
#define LIBCSTLH

#if defined(LIBCORE)
#  if defined(LIBCORE_EXPORTS)
#     define LIBSTL_API __declspec(dllexport)
#  else
#     define LIBSTL_API __declspec(dllimport)
#  endif
#else
#  define LIBSTL_API
#endif

/* C++ code.********************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/* C++ code.********************************************************/

/********************************************************************
 * Includes
 *******************************************************************/
#include <stdarg.h>
#include <stdlib.h>

/********************************************************************
 * Types
 *******************************************************************/
/********************************************************************
 * TYPE: Sset, SsetIter
 *
 * Set data type.  Basically a group of unique key data.  No data 
 * associated with the keys.
 *
 * The iterator is for iterating over the Sset.
 *******************************************************************/
#ifdef NDEBUG
   typedef void Sset;
#else
   typedef struct _Sset_TAG {
      int   size;
      void *stlset;
   } Sset;
#endif
typedef void SsetIter;


/********************************************************************
 * TYPE: Sstring
 *
 * Sstring is a dynamic string data type.  
 *******************************************************************/
#ifdef NDEBUG
   typedef void Sstring;   
#else
/* This just makes it easier to see the actual internal string
 * value during a debug build.
 *
 * Notice that in a production build the Sstring is nothing
 * more than the 'stlstr' component of the Debug structure.
 */
   typedef struct _Sstring_TAG {
      const char *c_str;
      void       *stlstr;
   } Sstring;
#endif


/********************************************************************
 * TYPE: Svector
 *
 * A 1 dimentional array of void *'s.  
 *******************************************************************/
#ifdef NDEBUG
   typedef void Svector;
#else 
   typedef struct _Svector_TAG {
      int   size;
      void *stlvector;
   } Svector;
#endif

typedef int(*SvectorBinaryPredicate)(void *data1, void *data2);
typedef int(*SvectorUnaryPredicate)(void *data1);
typedef void(*SvectorForEachFunc)(void *data);


/********************************************************************
 * TYPE: Sdict, SdictIter
 *
 * Like an Sset this one has key-value pairs instead of just keys.
 *******************************************************************/
#ifdef NDEBUG
   typedef void Sdict;
#else 
   typedef struct _Sdict_TAG {
      int   size;
      void *stldict;
   } Sdict;
#endif
typedef void SdictIter;
typedef void(*SdictForEachFunc)(const void *key, void *value);


/********************************************************************
 * TYPE: Sname, Snametable, SnametableIter
 *
 * Like Sset, Snametable and SnametableIter work on Sstrings.  Sname
 * is an Sstring and can be used in Sstring functions.  
 *******************************************************************/
typedef Sstring Sname;
typedef Sset    Snametable;
typedef int     SnametableIter;


/********************************************************************
 * DEF: WHITESPACE  
 *
 * Used for trimming whitespace.
 *******************************************************************/
#define WHITESPACE " \x09\x0a\x0b\x0c\x0d"

/********************************************************************
 * DEF: sstringINTERNAL_BUFFER_SIZE 
 *
 * size of static char* buffer
 *******************************************************************/
#define sstringINTERNAL_BUFFER_SIZE  (size_t)256

/********************************************************************
 * DEF: sstringDYNAMIC_EXPAND_FACTOR 
 *
 * factor to dynamically grow a buffer by
 *******************************************************************/
#define sstringDYNAMIC_EXPAND_FACTOR (size_t)2

/********************************************************************
 * Global Functions
 *******************************************************************/

/* Schar ************************************************************/

LIBSTL_API int             scharCompareInsensitive(const char *s1, const char *s2);
LIBSTL_API void            scharModifyCapitalize(char *s);
LIBSTL_API int             scharModifyReplaceLetters(char *buffer, size_t bufferLen, const char *letters, const char *with);
LIBSTL_API void            scharModifyTrimLeft(char *s, const char *trim);
LIBSTL_API void            scharModifyTrimRight(char *s, const char *trim);
LIBSTL_API void            scharModifyTrim(char *s, const char *trim);
LIBSTL_API void            scharModifyToLowerN(char *s, size_t len);
LIBSTL_API void            scharModifyToUpperN(char *s, size_t len);
LIBSTL_API void            scharToUpperN(char *dst, const char *src, size_t len);
LIBSTL_API void            scharSafeNCopy(char *to, const char *from, size_t len);
LIBSTL_API void            scharNullify(char *s, size_t len);
LIBSTL_API Svector        *scharSplit_Raw(const char *s, const char *delimiters);

/* Sset ************************************************************/
LIBSTL_API Sset           *ssetCreate(int (*lessthan)(const void *a, const void *b));

LIBSTL_API void            ssetDestroy(Sset *s);

LIBSTL_API const void     *ssetFind(Sset *s, const void *data);

LIBSTL_API int             ssetGetSize(Sset *s);

LIBSTL_API void            ssetInsert(Sset *s, const void *data);

LIBSTL_API SsetIter       *ssetIterCreate(const Sset *s);
LIBSTL_API void            ssetIterDestroy(SsetIter *si);
LIBSTL_API int             ssetIterGetBegin(SsetIter *si);
LIBSTL_API int             ssetIterGetNext(SsetIter *si);
LIBSTL_API const void     *ssetIterGetValue(SsetIter *si);

LIBSTL_API int             ssetIterIsValid(SsetIter *si);

/* Sstring *********************************************************/
LIBSTL_API void            sstringAddCharStrings(Sstring *dst, ...);
LIBSTL_API void            sstringAppend(Sstring *dst, const Sstring *src);
LIBSTL_API void            sstringAppendCharString(Sstring *dst, const char *src);
LIBSTL_API void            sstringAssign(Sstring *dst, const Sstring *src);
LIBSTL_API void            sstringAssignCharString(Sstring *dst, const char *src);
LIBSTL_API void            sstringAssignInteger(Sstring *dst, int integer);
LIBSTL_API void            sstringAssignReal(Sstring *dst, double real);
LIBSTL_API void            sstringAssignRealDecimalPrecision(Sstring *dst, int decimalCount, double real);
LIBSTL_API void            sstringAssignSubString(Sstring *dst, const Sstring *src, int positiona, int positionb);

LIBSTL_API int             sstringCompare(const Sstring *a, const Sstring *b);
LIBSTL_API int             sstringCompareInsensitive(const Sstring *a, const Sstring *b);
LIBSTL_API int             sstringCompareInsensitiveCharString(const Sstring *a, const char *b);
LIBSTL_API int             sstringConvertToInteger(const Sstring *str);
LIBSTL_API double          sstringConvertToReal(const Sstring *str);
LIBSTL_API Sstring        *sstringCreate(void);
LIBSTL_API Sstring        *sstringCreateFromCharString(const char *str);
LIBSTL_API Sstring        *sstringCreateFromFormat(const char *format, ...);
LIBSTL_API Sstring        *sstringCreateFromFormatV(const char *format, va_list args);
LIBSTL_API Sstring        *sstringCreateFromSstring(const Sstring *str);
LIBSTL_API Sstring        *sstringCreateFromSubString(const Sstring *str, int positiona, int positionb);

LIBSTL_API void            sstringDestroy(Sstring *str);

LIBSTL_API void            sstringErase(Sstring *str, int positiona, int positionb);
LIBSTL_API int             sstringEqual(const Sstring *str1, const Sstring *str2);
LIBSTL_API int             sstringEqualCharString(const Sstring *str, const char *charstr);

LIBSTL_API int             sstringFindFirstNotOf(const Sstring *str, int position, const Sstring *letters);
LIBSTL_API int             sstringFindFirstNotOfCharString(const Sstring *str, int position, const char *letters);
LIBSTL_API int             sstringFindFirstOf(const Sstring *str, int position, const Sstring *letters);
LIBSTL_API int             sstringFindFirstOfCharString(const Sstring *str, int position, const char *letters);
LIBSTL_API int             sstringFindLastNotOf(const Sstring *str, int position, const Sstring *letters);
LIBSTL_API int             sstringFindLastNotOfCharString(const Sstring *str, int position, const char *letters);
LIBSTL_API int             sstringFindLastOf(const Sstring *str, int position, const Sstring *letters);
LIBSTL_API int             sstringFindLastOfCharString(const Sstring *str, int position, const char *letters);
LIBSTL_API size_t          sstringFindSubString(const Sstring *str, const Sstring *substr);
LIBSTL_API size_t          sstringFindSubStringCharString(const Sstring *str, const char *substr);

LIBSTL_API char            sstringGetChar(const Sstring *str, int position);
LIBSTL_API const char     *sstringGetCharString(const Sstring *str);
LIBSTL_API int             sstringGetInteger(const Sstring *str);
LIBSTL_API int             sstringGetLength(const Sstring *str);
LIBSTL_API int             sstringGetUndefinedPosition(void);

LIBSTL_API int             sstringIsEmpty(const Sstring *str);

LIBSTL_API void            sstringInsert(Sstring *dst, int position, const Sstring *src);
LIBSTL_API void            sstringInsertCharString(Sstring *dst, int position, const char *src);

LIBSTL_API Sstring        *sstringJoin(const Svector *stringvec, const char *with);

LIBSTL_API void            sstringModifyCapitalize(Sstring *str);
LIBSTL_API void            sstringModifyEraseLeadingWhiteSpace(Sstring *str);
LIBSTL_API void            sstringModifyEraseTrailingWhiteSpace(Sstring *str);
LIBSTL_API void            sstringModifyFromCSV(Sstring *str);
LIBSTL_API void            sstringModifyLength(Sstring *str, int count, char fill);
LIBSTL_API void            sstringModifyToCSV(Sstring *str);
LIBSTL_API void            sstringModifyToLower(Sstring *str);
LIBSTL_API void            sstringModifyToUpper(Sstring *str);
LIBSTL_API void            sstringModifyTrim(Sstring *str, const char *letters);
LIBSTL_API void            sstringModifyTrimLeft(Sstring *str, const char *letters);
LIBSTL_API void            sstringModifyTrimRight(Sstring *str, const char *letters);

LIBSTL_API void            sstringReplaceLetters(Sstring *str, const char *letters, const char *with);

LIBSTL_API Svector        *sstringSplit(const Sstring *str, const char *delimiters);

/* Svector *********************************************************/
LIBSTL_API void            svectorAppendVector(Svector *v1, const Svector *v2);

LIBSTL_API int             svectorBinarySearch(Svector *v, void *value, 
                                    SvectorBinaryPredicate predicate);
LIBSTL_API int             svectorBinarySearchNear(Svector *v, void *value, 
                                        SvectorBinaryPredicate predicate);

LIBSTL_API void            svectorClear(Svector *v);
LIBSTL_API Svector        *svectorClone(Svector *v);
LIBSTL_API Svector        *svectorCreate(void);

LIBSTL_API void            svectorDestroy(Svector *v);

LIBSTL_API void           *svectorErase(Svector *v, int index);
LIBSTL_API Svector        *svectorEraseIf(Svector *v, SvectorUnaryPredicate predicate);

LIBSTL_API void            svectorForEach(Svector *v, SvectorForEachFunc forEachFn);

LIBSTL_API void           *svectorGet(const Svector *v, int index);
LIBSTL_API int             svectorGetSize(const Svector *v);

LIBSTL_API void            svectorInsert(Svector *v, int index);

LIBSTL_API int             svectorIsEmpty(Svector *v);

LIBSTL_API void            svectorPopBack(Svector *v);
LIBSTL_API void            svectorPushBack(Svector *v, void *data);

LIBSTL_API void            svectorReserve(Svector *v, int count);
LIBSTL_API void            svectorResize(Svector *v, int count);

LIBSTL_API void            svectorSet(Svector *v, int index, void *data);
LIBSTL_API void            svectorSort(Svector *v, SvectorBinaryPredicate predicate);

/* Svector -- Helper predicate ************************************/
LIBSTL_API int             svectorAuxIsDataNull(void *data);


/* Galaxy inspired *************************************************/
/* Sdict ***********************************************************/
LIBSTL_API Sdict          *sdictCreate(void);

LIBSTL_API void            sdictDestroy(Sdict *d);

LIBSTL_API void            sdictForEach(Sdict *dict, SdictForEachFunc forEachFn);

LIBSTL_API int             sdictGetSize(Sdict *d);

LIBSTL_API void           *sdictLoad(Sdict *d, const void *key);

LIBSTL_API void            sdictStore(Sdict *d, const void *key, void *data);

LIBSTL_API SdictIter      *sdictIterCreate(const Sdict *d);
LIBSTL_API void            sdictIterDestroy(SdictIter *di);
LIBSTL_API int             sdictIterGetBegin(SdictIter *di);
LIBSTL_API int             sdictIterGetNext(SdictIter *di);
LIBSTL_API const void     *sdictIterGetKey(SdictIter *di);
LIBSTL_API void           *sdictIterGetValue(SdictIter *di);

LIBSTL_API int             sdictIterIsValid(SdictIter *di);

/* Snamedict *******************************************************/

/* A Sdict that uses Snames as their 'keys'. Often you have to 
 * manage a Sdict and a corresponding Snametable, this object
 * wraps the functionality of both into one package.
 */

typedef struct _SNAMEDICT_TAG {
   Snametable *nt;
   Sdict *dict;
} Snamedict;

LIBSTL_API Snamedict *snamedictCreate(void);

LIBSTL_API void       snamedictDestroy(Snamedict *snd);

LIBSTL_API void       snamedictForEach(Snamedict *snd, SdictForEachFunc forEachFunc);

LIBSTL_API void      *snamedictLoadBySname(Snamedict *snd, const Sname *name);
LIBSTL_API void      *snamedictLoadByCStr(Snamedict *snd, const char *cname);

LIBSTL_API void       snamedictStoreBySname(Snamedict *snd, const Sname *name, void *data);
LIBSTL_API void       snamedictStoreByCStr(Snamedict *snd, const char *cname, void *data);

/* Sname ***********************************************************/
LIBSTL_API Snametable     *snametableCreate(void);

LIBSTL_API void            snametableDestroy(Snametable *nt);

LIBSTL_API const Sname    *snametableIntern(Snametable *nt, const Sstring *name);
LIBSTL_API const Sname    *snametableInternFormat(Snametable *nt, const char *format, ...);
LIBSTL_API const Sname    *snametableInternFormatV(Snametable *nt, const char * format, va_list args);
LIBSTL_API const Sname    *snametableInternCharString(Snametable *nt, const char *str);

LIBSTL_API SnametableIter *snametableIterCreate(const Snametable *nt);
LIBSTL_API void            snametableIterDestroy(SnametableIter *nti);
LIBSTL_API int             snametableIterGetBegin(SnametableIter *nti);
LIBSTL_API int             snametableIterGetNext(SnametableIter *nti);
LIBSTL_API const Sname    *snametableIterGetSname(SnametableIter *nti);

LIBSTL_API int             snametableIterIsValid(SnametableIter *nti);

/* C++ code.********************************************************/
#ifdef __cplusplus
}
#endif
/* C++ code.********************************************************/

#endif
