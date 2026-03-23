#if !defined(AFX_DATESUPPORT_H__518255E5_78A2_4945_B7D9_9769EA17488C__INCLUDED_)
#define AFX_DATESUPPORT_H__518255E5_78A2_4945_B7D9_9769EA17488C__INCLUDED_

inline	bool	isLeapYr( int yr )
				{  return !(yr % 400)  ||  (!(yr % 4)  &&  (yr % 100));		}

double			calcYears( COleDateTime fromDate, COleDateTime toDate, long& spDays );
COleDateTime	dayBefore( COleDateTime refDate, int yr );
CString			EuroFormat( COleDateTime aDate );
int				intMonthFrom3CharStr( CString cs_mo );
CString			MonYrFormat( COleDateTime aDate );
long			MakeLongMonYr( CString monYr );
COleDateTime	NextMonth( COleDateTime theDate );
int				numLeapDays( COleDateTime d1, COleDateTime d2 );
long			packOleDate( COleDateTime aDate );
COleDateTime	ThirdFriday( COleDateTime theDate );
COleDateTime	unpackOleDate( long	packedOleDate );

#endif