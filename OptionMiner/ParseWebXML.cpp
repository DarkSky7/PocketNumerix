#include "ParseWebXML.h"

LRESULT		OnParseData( WPARAM, LPARAM )
{	DWORD			retVal = 0;
	Cstring			expSt;
	CFile			fp;
	Cstring			csFileName = theApp.lpFileName;
	long			nBeforeOptions = m_nPuts + m_nCalls;

	if ( ! fp.Open( csFileName, CFile::modeRead ) )
	{
#ifdef _DEBUG
		{	Cstring	msg = theApp.lpFileName;
			TRACE( _T("OnParseData(-1): Can't open(r) '%s'\n"), msg );
		}
#endif
		wchar_t	wbuf[64];
		swprintf( wbuf, _T("Can't open(r) %s\n"), theApp.lpFileName );
		c_StatusResult.SetWindowText( wbuf );
		return  -1;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// parse phase
	c_StatusResult.SetWindowText( _T("Reading...") );
	theApp.DoWaitCursor( 1 );									// reset(-1) at Cleanup

		// title
	char*	title = NULL;
	if ( ReadPast( fp, "<title>" )  &&  GetToken( fp, "</title>", title ) )
	{	char*	invalidSymbol = "Invalid Ticker Symbol";
		bool found = ( FindSubStr( title, 0, invalidSymbol ) >= 0 );
		delete [] title;
		if ( found )
		{	Cstring cs = invalidSymbol;
			c_CompanyNameResult.SetWindowText( cs );
			c_StatusResult.SetWindowText( _T("") );
			goto	CloseFile;								// technically, a successful fetch
		}
	}
	else
	{	retVal = -2;
		goto	CloseFile;
	}

		// companyName
	char*	companyName;
	companyName = NULL;
	if ( ReadPast( fp, "<CoName>" )  &&  GetToken( fp, "</CoName>", companyName ) )
	{	Cstring cs = companyName;
		m_CompanyName = companyName;						// to Cstring conversion
		c_CompanyNameResult.SetWindowText( cs );
		delete [] companyName;
	}
	else
	{	retVal = -3;
		goto	CloseFile;
	}

		// stockPrice
	char*	stockPrice;
	stockPrice = NULL;
	if ( ReadPast( fp, "<StkPrc>" )  &&  GetToken( fp, "</StkPrc>", stockPrice ) )
	{
		HandleStockPrice( stockPrice );
		delete [] stockPrice;
	}
	else
	{	retVal = -4;
		goto	CloseFile;
	}

		// option expiration (<exp> and <date> entries)
	char*	expBuf;
	expBuf = NULL;				// can be put directly into the list control
	if ( ! ReadPast( fp, "<exp>" )  ||  ! GetToken( fp, "</exp>", expBuf ) )
	{	retVal = -5;
		goto	CloseFile;
	}
	expSt = expBuf;
	delete [] expBuf;

		// expiration housekeeping (the options list needs the coded value)
	long	codedExpiry;
	codedExpiry = MakeLongMonYr( expSt );
	short	nOptions;
	nOptions = m_nCalls + m_nPuts;

//	wchar_t*	expLit = _T("&exp");
//	bool   specificDateRequested = ( urlRequest.Find( expLit, 0 ) >= 0 );
//	bool   specificDateRequested;													// with encrypted param strings, this needs to be a dialog instance variable
//	specificDateRequested = ( FindSubStr(urlRequest, 0, "&exp") >= 0 );
	if ( ! m_specificDateRequested )
	{		// we didn't requested any specific expiration...
			// if there are any expiration dates, they're for another stock
		c_Expiration.SetRedraw( FALSE );
		if ( c_Expiration.FindstringExact( -1, expSt ) == LB_ERR )
		{
#ifdef _DEBUG
			TRACE( _T("OptionChainDialog::OnParseData: adding %s to c_Expiration\n"), expSt );
#endif
			c_Expiration.Addstring( expSt );
		}
			// any other expiration dates?
		char*	expDates = NULL;
		if ( ReadPast( fp, "<Dates>" ) )
		{	unsigned long	b4expDates = (unsigned long)fp.GetPosition();			// after "<Dates>"
			if ( GetToken( fp, "</Dates>", expDates) )
			{	unsigned long	afterDates = (unsigned long)fp.GetPosition();
				long	datesLen = afterDates - b4expDates - 8;						// 8 = strlen("</Dates>");
				if ( datesLen > 0 )
				{	long	ii = 0;													// index within expDates
					while ( 1 )
					{	ii = FindSubStr( expDates, ii, "<date>" );
						if ( ii < 0 )
							break;
						ii += 6;									// length of "<date>"
						long jj = FindSubStr( expDates, ii, "</date>" );
						if ( jj < 0 )	
							break;
						char*	aDate = ExtractToken( expDates+ii, jj - ii );
						Cstring	cs = aDate;
						delete [] aDate;
						if ( c_Expiration.FindstringExact( -1, cs ) == LB_ERR )
						{
#ifdef _DEBUG
							TRACE( _T("OptionChainDialog::OnParseData: adding %s to c_Expiration\n"), cs );
#endif
							c_Expiration.Addstring( cs );
						}
						ii = jj + 7;								// skip past the "</date>"
					}
				}
				delete [] expDates;
			}
		}
	}
	c_Expiration.SetRedraw( TRUE );							// may have to Invalidate and UpdateWindow later

		// handle the <Calls> (ii=0) and <Puts> (ii=1)
	c_OptionsList.SetRedraw( FALSE );						// inhibit redraw until all the options are inserted
	short	nOptionsLoaded;
	nOptionsLoaded = 0;
	char*	startToken[2];			// e.g. char*	startToken[2] = { "<Calls>", "<Puts>" };
	startToken[0] = "<Calls>";
	startToken[1] = "<Puts>";		// presumes that the Calls come first
	char*	endToken[2];			// e.g. char*	endToken[2] = { "</Calls>", "</Puts>" };
	endToken[0] = "</Calls>";
	endToken[1] = "</Puts>";

	short	lenEndToken[2];			// e.g. lenEndToken[2] = { 8, 7 };
	lenEndToken[0] = 8;
	lenEndToken[1] = 7;

	char*	allOpts;				// in VS'05, you can do:  char*	allOpts = NULL;
	allOpts = NULL;
	unsigned short ii;
	for ( ii = 0; ii < 2; ii++ )
	{	bool isCall = ( ii == 0 );
		bool readSuccess = ReadPast( fp, startToken[ii] );
#ifdef _DEBUG
		{	Cstring cs = startToken[ii];
			TRACE( _T("OptionChainDialog::OnParseData: ReadPast(%s) --> %s\n"), cs, (readSuccess ? _T("true") : _T("false")) );
			if ( ! readSuccess )
				AfxDebugBreak();
		}
#endif
		if ( readSuccess )
		{	unsigned long	b4Opts = (unsigned long)fp.GetPosition();				// after "<Calls>" or "<Puts>"
			bool gotToken = GetToken( fp, endToken[ii], allOpts );
#ifdef _DEBUG
			{	Cstring cs = endToken[ii];
				TRACE( _T("OptionChainDialog::OnParseData: GetToken(%s) --> %s\n"), cs, (gotToken ? _T("true") : _T("false")) );
				if ( ! gotToken )
					AfxDebugBreak();
			}
#endif
			if ( gotToken )
			{
#ifdef _DEBUG
//				{	Cstring cs = allOpts;		// the following TRACE causes an exception
//					TRACE( _T("OptionChainDialog::OnParseData: allOpts=%s\n"), cs );
//				}
#endif
				unsigned long afterOpts = (unsigned long)fp.GetPosition();
				long	optsLen = afterOpts - b4Opts - strlen(endToken[ii]);
#ifdef _DEBUG
				TRACE( _T("OptionChainDialog::OnParseData: b4Opts=%d, afterOpts=%d, optsLen=%d\n"), b4Opts, afterOpts, optsLen );
#endif
				if ( optsLen > 0 )
				{	long	jj = 0, kk;									// index within expDates
					char*	buf;
					while ( 1 )
					{	jj = FindSubStr( allOpts, jj, "<option>" );		if ( jj < 0 ) break;
						jj += 8;																// 8 = length of "<option>"
						kk = FindSubStr( allOpts, jj, "</option>" );	if ( kk < 0 ) break;
						char*	optBuf = ExtractToken( allOpts+jj, kk - jj );
						if ( optBuf == NULL )
						{
#ifdef _DEBUG
							TRACE( _T("OptionChainDialog::OnParseData: optBuf --> NULL\n") );
							AfxDebugBreak();
#endif
							break;
						}
#ifdef _DEBUG
						{	Cstring cs = optBuf;
							TRACE( _T("OptionChainDialog::OnParseData: optBuf=%s, <option> at jj=%d, </option> at kk=%d\n"), cs, jj, kk );
						}
#endif
						jj = kk;									// saves time next pass through the loop

							//-------------------------------------------------------
							// symbol
						kk = FindSubStr( optBuf, 0, "<sym>" );			if ( kk < 0 ) break;
						kk += 5;																// pass up "<sym>"
						long ll = FindSubStr( optBuf, kk, "</sym>" );	if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						Cstring	symbol = buf;
						kk = ll + 6;															// pass up "</sym>"
						delete [] buf;

							// strike price
						kk = FindSubStr( optBuf, kk, "<strk>" );		if ( kk < 0 ) break;
						kk += 6;																// pass up "<strk>"
						ll = FindSubStr( optBuf, kk, "</strk>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						float	strike;
						if ( sscanf( buf, "%f", &strike ) < 1 )
							strike = -1;
						kk = ll + 7;															// pass up "</strk>"
						delete [] buf;

							// last
						kk = FindSubStr( optBuf, kk, "<last>" );		if ( kk < 0 ) break;
						kk += 6;																// pass up "<last>"
						ll = FindSubStr( optBuf, kk, "</last>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						float	last;
						if ( sscanf( buf, "%f", &last ) < 1 )
							last = -1;
						kk = ll + 7;															// pass up "</last>"
						delete [] buf;

							// one day change
						kk = FindSubStr( optBuf, kk, "<chng>" );		if ( kk < 0 ) break;
						kk += 6;																// pass up "<chng>"
						ll = FindSubStr( optBuf, kk, "</chng>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						float	change;
						if ( sscanf( buf, "%f", &change ) < 1 )
							change = -1;
						kk = ll + 7;															// pass up "</chng>"
						delete [] buf;

							// bid
						kk = FindSubStr( optBuf, kk, "<bid>" );			if ( kk < 0 ) break;
						kk += 5;																// pass up "<bid>"
						ll = FindSubStr( optBuf, kk, "</bid>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						float	bid;
						if ( sscanf( buf, "%f", &bid ) < 1 )
							bid = -1;
						kk = ll + 6;															// pass up "</bid>"
						delete [] buf;

							// ask
						kk = FindSubStr( optBuf, kk, "<ask>" );			if ( kk < 0 ) break;
						kk += 5;																// pass up "<ask>"
						ll = FindSubStr( optBuf, kk, "</ask>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						float	ask;
						if ( sscanf( buf, "%f", &ask ) < 1 )
							ask = -1;
						kk = ll + 6;															// pass up "</ask>"
						delete [] buf;

							// volume
						kk = FindSubStr( optBuf, kk, "<vol>" );			if ( kk < 0 ) break;
						kk += 5;																// pass up "<vol>"
						ll = FindSubStr( optBuf, kk, "</vol>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						int	volume;
						if ( sscanf( buf, "%d", &volume ) < 1 )
							volume = -1;
						kk = ll + 6;															// pass up "</vol>"
						delete [] buf;

							// opening interest
						kk = FindSubStr( optBuf, kk, "<opnInt>" );		if ( kk < 0 ) break;
						kk += 8;																// pass up "<opnInt>"
						ll = FindSubStr( optBuf, kk, "</opnInt>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						int	openInt;
						if ( sscanf( buf, "%d", &openInt ) < 1 )
							openInt = -1;
						kk = ll + 9;															// pass up "</opnInt>"
						delete [] buf;

						OptionChainItemInfo*	pInfo = new OptionChainItemInfo;
						pInfo->ask = ask;
						pInfo->bid = bid;
						pInfo->change = change;
						pInfo->expiry = codedExpiry;
						pInfo->idx = c_OptionsList.GetItemCount();
						pInfo->isCall = isCall;
						pInfo->last = last;
						pInfo->openInt = openInt;
						pInfo->strike = strike;
						pInfo->symbol = symbol;
						pInfo->volume = volume;
						UpdateDB( pInfo );

						c_OptionsList.AddItem( (LPARAM)pInfo, pInfo->idx );
						nOptionsLoaded++;
						if ( ii == 0 )
							m_nCalls++;
						else
							m_nPuts++;
#ifdef _DEBUG
						TRACE( _T("OptionChainDialog::OnParseData: added %s, nOptionsLoaded=%d, m_nCalls=%d, m_nPuts=%d\n"),
								pInfo->symbol, nOptionsLoaded, m_nCalls, m_nPuts );
#endif
							//-------------------------------------------------------
						delete [] optBuf;
					}						// while option XML lines can be found
				}							// option XML lines exist
				delete [] allOpts;
			}								// GetToken( </Calls> or </Puts> ) succeeded
		}									// ReadPast( <Calls> or <Puts> ) succeeded
	}										// for ( ii = 0; ii < 2; ii++ )

		// track the (date,url) pair in m_Links
	if ( nOptionsLoaded > 0 )
	{	m_Links.SetAt( expSt, (void*)nOptions );		// the pre-load total of Calls and Puts
		c_Expiration.SetCurSel( c_Expiration.Findstring(0, expSt) );
	}
	else
	{	nOptions = m_nCalls + m_nPuts;
		if ( nOptions == 0 )
		{	int	cnt = c_Expiration.GetCount();
			for ( short ii = 0; ii < cnt; ii++ )
				c_Expiration.Deletestring( 0 );
//			c_Expiration.Clear();			// the loop above is there because Clear() doesn't work
		}
	}
	DisplayStatusLine();					// the number of calls and puts
	c_OptionsList.SetRedraw( TRUE );		// now we can let it redraw
											// downloaded file has non-zero length

CloseFile:
	fp.Close();
	theApp.DoWaitCursor(-1);

//	childThreadActive = false;			// we're not using this at the moment
//	c_Fetch.EnableWindow( TRUE );
//	c_Fetch.SetWindowText( _T("Fetch") );
	c_Expiration.EnableWindow( c_Expiration.GetCount() > 0 );
	if ( nOptionsLoaded > 0 )
		SortOptionsAndScrollTo( nBeforeOptions );

	return	retVal;
}			// OnParseData()
//-------------------------------------------------------------------------------------------------
