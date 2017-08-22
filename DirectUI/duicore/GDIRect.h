//
//		CGDIRect
//
//
//
//		Written by Jason Hattingh
//
//		http://www.greystonefx.com
//
//		jhattingh@greystonefx.com
//
//
//
//
//		Use freely...
//
//
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIRECT_H__2AC1B0EE_504D_4BF8_B567_0766B3555F88__INCLUDED_)
#define AFX_GDIRECT_H__2AC1B0EE_504D_4BF8_B567_0766B3555F88__INCLUDED_

#include <gdiplus.h>
//using namespace Gdiplus;

//#pragma message("     _Adding library: gdiplus.lib" ) 
#pragma comment(lib, "gdiplus.lib")

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGDIRect
{
public:
	//
	// Construction / Destruction:
	//

	CGDIRect( int nValue = 0 )				{	Assimilate( nValue );	};

	CGDIRect( Gdiplus::REAL fValue )		{	Assimilate( fValue );	};

	CGDIRect( Gdiplus::Rect rcInit )		{	Assimilate( rcInit );	};

	CGDIRect( Gdiplus::RectF rcInit )		{	Assimilate( rcInit );	};

	CGDIRect( RECT rcInit )				{	Assimilate( rcInit );	};

	CGDIRect( POINT point, SIZE size )	{	Assimilate( point.x, point.y, point.x + size.cx, point.y + size.cy );			};

	CGDIRect( Gdiplus::Point point, Gdiplus::Size size )		{	Assimilate( point.X, point.Y, point.X + size.Width, point.Y + size.Height );	};

	CGDIRect( Gdiplus::PointF point, Gdiplus::SizeF size )	{	Assimilate( point.X, point.Y, point.X + size.Width, point.Y + size.Height );	};

	CGDIRect( int nLeft, int nTop, int nRight, int nBottom )	
	{	
		Assimilate( nLeft, nTop, nRight, nBottom );
	};

	CGDIRect( Gdiplus::REAL Left, Gdiplus::REAL Top, Gdiplus::REAL Right, Gdiplus::REAL Bottom )	
	{	
		Assimilate( Left, Top, Right, Bottom );
	};

	virtual ~CGDIRect()			{							};


	//
	// Operators:
	//

	/////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CGDIRect& operator=( CGDIRect& rhs )
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		return Assimilate( rhs.left, rhs.top, rhs.right, rhs.bottom );
	};

	/////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CGDIRect& operator|=( CGDIRect& rhs )
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	Join with rhs in holy matrimony ( Union )
	//
	{
		Gdiplus::Rect u;

		if ( u.Union( u, *this, rhs ) )
			return Assimilate( u );
		else
			return Assimilate( 0 );
	};

	/////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CGDIRect& operator&=( CGDIRect& rhs )
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	Until intersections do us part...
	//
	{
		Gdiplus::Rect i;

		if ( i.Intersect( i, *this, rhs ) )
			return Assimilate( i );
		else
			return Assimilate( 0 );
	};

	/////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CGDIRect& operator=( int nValue )
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		return Assimilate( nValue );
	};

	/////////////////////////////////////////////////////////////////////////////////////////// Function Header
	CGDIRect& operator=( Gdiplus::REAL Value )
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		return Assimilate( Value );
	};

	//
	//		A d d i t i o n   O v e r l o a d s
	//

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator+( RECT& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	left	+ (Gdiplus::REAL)rhs.left,
								top		+ (Gdiplus::REAL)rhs.top, 
								right	+ (Gdiplus::REAL)rhs.right,
								bottom	+ (Gdiplus::REAL)rhs.bottom );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator+( Gdiplus::Rect& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	left	+ (Gdiplus::REAL)rhs.GetLeft(),
								top		+ (Gdiplus::REAL)rhs.GetTop(),
								right	+ (Gdiplus::REAL)rhs.GetRight(),
								bottom	+ (Gdiplus::REAL)rhs.GetBottom() );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator+( Gdiplus::RectF& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	left	+ rhs.GetLeft(),
								top		+ rhs.GetTop(),
								right	+ rhs.GetRight(),
								bottom	+ rhs.GetBottom() );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator+( CGDIRect& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	left	+ rhs.left,
								top		+ rhs.top, 
								right	+ rhs.right,
								bottom	+ rhs.bottom );
		};

		CGDIRect& operator+=( RECT& rhs )				{ return Assimilate( CGDIRect(*this) + CGDIRect(rhs) );			};

		CGDIRect& operator+=( Gdiplus::Rect& rhs )		{ return Assimilate( CGDIRect(*this) + CGDIRect(rhs) );			};
		
		CGDIRect& operator+=( Gdiplus::RectF& rhs )		{ return Assimilate( CGDIRect(*this) + CGDIRect(rhs) );			};
		
		CGDIRect& operator+=( CGDIRect& rhs )			{ return Assimilate( CGDIRect(*this) + CGDIRect(rhs) );			};

		CGDIRect& operator+=( int nValue )				{ return Assimilate( CGDIRect(*this) + CGDIRect(nValue) );		};

		CGDIRect& operator+=( Gdiplus::REAL fValue )	{ return Assimilate( CGDIRect(*this) + CGDIRect(fValue) );		};

	//
	//		S u b t r a c t i o n   O v e r l o a d s
	//
	
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator-( RECT& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	left	- (Gdiplus::REAL)rhs.left,
								top		- (Gdiplus::REAL)rhs.top, 
								right	- (Gdiplus::REAL)rhs.right,
								bottom	- (Gdiplus::REAL)rhs.bottom );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator-( Gdiplus::Rect& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	left	- (Gdiplus::REAL)rhs.GetLeft(),
								top		- (Gdiplus::REAL)rhs.GetTop(),
								right	- (Gdiplus::REAL)rhs.GetRight(),
								bottom	- (Gdiplus::REAL)rhs.GetBottom() );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator-( Gdiplus::RectF& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	left	- rhs.GetLeft(),
								top		- rhs.GetTop(),
								right	- rhs.GetRight(),
								bottom	- rhs.GetBottom() );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		CGDIRect& operator-( CGDIRect& rhs )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			return Assimilate(	left	- rhs.left,
								top		- rhs.top, 
								right	- rhs.right,
								bottom	- rhs.bottom );
		};

		CGDIRect& operator-=( RECT& rhs )				{ return Assimilate( CGDIRect(*this) - CGDIRect(rhs) );			};

		CGDIRect& operator-=( Gdiplus::Rect& rhs )		{ return Assimilate( CGDIRect(*this) - CGDIRect(rhs) );			};
		
		CGDIRect& operator-=( Gdiplus::RectF& rhs )		{ return Assimilate( CGDIRect(*this) - CGDIRect(rhs) );			};
		
		CGDIRect& operator-=( CGDIRect& rhs )			{ return Assimilate( CGDIRect(*this) - CGDIRect(rhs) );			};

		CGDIRect& operator-=( int nValue )				{ return Assimilate( CGDIRect(*this) - CGDIRect(nValue) );		};

		CGDIRect& operator-=( Gdiplus::REAL fValue )	{ return Assimilate( CGDIRect(*this) - CGDIRect(fValue) );		};

	//
	//	C o m p a r i s o n   O v e r l o a d s :
	//

		// Is Equal To?
		bool operator==( Gdiplus::REAL Value )		{ return top == Value  && bottom == Value  && left == Value  && right == Value;				};
		bool operator==( int nValue )				{ return top == nValue && bottom == nValue && left == nValue && right == nValue;			};
		bool operator==( CGDIRect& rhs )			{ return top == rhs.top && bottom == rhs.bottom && left == rhs.left && right == rhs.right;	};
		bool operator==( RECT& rhs )				{ return CGDIRect(rhs) == *this;	};
		bool operator==( Gdiplus::Rect&  rhs )		{ return CGDIRect(rhs) == *this;	};
		bool operator==( Gdiplus::RectF& rhs )		{ return CGDIRect(rhs) == *this;	};

		// Is Different To?

		bool operator!=( Gdiplus::REAL Value )		{ return top != Value || bottom != Value || left != Value || right != Value;				};
		bool operator!=( int nValue )				{ return top != nValue || bottom != nValue || left != nValue || right != nValue;			};
		bool operator!=( CGDIRect& rhs )			{ return top != rhs.top || bottom != rhs.bottom	|| left	!= rhs.left || right != rhs.right;	};
		bool operator!=( RECT& rhs )				{ return CGDIRect(rhs) != *this;	};
		bool operator!=( Gdiplus::Rect&  rhs )		{ return CGDIRect(rhs) != *this;	};
		bool operator!=( Gdiplus::RectF& rhs )		{ return CGDIRect(rhs) != *this;	};

	//
	//	S i z e   a n d   P o s i t i o n  :
	//

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void ReplicateBelow( CGDIRect rcSource, Gdiplus::REAL nOffset = 0)
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			top		= rcSource.bottom + nOffset;

			left	= rcSource.left;

			SetSize( rcSource );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void ReplicateAbove( CGDIRect rcSource, Gdiplus::REAL Offset = 0)
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			top		= rcSource.top - Height() - Offset;

			left	= rcSource.left;

			SetSize( rcSource );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void ReplicateLeft( CGDIRect rcSource, Gdiplus::REAL Offset = 0)
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			top		= rcSource.top;

			left	= rcSource.left - rcSource.Width() - Offset;

			SetSize( rcSource );
		};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void ReplicateRight( CGDIRect rcSource, Gdiplus::REAL Offset = 0)
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{
			top		= rcSource.top;

			left	= rcSource.right + rcSource.Width() + Offset;

			SetSize( rcSource );
		};

		void ReplicateBelow( CGDIRect rcSource, int nOffset = 0)	{ ReplicateBelow( rcSource, (Gdiplus::REAL)nOffset);	};

		void ReplicateAbove( CGDIRect rcSource, int nOffset = 0)	{ ReplicateAbove( rcSource, (Gdiplus::REAL)nOffset);	};

		void ReplicateLeft( CGDIRect rcSource, int nOffset = 0)		{ ReplicateLeft( rcSource, (Gdiplus::REAL)nOffset);	};

		void ReplicateRight( CGDIRect rcSource, int nOffset = 0)	{ ReplicateRight( rcSource, (Gdiplus::REAL)nOffset);	};

		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetSize( CGDIRect rcSource )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetWidth(	rcSource.Width() );
			SetHeight(	rcSource.Height() );
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetSize( SIZE size )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetWidth(	size.cx );
			SetHeight(	size.cy );
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetSize( Gdiplus::Size size )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetWidth(	size.Width );
			SetHeight(	size.Height );
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetSize( Gdiplus::SizeF size )
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetWidth(	size.Width );
			SetHeight(	size.Height );
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetWidth( Gdiplus::REAL nValue, bool bMaintainRight=false )			
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			if (bMaintainRight)
				left	= right - nValue;
			else
				right	= left + nValue;
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetHeight( Gdiplus::REAL Value, bool bMaintainBottom=false )			
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			if (bMaintainBottom)
				top		= bottom - Value;
			else
				bottom	= top + Value;
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetWidth( int nValue, bool bMaintainRight=false )			
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetWidth( (Gdiplus::REAL)nValue, bMaintainRight );
		};
		
		/////////////////////////////////////////////////////////////////////////////////////////// Function Header
		void		SetHeight( int nValue, bool bMaintainBottom=false )			
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		{ 
			SetHeight( (Gdiplus::REAL)nValue, bMaintainBottom );
		};

		Gdiplus::REAL		Width()		{ return right - left;			};
		Gdiplus::REAL		Height()	{ return bottom - top;			};

		int			WidthInt()	{ return (int)(right - left);	};
		int			HeightInt()	{ return (int)(bottom - top);	};

		operator	RECT()				{ return CRect( (int)left, (int)top, (int)right, (int)bottom );				};
		operator	Gdiplus::RectF()	{ return Gdiplus::RectF( left, top, Width(), Height() );								};
		operator	Gdiplus::Rect()		{ return Gdiplus::Rect(  (int)left, (int)top, (int)Width(), (int)Height() );			};

		operator	POINT()				{ return CPoint((int)left, (int)top );										};
		operator	Gdiplus::Point()	{ return Gdiplus::Point((int)left, (int)top );										};
		operator	Gdiplus::PointF()	{ return Gdiplus::PointF((Gdiplus::REAL)left, (Gdiplus::REAL)top );									};

		operator	SIZE()				{ return CSize( WidthInt(),	HeightInt() );									};
		operator	Gdiplus::Size()		{ return Gdiplus::Size(	WidthInt(),	HeightInt() );									};
		operator	Gdiplus::SizeF()	{ return Gdiplus::SizeF(	Width(),	Height() );										};

		//		Inflate and Deflate Methods:

		void	InflateWidth(Gdiplus::REAL x )	{ left -= x;			right += x;							};
		void	InflateWidthInt( int nX )		{ left -= (Gdiplus::REAL)nX;		right += (Gdiplus::REAL)nX;					};

		void	InflateHeight( Gdiplus::REAL y )			{ top -= y;				bottom += y;						};
		void	InflateHeightInt( int nY )		{ top -= nY;			bottom += (Gdiplus::REAL)nY;					};

		void	Inflate( SIZE Size )			{ InflateWidthInt( Size.cx );	  InflateHeightInt( Size.cy );	    };
		void	Inflate( Gdiplus::Size  Size )			{ InflateWidthInt( Size.Width ); InflateHeightInt( Size.Height );	};
		void	Inflate( Gdiplus::SizeF Size )			{ InflateWidth( Size.Width ); InflateHeight( Size.Height );	};
		void	Inflate( Gdiplus::REAL X, Gdiplus::REAL Y)		{ InflateWidth( X );		InflateHeight( Y );				};
		void	Inflate( Gdiplus::REAL Val)				{ InflateWidth( Val );		InflateHeight( Val );			};
		void	InflateInt( int nX, int nY)		{ InflateWidthInt( nX );	InflateHeightInt( nY );			};
		void	InflateInt( int nVal)			{ InflateWidthInt( nVal );	InflateHeightInt( nVal );		};

		void	Deflate( SIZE Size )			{ DeflateWidthInt( Size.cx ); InflateHeightInt( Size.cy );	};
		void	Deflate( Gdiplus::Size  Size )			{ DeflateWidthInt( Size.Width ); InflateHeightInt( Size.Height );	};
		void	Deflate( Gdiplus::SizeF Size )			{ DeflateWidth( Size.Width ); InflateHeight( Size.Height );	};
		void	Deflate( Gdiplus::REAL X, Gdiplus::REAL Y )		{ DeflateWidth( X );		DeflateHeight( Y );				};
		void	Deflate( Gdiplus::REAL Val)				{ DeflateWidth( Val );		DeflateHeight( Val );			};
		void	DeflateInt( int nX, int nY )	{ DeflateWidthInt( nX );	DeflateHeightInt(nY);			};
		void	DeflateInt( int nVal)			{ DeflateWidthInt( nVal );	DeflateHeightInt(nVal);			};

		void	DeflateWidth(  Gdiplus::REAL X )			{ left += X;			right -= X;							};
		void	DeflateWidthInt(  int nX )		{ left += (Gdiplus::REAL)nX;		right -= (Gdiplus::REAL)nX;					};

		void	DeflateHeight( Gdiplus::REAL Y )			{ top += Y;				bottom -= Y;						};
		void	DeflateHeightInt( int nY )		{ top += (Gdiplus::REAL)nY;		bottom -= (Gdiplus::REAL)nY;					};

		//		Offset, Extend and Collapse

		void	Offset(		Gdiplus::REAL nX, Gdiplus::REAL nY )	{ Assimilate( left+nX, top+nY, right+nX, bottom+nY);							};
		void	OffsetInt(	int nX, int nY )	{ Assimilate( left+(Gdiplus::REAL)nX, top+(Gdiplus::REAL)nY, right+(Gdiplus::REAL)nX, bottom+(Gdiplus::REAL)nY);	};

		void	Extend(		Gdiplus::REAL nX, Gdiplus::REAL nY )	{ Assimilate( left, top, right+nX, bottom+nY);									};
		void	ExtendInt( int nX, int nY )		{ Assimilate( left, top, right+(Gdiplus::REAL)nX, bottom+(Gdiplus::REAL)nY);						};

		void	Collapse(	 Gdiplus::REAL nX, Gdiplus::REAL nY )	{ Assimilate( left, top, right-nX, bottom-nY);									};
		void	CollapseInt( int nX, int nY )	{ Assimilate( left, top, right-(Gdiplus::REAL)nX, bottom-(Gdiplus::REAL)nY);						};

	//
	//	P o i n t   R e t r i e v a l:
	//

		POINT	TopLeftCPoint()			{ return (POINT)(*this);							};
		Gdiplus::Point	TopLeftPoint()			{ return (Gdiplus::Point)(*this);							};
		Gdiplus::PointF	TopLeftPointF()			{ return (Gdiplus::PointF)(*this);							};

		POINT	TopRightCPoint()		{ return CPoint((int)right, (int)top);				};
		Gdiplus::Point	TopRightPoint()			{ return Gdiplus::Point((int)right, (int)top);				};
		Gdiplus::PointF	TopRightPointF()		{ return Gdiplus::PointF(right, top);						};

		POINT	BottomRightCPoint()		{ return CPoint((int)right, (int)bottom);			};
		Gdiplus::Point	BottomRightPoint()		{ return Gdiplus::Point((int)right, (int)bottom);			};
		Gdiplus::PointF	BottomRightPointF()		{ return Gdiplus::PointF(right, bottom);						};

		POINT	BottomLeftCPoint()		{ return CPoint((int)left, (int)bottom);			};
		Gdiplus::Point	BottomLeftPoint()		{ return Gdiplus::Point((int)left, (int)bottom);				};
		Gdiplus::PointF	BottomLeftPointF()		{ return Gdiplus::PointF(left, bottom);						};

	//
	//		Various Others:
	//

		bool	HitTest( POINT point )	{ return CRect(*this).PtInRect( point )==TRUE;						};
		bool	HitTest( Gdiplus::Point  point )	{ return CRect(*this).PtInRect( CPoint(point.X, point.Y) )==TRUE;	};
		bool	HitTest( Gdiplus::PointF point )	{ return CRect(*this).PtInRect( CPoint((int)point.X, (int)point.Y) )==TRUE;	};

		//void	Dump()
		//{
		//	TRACE0("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\nCGDIRect Values:\n");
		//	TRACE("\ttop:\t%.2f",		top);
		//	TRACE("\tleft:\t%.2f\n",	left);
		//	TRACE("\tright:\t%.2f\t",	right);
		//	TRACE("\tbottom:\t%.2f\n",	bottom);
		//	TRACE("\tWidth:\t%d",		WidthInt());
		//	TRACE("\tHeight:\t%d\n\n",	HeightInt());
		//	TRACE0("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
		//}

	//
	//		Private Methods:
	//
