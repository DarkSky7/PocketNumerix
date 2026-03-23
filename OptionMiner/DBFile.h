#if !defined(AFX_DBFILE_H__FBC8C46A_3375_43D9_A173_EA13AC883825__INCLUDED_)
#define AFX_DBFILE_H__FBC8C46A_3375_43D9_A173_EA13AC883825__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DBFile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDBFile dialog

class CDBFile : public CFile
{
public:
		// overrides
	bool	Eof( void );
	int		GetLine( char*& str );			// returns the length off the line
protected:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBFILE_H__FBC8C46A_3375_43D9_A173_EA13AC883825__INCLUDED_)
