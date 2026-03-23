// Database.cpp
#include "StdAfx.h"
#include "Database.h"

using namespace std;

const char*		pgUser = "postgres";			// postgres admin acct
const char*		pgPass = "xkj5793$q";

Database::Database()
	: conn(NULL)
{
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Database::~Database()
{
	if ( IsConnected() )
		PQfinish( conn );
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int		Database::InstallConnect( const char* user, const char* pass, const char* dbms  )
{		// connect to marketminer db ?
	if ( Connect( user, pass, dbms ) )
		return	0;										// no errors
		// connect to postgres db and setup marketminer db
	Connect( pgUser, pgPass );							// default dbms is postgres
	if ( ! IsConnected() )
		return	-1;										// couldn't connect postgres db

	mystring constraint("usename='");
	constraint += user;
	constraint += "'";
	int		cnt;
	if ( CountRecords("pg_user", constraint, cnt)  &&  cnt < 1 )
		if ( ! CreateUser( user, pass ) )
			return	-2;

	constraint = "datname='";
	constraint += dbms;
	constraint += "'";
	if ( CountRecords("pg_database", constraint, cnt)  &&  cnt < 1 )
		if ( ! CreateDbms( dbms, user ) )
			return	-3;

	PQfinish( conn );
	return	Connect( user, pass, dbms )  ?  0  :  -4;	// -4 -> couldn't connect marketminer db
}			// InstallConnect()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool	Database::Connect( mystring& connInfo )
{		// kills any existing connection; returns true if Connect() is successful
	if ( IsConnected() )
		PQfinish( conn );
		// attempt to connect with the database
	conn = PQconnectdb( connInfo.c_str() );
	bool	okay = IsConnected();
#ifdef _DEBUG
	fprintf( stderr, "Database::Connect(%s) PQconnectdb %s\n",
			connInfo.c_str(), (okay ? "succeeded" : "failed") );
#endif
	return	okay;
}			// Connect()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool	Database::Connect(
	const char* user,
	const char* pass,
	const char* dbms,		// = NULL
	const char* host,		// = NULL
	const char* port )		// = NULL
{	mystring	connSt;
	char		buf[128];
		// const char *connInfo = "host='localhost' port='5432' dbname='postgres' user='postgres' password='xkj5793$q'";
		// const char *connInfo = "host=localhost port=5432 dbname=postgres user=postgres password=xkj5793$q";
		// const char *connInfo = "user=postgres password=xkj5793$q";	// defaults to dbname=postgres
	sprintf( buf, "user=%s password=%s", user, pass );
	connSt = buf;
	if ( dbms )
	{	connSt += " dbname=";
		connSt += dbms;
	}
	if ( host )
	{	connSt += " host=";
		connSt += host;
	}
	if ( port )
	{	connSt += " port=";
		connSt += port;
	}
	return	Connect( connSt );
}			// Connect()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool	Database::CreateUser( const char* user, const char* pass )
{		// create a user with login ability
	mystring	sql;
	sql = "CREATE USER ";
	sql += user;
	sql += " WITH PASSWORD '";
	sql += pass;
	sql += "'";
	bool okay = RunUpdate( sql );
	if ( ! okay )
		fprintf( stderr, "Database::CreateUser: RunUpdate(\"%s\") failed\n", sql.c_str() );
	return	okay;
}			// CreateUser()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool	Database::CreateDbms( const char* dbms, const char* owner )
{		// create a database
	mystring	sql;
	sql = "CREATE DATABASE ";
	sql += dbms;
	sql += " WITH OWNER ";
	sql += owner;
	bool okay = RunUpdate( sql );
	if ( ! okay )
		fprintf( stderr, "Database::CreateDbms: RunUpdate(\"%s\") failed\n", sql.c_str() );
	return	okay;
}			// CreateDbms()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool	Database::CreateTable( const char *name, mystring& desc )
{		// desc doesn't need to supply the enclosing parens
	mystring sql;
	sql = "CREATE TABLE ";
	sql += name;
	sql += " (";
	sql += desc;
	sql += ")";
	bool okay = RunUpdate( sql );
	if ( ! okay )
		fprintf( stderr, "Database::CreateTable: RunUpdate(\"%s\") failed\n", sql.c_str() );
	return	okay;
}			// CreateTable()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool	Database::RunUpdate( mystring& sql )
{		// run a query that is not expected to produce a result set
	bool		okay = false;
	PGresult*	res = PQexec( conn, sql.c_str() );
	if ( res )
	{	okay = ( PQresultStatus(res) == PGRES_COMMAND_OK );	// success with empty result set
		if ( okay )
		{
#ifdef _DEBUG
			fprintf( stderr, "Database::RunUpdate(\"%s\") -> COMMAND_OK\n", sql.c_str() );
#endif
		}
		else
			fprintf( stderr, "Database::RunUpdate(\"%s\") -> %s\n", sql.c_str(), PQerrorMessage(conn) );
		PQclear( res );										// not expecting a result set
	}
	else
	{	fprintf( stderr, "RunUpdate: insufficient memory to run: %s\n", sql.c_str() );
		throw "RunUpdate: insufficient memory";
	}
	return	okay;
}			// RunUpdate()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
PGresult*	Database::GetQueryResult( mystring& sql )
{		// run a query that may return a result set
	PGresult*	res = PQexec( conn, sql.c_str() );
	if ( res )
	{	int	status = PQresultStatus(res);
			//	  empty result set			  non-empty result set
		if ( status == PGRES_TUPLES_OK )
		{
#ifdef _DEBUG
			fprintf( stderr, "Database::GetQueryResult(\"%s\") -> TUPLES_OK\n", sql.c_str() );
#endif
		}
		else
		{		// what happened?
			fprintf( stderr, "Database::GetQueryResult(\"%s\") -> %s\n", sql.c_str(), PQerrorMessage(conn) );
			PQclear( res );						// possibly moot
			res = NULL;
		}
	}
	else
	{	fprintf( stderr, "Database::GetQueryResult: insufficient memory to run: %s\n", sql.c_str() );
		throw "Database::GetQueryResult insufficient memory";
	}
	return	res;
}			// GetQueryResult()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool		Database::CountRecords( const char* tableName, mystring& constraint, int& res )
{	mystring	sql;
	int			cnt = 0;
		// constraint might look like:  "portfolio_id = 7";
	sql = "SELECT COUNT(*) FROM ";
	sql += tableName;
	if ( ! constraint.empty() )
	{	sql += " WHERE ";
		sql += constraint;
	}
	return	GetIntFromSQL( sql, res );
}			// CountRecords()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool		Database::GetIntFromSQL( mystring& sql, int& res )
{		// if there's more than one record in the ResultSet, still only returns the first float
	bool	okay = false;
	PGresult*	pgres = GetQueryResult( sql );
	if ( pgres )
	{	if ( PQntuples(pgres) > 0  &&  PQnfields(pgres) > 0 )
		{	char*	val = PQgetvalue(pgres,0,0);		// get first column, first field
			res = atoi( val );
			okay = true;
		}
		PQclear( pgres );
	}
	return	okay;
}			// GetIntFromSQL()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool		Database::GetDoubleFromSQL( mystring& sql, double& res )
{		// if there's more than one record in the ResultSet, still only returns the first float
	bool	okay = false;
	PGresult*	pgres = GetQueryResult( sql );
	if ( pgres )
	{	if ( PQntuples(pgres) > 0  &&  PQnfields(pgres) > 0 )
		{	char*	val = PQgetvalue(pgres,0,0);			// get first column, first field
			res = atof( val );
			okay = true;
		}
		PQclear( pgres );
	}
	return	okay;
}			// GetDoubleFromSQL()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
char*		Database::GetCstringFromSQL( mystring& sql )
{		// if there's more than one record in the result set, still only returns the first float
	char*	retVal = NULL;
	PGresult*	res = GetQueryResult( sql );
	if ( PQntuples(res) > 0 )
	{	char*	val = PQgetvalue(res,0,0);		// get first column, first field
		size_t	slen = strlen( val );
		retVal = new char[slen+1];				// on heap, must delete later
		strcpy( retVal, val );
	}
	PQclear( res );
	return	retVal;
}			// GetCstringFromSQL()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
mystring*		Database::GetMystringFromSQL( mystring& sql )
{		// if there's more than one record in the result set, still only returns the first float
	mystring*	retVal = NULL;
	PGresult*	res = GetQueryResult( sql );
	if ( PQntuples(res) > 0 )
	{	char*	val = PQgetvalue(res,0,0);		// get first column, first field
		retVal = new mystring(val);				// on heap, must delete later
	}
	PQclear( res );
	return	retVal;
}			// GetMystringFromSQL()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool		Database::CountTables( const char* tableName, int& res )
{		// count number of tables (hopefully 0 or 1) named tableName
	mystring	whereClause;
	whereClause = "table_name='";
	whereClause += tableName;
	whereClause += "'";
	return	CountRecords( "information_schema.tables", whereClause, res );
}			// CountTables()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
