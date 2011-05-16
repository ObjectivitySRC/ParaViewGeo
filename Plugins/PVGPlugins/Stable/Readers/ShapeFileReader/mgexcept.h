////////////////////////////////////////////////////////////////////////////////
//
//  File:        mgexcept.h
//  Created by:  Jerzy Majewski - jmajewsk@meil.pw.edu.pl
//  Modified by: 
//
// Copyright notice:
//    Copyright (C) 2000-2003 Jerzy Majewski 
// 
//    This is free software. You can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public License
//    as published by the Free Software Foundation.
//
//    This software is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY. See the GNU Lesser General Public License 
//    for more details.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MGEXCEPT_H__
#define __MGEXCEPT_H__


#define THIS_FILE __FILE__	// defines name of header or implementation file

// constants used for exception handling
extern const MGInt EX_FILE_CODE;
extern const MGInt EX_MATH_CODE;
extern const MGInt EX_MEMORY_CODE;
extern const MGInt EX_INTERNAL_CODE;
extern const MGInt EX_ASSERT_CODE;
extern const MGInt EX_REXP_CODE;

extern const char EX_FILE_STR[];
extern const char EX_MATH_STR[];
extern const char EX_MEMORY_STR[];
extern const char EX_INTERNAL_STR[];
extern const char EX_ASSERT_STR[];
extern const char EX_REXP_STR[];


// name of file used for tracing
extern const char TRACE_FILE_NAME[];


//////////////////////////////////////////////////////////////////////
/// class Trace
//////////////////////////////////////////////////////////////////////
class Trace
{
public:
	Trace()	{ FILE *ftrc = fopen( TRACE_FILE_NAME, "wt"); Verify(ftrc); fclose( ftrc); }

	FILE*	Open() { FILE *ftrc = fopen( TRACE_FILE_NAME, "at"); Verify(ftrc); return ftrc; }
	void	Close( FILE *f)	{ fclose( f);}
	void	Verify( FILE *f);
	void	Out( char *sfile, MGInt nline);
};

inline void Trace::Verify( FILE *f)
{
	if ( !f)
		printf( "mgtrace file '%s' opening error\n", TRACE_FILE_NAME);
}

inline void Trace::Out( char *sfile, MGInt nline)
{	
	FILE *ftrc = Open(); 
	fprintf( ftrc, "FILE:%s - LINE:%d;  ", sfile, nline);
	Close( ftrc);	
}


//////////////////////////////////////////////////////////////////////
// macros for traceing
//////////////////////////////////////////////////////////////////////
/***
//-----------------------------------------
#ifdef _DEBUG
//-----------------------------------------
#define INIT_TRACE	Trace mgtrace

#define	TRACE(sz) \
	{ \
		FILE *f = mgtrace.Open(); \
		mgtrace.Out(THIS_FILE, __LINE__); \
		fprintf( f, sz); \
		fprintf( f, "\n"); \
		mgtrace.Close( f); \
	}

#define	TRACE1(sz, x1) \
	{ \
		FILE *f = mgtrace.Open(); \
		mgtrace.Out(THIS_FILE, __LINE__); \
		fprintf( f, sz, x1); \
		fprintf( f, "\n"); \
		mgtrace.Close( f); \
	}

#define	TRACE2(sz, x1, x2) \
	{ \
		FILE *f = mgtrace.Open(); \
		mgtrace.Out(THIS_FILE, __LINE__); \
		fprintf( f, sz, x1, x2); \
		fprintf( f, "\n"); \
		mgtrace.Close( f); \
	}

#define	TRACE_EXCEPTION(e) \
	{ \
		FILE *f = mgtrace.Open(); \
		(e).WriteInfo( f); \
		mgtrace.Close( f); \
	}


extern Trace mgtrace;

//-----------------------------------------
#else // _DEBUG
//-----------------------------------------
***/
#define INIT_TRACE

#define	TRACE(sz) \
	{ \
	}

#define	TRACE1(sz, x1) \
	{ \
	}

