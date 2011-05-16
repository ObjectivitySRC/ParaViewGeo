#pragma once
/*
 * Copyright 2004 Mintec, Inc.
 * All Rights Reserved.
 *
 * Minesight: msextern_c.h
 *
 * Description: 
 *    Provides a simple macro for creating extern c commands.
 *
 * Usage:
 * 
 *   In your API header file just use the following format,
 *
 *      #include <msextern_c.h>
 *      msBEGIN_EXTERN_C
 *
 *      ... your API definitions ...
 *
 *      msEND_EXTERN_C
 */

#if !defined(MSEXTERN_C_HEADER)
#define MSEXTERN_C_HEADER

#ifdef __cplusplus
#define msBEGIN_EXTERN_C           extern "C" {
#define msEND_EXTERN_C             }
#else
#define msBEGIN_EXTERN_C
#define msEND_EXTERN_C
#endif

#endif /* MSEXTERN_C_HEADER */
