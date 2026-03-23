// DBFile.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "DBFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBFile

bool	CDBFile::Eof( void )
{	
	return	GetPosition() < GetLength();
}
//---------------------------------------------------------------------------
int		CDBFile::GetLine( char*& str )
{	ULONGLONG	pos = GetPosition();
	bool ok = ReadPast( FILE* fp, const char* terminator );

}			// GetLine()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
