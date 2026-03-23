// DBFile.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "DBFile.h"
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBFile

CDBFile::CDBFile( void ) : CFile()
{	
}

BOOL	CDBFile::Open( LPCTSTR lpszFileName, CFileException* pError /* = NULL */ )
{		// if a Read mode Open fails, use Create mode
	if ( CFile::Open( lpszFileName, CDBFile::dbFileFlags, pError ) )
		return	TRUE;
#ifdef _DEBUG
	CString causeStr = _T("");
	switch ( pError->m_cause )
	{
#if ( _WIN_WCE < 0x400 )
		case CFileException::generic:
#else
		case CFileException::genericException:
#endif
			causeStr = _T("generic - an unspecified error occured");
			break;
		case CFileException::fileNotFound:
			causeStr = _T("fileNotFound - the file could not be located");
			break;
		case CFileException::badPath   :
			causeStr = _T("badPath - all or part of the path is invalid");
			break;
		case CFileException::tooManyOpenFiles:
			causeStr = _T("tooManyOpenFiles - the permitted number of open files was exceeded");
			break;
		case CFileException::accessDenied:
			causeStr = _T("accessDenied - the file could not be accessed");
			break;
		case CFileException::invalidFile:
			causeStr = _T("invalidFile - there was an attempt to use an invalid file handle");
			break;
		case CFileException::removeCurrentDir:
			causeStr = _T("removeCurrentDir - the current working directory cannot be removed");
			break;
		case CFileException::directoryFull:
			causeStr = _T("directoryFull - there are no more directory entries");
			break;
		case CFileException::badSeek:
			causeStr = _T("badSeek - there was an error trying to set the file pointer");
			break;
		case CFileException::hardIO:
			causeStr = _T("hardIO - there was a hardware error");
			break;
		case CFileException::sharingViolation:
			causeStr = _T("sharingViolation - SHARE.EXE was not loaded, or a shared region was locked");
			break;
		case CFileException::lockViolation:
			causeStr = _T("lockViolation - there was an attempt to lock a region that was already locked");
			break;
		case CFileException::diskFull:
			causeStr = _T("diskFull - the disk is full");
			break;
		case CFileException::endOfFile:
			causeStr = _T("endOfFile - the end of file was reached");
			break;
		case CFileException::none:
		default:
			causeStr = _T("none - no error occurred");
			break;
	}
	TRACE(	_T("DBFile::Open: couldn't open %s in open/create/noTruncate mode, cause = %d, %s\n"),
				lpszFileName, pError->m_cause, causeStr );
#endif
	return	FALSE;
}
//---------------------------------------------------------------------------
/*
ULONGLONG	CDBFile::GetLength( void )
{
	ULONGLONG curPos = GetPosition();
	ULONGLONG len = SeekToEnd();
	ULONGLONG actual = Seek( curPos, CFile::begin );
	CFileStatus		rStatus;
	BOOL b1 = GetStatus( rStatus );
	SetLength( rStatus.m_size );
	return	rStatus.m_size;
}			// GetLength()
*/
//---------------------------------------------------------------------------
