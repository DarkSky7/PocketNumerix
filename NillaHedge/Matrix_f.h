// Matrix_f.h: interface for the CMatrix_f class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_f_H__0FE100B2_3577_4C1C_BB0E_99E7B3DBB952__INCLUDED_)
#define AFX_MATRIX_f_H__0FE100B2_3577_4C1C_BB0E_99E7B3DBB952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Vector_f.h"

class CMatrix_f : public CObject
{
public:
								CMatrix_f( unsigned short rr, unsigned short cc );
									// transform (unary apply) constructor
								CMatrix_f( CMatrix_f& am, float func( float elem) );
								CMatrix_f( CMatrix_f& exmp );		// deep copy
	virtual						~CMatrix_f( void );

		// operators
	float*						operator[]( unsigned short rr );	// return the address of a row
	CMatrix_f*					operator*( CMatrix_f& right );
	CMatrix_f*					operator*( CMatrix_f* right )  { return this->operator*( *right ); }

	CVector_f*					operator*( CVector_f& right );		// Matrix * col vector -> col Vector
	CVector_f*					operator*( CVector_f* right )  { return this->operator*( *right ); }

		// methods
public:
	void						apply( float func( float elem ) );	// for unary operations
	CMatrix_f*					inverse( void );
	float						colMax( unsigned short col, unsigned short startRow, unsigned short& row );
	void						scaleRow( unsigned short row, float factor );
	void						swapRows( unsigned short r1, unsigned short r2 );
	CMatrix_f*					transpose( void );
	void						xlateRow( unsigned short row, unsigned short byRow, float factor );

		// attributes
public:
	unsigned short				rows;
	unsigned short				cols;
	float**						elems;

		// utilities
protected:
};

/*
	void						svbksb(  // CMatrix_f& uu,
											CMatrix_f& vv, CVector_f& ww,
											CVector_f& bb, CVector_f& aa );

	void						svdcmp(  // CMatrix_f& aa,
											CMatrix_f& vv, CVector_f& ww );

	float						svdfit( CVector_f& xx, CVector_f& yy, CVector_f& sig,  // CMatrix_f& uu,
										CMatrix_f& vv, CVector_f& ww, CVector_f& aa,
										void func( float, CVector_f& )										);

	CMatrix_f*					svdvar( CMatrix_f& vv,  CVector_f& ww );
	float						COMPL_A( float aa, float bb );		// return -A when signs differ
//	float						MAX( float aa, float bb )	{	return aa > bb ? aa : bb; }
//	unsigned short				MIN( unsigned short aa, unsigned short bb )	{	return aa < bb ? aa : bb; }
	float						pythag( float aa, float bb );
*/

//	float						rowMax( unsigned short row, unsigned short& col );
//	float						getAt( unsigned short rr, unsigned short cc );
//	void						setAt( float val, unsigned short rr, unsigned short cc );
//	bool						findSwapRow( unsigned short inCol,
//											 unsigned short afterRow,
//											 unsigned short& rowResult );

#endif // !defined(AFX_MATRIX_f_H__0FE100B2_3577_4C1C_BB0E_99E7B3DBB952__INCLUDED_)