private:

	CGDIRect& Assimilate( Gdiplus::Point location, Gdiplus::Size size)		
	{
		return Assimilate( Gdiplus::Rect( location, size ) );
	};

	CGDIRect& Assimilate( POINT location, SIZE size)		
	{
		return Assimilate( CRect( location, size ) );
	};

	CGDIRect& Assimilate( int nValue )		
	{	
		top		= (Gdiplus::REAL)nValue; 
		bottom	= (Gdiplus::REAL)nValue; 
		left	= (Gdiplus::REAL)nValue; 
		right	= (Gdiplus::REAL)nValue; 

		return	*this;
	};

	CGDIRect& Assimilate( Gdiplus::REAL fValue )		
	{	
		top		= fValue; 
		bottom	= fValue;
		left	= fValue;
		right	= fValue;

		return	*this;
	};

	CGDIRect& Assimilate( RECT rcData )		
	{	
		top		= (Gdiplus::REAL)rcData.top; 
		bottom	= (Gdiplus::REAL)rcData.bottom; 
		left	= (Gdiplus::REAL)rcData.left; 
		right	= (Gdiplus::REAL)rcData.right; 

		return	*this;
	};

	CGDIRect& Assimilate( Gdiplus::REAL rleft, Gdiplus::REAL rtop, Gdiplus::REAL rright, Gdiplus::REAL rbottom)		
	{	
		top		= rtop; 
		bottom	= rbottom; 
		left	= rleft; 
		right	= rright; 

		return	*this;
	};

	CGDIRect& Assimilate( int nleft, int ntop, int nright, int nbottom)		
	{	
		top		= (Gdiplus::REAL)ntop; 
		bottom	= (Gdiplus::REAL)nbottom; 
		left	= (Gdiplus::REAL)nleft; 
		right	= (Gdiplus::REAL)nright; 

		return	*this;
	};

	CGDIRect& Assimilate( Gdiplus::Rect rcData )		
	{	
		top		= (Gdiplus::REAL)rcData.GetTop(); 
		bottom	= (Gdiplus::REAL)rcData.GetBottom(); 
		left	= (Gdiplus::REAL)rcData.GetLeft(); 
		right	= (Gdiplus::REAL)rcData.GetRight(); 

		return	*this;
	};

	CGDIRect& Assimilate( Gdiplus::RectF rcData )		
	{	
		top		= rcData.GetTop(); 
		bottom	= rcData.GetBottom(); 
		left	= rcData.GetLeft(); 
		right	= rcData.GetRight(); 

		return	*this;
	};

	CGDIRect& Assimilate( CGDIRect& rcData )		
	{	
		top		= rcData.top; 
		bottom	= rcData.bottom; 
		left	= rcData.left; 
		right	= rcData.right; 

		return	*this;
	};

	//
	//		V a r i a b l e s
	//
public:

	Gdiplus::REAL	top,
					bottom,
					left,
					right;
};

#endif // !defined(AFX_GDIRECT_H__2AC1B0EE_504D_4BF8_B567_0766B3555F88__INCLUDED_)
