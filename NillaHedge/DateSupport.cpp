#include "StdAfx.h"
#include "DateSupport.h"
//#include "Time.h"

CString		months[12] = {	_T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"),
							_T("May"), _T("Jun"), _T("Jul"), _T("Aug"),
							_T("Sep"), _T("Oct"), _T("Nov"), _T("Dec") };

double	calcYears( COleDateTime fromDate, COleDateTime toDate, long& spDays )
{
#ifdef _DEBUG
	if ( fromDate.GetStatus() != COleDateTime::valid )
	{	TRACE( _T("calcYears: fromDate is invalid or null\n") );
		AfxDebugBreak();
	}
	if ( toDate.GetStatus() != COleDateTime::valid )
	{	TRACE( _T("calcYears: toDate is invalid or null\n") );
		AfxDebugBreak();
	}
//	TRACE( _T("calcYears: from %s to %s\n"),
//		fromDate.Format(VAR_DATEVALUEONLY), toDate.Format(VAR_DATEVALUEONLY) );
#endif
	if ( fromDate == toDate )
		return	0.0;

	double	yrs = 0.0;												// the return value
	bool	reverse = ( fromDate > toDate ) != 0;
	if ( reverse )
	{		// ensure that fromDate is really before the toDate
		COleDateTime	tmp(toDate);
		toDate = fromDate;
		fromDate = tmp;
	}
		// whole years
	int	fromYr = fromDate.GetYear();
	int	toYr   = toDate.GetYear();
	yrs = toYr - fromYr - 1;
#ifdef _DEBUG
//	TRACE( _T("calcYears: from %s to %s, wholeYrs=%d\n"),
//		fromDate.Format(VAR_DATEVALUEONLY), toDate.Format(VAR_DATEVALUEONLY), wholeYrs );
#endif
	if ( yrs < 0 )
		yrs = 0;

	int	wholeYrs = (int)yrs;
	int	fromDay = fromDate.GetDayOfYear();					// XXX does GetDayOfYear() account for leap years ???
	int	toDay = toDate.GetDayOfYear();			
	int daysInFromYr = isLeapYr(fromYr)  ?  366  :  365;

	if ( fromYr == toYr )
	{	spDays = toDay - fromDay;
		yrs += (double)spDays / (double)daysInFromYr;
	}
	else
	{	int	fromYrSpDays = daysInFromYr - fromDay;
		yrs += (double)fromYrSpDays / (double)daysInFromYr;
		yrs += (double)toDay / (double)(isLeapYr(toYr)  ?  366  :  365);
		spDays = fromYrSpDays + toDay;
	}
	spDays += wholeYrs * 365 + wholeYrs / 4;				// an approximation

	if ( reverse )
		yrs = -yrs;
#ifdef _DEBUG
//	TRACE( _T("calcYears: from %s to %s, yrs=%.3f, spDays=%d\n"),
//		fromDate.Format(VAR_DATEVALUEONLY), toDate.Format(VAR_DATEVALUEONLY), yrs, spDays );
#endif
	return	yrs;
}			// calcYears()
//------------------------------------------------------------------------------------------------
long			packOleDate( COleDateTime aDate )
{
	int		yr = aDate.GetYear();				// 15 bits
	int		mo = aDate.GetMonth();				//  4 bits
	int		da = aDate.GetDay();				//  5 bits
//	yr &= 0x7FFF;								// range = 0 .. 32767
	return	(yr << 9)  |  (mo << 5)  |  da;
}			// packOleDate()
//----------------------------------------------------------------------------------------
COleDateTime	unpackOleDate( long	packedOleDate )
{	COleDateTime	res;
	int		da = packedOleDate & 0x1F;			//  5 bits
				 packedOleDate >>= 5;						// shift 5 bits off right
	int		mo = packedOleDate & 0xF;			//  4 bits
				 packedOleDate >>= 4;						// shift 4 bits off right
	int		yr = packedOleDate & 0x7FFF;		// 15 bits
	res.SetDate( yr, mo, da );
	return	res;
}			// unpackOleDate()
//----------------------------------------------------------------------------------------
CString			EuroFormat( COleDateTime aDate )
{	int da = aDate.GetDay();
	int mo = aDate.GetMonth();
	int	yr = aDate.GetYear();
	ASSERT( da > 0  &&  mo > 0  &&  yr > 0 );
	wchar_t		buf[12];
	swprintf( buf, _T("%02d%3s%4d"), da, months[mo-1], yr );
	CString	cs(buf);
	return	cs;
}			// EuroFormat()
//----------------------------------------------------------------------------------------
CString			MonYrFormat( COleDateTime aDate )
{	int mo = aDate.GetMonth();
	int	yr = aDate.GetYear();
	ASSERT( mo > 0  &&  yr > 0 );
	wchar_t		buf[6];
	swprintf( buf, _T("%3s%4d"), months[mo-1], yr );
	CString	cs(buf);
	return	cs;
}			// EuroFormat()
//----------------------------------------------------------------------------------------
long		MakeLongMonYr( CString monYr )
{
#ifdef _DEBUG
	int	len = monYr.GetLength();
	if ( len < 5 )
	{	TRACE( _T("MakeLongMonYr: GetLength(%s) --> %d\n"), monYr, len );
		AfxDebugBreak();
	}
#endif
	CString	moSt = monYr.Mid( 0, 3 );
	CString	yrSt = monYr.Mid( 3 );					// gets the remainder of monYr
	int		ii = intMonthFrom3CharStr( moSt );		// Jan is 0-based
	int		yr = 2000 + _wtoi( yrSt );
	return	(yr << 4) + ii;			// e.g. Dec05 --> 200511 (so we can sort on it)
}			// MakeLongMonYr()
//--------------------------------------------------------------------------------------
int		intMonthFrom3CharStr( CString cs_mo )
{
	for ( unsigned short ii = 0; ii < 12; ii++ )
		if ( cs_mo.CompareNoCase( months[ii] ) == 0 )		// they're equal
			return ii;										// interval [0..11]
	return -1;
}			// intMonthFrom3CharStr()
//--------------------------------------------------------------------------------------
COleDateTime	NextMonth( COleDateTime theDate )
{	COleDateTime	ret;							// the return value
		// return a date which is one month ahead of theDate
	int	yr = theDate.GetYear();
	int	mo = theDate.GetMonth() + 1;
	if ( mo > 12 )
	{	mo = 1;
		yr++;
	}
	int	da = theDate.GetDay();

		// fix the day of the month
	if ( da > 28  &&  mo == 2 )
		da = (yr % 4 == 0) ? 29 : 28;
	else if ( da > 30  &&  (mo == 4  ||  mo ==  6  ||  mo ==  9  ||  mo == 11 ) )
		da = 30;		// April, June, September, November

	ret.SetDate( yr, mo, da );
	return	ret;
}			// NextMonth()
//--------------------------------------------------------------------------------------
COleDateTime	ThirdFriday( COleDateTime theDate )
{		// return a date which is the third Friday of date's month & year
	COleDateTime	ret;							// the return value
		// GetDayOfWeek(): Sunday = 1 .. Saturday = 7, so Friday = 6
		// find a Friday in the same week
	int	someFriday = theDate.GetDay() - theDate.GetDayOfWeek() + 6;
	int	thirdFriday = 14 + someFriday % 7;
	if ( thirdFriday < 15 )
		thirdFriday += 7;

/*
		// don't have to do this here, setExpiry() will handle it
	TIME_ZONE_INFORMATION	tzi;
	int	hour = 21;								// 5:30PM Eastern Standard Time is 21:30 UTC
	DWORD	res = GetTimeZoneInformation( &tzi );
	if ( res != TIME_ZONE_ID_UNKNOWN )
		hour -= tzi.Bias / 60;
	else
		hour -= 5;							// default is Eastern Standard Time
	ret.SetDateTime( theDate.GetYear(), theDate.GetMonth(), thirdFriday, hour, 30, 0 );
*/
	ret.SetDate( theDate.GetYear(), theDate.GetMonth(), thirdFriday );
	return	ret;
}			// ThirdFriday()
//--------------------------------------------------------------------------------------
int		numLeapDays( COleDateTime d1, COleDateTime d2 )
{
	int leapDays = 0;								// return value
	COleDateTime ed = ( d1 < d2 ) ? d1 : d2;		// the earlier date
	COleDateTime ld = ( d1 < d2 ) ? d2 : d1;		// the later date
	int	ey = ed.GetYear() + 1;						// the earlier year
	int	ly = ld.GetYear() - 1;						// the later year

		// count leap years between the given dates
	int	ym1 = ly - ey - 1;
	if ( ym1 > 0 ) leapDays += (ym1 / 400) + (ym1 / 4) - (ym1 / 100);

		// if early date is a leap year and its on or before Feb-29, bump leaps
	if ( isLeapYr(ey) )
	{	COleDateTime eld( ey, 2, 29, 0, 0, 0 );
		if ( ed <= eld )
			leapDays++;
	}
		// if late date is a leap year and its on or after Feb-29, bump leaps
	if ( isLeapYr(ly) )
	{	COleDateTime lld( ly, 2, 29, 0, 0, 0 );
		if ( ld >= lld )
			leapDays++;
	}
	return 	leapDays;
}			// numLeapDays()
//----------------------------------------------------------------------------------------
COleDateTime	dayBefore( COleDateTime refDate, int yr )		// use refDate's year when yr = 0
{		// XXX forgot why this function is necessary - why not just subtract a day off the refDate?
		// useful for creating a pseudo (accrual accounting) dividend date from an Ex-Dividend date
	if ( yr == 0 ) yr = refDate.GetYear();		// valid COleDateTime's have yr >= 100, so...
	int		mo = refDate.GetMonth();			// we're not guaranteeing validity for MFC
	int		da = refDate.GetDay() - 1;			// consider divDate to be the day before exDivDate
		// Currently doesn't account for non-trading days... XXX
	if ( da < 1 )
	{	mo = (mo > 1) ? mo - 1 : yr--, 12;
		if ( mo == 2 )
			da = (yr % 4 == 0) ? 29 : 28;
		else if ( mo == 4  ||  mo ==  6  ||  mo ==  9  ||  mo == 11 )
			da = 30;		// April, June, September, November
		else				// mo == 1, 3, 5, 7, 8, 10, 12
			da = 31;
	}
	COleDateTime	dayBefore;			// return value
	dayBefore.SetDate( yr, mo, da );
	return	dayBefore;
}			// dayBefore()
//----------------------------------------------------------------------------------------------------
