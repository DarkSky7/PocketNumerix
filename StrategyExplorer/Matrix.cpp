// Matrix_f.cpp: implementation of the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "NillaHedge.h"
#include "Matrix.h"
#include "Vector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

			CMatrix::CMatrix(
	unsigned short	nRows,
	unsigned short	nCols			 ) 
		: rows(nRows), cols(nCols), elems( new double*[rows] )
{	unsigned short	rr;
	elems[0] = new double[rows * cols];
		// set up dereferencing index
	for ( rr = 1; rr < rows; rr++ )
	{	elems[rr] = elems[rr-1] + cols;
	}
		// initialize the elements
	for ( rr = 0; rr < rows; rr++ )
	{	for ( unsigned short cc = 0; cc < cols; cc++ )
		{	elems[rr][cc] = 0.0;
		}
	}
}

	// transform (unary apply) constructor
			CMatrix::CMatrix(
	CMatrix&	am,
	double				func( double elem )		)
		: rows(am.rows), cols(am.cols), elems( new double*[rows] )
{	unsigned short	rr;
	elems[0] = new double[rows * cols];
		// set up dereferencing index
	for ( rr = 1; rr < rows; rr++ )
	{	elems[rr] = elems[rr-1] + cols;
	}
		// initialize the elements
	for ( rr = 0; rr < rows; rr++ )
	{	for ( unsigned short cc = 0; cc < cols; cc++ )
		{	elems[rr][cc] = func( am.elems[rr][cc] );
		}
	}
}

	// deep copy constructor
			CMatrix::CMatrix(
	CMatrix&	exmp			 )
		: rows(exmp.rows), cols(exmp.cols), elems( new double*[rows] )
{	unsigned short	rr;
	elems[0] = new double[rows * cols];
		// set up dereferencing index
	for ( rr = 1; rr < rows; rr++ )
	{	elems[rr] = elems[rr-1] + cols;
	}
		// initialize the elements
	for ( rr = 0; rr < rows; rr++ )
	{	for ( unsigned short cc = 0; cc < cols; cc++ )
		{	elems[rr][cc] = exmp.elems[rr][cc];
		}
	}
}

			CMatrix::~CMatrix( void )
{	if ( elems )
	{	delete [] (elems[0]);
		delete []  elems;
	}
}
//---------------------------------------------------------------------
double*		CMatrix::operator[](
	unsigned short	rr			  )
{		// return address of a row
	ASSERT( elems );
	ASSERT( rr < rows );
	return elems[rr];
}			// operator[]()
//--------------------------------------------------------------------
CMatrix*		CMatrix::operator*( CMatrix& right )
{	ASSERT( elems );
	ASSERT( right.elems );
	if ( cols != right.rows  &&  rows != right.cols )
		return 0;
	CMatrix*	prod = new CMatrix( rows, right.cols );
	double**		prdEl = prod->elems;
	double**		rgtEl = right.elems;
	for ( unsigned short rr = 0; rr < rows; rr++ )
		for ( unsigned short cc = 0; cc < right.cols; cc++ )
			for ( unsigned short kk = 0; kk < cols; kk++ )
				prdEl[rr][cc] += elems[rr][kk] * rgtEl[kk][cc];
	return	prod;
}			// operator*()
//--------------------------------------------------------------------
CVector*		CMatrix::operator*( CVector& right )
{	ASSERT( elems );
	ASSERT( right.elems );
	if ( cols != right.nElems )
		return 0;
	CVector*	prod = new CVector( cols );
	double*		prdEl = prod->elems;
	double*		rgtEl = right.elems;
	for ( unsigned short rr = 0; rr < rows; rr++ )
		for ( unsigned short ii = 0; ii < cols; ii++ )
			prdEl[rr] += elems[rr][ii] * rgtEl[ii];
	return	prod;
}			// operator*()
//--------------------------------------------------------------------
void		CMatrix::apply( double func( double elem ) )
{	for ( unsigned short rr = 0; rr < rows; rr++ )
	{	for ( unsigned short cc = 0; cc < cols; cc++ )
		{	elems[rr][cc] = func( elems[rr][cc] );
		}
	}
}			// apply()
//--------------------------------------------------------------------
double			CMatrix::colMax(
	unsigned short		col,
	unsigned short		startRow,
	unsigned short&		row			)
{	ASSERT( elems );
	ASSERT( col < cols );
	ASSERT( startRow < rows );
		// find the largest element in a column
	double	cM = 0.0f;
	row = startRow;
	for ( unsigned short rr = startRow; rr < rows; rr++ )
		if ( fabs(elems[rr][col]) > fabs((double)cM) )
		{	cM = elems[rr][col];
			row = rr;
		}
	return cM;
}			// colMax()
//--------------------------------------------------------------------
CMatrix*		CMatrix::transpose( void )
{	ASSERT( elems );
	CMatrix* trans = new CMatrix( cols, rows );
	double**  trnEl = trans->elems;
	for ( unsigned short rr = 0; rr < rows; rr++ )
		for ( unsigned short cc = 0; cc < cols; cc++ )
			trnEl[cc][rr] = elems[rr][cc];
	return trans;
}			// transpose()
//--------------------------------------------------------------------
void	CMatrix::swapRows(
	unsigned short	r1,
	unsigned short	r2		)
{	ASSERT( elems );
	ASSERT( r1 < rows );
	ASSERT( r2 < rows );

	for ( unsigned short cc = 0; cc < cols; cc++ )
	{	double	  tmp = elems[r1][cc];
		elems[r1][cc] = elems[r2][cc];
		elems[r2][cc] = tmp;
	}
}			// swapRows()
//--------------------------------------------------------------------
void			CMatrix::scaleRow(
	unsigned short	row,
	double			factor			)
{		// divide all elements in the row by the pivot
	for ( unsigned short cc = 0; cc < cols; cc++ )
	{	elems[row][cc] *= factor;
	}
}			// scaleRow()
//--------------------------------------------------------------------
void			CMatrix::xlateRow(
	unsigned short	row,
	unsigned short	byRow,
	double			factor			)
{		// translate row by -elems[byRow][pivotCol]
	for ( unsigned short cc = 0; cc < cols; cc++ )
		elems[row][cc] -= factor * elems[byRow][cc];
}			// xlateRow()
//--------------------------------------------------------------------
CMatrix*		CMatrix::inverse( void )
{	ASSERT( elems );
	if ( rows != cols ) return 0;
	CMatrix*	copy = new CMatrix( *this );		// create a deep copy
	CMatrix*	invs = new CMatrix( rows, cols );
	double**		invEl = invs->elems;
	unsigned short	cc, rr, bigRow;

		// initialize the inverse to an identity matrix 
	unsigned short*	rowMap = new unsigned short[ rows ];
	for ( rr = 0; rr < rows; rr++ )
	{	invEl[rr][rr] = 1.0f;		// the rest of invs is already 0.0f
		*(rowMap+rr) = rr;			// setup to track row swaps
	}

		// scan across the columns of this Matrix
	for ( cc = 0; cc < cols; cc++ )
	{		// pick the largest element in this column to pivot on
		double	pivot = copy->colMax( cc, cc, bigRow );
		if ( pivot == 0.0f )
		{		// this is a singular Matrix, there's no inverse!
			delete copy, invs;
			delete [] rowMap;
			return 0;
		}
		if ( bigRow != cc )
		{	copy->swapRows( bigRow, cc );
			invs->swapRows( bigRow, cc );
			unsigned short tmp;
			tmp = *(rowMap+cc);
			*(rowMap+cc) = *(rowMap+bigRow);
			*(rowMap+bigRow) = tmp;
		}
			// rescale this row (to get a 1 where the pivot is)
		double	ooPivot = 1.0f / pivot;
		copy->scaleRow( cc, ooPivot );
		invs->scaleRow( cc, ooPivot );

			// create zeroes above/below the pivot (cc,cc)
			// translating the rest of the row similarly
		for ( rr = 0; rr < rows; rr++ )
		{	if ( rr == cc )
				continue;							// cc is the row we just scaled
			double	factor = copy->elems[rr][cc];
			copy->xlateRow( rr, cc, factor );
			invs->xlateRow( rr, cc, factor );
		}
	}
	delete copy;									// don't need copy anymore
/*
		// now swap the rows back into their original locations
	for ( rr = 0; rr < rows; rr++ )
	{	bigRow = *(rowMap+rr);
		if ( bigRow != rr )
		{		// copy->swapRows( rr, bigRow );	// its gone now
			invs->swapRows( rr, bigRow );
				// rowMap[rr] = rr;					// won't visit rr again
			*(rowMap+bigRow) = bigRow;
		}
	}
*/
	delete [] rowMap;
	return invs;
}			// inverse()
//--------------------------------------------------------------------
/*
double			CMatrix::rowMax(
	unsigned short		row,
	unsigned short&		col			)
{	ASSERT( elems );
	ASSERT( row < rows );
		// find the largest element in a row
	double	rM = 0.0f;
	col = 0;
	for ( unsigned short cc = 0; cc < cols; cc++ )
		if ( fabs(elems[row][cc]) > (double)rM )
		{	rM = elems[row][cc];
			col = cc;
		}
	return rM;
}			// rowMax()
*/
//---------------------------------------------------------------------
/*
inline double			CMatrix::COMPL_A( double aa, double bb )
{	return bb >= 0.0 ? (aa >= 0.0 ?  aa : -aa)
					 : (aa >= 0.0 ? -aa :  aa);
}			// COMPL_A()
//--------------------------------------------------------------------
double			CMatrix::pythag( double aa, double bb )
{		// compute sqrt( a^2 + b^2 ) without underflow or overflow
	double	absA = (double)fabs((double)aa);
	double	absB = (double)fabs((double)bb);
	if ( absA > absB )
	{	if ( absA == 0.0 )
			return 0.0;
		double	rr = absB / absA;
		return absA * (double)sqrt( 1.0 + (double)rr*(double)rr );
	}
	else
	{	if ( absB == 0.0 )
			return 0.0;
		double	rr = absA / absB;
		return absB * (double)sqrt( 1.0 + (double)rr*(double)rr );
	}
}			// pythag()
//--------------------------------------------------------------------
void			CMatrix::svbksb(
//	CMatrix&	uu,
	CMatrix&	vv,
	CVector&	ww,
	CVector&	bb,
	CVector&	aa				)				// the return vector
{	CMatrix&	uu = *this;
	unsigned short	ii, jj, pp;					// NR::j --> jj,	NR::jj -> pp
	unsigned short	uuRows = uu.rows;			// NR::m
	unsigned short	uuCols = uu.cols;			// NR::n
	CVector		tmp( uuCols );
	double				ss;

	for ( jj = 0; jj < uuCols; jj++ )				// Calculate U' * B
	{	ss = 0.0;
		if ( ww[jj] != 0.0 )
		{	for ( ii = 0; ii < uuRows; ii++ )	ss += uu.elems[ii][jj] * bb.elems[ii];
			ss /= ww.elems[jj];							// this is the divide by w_j
		}
		tmp.elems[jj] = ss;
	}

	for ( jj = 0; jj < uuCols; jj++ )
	{	ss = 0.0;
		for ( pp = 0; pp < uuCols; pp++ )	ss += vv.elems[jj][pp] * tmp.elems[pp];
		aa.elems[jj] = ss;
	}
}			// svbksb()
//--------------------------------------------------------------------
	// given matrix aa, find uu (returned in aa), ww, vv, by solving A = UWV'
void			CMatrix::svdcmp(
//	CMatrix&	aa,						// A upon input, U upon return
	CMatrix&	vv,
	CVector&	ww				)
{	CMatrix&	aa = *this;				// A upon input, U upon return
	unsigned short	ii, jj, kk, ll, pp;
	unsigned short	aaRows = aa.rows, aaCols = aa.cols;
	double			aNorm = 0.0, ff, gg, hh, ss, scale;
	CVector		rv1( aaCols );
		//
		// Householder reduction to bidiagonal form...
		//
	for ( ii = 0; ii < aaCols; ii++ )
	{	ll = ii + 2;
		gg = ss = scale = 0.0;
		rv1.elems[ii] = scale * gg;									// zero on the first pass
		if ( ii < aaRows )
		{	for ( kk = ii; kk < aaRows; kk++ )	scale += (double)fabs( (double)aa.elems[kk][ii] );
			if ( scale != 0.0 )
			{	for ( kk = ii; kk < aaRows; kk++ )
				{	aa.elems[kk][ii] /= scale;
					ss += aa.elems[kk][ii] * aa.elems[kk][ii];
				}
				ff = aa[ii][ii];
				gg = - COMPL_A( (double)sqrt((double)ss), ff );					// gg = -sqrt(ss) or sqrt(ss)
				hh = ff * gg - ss;
				aa[ii][ii] = ff - gg;
				for ( jj = ll - 1; jj < aaCols; jj++ )
				{	ss = 0.0;
					for ( kk = ii; kk < aaRows; kk++ )		ss += aa.elems[kk][ii] * aa.elems[kk][jj];
					ff = ss / hh;
					for ( kk = ii; kk < aaRows; kk++ )		aa.elems[kk][jj] += ff * aa.elems[kk][ii];
				}
				for ( kk = ii; kk < aaRows; kk++ )			aa.elems[kk][ii] *= scale;
			}
		}
			// now, analogous to the if ( ii < aaRows ) above ...
			// with commented changes from that block
		ww.elems[ii] = scale * gg;
		gg = ss = scale = 0.0;
		if ( ii + 1 <= aaRows  &&  ii != aaCols )									// ( ii < aaRows )
		{	for ( kk = ll - 1; kk < aaCols; kk++ )	scale += (double)fabs( (double)aa.elems[ii][kk] );	// ( kk = ii; kk < aaRows; kk++ ) ... aa[kk][ii]
			if ( scale != 0.0 )
			{	for ( kk = ll - 1; kk < aaCols; kk++ )								// ( kk = ii; kk < aaRows; kk++ )
				{	aa.elems[ii][kk] /= scale;											// [kk][ii]
					ss += aa.elems[ii][kk] * aa.elems[ii][kk];									// [kk][ii]
				}
				ff = aa.elems[ii][ll-1];													// [ii][ll-1]
				gg = - COMPL_A( (double)sqrt((double)ss), ff );
				hh = ff * gg - ss;
				aa.elems[ii][ll-1] = ff - gg;												// [ii][ll-1]
					// much more substantive differemces below ...
				for ( kk = ll - 1; kk < aaCols; kk++ )	rv1.elems[kk] = aa.elems[ii][kk] / hh;
				for ( jj = ll - 1; jj < aaRows; jj++ )								// ( kk = ll - 1; kk < aaCols; kk++ )
				{	ss = 0.0;
					for ( kk = ll - 1; kk < aaCols; kk++ )	ss += aa.elems[jj][kk] * aa.elems[ii][kk];
					for ( kk = ll - 1; kk < aaCols; kk++ )		  aa.elems[jj][kk] += ss * rv1.elems[kk];
				}
				for ( kk = ll - 1; kk < aaCols; kk++ )	aa.elems[ii][kk] += scale;
			}
		}
		aNorm = (double)MAX( aNorm, (double)(fabs((double)ww.elems[ii]) + fabs((double)rv1.elems[ii])) );
	}
		//
		// Accumulation of right-hand transformations...
		//
	for ( ii = aaCols - 1; ii > 0; ii-- )		// MRL changed condition from ii >= 0
	{	if ( ii < aaCols - 1 )
		{	if ( gg != 0.0 )
			{	for ( jj = ll; jj < aaCols; jj++ )	vv.elems[jj][ii] = (aa.elems[ii][jj] / aa.elems[ii][ll]) / gg;
				for ( jj = ll; jj < aaCols; jj++ )
				{	ss = 0.0;
					for ( kk = ll; kk < aaCols; kk++ )	ss += aa.elems[ii][kk] * vv.elems[kk][jj];
					for ( kk = ll; kk < aaCols; kk++ )	vv.elems[kk][jj] += ss * vv.elems[kk][ii];
				}
			}
			for ( jj = ll; jj < aaCols; jj++ )	vv.elems[ii][jj] = vv.elems[jj][ii] = 0.0;
		}
		vv.elems[ii][ii] = 1.0;
		gg = rv1.elems[ii];
		ll = ii;
	}
		//
		// Accumulation of left-hand transformations...
		//
	for ( ii = min(aaRows,aaCols) - 1; ii > 0; ii-- )		// MRL changed condition from ii >= 0
	{	ll = ii + 1;
		gg = ww.elems[ii];
		for ( jj = ll; jj < aaCols; jj++ )	aa.elems[ii][jj] = 0.0f;
		if ( gg != 0.0f )
		{	gg = (double)(1.0 / (double)gg);
			for ( jj = ll; jj < aaCols; jj++ )
			{	ss = 0.0f;
				for ( kk = ll; kk < aaRows; kk++ )	ss += aa.elems[kk][ii] * aa.elems[kk][jj];
				ff = ( ss / aa.elems[ii][ii] ) * gg;
				for ( kk = ii; kk < aaRows; kk++ )	aa.elems[kk][jj] += ff * aa.elems[kk][ii];
			}
			for ( jj = ii; jj < aaRows; jj++ )	aa.elems[jj][ii] *= gg;
		}
		else for ( jj = ii; jj < aaRows; jj++ )	aa.elems[jj][ii] = 0.0;
		++(aa.elems[ii][ii]);
	}
		//
		// Diagonalization of the bidiagonal form.
		// Loop over singular values, and over allowed iterations...
		//
	for ( kk = aaCols - 1; kk > 0; kk-- )		// MRL changed condition from ii >= 0
	{	unsigned short nm;
		for ( unsigned short its = 0; its < 30; its++ )
		{	bool	flag = TRUE;
				// test for splitting?
			for ( ll = kk; ll > 0; ll-- )		// MRL changed condition from ii >= 0
			{	nm = ll - 1;					// note that rv1[0] is always zero
				if ( (double)fabs((double)rv1.elems[ll]) + aNorm == aNorm )
				{	flag = FALSE;
					break;
				}
				if ( (double)fabs((double)ww.elems[nm]) + aNorm == aNorm )
					break;
			}
			double yy, zz;
			if ( flag )
			{		// Cancellation of rv1.elems[ll], if ll > 0.
				double cc = 0.0; ss = 1.0;
				for ( ii = ll - 1; ii < kk + 1; ii++ )
				{	ff = ss * rv1[ii];
					rv1.elems[ii] *= cc;
					if ( (double)fabs((double)ff) + aNorm == aNorm )
						break;
					gg = ww.elems[ii];
					hh = pythag( ff, gg );
					ww.elems[ii] = hh;
					hh = double(1.0 / (double)hh);
					cc =   gg * hh;
					ss = - ff * hh;
					for ( jj = 0; jj < aaRows; jj++ )
					{	yy = aa.elems[jj][nm];
						zz = aa.elems[jj][ii];
						aa.elems[jj][nm] = yy * cc + zz * ss;
						aa.elems[jj][ii] = zz * cc - yy * ss;
					}
				}
			}
			zz = ww.elems[kk];
			if ( ll == kk )
			{		// Convergence
				if ( zz < 0.0 )
				{		// Singular value is made nonnegative
					ww.elems[kk] = -zz;
					for ( jj = 0; jj < aaCols; jj++ )	vv.elems[jj][kk] = - vv.elems[jj][kk];
				}
				break;
			}

				// Shift from bottom 2 by 2 mirror
			double xx = ww.elems[ll];		nm = kk - 1;		yy = ww.elems[nm];
			gg = rv1.elems[nm];		hh = rv1.elems[kk];
			ff = (double)(((yy - zz) * (yy + zz) + (gg - hh) * (gg + hh)) / (2.0 * hh * yy));
			gg = pythag( ff, 1.0 );
			ff = ( (xx - zz) * (xx + zz) + hh * ((yy / (ff + COMPL_A(gg,ff))) - hh) ) / xx;
			double	cc = ss = 1.0f;						// Next QR transformation
			for ( jj = ll; jj <= nm; jj++ )
			{	ii = jj + 1;
				gg = rv1.elems[ii];
				yy = ww.elems[ii];
				hh = ss * gg;
				gg *= cc;
				zz = pythag( ff, hh );
				rv1.elems[jj] = zz;
				cc = (double)((double)ff / (double)zz);
				ss = (double)((double)hh / (double)zz);
				ff = xx * cc + gg * ss;
				gg = gg * cc - xx * ss;
				hh = yy * ss;
				yy *= cc;
				for ( pp = 0; pp < aaCols; pp++ )
				{	xx = vv.elems[pp][jj];
					zz = vv.elems[pp][ii];
						 vv.elems[pp][jj] = xx * cc + zz * ss;
						 vv.elems[pp][ii] = zz * cc - xx * ss;
				}
				zz = pythag( ff, hh );
				ww.elems[jj] = zz;
				if ( zz )
				{	zz = (double)(1.0 / (double)zz);
					cc = ff * zz;
					ss = hh * zz;
				}
				ff = cc * gg + ss * yy;
				xx = cc * yy - ss * gg;
				for ( pp = 0; pp < aaRows; pp++ )
				{	yy = aa.elems[pp][jj];
					zz = aa.elems[pp][ii];
					aa.elems[pp][jj] = yy * cc + zz * ss;
					aa.elems[pp][ii] = zz * cc - yy * ss;
				}
			}
			rv1.elems[ll] = 0.0;
			rv1.elems[kk] = ff;
			ww.elems[kk] = xx;
		}
	}
}			// svdcmp()
//--------------------------------------------------------------------
double				CMatrix::svdfit(				// the return value is chiSq
	CVector&  xx,	CVector&  yy,	CVector&  sig,
	// CMatrix&  uu,
	CMatrix&  vv,	CVector&  ww,	CVector&	aa,
	void	func( double, CVector& )												)
{	CMatrix&  uu = *this;
	unsigned short	nDataPts = xx.nElems;
	unsigned short	nCoefs   = aa.nElems;
	unsigned short	ii, jj;
	CVector		bb(nDataPts), xxVec(nDataPts);

		// create the A matrix (in uu) for the svdcmp routine.
	for ( ii = 0; ii < nDataPts; ii++ )
	{	func( xx[ii], xxVec );
		double	ooSig = (double)(1.0 / (double)sig.elems[ii]);
			// fitVec used here
		for ( jj = 0; jj < nCoefs; jj++ )	uu.elems[ii][jj] = xxVec.elems[jj] * ooSig;
		bb.elems[ii] = yy.elems[ii] * ooSig;
	}

		// find U, V, W from A (input in uu)
	svdcmp( // uu,
			   vv, ww );
	double	wmax = ww.maxElem();

		// edit the singular values, given the TOLerance defined here
	const double	TOL = 1.0e-6f;					// NR uses 1.0e-13
	double			thresh = TOL * wmax;
	for ( jj = 0; jj < nCoefs; jj++ )
	{	if ( ww.elems[jj] < thresh )
			ww.elems[jj] = 0.0;
	}

		// back substitute B into UVW to get A, the coefficients of the objective function 
	svbksb( // uu,
			   vv, ww, bb, aa );

		// check the results, calculating chi squared
	double		chiSq = 0.0;
	for ( ii = 0; ii < nDataPts; ii++ )
	{		// func( xx[ii], xxVec );			// why do this twice?  reuse the first one
		double		sum = 0.0;
		for ( jj = 0; jj < nCoefs; jj++ )	sum += aa.elems[jj] * xxVec.elems[jj];		// used here
		double	zz = (yy[ii] - sum) / sig.elems[ii];
		chiSq += zz*zz;
	}
	return chiSq;
}			// svdfit()
//--------------------------------------------------------------------
CMatrix*	CMatrix::svdvar(
	CMatrix&	vv,
	CVector&	ww				)
{	unsigned short ii, jj, kk, wItems = ww.nElems;
		// cvm (the covariance matrix) is the return value
	CMatrix*	cvm = new CMatrix( wItems, wItems );	// heap based return matrix

	CVector wti( wItems );								// stack based
	for ( ii = 0; ii < wItems; ii++ )
	{	wti.elems[ii] = ( ww.elems[ii] == 0.0f ) ? 0.0f : 1.0f / (ww.elems[ii]*ww.elems[ii]);
	}
	for ( ii = 0; ii < wItems; ii++ )
	{	for ( jj = 0; jj < wItems; jj++ )
		{	double	sum = 0.0f;
			for ( kk = 0; kk < wItems; kk++ )
			{	sum += vv.elems[ii][kk] * vv.elems[jj][kk] * wti.elems[kk];
			}
			cvm->elems[ii][jj] = cvm->elems[jj][ii] = sum;
		}
	}
	return cvm;
}			// svdvar()
*/
//--------------------------------------------------------------------
/*
double			CMatrix::getAt(
	unsigned short	rr,
	unsigned short	cc			)
{	ASSERT( elems );
	ASSERT( rr < rows );
	ASSERT( cc < cols );
	return	elems[rr][cc];
}			// getAt()
*/
//--------------------------------------------------------------------
/*
void		CMatrix::setAt(
	double				val,
	unsigned short	rr,
	unsigned short	cc			)
{	ASSERT( elems );
	ASSERT( rr < rows );
	ASSERT( cc < cols );
	elems[rr][cc] = val;
}			// SetAt()
*/
//--------------------------------------------------------------------
/*
bool 	CMatrix::findSwapRow(
	unsigned short   inCol, 
	unsigned short   afterRow,
	unsigned short&  rowResult	 )
{	bool  found = FALSE;	// the return value indicates success or failure
	rowResult = afterRow + 1;
	while ( rowResult < rows )
	{	found = ( elems[rowResult][inCol] != 0.0 );
		if ( found )
			break;
		rowResult++;
	}
	return found;
}			// findSwapRow()
*/
//--------------------------------------------------------------------
/*
void	CMatrix::swapCols(
	unsigned short	c1,
	unsigned short	c2					)
{	for ( unsigned short rr = 0; rr < rows; rr++ )
	{	double		tmp = mat[rr][c1];
		mat[rr][c1] = mat[rr][c2];
		mat[rr][c2] = tmp;
	}
}			// swapCols()
*/
//--------------------------------------------------------------------
