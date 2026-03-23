// Database.h
#ifndef __Database__
#define __Database__

#include "mystring.h"
#include <libpq-fe.h>		// from D:\Programs\PostgreSQL\8.4\include

class Database
{
public:
				Database();
	virtual		~Database();
	bool		Connect( mystring& connInfo );
	bool		Connect( const char* user, const char* pass,
						const char* dbms=NULL, const char* host=NULL, const char* port=NULL );
	int			InstallConnect( const char* user, const char* pass, const char* dbms );
	bool		IsConnected()
				{	return	PQstatus(conn) == CONNECTION_OK;	}

	bool		CreateUser( const char *user, const char *pass );
	bool		CreateDbms( const char *dbms, const char *owner );
	bool		CreateTable( const char	*name, mystring& desc );

	bool		RunUpdate( mystring& sql );
	PGresult*	GetQueryResult( mystring& sql );
	bool		GetIntFromSQL( mystring& sql, int& res );
	bool		GetDoubleFromSQL( mystring& sql, double& res );
	char*		GetCstringFromSQL( mystring& sql );
	mystring*	GetMystringFromSQL( mystring& sql );
	bool		CountRecords( const char* table, mystring& constraint, int& res );

protected:
	bool		CountTables( const char* tableName, int& res );
	PGconn*		conn;
};
#endif