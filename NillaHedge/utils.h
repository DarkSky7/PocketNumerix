// utils.h
long			PackOleDate( COleDateTime aDate );
COleDateTime	UnpackOleDate( long	packedOleDate );
double			expRand( double lambda );
char*			RandomizeParams( char* osVer, DWORD dv, char* uuid, char* huid, char* stk, char* expBuf );
// double		BoxMuller( void );
//HWND			GlobalGetFocus( void );
//HWND			PFWGetFocus( void );
