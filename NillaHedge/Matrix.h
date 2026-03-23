// Matrix.h: interface for the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__0FE100B2_3577_4C1C_BB0E_99E7B3DBB952__INCLUDED_)
#define AFX_MATRIX_H__0FE100B2_3577_4C1C_BB0E_99E7B3DBB952__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVector;

class CMatrix : public CObject
{
public:
						CMatrix( unsigned short rr, unsigned short cc );
							// transform (unary apply) constructor
						CMatrix( CMatrix& am, double func( double elem) );
						CMatrix( CMatrix& exmp );		// deep copy
	virtual				~CMatrix( void );

		// operators
	double*				operator[]( unsigned short rr );	// return the address of a row
	CMatrix*			operator*( CMatrix& right );
	CMatrix*			operator*( CMatrix* right )  { return this->operator*( *right ); }

	CVector*			operator*( CVector& right );		// Matrix * col vector -> col Vector
	CVector*			operator*( CVector* right )  { return this->operator*( *right ); }

		// methods
public:
	void				apply( double func( double elem ) );	// for unary operations
	CMatrix*			inverse( void );
	double				colMax( unsigned short col, unsigned short startRow, unsigned short& row );
	void				scaleRow( unsigned short row, double factor );
	void				swapRows( unsigned short r1, unsigned short r2 );
	CMatrix*			transpose( void );
	void				xlateRow( unsigned short row, unsigned short byRow, double factor );

		// attributes
public:
	unsigned short		rows;
	unsigned short		cols;
	double**			elems;

		// utilities
protected:
};

/*
	void				svbksb(  // CMatrix& uu,
									CMatrix& vv, CVector& ww,
									CVector& bb, CVector& aa );

	void				svdcmp(  // CMatrix& aa,
									CMatrix& vv, CVector& ww );

	double				svdfit( CVector& xx, CVector& yy, CVector& sig,  // CMatrix& uu,
								CMatrix& vv, CVector& ww, CVector& aa,
								void func( double, CVector& )										);

	CMatrix*			svdvar( CMatrix& vv,  CVector& ww );
	double				COMPL_A( double aa, double bb );		// return -A when signs differ
//	double				MAX( double aa, double bb )	{	return aa > bb ? aa : bb; }
//	unsigned short		MIN( unsigned short aa, unsigned short bb )	{	return aa < bb ? aa : bb; }
	double				pythag( double aa, double bb );
*/

//	double				rowMax( unsigned short row, unsigned short& col );
//	double				getAt( unsigned short rr, unsigned short cc );
//	void				setAt( double val, unsigned short rr, unsigned short cc );
//	bool				findSwapRow( unsigned short inCol,
//									 unsigned short afterRow,
//									 unsigned short& rowResult );

#endif // !defined(AFX_MATRIX_H__0FE100B2_3577_4C1C_BB0E_99E7B3DBB952__INCLUDED_)