#define	TRACE2(sz, x1, x2) \
	{ \
	}

#define	TRACE_EXCEPTION(e) \
	{ \
	}
/***
//-----------------------------------------
#endif // _DEBUG
//-----------------------------------------
***/




#define	TRACE_TO_STDERR(e) \
	{ \
		(e).WriteInfo( stderr); \
	}

#define	TRACE_TO_CERR TRACE_TO_STDERR


/*
#define INIT_TRACE
	
#define	TRACE(sz) \
	{ \
	}

#define	TRACE1(sz, x1) \
	{ \
	}

#define	TRACE2(sz, x1, x2) \
	{ \
	}

#define	TRACE_EXCEPTION(e) \
	{ \
	}

#define	TRACE_TO_STDERR(e) \
	{ \
	}

#define	TRACE_TO_CERR TRACE_TO_STDERR
*/


//////////////////////////////////////////////////////////////////////
// class Except - base, abstract class for all exceptions
//////////////////////////////////////////////////////////////////////
class Except
{
public:
	Except()				{ mComment = mFileName = ""; mLineNo = 0;}	
	Except( const Except& ex) 
		: mComment(ex.mComment), mFileName(ex.mFileName), mLineNo(ex.mLineNo) {};
		
	Except( MGString com, MGString fname, MGInt line) 
		: mComment(com), mFileName(fname), mLineNo(line) {};
		
	virtual ~Except()	{};

    Except& operator = (const Except& ex);
	
	virtual MGInt		GetExType()		 const 	= 0;
	virtual MGString	GetExPrefix() 	 const 	= 0;
	
	virtual void		WriteInfo( FILE *f);

protected:
	MGString	mFileName;
	MGInt		mLineNo;
	MGString	mComment;	
};


inline Except& Except::operator = (const Except& ex)
{ 
	mFileName = ex.mFileName;
	mLineNo   = ex.mLineNo;
	mComment  = ex.mComment;
	return *this; 
}

inline void Except::WriteInfo( FILE *f)
{
	fprintf( f, "FILE:%s - LINE:%d;\n", mFileName.c_str(), mLineNo);
	fprintf( f, "    %s: %s\n", (GetExPrefix()).c_str(), mComment.c_str() );
}



//////////////////////////////////////////////////////////////////////
// class ExceptFile
//  used when some problems occur with opening, reading and parsing
//  information from files
//  passes to handler comment + add. info. (e.g. file name)
//////////////////////////////////////////////////////////////////////
class ExceptFile : public Except
{	
protected:
	MGString	mFileInfo;
	
public:
	ExceptFile()						{ mFileInfo = "";}
	ExceptFile( MGString com, MGString info, MGString fname, MGInt line)
		: Except( com, fname, line)		{mFileInfo = info;};
	virtual ~ExceptFile()	{};
	
	virtual MGInt		GetExType()		const 	{ return EX_FILE_CODE;}
	virtual MGString	GetExPrefix()	const 	{ return EX_FILE_STR;}

	virtual void		WriteInfo( FILE *f);
	
};

inline void ExceptFile::WriteInfo( FILE *f)
{
	fprintf( f, "FILE:%s - LINE:%d;\n", mFileName.c_str(), mLineNo);
	fprintf( f, "    %s: %s '%s'\n", (GetExPrefix()).c_str(), mComment.c_str(), mFileInfo.c_str() );
}


//////////////////////////////////////////////////////////////////////
// class ExceptMath
//  for math. calc. exceptions
//////////////////////////////////////////////////////////////////////
class ExceptMath : public Except
{	
public:
	ExceptMath()				{};
	ExceptMath( MGString com, MGString fname, MGInt line) 
		: Except( com, fname, line) {};
		
	virtual ~ExceptMath()	{};
	
	virtual MGInt	GetExType()		const { return EX_MATH_CODE;}
	virtual MGString	GetExPrefix()	const { return EX_MATH_STR;}
};

