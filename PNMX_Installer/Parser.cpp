#include "StdAfx.h"
//#include "Afx.h"				// CString's require all of MFC

//#include "Winsock2.h"
#include "HttpGetFile.h"
#include "PNMX_InstallerDlg.h"
#include "PNMX_Installer.h"
#include <string.h>				// _stricmp
#include <stdio.h>				// files
#include <stdlib.h>				// atoi

extern	CPNMX_InstallerApp	theApp;
//extern	CString	months[12];
/*
Commands:	format						directory prefix assumed		arguments
	Download <BOMfile>					// to <deskTemp>
	ParseBOM <BOMfile>					// <deskTemp>					<BOMfile>
	CheckSpace <memReq>					// space is in bytes			<memReq>
	WebDownload <dir> <srcF> <tgtF>		// pnmx.com/Downloads/			<ranDir> <srcF> <tgtF> 
	DeviceUpload <src> <tgt>			// <deskTemp>, <devTemp>		<srcF> <tgtF>		
	CabFileInstall <cabfile>			// <devTemp>					<cabFile>
	Execute	<program>					// on the <device>
	Register <uuidFile>					// on the device 
//	DeviceDownload <dir> <srcF> <tgtF>	// same as WebDownload			<dir> <srcF> <tgtF>

	Load up the ToDo list with:
		Download <BOMfile>
		ParseBOM <BOMfile>
	and start a thread that processes the list
*/
//------------------------------------------------------------------------------------
short	ParseBOMentry( char* BOMbuffer, int& BOMoffset, char*& progName, int& size, char*& ranName )
{		// return program name, size, and source filename (within the <ranDir>
	short	retVal = 0;
		// get the progName
	char*	theLine = GetToken( BOMbuffer, BOMoffset, '\n' );
	int		tt = 0;

		// avoid stepping on memory without releasing it first
	if ( progName ) delete [] progName;
	char*	progName = GetToken( theLine, tt );
	if ( progName == NULL )
		retVal = -1;
	else
	{
		char* sizeSt = GetToken( theLine, tt );
		if ( sizeSt == NULL )
			retVal = -2;
		else
		{		// convert the sizeSt to an integer
			size = atoi( sizeSt );
			delete [] sizeSt;

				// avoid stepping on memory without releasing it first
			if ( ranName ) delete [] ranName;
			ranName = GetToken( theLine, tt );
			if ( ranName == NULL )
				retVal = -3;
		}
	}
	if ( theLine ) delete [] theLine;
	return	retVal;
}			// ParseBOMentry()
//------------------------------------------------------------------------------------
// assumes that theApp.m_DeskTempPath and theApp.m_DeviceTempPath are already defined
short	ParseBOM( char*& commandBuffer )
{	wchar_t		deskTempFile[300];
	short	retVal = 0;
	swprintf( deskTempFile, _T("%s\\PocketNumerix\\PNMX_BOM.dat"), theApp.m_DeskTempPath );

	FILE* fp = fopen( deskTempFile, "r" );
	if ( fp == NULL )		{	retVal = -1;	goto	Exit;	}

	int	res = fseek( fp, 0, SEEK_END );
	if ( res )				{	retVal = -2;	goto	Exit;	}

	long	flen = ftell( fp );
	if ( flen <= 0 )		{	retVal = -3;	goto	Exit;	}

	res = fseek( fp, 0, SEEK_SET );			// back to BOF
	if ( res )				{	retVal = -4;	goto	Exit;	}

		// read the entire BOM file into memory (for the benefit of GetToken)
	char* BOMbuffer = new char[ flen + 1 ];
	res = fread( BOMbuffer, 1, flen, fp );
	*(BOMbuffer+flen) = '\0';
	fclose( fp );

		// get the ranDir
	int	bb = 0;
	char*	ranDir = GetToken( BOMbuffer, bb );
	if ( ranDir == NULL )	{	retVal = -5;	goto	Exit;	}

		// get the memory required
	char*	memReq = GetToken( BOMbuffer, bb );
	if ( memReq == NULL )	{	retVal = -6;	goto	Exit;	}

		// get the rest of the BOM entries
	swprintf( deskTempFile, _T("%s\\PocketNumerix\\ToDo.dat"), theApp.m_DeskTempPath );
	fp = fopen( deskTempFile, "w" );
	if ( fp == NULL )		{	retVal = -7;	goto	Exit;	}

	fprintf( fp, "CheckSpace %s\n", memReq );		// update the GUI and possibly bail

	char*	progName = NULL, ranName = NULL;
	while ( 1 )
	{	int		progSize = 0;
		short retVal = ParseBOMentry( BOMbuffer, bb, progName, progSize, ranName );
		if ( progName == NULL ) break;		// nothing more to read: retVal == -1
		if ( retVal < 0 )	{	retVal = -8;	goto	Exit;	}

			// we have a BOM entry in hand
		fprintf( fp, "WebDownload %s %s %s\n", ranDir, ranName, ranName );
		fprintf( fp, "DeviceUpload %s %s.cab\n", ranName, progName );
		fprintf( fp, "CabFileInstall %s.cab\n", progName );
		if ( stricmp( progName, "GetUUID" ) == 0 )
		{		// deskTempFile is reused here as a deviceTempFilename
			sprintf( deskTempFile, "%s\\PocketNumerix\\UUID.exe\n", theApp.m_DeviceTempPath );
			fprintf( fp, "Execute %s\n", deskTempFile );		// produces /Temp/PocketNumerix/UUID
				// deskTempFile is reused here as a deviceTempFilename
			sprintf( deskTempFile, "%s\\PocketNumerix\\UUID\n", theApp.m_DeviceTempPath );
			fprintf( fp, "Register %s\n", deskTempFile );
		}
	}
	long	flen = ftell( fp );
	fclose( fp );

		// suck the entire ToDo list into the commandBuffer
	swprintf( deskTempFile, _T("%s\\PocketNumerix\\ToDo.dat"), theApp.m_DeskTempPath );
	fp = fopen( deskTempFile, "r" );
	if ( commandBuffer ) delete [] commandBuffer;

	commandBuffer = new char[ flen + 1 ];
	fread( commandBuffer, 1, flen, fp );
	*(commandBuffer+flen) = '\0';

Exit:
	if ( fp ) fclose( fp );
	if ( memReq ) delete [] memReq;
	if ( progName ) delete [] progName;
	if ( ranName ) delete [] ranName;
	if ( BOMbuffer ) delete [] BOMbuffer;
	if ( ranDir ) delete [] ranDir;
	return	retVal;
}			// ParseBOM()
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------