//////////////////////////////////////////////////////////////////////
// class ExceptMem
//  for exception caused by memory (problems with alloc., memory corrupt.)
//  C++ bad_alloc is switched off (at least for obj. allocated with GGNEW)
//  because bad_alloc does not return info. where except. occured
//////////////////////////////////////////////////////////////////////
class ExceptMem : public Except
{	
public:
	ExceptMem()				{};
	ExceptMem( MGString com, MGString fname, MGInt line) 
		: Except( com, fname, line) {};
		
	virtual ~ExceptMem()	{};
	
	virtual MGInt	GetExType()		const { return EX_MEMORY_CODE;}
	virtual MGString	GetExPrefix()	const { return EX_MEMORY_STR;}
};

//////////////////////////////////////////////////////////////////////
// class ExceptAssert
//	used in ASSERT macro
//////////////////////////////////////////////////////////////////////
class ExceptAssert : public Except
{	
public:
	ExceptAssert()			{};
	ExceptAssert( MGString com, MGString fname, MGInt line) 
		: Except( com, fname, line) {};
		
	virtual ~ExceptAssert()	{};
	
	virtual MGInt	GetExType()		const { return EX_ASSERT_CODE;}
	virtual MGString	GetExPrefix()	const { return EX_ASSERT_STR;}
};

//////////////////////////////////////////////////////////////////////
// class InternalException
//  for handling abnormal events specific for mesh gen. program
//////////////////////////////////////////////////////////////////////
class ExceptInter : public Except
{
public:
	ExceptInter()			{};
	ExceptInter( MGString com, MGString fname, MGInt line) 
		: Except( com, fname, line) {};
		
	virtual ~ExceptInter()	{};
	
	virtual MGInt	GetExType()		const { return EX_INTERNAL_CODE;}
	virtual MGString	GetExPrefix()	const { return EX_INTERNAL_STR;}
};

//////////////////////////////////////////////////////////////////////
// class ExceptRExp
//	for handling regular expresion exceptions
//////////////////////////////////////////////////////////////////////
class ExceptRExp : public Except
{	
public:
	ExceptRExp()				{};
	ExceptRExp( MGString com, MGString fname, MGInt line) 
		: Except( com, fname, line) {};
		
	virtual ~ExceptRExp()	{};
	
	virtual MGInt	GetExType()		const { return EX_REXP_CODE;}
	virtual MGString	GetExPrefix()	const { return EX_REXP_STR;}
};




//////////////////////////////////////////////////////////////////////
// macros for throwing Exceptions 
//////////////////////////////////////////////////////////////////////

#ifdef assert
	// if compiler provides a assert macro in "assert.h"
	#define ASSERT assert
#else
	// assert macro which does not depend on compiler; uses exception ExceptAssert
	#ifdef _DEBUG
		#define ASSERT(f) \
		{ \
		if (!(f)) \
			throw new ExceptAssert( "assertion failed: '" #f "'", THIS_FILE, __LINE__ ); \
		}
	#else
		#define ASSERT(f) \
		{ \
		}
	#endif // _DEBUG
#endif // assert


#define THROW_ALLOC(f) \
	{ \
	if (!(f)) \
		throw new ExceptMem( "allocation error: '" #f "'", THIS_FILE, __LINE__ ); \
	}

#define THROW_MEMORY(f) \
	{ \
		throw new ExceptMem( f, THIS_FILE, __LINE__ ); \
	}

#define THROW_FILE(f1, f2) \
	{ \
		throw new ExceptFile( f1, f2, THIS_FILE, __LINE__ ); \
	}

#define THROW_MATH(f) \
	{ \
		throw new ExceptMath( f, THIS_FILE, __LINE__ ); \
	}

#define THROW_INTERNAL(f) \
	{ \
		throw new ExceptInter( f, THIS_FILE, __LINE__ ); \
	}

#define THROW_REGEXP(f) \
	{ \
		throw new ExceptRExp( f, THIS_FILE, __LINE__ ); \
	}




#endif	// __MGEXCEPT_H__
		
