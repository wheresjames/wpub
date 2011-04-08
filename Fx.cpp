/*******************************************************************
// Copyright (c) 2002, 2003, 2004, 2005, 
// Robert Umbehant
// mailto:rumbehant@wheresjames.com
//
// This program is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as 
// published by the Free Software Foundation; either version 2 of 
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public 
// License along with this program; if not, write to the Free 
// Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA 02111-1307 USA
//
*******************************************************************/
// Fx.cpp: implementation of the CFx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "Fx.h"

#include "MainFrm.h"

#include <math.h>

#include "ImgEffects.h"
#include "VideoList.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static char s_days[][ 8 ] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static char s_months[][ 8 ] = {	"Inv",	"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
								"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char s_fdays[][ 16 ] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
static char s_fmonths[][ 16 ] = {	"Inv",	"January", "Febuary", "March", "April", "May", "June", 
									"July", "August", "September", "October", "November", "December" };

#define		PI		((double)3.141592654)	
#define		PI2		((double)2 * PI )	

static char *g_olddates[] =
{
	"%w, %b %D, %Y",
	"%W, %B %D, %Y",
	"%c-%d-%Y",
	"%c-%d-%y",
	"%d-%c-%Y",
	"%d-%c-%y",
	"%Y.%c.%d",
	NULL,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFx::CFx()
{
	m_dwSize = 0;
	m_pHead = NULL;
	m_pTail = NULL; 

	*m_szFileName = 0;

	m_dwInsert = FXTYPE_NONE;

	ZeroMemory( &m_rectScreen, sizeof( m_rectScreen ) );
}

CFx::~CFx()
{
	Destroy();
}

void CFx::Destroy()
{
	m_dwSize = 0;
	m_pTail = NULL;
	while ( m_pHead != NULL )
	{	LPFX next = m_pHead->pNext;
//		if ( m_pHead->bmp != NULL )
//		{	delete m_pHead->bmp;
//			m_pHead->bmp = NULL;
//		} // end if
		if ( m_pHead->imgfx != NULL )
		{	delete m_pHead->imgfx;
			m_pHead->imgfx = NULL;
		} // end if
		if ( m_pHead->os != NULL )
		{	delete m_pHead->os;
			m_pHead->os = NULL;
		} // end if
		if ( m_pHead->nf != NULL )
		{	delete m_pHead->nf;
			m_pHead->nf = NULL;
		} // end if
		delete m_pHead;
		m_pHead = next;
	} // end while	

	*m_szFileName = 0;
	m_dwInsert = FXTYPE_NONE;
	ZeroMemory( &m_rectScreen, sizeof( m_rectScreen ) );
}


LPFX CFx::AddFx( LPFX pFx)
{
	LPFX node = new FX;
	if ( node == NULL ) return NULL;

	// Copy data
	if ( pFx == NULL ) ZeroMemory( node, sizeof( FX ) );
	else memcpy( node, pFx, sizeof( FX ) );
	
	// Don't copy objects
//	node->bmp = NULL;
	node->imgfx = NULL;
	node->nf = NULL;
	node->imgfx = NULL;

	*node->preloaded = 0;

	// Adjust rect if needed
	if ( node->rect.left == node->rect.right ||
		 node->rect.top == node->rect.bottom )
		 AdjustRect( node );

	// Copy the fx data
/*	node->type = pFx->type;
	node->f1 = pFx->f1;
	node->flags = pFx->flags;
	node->rgb = pFx->rgb;
	node->translucency = pFx->translucency;
	strcpy_sz( node->data, pFx->data );
	strcpy_sz( node->sep, pFx->sep );
	memcpy( &node->lf, &pFx->lf, sizeof( LOGFONT ) );
	memcpy( &node->rect, &pFx->rect, sizeof( RECT ) );
*/

	// Add this node to the list
	node->pNext = NULL;
	node->pPrev = m_pTail;
	if ( m_pTail == NULL ) m_pHead = node;
	else m_pTail->pNext = node;
	m_pTail = node;
	m_dwSize++;	
	
	return node;
}

BOOL CFx::RemoveFx(LPFX node, BOOL bDelete )
{
	// Find the NULL group
	if ( node == NULL ) return FALSE;

	// Remove this node from the list
	if ( node->pPrev != NULL ) node->pPrev->pNext = node->pNext;
	if ( node->pNext != NULL ) node->pNext->pPrev = node->pPrev;
	if ( m_pHead == node ) m_pHead = node->pNext;			
	if ( m_pTail == node ) m_pTail = node->pPrev;		
	
	// Delete this node
	if ( bDelete ) delete node;

	// Decrement the count
	if ( m_dwSize ) m_dwSize--;

	return TRUE;
}

BOOL CFx::MoveUp(LPFX node)
{
	// Can't move up any more if we're already first
	if ( node == NULL || node->pPrev == NULL ) return FALSE;

	// Swap us and the previous
	LPFX prev = node->pPrev;

	// Adjust head / tail pointers
	if ( m_pHead == prev ) m_pHead = node;
	if ( m_pTail == node ) m_pTail = prev;
	
	// Adjust pointers
	node->pPrev = prev->pPrev;
	if ( node->pPrev != NULL ) node->pPrev->pNext = node;
	prev->pNext = node->pNext;
	if ( prev->pNext != NULL ) prev->pNext->pPrev = prev;
	node->pNext = prev;
	prev->pPrev = node;
	
	return TRUE;
}

BOOL CFx::MoveDown(LPFX node)
{
	// Can't move down any more if we're already last
	if ( node == NULL || node->pNext == NULL ) return FALSE;

	// Swap us and the previous
	LPFX next = node->pNext;

	// Adjust head / tail pointers
	if ( m_pHead == node ) m_pHead = next;
	if ( m_pTail == next ) m_pTail = node;

	// Swap us and the next node
	node->pNext = next->pNext;
	if ( node->pNext != NULL ) node->pNext->pPrev = node;
	next->pPrev = node->pPrev;
	if ( next->pPrev != NULL ) next->pPrev->pNext = next;
	next->pNext = node;
	node->pPrev = next;

	return TRUE;
}


BOOL CFx::Draw(HDC hDC, LPRECT pRect, BOOL bPlaceholders, BOOL bAll )
{
	// Sanity checks
	if ( hDC == NULL || pRect == NULL ) return FALSE;

	// Save target rect
	SetScreenRect( pRect );

	LPFX pfx = NULL;

	while ( ( pfx = GetNext( pfx ) ) != NULL )
	{
		try
		{	ScaleFx( pfx );
			DrawFx( pfx, hDC, pRect, bPlaceholders, bAll );
		} // end try

		// Just ignore erroneous overlay elements
		catch( ... ) { ASSERT( 0 ); }

	} // end while

	return TRUE;
}

BOOL CFx::DrawFx(LPFX pFx, HDC hDC, LPRECT pRect, BOOL bPlaceholders, BOOL bAll)
{
	// Validate params
	if ( pFx == NULL || hDC == NULL || pRect == NULL ) 
		return FALSE;

	switch ( pFx->type )
	{
		case FXTYPE_TEXT :

			// Draw the text
			DrawText( pFx, hDC, pRect, bPlaceholders );
			break;

		case FXTYPE_TIME :
		{
			CWinTime wt;
			if ( ( pFx->f1 & FXF1_GMT ) != 0 ) wt.GetSystemTime();
			else wt.GetLocalTime();

			// What style clock do we want?
			if ( ( pFx->f1 & FXF1_ANALOG ) == 0 )
			{
				if ( *pFx->sep == 0 ) strcpy( pFx->sep, "%h:%m:%s %A" );

				// Create time string
				wt.GetString( pFx->data, pFx->sep );

				// Draw the time
				DrawText( pFx, hDC, pRect, bPlaceholders );

			} // end if

			// Draw analog clock
			else DrawClock( hDC, &pFx->rect, pFx, wt );

		} // end case
		break;

		case FXTYPE_DATE :
		{
			CWinTime wt;
			if ( ( pFx->f1 & FXF1_GMT ) != 0 ) wt.GetSystemTime();
			else wt.GetLocalTime();

			if ( ( pFx->f1 & FXF1_ANALOGCAL ) == 0 )
			{
				// Backward compatible
				if ( ( pFx->f1 & FXF1_DATEFORM1 ) != 0 )
				{	strcpy_sz( pFx->sep, g_olddates[ 0 ] ); }
				else if ( ( pFx->f1 & FXF1_DATEFORM2 ) != 0 )
				{	strcpy_sz( pFx->sep, g_olddates[ 1 ] ); }
				else if ( ( pFx->f1 & FXF1_DATEFORM3 ) != 0 )
				{	strcpy_sz( pFx->sep, g_olddates[ 1 ] ); }
				else if ( ( pFx->f1 & FXF1_DATEFORM4 ) != 0 )
				{	strcpy_sz( pFx->sep, g_olddates[ 1 ] ); }
				else if ( ( pFx->f1 & FXF1_DATEFORM5 ) != 0 )
				{	strcpy_sz( pFx->sep, g_olddates[ 1 ] ); }
				else if ( ( pFx->f1 & FXF1_DATEFORM6 ) != 0 )
				{	strcpy_sz( pFx->sep, g_olddates[ 1 ] ); }
				else if ( ( pFx->f1 & FXF1_DATEFORM7 ) != 0 )
				{	strcpy_sz( pFx->sep, g_olddates[ 1 ] ); }

				// Clear old date flags	
				pFx->f1 &= ~(	FXF1_DATEFORM1 | FXF1_DATEFORM2 | 
								FXF1_DATEFORM3 | FXF1_DATEFORM4 | 
								FXF1_DATEFORM5 | FXF1_DATEFORM6 | 
								FXF1_DATEFORM7 );

				// Create time string
				wt.GetString( pFx->data, pFx->sep );

				// Draw the date
				DrawText( pFx, hDC, pRect, bPlaceholders );

			} // end if

			// Draw analog calender
			else DrawAnalogCalender( pFx, hDC, pRect, wt );
		
		} // end case
		break;
	
		case FXTYPE_RECT :
			CGrDC::FillSolidRect( hDC, &pFx->rect, pFx->bck );
			break;

		case FXTYPE_IMAGE :
		case FXTYPE_WATERMARK :
		{
			BOOL tchg = bAll;
//			if ( pFx->bmp != NULL )
//			{
//				if ( pFx->bmp->GetTransparency() )
//				{	if ( ( pFx->f1 & FXF1_TRANSPARENCY ) == 0 ) tchg = TRUE; }
//				else if ( ( pFx->f1 & FXF1_TRANSPARENCY ) != 0 ) tchg = TRUE;
//
//			} // end if

			if ( tchg || 
				 ( pFx->f1 & FXF1_REALSIZE ) != ( pFx->lastflags & FXF1_REALSIZE ) ||
				 ( pFx->f1 & FXF1_BORDER ) != ( pFx->lastflags & FXF1_BORDER ) ||
				 ( pFx->f1 & FXF1_FOUNDERED ) != ( pFx->lastflags & FXF1_FOUNDERED ) ||
				 ( pFx->f1 & FXF1_NOSMOOTHING ) != ( pFx->lastflags & FXF1_NOSMOOTHING ) ||
				 ( pFx->os == NULL && CWinFile::DoesExist( pFx->data ) ) ||
				 ( pFx->os != NULL && ( pFx->rect.right - pFx->rect.left ) != (long)pFx->os->GetWidth() ) ||
				 ( pFx->os != NULL && ( pFx->rect.bottom - pFx->rect.top ) != (long)pFx->os->GetHeight() ) ||
				 ( strcmpi( pFx->data, pFx->preloaded ) ) ||
				 ( pFx->os != NULL && pFx->os->GetTransparentColor() != pFx->trans ) ||
				 ( pFx->os != NULL && pFx->os->GetPosChromaErr() != pFx->cpos ) ||
				 ( pFx->os != NULL && pFx->os->GetNegChromaErr() != pFx->cneg ) )
			{	pFx->lastflags = pFx->f1;

				CWinImg		img;

				// Lose old bitmap
				if ( pFx->os != NULL )
				{	delete pFx->os;
					pFx->os = NULL;
				} // end if

				// Draw the image
				if ( img.Load( pFx->data ) )
				{						
					RECT draw;
					CopyRect( &draw, &pFx->rect );

					// Save original width and height
					pFx->orw = img.GetWidth();
					pFx->orh = img.GetHeight();

					// Get image target size
					if ( ( pFx->f1 & FXF1_REALSIZE ) != 0 )
						SetRect( &draw, 0, 0, img.GetWidth(), img.GetHeight() );
					else
					{
						CopyRect( &draw, &pFx->rect );

						// Correct aspect ratio if needed
						if ( ( pFx->f1 & FXF1_ASPECT ) != 0 )
							CGrDC::AspectCorrect( &draw, img.GetWidth(), img.GetHeight() );

						// Correct offset
						OffsetRect( &draw, -draw.left, -draw.top );

					} // end else

					// Resample image if needed
					if ( RW( draw ) != img.GetWidth() || RH( draw ) != img.GetWidth() )
						img.Resample( &draw, 0 );

					// Apply image effects
					if ( pFx->imgfx != NULL ) 
						pFx->imgfx->DoEffects( &img );

					// Create dc object
					pFx->os = new CWinDc();
					if ( pFx->os != NULL )
					{
						// Create DC
						pFx->os->Create( hDC, &draw );

						// Copy to offscreen
						if ( pFx->os->GetSafeHdc() != NULL &&
							 img.Draw( pFx->os->GetSafeHdc(), &draw ) )
						{
							// Draw border
							DrawBorder( *pFx->os, &draw, pFx );

							strcpy_sz( pFx->preloaded, pFx->data );
							pFx->os->SetChromaErr( pFx->cpos, pFx->cneg );
							pFx->os->SetTransparentColor( pFx->trans );
							pFx->os->SetAntiAlias( ( pFx->f1 & FXF1_NOSMOOTHING ) == 0 );

						} // end if

						else _Log( MB_ICONERROR, "Image", img.GetLastError() ); 

					} // end if

				} // end if
				
				// Log error
//				else _Log( MB_ICONERROR, "Image", img.GetLastError() ); 

			} // end if

			// Fill in background if needed
			if ( ( pFx->f1 & FXF1_BCK ) != 0 )
				CGrDC::FillSolidRect( hDC, &pFx->rect, pFx->bck );

			if ( pFx->os != NULL )
			{
				RECT rect;
				SetRect( &rect, pFx->rect.left, pFx->rect.top, 
								pFx->rect.left + pFx->os->GetWidth(), 
								pFx->rect.top + pFx->os->GetHeight() );
				OffsetRect( &rect, -rect.left, -rect.top );
				CGrDC::Center( &rect, &pFx->rect );

				pFx->os->SetTransparency( ( pFx->f1 & FXF1_TRANSPARENCY ) != 0 );

				if ( pFx->translucency == 0 )
					pFx->os->Draw( hDC, &rect, FALSE );
				else pFx->os->TlDraw( hDC, &rect, pFx->translucency, FALSE );

			} // end if

			// Draw placeholder if needed
			else if ( bPlaceholders ) CGrDC::DottedBox( hDC, &pFx->rect, RGB( 255, 0, 0 ), RGB( 0, 0, 0 ) );

		} // end case
		break;

		case FXTYPE_VIDEO :
		{
			LPVIDEOINFO pvi = NULL;
			if (	( pvi = VIDEOLIST().Add( pFx->driver ) ) != NULL &&
					pvi->os != NULL && pvi->os->GetSafeHdc() != NULL )

			{
				CWinDc *src = pvi->os;

				// Used averaged frame if needed
				if ( ( pvi->f1 & VIF1_FRAMEAVG ) != 0 && pvi->tavg->GetSafeHdc() != NULL )
					src = pvi->tavg;

				// Regrab video
//				if ( bAll ) pvi->cap->GrabFrame();

				RECT draw;
				CopyRect( &draw, &pFx->rect );
//				OffsetRect( &draw, -draw.left, -draw.top );

				// Correct aspect ratio if needed
				if ( ( pFx->f1 & FXF1_ASPECT ) != 0 )
					CGrDC::AspectCorrect( &draw, src->GetWidth(), src->GetHeight() );

				if ( ( pFx->f1 & FXF1_TRANSPARENCY ) == 0 && pFx->translucency == 0 )
				{
					src->SetTransparency( FALSE );
					src->Draw( hDC, &draw );
					
					// Draw border
					DrawBorder( hDC, &draw, pFx );

				} // end if

				else
				{
					CWinDc extra;

					BOOL bStretch = !EqualRect( *src, &draw );

					if ( bStretch || ( pFx->f1 & FXF1_BORDER ) != 0 )
					{	extra.Create( *src, &draw );
						src->Draw( extra, extra );
						src = &extra;
						src->SetTransparency( FALSE );
						DrawBorder( extra, extra, pFx );		
					} // end if

					if  ( src != NULL )
					{
						src->SetTransparency( ( pFx->f1 & FXF1_TRANSPARENCY ) != 0 );
						src->SetChromaErr( pFx->cpos, pFx->cneg );
						src->SetTransparentColor( pFx->trans );
						src->SetAntiAlias( ( pFx->f1 & FXF1_NOSMOOTHING ) == 0 );

						if ( pFx->translucency == 0 )
							src->Draw( hDC, &draw, bStretch );
						else src->TlDraw( hDC, &draw, pFx->translucency, bStretch );
					} // end if

					// Draw placeholder if needed
					else if ( bPlaceholders ) CGrDC::DottedBox( hDC, &pFx->rect, RGB( 255, 0, 0 ), RGB( 0, 0, 0 ) );

				} // end else

			} // end if

			// Draw placeholder if needed
			else if ( bPlaceholders ) CGrDC::DottedBox( hDC, &pFx->rect, RGB( 255, 0, 0 ), RGB( 0, 0, 0 ) );

		} // end case
		break;
		
		case FXTYPE_SCREENSHOT :
		{
			// Capture screen if needed
//			if ( SCREENSHOT().GetSafeHdc() == NULL )

			// Get capture
			if ( pFx->os == NULL ) pFx->os = new CWinDc();			
			if ( pFx->os != NULL && ( bAll || pFx->os->GetSafeHdc() == NULL ) ) 
				CaptureScreen( pFx, pFx->os );
			
			if ( pFx->os->GetSafeHdc() != NULL )
			{
				RECT draw;
				CopyRect( &draw, &pFx->rect );

				// Correct aspect ratio if needed
				if ( ( pFx->f1 & FXF1_ASPECT ) != 0 )
					CGrDC::AspectCorrect( &draw, pFx->os->GetWidth(), pFx->os->GetHeight() );

				if ( ( pFx->f1 & FXF1_TRANSPARENCY ) == 0 && pFx->translucency == 0 )
				{

					pFx->os->Draw( hDC, &draw );

					// Draw border
					DrawBorder( hDC, &draw, pFx );

				} // end if

				else
				{
					CWinDc extra;
					CWinDc *src = pFx->os;
					BOOL bStretch = !EqualRect( *pFx->os, &draw );

					if ( bStretch || ( pFx->f1 & FXF1_BORDER ) != 0 )
					{	extra.Create( *pFx->os, &draw );
						src = &extra;
						pFx->os->Draw( extra, extra );
						DrawBorder( extra, extra, pFx );		
					} // end if

					if  ( src != NULL )
					{
						src->SetTransparency( ( pFx->f1 & FXF1_TRANSPARENCY ) != 0 );
						src->SetChromaErr( pFx->cpos, pFx->cneg );
						src->SetTransparentColor( pFx->trans );
						src->SetAntiAlias( ( pFx->f1 & FXF1_NOSMOOTHING ) == 0 );

						if ( pFx->translucency == 0 )
							src->Draw( hDC, &draw, bStretch );
						else src->TlDraw( hDC, &draw, pFx->translucency, bStretch );
					} // end if

					// Draw placeholder if needed
					else if ( bPlaceholders ) CGrDC::DottedBox( hDC, &pFx->rect, RGB( 255, 0, 0 ), RGB( 0, 0, 0 ) );

				} // end else

			} // end if

			// Draw placeholder if needed
			else if ( bPlaceholders ) CGrDC::DottedBox( hDC, &pFx->rect, RGB( 255, 0, 0 ), RGB( 0, 0, 0 ) );

		} // end case
		break;

		case FXTYPE_URL :

			if (	pFx->os == NULL || pFx->os->GetSafeHdc() == NULL || 
					strcmpi( pFx->data, pFx->preloaded ) )
				GetInternetImage( pFx );

			// Fill in background if needed
			if ( ( pFx->f1 & FXF1_BCK ) != 0 )
				CGrDC::FillSolidRect( hDC, &pFx->rect, pFx->bck );

			// Do we have something to draw?
			if ( pFx->os != NULL && pFx->os->GetSafeHdc() != NULL )
			{
				RECT draw;
				CopyRect( &draw, &pFx->rect );

				// Correct aspect ratio if needed
				if ( ( pFx->f1 & FXF1_ASPECT ) != 0 )
					CGrDC::AspectCorrect( &draw, pFx->os->GetWidth(), pFx->os->GetHeight() );

				// Set transparency mode
				pFx->os->SetTransparency( ( pFx->f1 & FXF1_TRANSPARENCY ) != 0 );
				pFx->os->SetChromaErr( pFx->cpos, pFx->cneg );
				pFx->os->SetTransparentColor( pFx->trans );
				pFx->os->SetAntiAlias( ( pFx->f1 & FXF1_NOSMOOTHING ) == 0 );

				// Draw the thing
				if ( pFx->translucency == 0 )
					pFx->os->Draw( hDC, &draw, TRUE );
				else pFx->os->TlDraw( hDC, &draw, pFx->translucency, TRUE );

				// Draw border
				DrawBorder( hDC, &draw, pFx );

			} // end if

			// Draw placeholder if needed
			else if ( bPlaceholders ) CGrDC::DottedBox( hDC, &pFx->rect, RGB( 255, 0, 0 ), RGB( 0, 0, 0 ) );

			break;

		case FXTYPE_STEXT :

			// Get special text
			GetSpecialText( pFx->driver, pFx->data, sizeof( pFx->data ) );

			// Draw the text
			DrawText( pFx, hDC, pRect, bPlaceholders );

			break;


		// Unknown fx
		default : return FALSE; break;

	} // end switch

	return TRUE;
}

BOOL CFx::DrawText(LPFX pFx, HDC hDC, LPRECT pRect, BOOL bPlaceholders )
{
	BOOL	bValid = FALSE;

	for ( DWORD i = 0; !bValid && pFx->data[ 0 ] != 0; i++ )
		if ( pFx->data[ i ] > ' ' && pFx->data[ i ] <= '~' )
			bValid = TRUE;

	CBmp		bmp;
	CWinDc	os;
	RECT		draw;
	HDC			hOs = hDC;

	CopyRect( &draw, &pFx->rect );

	// If translucency
	if ( pFx->translucency != 0 )
	{	OffsetRect( &draw, -draw.left, -draw.top );
		if( os.Create( hDC, &draw ) ) hOs = os;
	} // end if
	
	// Fill in background if needed
	if ( ( pFx->f1 & FXF1_BCK ) != 0 )
		CGrDC::FillSolidRect( hOs, &draw, pFx->bck );

	// Need to use transparency if no background
	else if ( pFx->translucency != 0 && os.GetSafeHdc() != NULL )
		CGrDC::FillSolidRect( os, os, RGB( 0, 0, 0 ) );

	{ // Draw the text
		CText	text;

		// Set text information
		text.SetFlags( pFx->flags );
		text.SetFont( &pFx->lf );

		// Draw shadow
		if ( pFx->shadow != 0 )
		{	
			RECT sh;
			CopyRect( &sh, &draw );
			OffsetRect( &sh, pFx->shadow, pFx->shadow );
			text.SetColor( RGB( 20, 20, 20 ) );
			text.DrawText( hOs, pFx->data, &sh );
		} // end if

		// Draw the text
		text.SetColor( pFx->rgb );
		text.DrawText( hOs, pFx->data, &draw );

	} // end if

	// Draw the border if needed
	DrawBorder( hOs, &draw, pFx );		

	// Draw placeholder if needed
	if ( !bValid && bPlaceholders ) CGrDC::DottedBox( hDC, &draw, RGB( 255, 0, 0 ), RGB( 0, 0, 0 ) );

	// If translucency
	if ( os.GetSafeHdc() != 0 )
	{
		// Load into bitmap
		bmp.LoadFromHDC( os );

		// Setup for trasparent draw
		if ( ( pFx->f1 & FXF1_BCK ) == 0 )
		{	bmp.SetTransparency( TRUE );
			bmp.SetTransparentColor( RGB( 0, 0, 0 ) );
			bmp.CreateMask( hDC );
		} // end if

		// Translucent draw
		bmp.TlFastDraw( hDC, &pFx->rect, pFx->translucency, TRUE );

	} // end if

	return TRUE;
}

BOOL CFx::Load(LPCTSTR pFile, BOOL bMerge)
{
	FX			fx;
	CCfgFile	cfg;

	if ( !bMerge ) Destroy();

	// Copy the file name
	strcpy_sz( m_szFileName, pFile );
	
	if ( !cfg.Load( pFile ) ) return FALSE;

	// Load each fx element
	HGROUP hGroup = NULL;
	while ( ( hGroup = cfg.GetNext( hGroup ) ) != NULL )
	{
		ZeroMemory( &fx, sizeof( fx ) );
		// Load this element
		if (	cfg.GetValue( hGroup, "Type", &fx.type ) &&
				cfg.GetValue( hGroup, "F1", &fx.f1 ) &&
				cfg.GetValue( hGroup, "Rect", &fx.rect, sizeof( fx.rect ) ) )
		{
			// Load the rest of the params
			cfg.GetValue( hGroup, "Scale", &fx.scale, sizeof( fx.scale ) );
			cfg.GetValue( hGroup, "Data", fx.data, sizeof( fx.data ) );
			cfg.GetValue( hGroup, "Format", fx.sep, sizeof( fx.sep ) );
			cfg.GetValue( hGroup, "Font", &fx.lf, sizeof( fx.lf ) );
			cfg.GetValue( hGroup, "Color", (LPDWORD)&fx.rgb );
			cfg.GetValue( hGroup, "BackgroundColor", (LPDWORD)&fx.bck );
			cfg.GetValue( hGroup, "TransparentColor", (LPDWORD)&fx.trans );
			cfg.GetValue( hGroup, "ChromaPositive", (LPDWORD)&fx.cpos );
			cfg.GetValue( hGroup, "ChromaNegative", (LPDWORD)&fx.cneg );
			cfg.GetValue( hGroup, "Flags", &fx.flags );
			cfg.GetValue( hGroup, "Flags", &fx.flags );
			cfg.GetValue( hGroup, "Shadow", (LPDWORD)&fx.shadow );
			cfg.GetValue( hGroup, "Driver", (LPDWORD)&fx.driver );
			cfg.GetValue( hGroup, "Translucency", &fx.translucency );

			// Ensure it is big enough to worry about
			if (	( fx.rect.right - fx.rect.left ) > 10 &&
					( fx.rect.bottom - fx.rect.top ) > 10 )
			{
				LPFX pfx = AddFx( &fx );
				if ( pfx != NULL && cfg.FindElement( hGroup, "ImageFx" ) )
				{	pfx->imgfx = new CImgEffects();
//					pfx->imgfx->SetFx( pfx );
					if ( pfx->imgfx != NULL ) 
						pfx->imgfx->Load( &cfg, hGroup, "ImageFx" );
				} // end if
				else pfx->imgfx = NULL;

			} // end if
		} // end if

	} // end while

	// Copy the file name
	strcpy_sz( m_szFileName, pFile );

	return TRUE;
}

BOOL CFx::Save(LPCTSTR pFile)
{
	DWORD		i = 1;
	LPFX		pfx = NULL;
	CCfgFile	cfg;
	char		gname[ 256 ];

	// For each element
	while( ( pfx = GetNext( pfx ) ) != NULL )
	{
		RECT intr;
		if (	IntersectRect( &intr, &pfx->rect, &pfx->scale ) &&
				( intr.right - intr.left ) > 5 &&
				( intr.bottom - intr.top ) > 5 )
		{
			wsprintf( gname, "Element %lu", i++ );
			HGROUP hGroup = cfg.AddGroup( gname );
			if ( hGroup != NULL )
			{
				cfg.SetValue( hGroup, "Type", pfx->type );
				cfg.SetValue( hGroup, "F1", pfx->f1 );
				cfg.SetValue( hGroup, "Rect", &pfx->rect, sizeof( pfx->rect ) );
				cfg.SetValue( hGroup, "Scale", &pfx->scale, sizeof( pfx->scale ) );
				cfg.SetValue( hGroup, "Data", pfx->data );
				cfg.SetValue( hGroup, "Format", pfx->sep );
				cfg.SetValue( hGroup, "Font", &pfx->lf, sizeof( pfx->lf ) );
				cfg.SetValue( hGroup, "Color", (DWORD)pfx->rgb );
				cfg.SetValue( hGroup, "BackgroundColor", (DWORD)pfx->bck );
				cfg.SetValue( hGroup, "TransparentColor", (DWORD)pfx->trans );
				cfg.SetValue( hGroup, "ChromaPositive", (DWORD)pfx->cpos );
				cfg.SetValue( hGroup, "ChromaNegative", (DWORD)pfx->cneg );
				cfg.SetValue( hGroup, "Flags", pfx->flags );
				cfg.SetValue( hGroup, "Shadow", (DWORD)pfx->shadow );
				cfg.SetValue( hGroup, "Driver", (DWORD)pfx->driver );
				cfg.SetValue( hGroup, "Translucency", pfx->translucency );
				if ( pfx->imgfx != NULL )
					pfx->imgfx->Save( &cfg, hGroup, "ImageFx" );
			} // end if
		} // end if
	} // end while

	// Use old file name if not supplied
	if ( pFile == NULL ) pFile = m_szFileName;

	// Save the data
	return cfg.Save( pFile );
}

BOOL CFx::Draw(LPCTSTR pBmp, LPCTSTR pNew)
{
	// Load the bitmap
	CBmp	bmp;
	if ( !bmp.Load( pBmp ) ) return FALSE;

	// Create a bitmap we can draw on
	HBITMAP	hBmp = bmp.CreateHBITMAP();
	if ( hBmp == NULL ) return FALSE;

	// Create a dc to draw on
	HDC hDesktop = ::GetDC( NULL );
	HDC hDC = CreateCompatibleDC( hDesktop );
	if ( hDesktop != NULL ) ::ReleaseDC( NULL, hDesktop );
	if ( hDC == NULL ) { DeleteObject( hBmp ); return FALSE; }

	// Select bitmap into dc
	HBITMAP hOldBmp = (HBITMAP)SelectObject( hDC, hBmp );

	// Create rectangle
	RECT rect;
	rect.left = 0; rect.right = bmp.GetWidth();	
	rect.top = 0; rect.bottom = bmp.GetHeight();

	// Let's do it
	BOOL ret = Draw( hDC, &rect );

	// Release the DC
	::SelectObject( hDC, hOldBmp );
	::DeleteDC( hDC );

	// Load the new bitmap
	if ( !bmp.Load( hBmp ) )
	{	DeleteObject( hBmp ); return FALSE; }

	// Lose the memory bitmap
	DeleteObject( hBmp );

	// Save the bitmap
	if ( pNew != NULL ) { if ( !bmp.Save( pNew ) ) return FALSE; }
	else if ( !bmp.Save( pBmp ) ) return FALSE;

	return ret;
}

HCURSOR CFx::GetInsertCursor( DWORD dwInsert )
{
	// Are we inserting anything?
	if ( dwInsert == FXTYPE_NONE ) return NULL;

	switch( dwInsert )
	{
		case FXTYPE_TEXT : 
			return APP()->LoadCursor( IDP_TEXT );	
			break;

		case FXTYPE_TIME : 
			return APP()->LoadCursor( IDP_TIME );	
			break;

		case FXTYPE_DATE : 
			return APP()->LoadCursor( IDP_DATE );	
			break;

		case FXTYPE_RECT : 
			return APP()->LoadCursor( IDP_RECT );	
			break;

		case FXTYPE_IMAGE : 
			return APP()->LoadCursor( IDP_IMAGE );	
			break;

		case FXTYPE_WATERMARK : 
			return APP()->LoadCursor( IDP_WATERMARK );	
			break;

		case FXTYPE_VIDEO : 
			return APP()->LoadCursor( IDP_VIDEO );	
			break;

		case FXTYPE_URL : 
			return APP()->LoadCursor( IDP_URL );	
			break;

		case FXTYPE_SCREENSHOT : 
			return APP()->LoadCursor( IDP_SCREENSHOT );	
			break;

		case FXTYPE_STEXT : 
			return APP()->LoadCursor( IDP_STEXT );	
			break;

	} // end switch

	return NULL;
}

LPFX CFx::HitTest(LPPOINT pt, LPFX plast)
{
	LPFX	pfx = plast;
	LPFX	found = NULL;
	DWORD 	vol = MAXDWORD;

	// For each element
	while( ( pfx = GetNext( pfx ) ) != NULL )
	{
		// Are we hitting this object?
		if ( PtInRect( &pfx->rect, *pt ) )
		{
			DWORD v = ( pfx->rect.right - pfx->rect.left ) *
						( pfx->rect.bottom - pfx->rect.top ); 

			// If we don't have a target or this one is smaller
			if ( found == NULL || v < vol )
			{	found = pfx;
				vol = v;
			} // end if

		} // end if

	} // end while

	return found;
}

BOOL CFx::VerifyFx(LPFX node)
{
	LPFX pfx = NULL;

	// For each element
	while( ( pfx = GetNext( pfx ) ) != NULL )
		if ( node == pfx ) return TRUE;

	return FALSE;
}

BOOL CFx::AdjustRect(LPFX pFx)
{
	if ( pFx == NULL ) return FALSE;

	long w = RW( pFx->rect );
	long h = RH( pFx->rect );

	// Minimum size
	if ( w < 15 ) pFx->rect.right = pFx->rect.left + 15;
	if ( h < 15 ) pFx->rect.bottom = pFx->rect.top + 15;

	// Do we want auto sizing?
	if ( ( pFx->f1 & FXF1_NOAUTOSIZE ) != 0 ) return TRUE;

	// Correct video size
	if ( pFx->type == FXTYPE_VIDEO && ( w < 15 || h < 15 ) )
	{	pFx->rect.right = pFx->rect.left + 32;
		pFx->rect.bottom = pFx->rect.top + 24;
		return TRUE;
	} // end if

	// Punt if not something we can auto-size
	if (	pFx->type != FXTYPE_TEXT &&
			pFx->type != FXTYPE_TIME && 
			pFx->type != FXTYPE_DATE ) return FALSE;

	// No text, no adjustment
	if ( *pFx->data == 0 ) return TRUE;

	HDC hDC = GetDC( NULL );
	if ( hDC == NULL ) return FALSE;

	BOOL  adj = FALSE;
	CText text;
	RECT  rect;
//	SetRect( &rect, 0, 0, 10, 10 );
	CopyRect( &rect, &pFx->rect );

	// Set text information
	text.SetFlags( pFx->flags );
	text.SetFont( &pFx->lf );

	if ( text.CalcRect( hDC, pFx->data, &rect ) )
	{
		long w = ( rect.right - rect.left ) + 16;
		long h = ( rect.bottom - rect.top ) + 4;

		if ( ( pFx->rect.right - pFx->rect.left ) < w )
		{	adj = TRUE; 
			long dif = ( w - ( pFx->rect.right - pFx->rect.left ) ) >> 1;
			pFx->rect.left -= dif; pFx->rect.right += dif;
		}

		if ( ( pFx->rect.bottom - pFx->rect.top ) < h )
		{	adj = TRUE;
			long dif = ( h - ( pFx->rect.bottom - pFx->rect.top ) ) >> 1;
			pFx->rect.top -= dif; pFx->rect.bottom += dif;
		} // end if

	} // end if

	// Release the Desktop DC
	ReleaseDC( NULL, hDC );

	return adj;
}


#define _SCALE( val ) double( ( (double)( val ) / (double)100 ) )

BOOL CFx::DrawHands( LPFX pfx, HDC hDC, LPRECT pRect, LPSYSTEMTIME pTime)
{
	if ( hDC == NULL || pRect == NULL ) return FALSE;

	long x,y,lx,ly;
	GetClockMetrics( pRect, &x, &y, &lx, &ly, FALSE );
	
/*	RECT	rect;
	::CopyRect( &rect, pRect );
	::InflateRect(	&rect, 
					( rect.right - rect.left ) / 10,
					( rect.bottom - rect.top ) / 10 );
*/

	// Are they forcing a time?
	SYSTEMTIME time;
	if ( pTime != NULL ) memcpy( &time, pTime, sizeof( SYSTEMTIME ) );
	else ::GetLocalTime( &time );

	// Get the angle in radians
	// Get hour 1 - 12
	long hour = ( ( time.wHour ) % 12 );
	hour *= 60;
	hour += time.wMinute;
	hour = ( hour + 540 ) % 720;
	double hangle = hour * ( (double)PI2 / (double)720 );

	long	min = time.wMinute;
	min *= 60;
	min += time.wSecond;
	min = ( min + 2700 ) % 3600;
	double mangle =	 min * ( (double)PI2 / (double)3600 );

	// Get the angle in radians
	double sangle = ( ( time.wSecond + 45 ) % 60 ) * ( (double)PI2 / (double)60 );

	// Do we want a shadow
	if ( true )
	{
//		long xsoff = (long)( ( ( ( lx < ly ) ? lx : ly ) / 2 ) * _SCALE( pfx->shadow * 8 ) );
//		long ysoff = (long)( ( ( ( lx < ly ) ? lx : ly ) / 2 ) * _SCALE( pfx->shadow * 8 ) );
		long ysoff = pfx->shadow;
		long xsoff = pfx->shadow;

/*		if ( m_ci.dwFlags & FACE_CENTERPOST )
		{
			long post = (long)( ( ( lx < ly ) ? lx : ly ) * _SCALE( m_ci.dwPostSize ) );

			DrawLine( hDC, m_ci.rgbShadow, INVALID_COLOR, post, x, y, xsoff + x, ysoff + y );			
		} // end if

		if ( m_ci.dwFlags & FACE_CENTERPIN )
		{
			long spin = (long)( ( ( lx < ly ) ? lx : ly ) * _SCALE( m_ci.dwPinSize ) );
			DrawCircle( hDC, m_ci.rgbShadow, m_ci.rgbShadow, xsoff + x - spin, ysoff + y - spin, xsoff + x + spin, ysoff + y + spin );
		} // end if
*/
		COLORREF shadow = RGB( 50, 50, 50 );
		// Draw the hands
		DrawHand( pfx, hDC, shadow, shadow, shadow, shadow, 4, 3, lx, ly, x + xsoff, y + ysoff, hangle, _SCALE( 40 ), 4 );
		DrawHand( pfx, hDC, shadow, shadow, shadow, shadow, 3, 2, lx, ly, x + xsoff, y + ysoff, mangle, _SCALE( 80 ), 4 );
		DrawHand( pfx, hDC, shadow, shadow, shadow, shadow, 2, 1, lx, ly, x + xsoff, y + ysoff, sangle, _SCALE( 80 ), 4 );

	} // end if

	// Draw the hands
	DrawHand( pfx, hDC, pfx->rgb, pfx->rgb, pfx->rgb, pfx->rgb, 4, 3, lx, ly, x, y, hangle, _SCALE( 40 ), 4 );
	DrawHand( pfx, hDC, pfx->rgb, pfx->rgb, pfx->rgb, pfx->rgb, 3, 2, lx, ly, x, y, mangle, _SCALE( 80 ), 4 );
	DrawHand( pfx, hDC, pfx->rgb, pfx->rgb, pfx->rgb, pfx->rgb, 2, 1, lx, ly, x, y, sangle, _SCALE( 80 ), 4 );

/*	// Do we want a center pin?
	if ( m_ci.dwFlags & FACE_CENTERPIN )
	{
		long spin = (long)( ( ( lx < ly ) ? lx : ly ) * _SCALE( m_ci.dwPinSize ) );

		DrawCircle( hDC, m_ci.rgbCenterPin, m_ci.rgbCenterPinOL, x - spin, y - spin, x + spin, y + spin );

	} // end if
*/
	return TRUE;
}

BOOL CFx::DrawTicks( LPFX pfx, HDC hDC, LPRECT pRect)
{
	if ( hDC == NULL || pRect == NULL ) return FALSE;

	long x,y,lx,ly;
	GetClockMetrics( pRect, &x, &y, &lx, &ly, FALSE );

/*	if ( false )
	{
		for ( DWORD i = 0; i < 60; i++ )
		{
			if ( ( i % 5 ) )
			{
				// Get the angle in radians
				double angle = i * ( (double)PI2 / (double)60 );

				// Get the point
				long cx = (long)( cos( angle ) * (double)lx ) + x;
				long cy = (long)( sin( angle ) * (double)ly ) + y;
				long size = (long)( ( ( ( lx < ly ) ? lx : ly ) / 4 ) * _SCALE( m_ci.dwMMarkSize ) );
				if ( size < 1 ) size = 1;

				DrawCircle( hDC, m_ci.rgbTickMarks, m_ci.rgbTickMarkOL, cx - size, cy - size, cx + size, cy + size );
			} // end if
		} // end for
	} // end if
*/
	if ( true )
	{ // Draw hour ticks

		for ( DWORD i = 0; i < 12; i++ )
		{
			// Get the angle in radians
			double angle = i * ( (double)PI2 / (double)12 );

			// Get the point
			long cx = (long)( cos( angle ) * (double)lx ) + x;
			long cy = (long)( sin( angle ) * (double)ly ) + y;
			long size = (long)( ( ( ( lx < ly ) ? lx : ly ) / 4 ) * _SCALE( 30 ) );
			if ( size < 1 ) size = 1;

			COLORREF shadow = RGB( 50, 50, 50 );

			DrawCircle( hDC, shadow, shadow, pfx->shadow + cx - size, pfx->shadow + cy - size, pfx->shadow + cx + size, pfx->shadow + cy + size );
			DrawCircle( hDC, pfx->rgb, pfx->rgb, cx - size, cy - size, cx + size, cy + size );

//			if ( lx < 80 || ly < 80 )
//			{
//				::Ellipse( hDC, cx - 2, cy - 2, cx + 2, cy + 2 );
//			} // end if
//			else DrawIcon( hDC, cx - 16, cy - 16, AfxGetApp()->LoadIcon( IDI_HOURMARK2 ) );

		} // end for

	} // end Draw hour hand

	return TRUE;
}

void CFx::GetClockMetrics(LPRECT pRect, long * cx, long * cy, long * rx, long * ry, BOOL bSquare )
{
	// Calculate the center point
	*cx = (long)( pRect->left + ( ( pRect->right - pRect->left ) / 2 ) );
	*cy = (long)( pRect->top + ( ( pRect->bottom - pRect->top ) / 2 ) );

	// Calculate half the box size
	*rx = (long)( ( pRect->right - pRect->left ) / 2 );
	*ry = (long)( ( pRect->bottom - pRect->top ) / 2 );

	if ( bSquare ) { if ( *rx < *ry ) *ry = *rx; else *rx = *ry; }
}

void CFx::DrawCircle(HDC hDC, COLORREF col, COLORREF ol, long left, long top, long right, long bottom)
{
	HPEN	hPen = CreatePen( PS_SOLID, 1, ol ); 
	HPEN	oldpen = (HPEN)::SelectObject( hDC, hPen );
	HBRUSH	hBrush = CreateSolidBrush( col ); 
	HBRUSH	oldbrush = (HBRUSH)::SelectObject( hDC, hBrush );

	::Ellipse( hDC, left, top, right, bottom );

	::SelectObject( hDC, oldpen );
	::SelectObject( hDC, oldbrush );
	::DeleteObject( hPen );
	::DeleteObject( hBrush );
}
void CFx::DrawLine(HDC hDC, COLORREF col, COLORREF ol, DWORD size, long sx, long sy, long ex, long ey)
{
	// Can't draw a line thinner than 1
	if ( size < 1 ) size = 1;

	// Do we want an outline ?
/*	if ( ol != INVALID_COLOR && size > 4 )
	{
		HPEN	hPen = CreatePen( PS_SOLID, size, ol ); 
		HPEN	oldpen = (HPEN)::SelectObject( hDC, hPen );

		::MoveToEx( hDC, sx, sy, NULL );
		::LineTo( hDC, ex, ey );

		::SelectObject( hDC, oldpen );
		::DeleteObject( hPen );

		size -= 2;

	} // end if
*/

	HPEN	hPen = CreatePen( PS_SOLID, size, col ); 
	HPEN	oldpen = (HPEN)::SelectObject( hDC, hPen );

	::MoveToEx( hDC, sx, sy, NULL );
	::LineTo( hDC, ex, ey );

	::SelectObject( hDC, oldpen );
	::DeleteObject( hPen );
}

void CFx::DrawHand(	LPFX pfx, HDC hDC, COLORREF col, COLORREF ol, 
						COLORREF fc, COLORREF bc, 
						long width, long mwidth, 
						long scx, long scy, 
						long ox, long oy, 
						double angle, double len, 
						DWORD gap )
{
	long cx = (long)( cos( angle ) * (double)scx * len ) + ox;
	long cy = (long)( sin( angle ) * (double)scy * len ) + oy;
	long sx = ox;
	long sy = oy;
	long lw = (long)( ( ( scx < scy ) ? scx : scy ) * _SCALE( width ) );
	if ( lw < mwidth ) lw = mwidth;

	// Do we want the hands to overlap ?
/*	if ( m_ci.dwFlags & FACE_HANDOVERLAP )
	{
		double opposite = angle + PI;
		if ( opposite > PI2 ) opposite -= PI2;
		sx = (long)( cos( opposite ) * ( (double)scx * _SCALE( gap ) ) + ox );
		sy = (long)( sin( opposite ) * ( (double)scy * _SCALE( gap ) ) + oy );
	} // end if
	else if ( m_ci.dwFlags & FACE_HANDGAP )
*/	{
		sx = (long)( cos( angle ) * ( (double)scx * _SCALE( gap ) ) + ox );
		sy = (long)( sin( angle ) * ( (double)scy * _SCALE( gap ) ) + oy );
	} // end if

	// Draw the hand
//	if ( m_ci.dwFlags & FACE_HANDNORMAL )
		DrawLine( hDC, col, ol, lw, sx, sy, cx, cy );

	// Draw circles if needed
/*	if ( m_ci.dwFlags & FACE_HANDCIRCLES )
	{
		long csize = (long)( ( ( ( scx < scy ) ? scx : scy ) / 2 ) * _SCALE( m_ci.dwCircleSize ) );

		DrawCircle( hDC, fc, ol, cx - csize, cy - csize, cx + csize, cy + csize );

	} // end if

	// Draw back circles if needed
	if ( m_ci.dwFlags & FACE_HANDBCIRCLES )
	{
		long csize = (long)( ( ( ( scx < scy ) ? scx : scy ) / 2 ) * _SCALE( m_ci.dwBCircleSize ) );
		DrawCircle( hDC, bc, ol, sx - csize, sy - csize, sx + csize, sy + csize );
	} // end if
*/
}

BOOL CFx::DrawClock(HDC hDC, LPRECT pRect, LPFX pFx, LPSYSTEMTIME pst)
{
	CBmp		bmp;
	CWinDc	os;
	RECT		draw;
	HDC			hOs = hDC;

	CopyRect( &draw, &pFx->rect );

	// If translucency
	if ( pFx->translucency != 0 )
	{	OffsetRect( &draw, -draw.left, -draw.top );
		os.Create( hDC, &pFx->rect );
		hOs = os;
	} // end if
	
	// Fill in background if needed
	if ( ( pFx->f1 & FXF1_BCK ) != 0 )
		CGrDC::FillSolidRect( hOs, &draw, pFx->bck );

	// Need to use transparency if no background
	else if ( pFx->translucency != 0 )
		FillRect( os, os, (HBRUSH)GetStockObject( BLACK_BRUSH ) );

	{ // Draw the clock

		RECT face;
		CopyRect( &face, &draw );
		InflateRect( &face, -4, -4 );

		DrawTicks( pFx, hOs, &face );
		DrawHands( pFx, hOs, &face, pst );

	} // end if

	// Draw the border if needed
	DrawBorder( hOs, &draw, pFx );		

	// If translucency
	if ( pFx->translucency != 0 )
	{
		// Load into bitmap
		bmp.LoadFromHDC( os );

		// Setup for trasparent draw
		if ( ( pFx->f1 & FXF1_BCK ) == 0 )
		{	bmp.SetTransparency( TRUE );
			bmp.SetTransparentColor( RGB( 0, 0, 0 ) );
			bmp.CreateMask( hDC );
		} // end if

		// Translucent draw
		bmp.TlFastDraw( hDC, &pFx->rect, pFx->translucency, TRUE );

	} // end if

	return TRUE;
}

BOOL CFx::ScaleFx(LPFX pFx)
{
	if ( pFx == NULL ) return FALSE;

	// Do the scales already match?
	if ( EqualRect( &pFx->scale, &m_rectScreen ) )
		return TRUE;

	// Get old scale
	long sw = pFx->scale.right - pFx->scale.left;
	long sh = pFx->scale.bottom - pFx->scale.top;

	// Scale X
	if ( sw != 0 )
	{	pFx->rect.left = ( pFx->rect.left * GetScreenWidth() ) / sw;
		pFx->rect.right = ( pFx->rect.right * GetScreenWidth() ) / sw;
	} // end if

	// Scale Y
	if ( sh != 0 )
	{	pFx->rect.top = ( pFx->rect.top * GetScreenHeight() ) / sh;
		pFx->rect.bottom = ( pFx->rect.bottom * GetScreenHeight() ) / sh;
	} // end if

	// Save that rect for later
	CopyRect( &pFx->scale, &m_rectScreen );

	return TRUE;
}

BOOL CFx::MoveToTop(LPFX node)
{
	// Can't move up any more if we're already first
	if ( node == NULL || node->pPrev == NULL ) return FALSE;

	// Remove from current position
	RemoveFx( node, FALSE );

	// Move to top of list
	return Insert( node, NULL );
}

BOOL CFx::MoveToBottom(LPFX node)
{
	// Can't move up any more if we're already first
	if ( node == NULL || node->pNext == NULL ) return FALSE;

	// Remove from current position
	RemoveFx( node, FALSE );

	// Move to bottom of list
	return Append( node, NULL );
}

BOOL CFx::Insert(LPFX node, LPFX pBefore)
{
	if ( node == NULL ) return FALSE;

	if ( pBefore == NULL )
	{
		// Add to the start
		node->pNext = m_pHead;
		node->pPrev = NULL;

		// Add this node to the list
		if ( m_pHead != NULL ) m_pHead->pPrev = node;
		m_pHead = node;
		if ( m_pTail == NULL ) m_pTail = node;

		// Count one
		m_dwSize++;

		return TRUE;		
	} // end if

	// Add us in
	node->pNext = pBefore;
	node->pPrev = pBefore->pPrev;
	if ( node->pPrev != NULL ) node->pPrev->pNext = node;
	pBefore->pPrev = node;

	// Take the old guys place
	if ( m_pHead == pBefore ) m_pHead = node;
	if ( m_pTail == NULL ) m_pTail = node;

	m_dwSize++;

	return TRUE;
}

BOOL CFx::Append(LPFX node, LPFX pAfter)
{
	if ( node == NULL ) return FALSE;

	if ( pAfter == NULL )
	{
		// Add to the end
		node->pNext = NULL;
		node->pPrev = m_pTail;

		// Add this node to the list
		if ( m_pTail == NULL ) m_pHead = node;
		else m_pTail->pNext = node;
		m_pTail = node;

		m_dwSize++;

		return TRUE;		
	} // end if

	// Add us in
	node->pNext = pAfter->pNext;
	node->pPrev = pAfter;
	if ( node->pNext != NULL ) node->pNext->pPrev = node;
	pAfter->pNext = node;

	// Take the old guys place
	if ( m_pHead == NULL ) m_pHead = node;
	if ( m_pTail == pAfter ) m_pTail = node;

	m_dwSize++;

	return TRUE;
}

static DWORD g_mdays[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

BOOL CFx::DrawAnalogCalender(LPFX pFx, HDC hDC, LPRECT pRect, LPSYSTEMTIME pTime)
{
	CBmp		bmp;
	CWinDc	os;
	RECT		draw, border;
	HDC			hOs = hDC;

	CopyRect( &draw, &pFx->rect );

	// If translucency
	if ( pFx->translucency != 0 )
	{	OffsetRect( &draw, -draw.left, -draw.top );
		os.Create( hDC, &pFx->rect );
		hOs = os;
	} // end if

	// Copy rect
	CopyRect( &border, &draw );
	
	// Fill in background if needed
	if ( ( pFx->f1 & FXF1_BCK ) != 0 )
		CGrDC::FillSolidRect( hOs, &draw, pFx->bck );

	// Need to use transparency if no background
	else if ( pFx->translucency != 0 )
		FillRect( os, os, (HBRUSH)GetStockObject( BLACK_BRUSH ) );

	{ // Draw calender

		// Ensure no month overflow
		if ( pTime->wMonth > 12 ) pTime->wMonth = 12;

		// Make room for header
		long hy = ( ( draw.bottom - draw.top ) / 5 ) / 2;
		draw.top += hy;

		long first = ( pTime->wDayOfWeek + 7 ) - ( ( pTime->wDay - 1 ) % 7 );
		if ( first >= 7 ) first -= 7;
		long col = pTime->wDayOfWeek;
		long row = ( first + ( pTime->wDay - 1 ) ) / 7;
		long rows = ( first + g_mdays[ pTime->wMonth ] + 6 ) / 7;

		long cw = ( draw.right - draw.left ) / 7;
		long cwo = ( ( draw.right - draw.left ) % 7 ) / 2;
		long ch = ( draw.bottom - draw.top ) / rows;
		long cho = ( ( draw.bottom - draw.top ) % rows ) / 2;

		long dw = cw / 3;
		long dh = ch / 3;


		HPEN hPen = CreatePen( PS_SOLID, 1, pFx->rgb );
		HPEN hOldPen = (HPEN)SelectObject( hOs, hPen );
		HBRUSH hBrush = CreateSolidBrush( pFx->rgb );
		HBRUSH hOldBrush = (HBRUSH)SelectObject( hOs, hBrush );

		// Draw the header
		Rectangle( hOs, draw.left + cwo, draw.top - hy, 
					draw.left + cwo + ( cw * 7 ) + 1, draw.top + cho );

		for ( long y = 0; y <= rows; y++ )
		{
			long cy = draw.top + cho + ( ch * y );

			MoveToEx( hOs, draw.left + cwo, cy, NULL );
			LineTo( hOs, draw.right - cwo, cy );
		} // end for

		for ( long x = 0; x <= 7; x++ )
		{
			long cx = draw.left + cwo + ( cw * x );
			
			MoveToEx( hOs, cx, draw.top + cho, NULL );
			LineTo( hOs, cx, draw.bottom - cho );

		} // end for

		// Draw day ticks in corner
		DWORD day = 0;
		for ( long y = 0; y < rows; y++ )
		{
			long cy = draw.top + cho + ( ch * y );
			for ( long x = 0; x < 7; x++ )
			{
				if ( ( y || x >= first ) && day < g_mdays[ pTime->wMonth ] )
				{	day++;
					long cx = draw.left + cwo + ( cw * x );
					Rectangle( hOs, cx, cy, cx + dw, cy + dh );
				} // end if
			} // end for
		} // end for

		// Draw current day
		long cx = draw.left + cwo + ( cw * col ); 
		long cy = draw.top + cho + ( ch * row );
		Rectangle( hOs, cx, cy, cx + cw, cy + ch );

		// Release drawing objects
		SelectObject( hOs, hOldPen );
		DeleteObject( hPen );
		SelectObject( hOs, hOldBrush );
		DeleteObject( hBrush );

	} // end Draw calender

	// Draw the border if needed
	DrawBorder( hOs, &border, pFx );		

	// If translucency
	if ( pFx->translucency != 0 )
	{
		// Load into bitmap
		bmp.LoadFromHDC( os );

		// Setup for trasparent draw
		if ( ( pFx->f1 & FXF1_BCK ) == 0 )
		{	bmp.SetTransparency( TRUE );
			bmp.SetTransparentColor( RGB( 0, 0, 0 ) );
			bmp.CreateMask( hDC );
		} // end if

		// Translucent draw
		bmp.TlFastDraw( hDC, &pFx->rect, pFx->translucency, TRUE );

	} // end if

	return TRUE;
}

BOOL CFx::CaptureScreen(LPFX pFx, CWinDc *pos)
{
	DWORD w = 320;
	DWORD h = 240;

	// Get screen shot size
	CFG().GetValue( "Settings", "SShotWidth", &w );
	CFG().GetValue( "Settings", "SShotHeight", &h );

	// Validate size
	if ( w == 0 || w > 5000 || h == 0 || h > 5000 )
		return FALSE;

	// Get the window
	HDC hDC = NULL;
	HWND hWnd = NULL;
	LPRECT pRect = NULL;
	RECT rect;
	if ( *pFx->data != 0 && strcmpi( pFx->data, "< desktop >" ) )
	{	
		// Get active window if needed
		if ( !strcmpi( pFx->data, "< active window >" ) )
			hWnd = ::GetForegroundWindow();

		// Find the window
		else hWnd = ::FindWindow( NULL, pFx->data );

		// Is there such a window and is it visible?
		if ( !::IsWindow( hWnd ) || !::IsWindowVisible( hWnd ) )
		{	pos->Destroy(); return TRUE; }

		// Get the window DC
		if ( hWnd != NULL ) hDC = ::GetDC( hWnd );

		// Get the window rect
		if ( hDC != NULL ) { pRect = &rect; GetClientRect( hWnd, pRect ); }

		w = PRW( pRect );
		h = PRH( pRect );

	} // end if	


	// Get screen capture
	BOOL ret = pos->Create( NULL, 0, 0, w, h ) && pos->Copy( hDC, pRect );

	// Release DC if needed
	if ( hDC != NULL && ::IsWindow( hWnd ) )
		ReleaseDC( hWnd, hDC );

	return ret;
}

BOOL CFx::DrawBorder(HDC hDC, LPRECT pRect, LPFX pfx)
{
	// Draw border if needed
	if ( ( pfx->f1 & FXF1_BORDER ) != 0 )
	{
		RECT rect;
		CopyRect( &rect, pRect );
//		InflateRect( &rect, -1, -1 );
		CGrDC::Box3d(	hDC, &rect, 2, 
						RGB( 200, 200, 200 ), RGB( 40, 40, 40 ),
						( pfx->f1 & FXF1_FOUNDERED ) != 0 );
	} // end if

	return TRUE;
}

BOOL CFx::GetSpecialText(DWORD i, LPSTR buf, DWORD size)
{
	switch( i )
	{
		case STEXT_AWINDOW :
		{	HWND hParent, hWnd = GetForegroundWindow();
			while ( ( hParent = GetParent( hWnd ) ) != NULL ) hWnd = hParent;			
			if ( !::IsWindow( hWnd ) ) return FALSE;
			return GetWindowText( hWnd, buf, size );
		} // end case

		case STEXT_CPU :
		{
			double cpu = GetCPU();
			sprintf( buf, "%2.0f%%", cpu );
			return TRUE;
		} // end case
	
		default : 
			*buf = 0;
			return FALSE;
			break;

	} // end switch

	return TRUE;
}

LPCTSTR CFx::GetSpecialTextName(DWORD i)
{
	switch( i )
	{
		case STEXT_AWINDOW :	return "Foreground Window";	break;
		case STEXT_CPU :		return "CPU Load";			break;
//		case STEXT_NUMCPU :		return "Number of CPU's";	break;
//		case STEXT_CPUSPEED :	return "CPU Speed";			break;
//		case STEXT_DISK :		return "Free Disk Space";	break;
//		case STEXT_MEM :		return "Free RAM";			break;
//		case STEXT_BANDWIDTH :	return "Network Bandwidth";	break;

		default : return NULL;

	} // end switch

	return NULL;
}

typedef LONG (WINAPI *tQuerySystemInfo)(UINT,PVOID,ULONG,PULONG);

typedef struct tagSYSBASICINFO
{
    DWORD			dwUnknown1;
    ULONG			uKeMaximumIncrement;
    ULONG			uPageSize;
    ULONG			uMmNumberOfPhysicalPages;
    ULONG			uMmLowestPhysicalPage;
    ULONG			uMmHighestPhysicalPage;
    ULONG			uAllocationGranularity;
    PVOID			pLowestUserAddress;
    PVOID			pMmHighestUserAddress;
    ULONG			uKeActiveProcessors;
    BYTE			bKeNumberProcessors;
    BYTE			bUnknown2;
    WORD			wUnknown3;
} SYSBASICINFO, *LPSYSBASICINFO; // end typedef struct

typedef struct tagSYSPERFINFO
{
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
} SYSPERFINFO, *LPSYSPERFINFO; // end typedef struct

typedef struct tagSYSTIMEINFO
{
    LARGE_INTEGER	liKeBootTime;
    LARGE_INTEGER	liKeSystemTime;
    LARGE_INTEGER	liExpTimeZoneBias;
    ULONG			uCurrentTimeZoneId;
    DWORD			dwReserved;

} SYSTIMEINFO, *LPSYSTIMEINFO; // end typedef struct

#define SYS_BASIC_INFO       0
#define SYS_PERFORMANCE_INFO 2
#define SYS_TIME_INFO        3

//#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))
#define Li2Double(x) ((double)((x).HighPart) * double( 0x100000000 ) + (double)((x).LowPart))


double CFx::GetCPU()
{
	// Get OS info
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	if( !GetVersionEx( &os ) ) return 0;

	// If Windows 95/98/ME
	if ( os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		HKEY hKey = NULL;

		// Open performance key
		if ( RegOpenKeyEx(	HKEY_DYN_DATA, "PerfStats\\StartStat", 
							0, KEY_READ, &hKey ) != ERROR_SUCCESS )
			return 0;

		DWORD dwCpu = 0, dwsize = sizeof( DWORD );
		if ( RegQueryValueEx(	hKey, "KERNEL\\CPUUsage", NULL, NULL,
								(LPBYTE)&dwCpu, &dwsize ) != ERROR_SUCCESS )
			dwCpu = 0;

		RegCloseKey( hKey );
		
		return (double)dwCpu;

	} // end if

	// Windows NT/2K/XP
	else if ( os.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		static BOOL		bFirst = TRUE;
		static double	oldIdle = 0, oldSys = 0;
		SYSBASICINFO	sbi;
		SYSPERFINFO		spi;
		SYSTIMEINFO		sti;

		// Get magic function address
		tQuerySystemInfo	fQsi = (tQuerySystemInfo)GetProcAddress(
														GetModuleHandle( "ntdll" ),
														"NtQuerySystemInformation" );
		if ( fQsi == NULL ) return FALSE;

		// Get system info
		if ( fQsi( SYS_BASIC_INFO, &sbi, sizeof( sbi ), NULL ) != NO_ERROR )
			return 0;

		if ( fQsi( SYS_PERFORMANCE_INFO, &spi, sizeof( spi ), NULL ) != NO_ERROR )
			return 0;

		if ( fQsi( SYS_TIME_INFO, &sti, sizeof( sti ), NULL ) != NO_ERROR )
			return 0;

		double cpu = 0;
		if ( !bFirst )
		{
			// CurrentValue = NewValue - OldValue
			double dIdle = Li2Double( spi.liIdleTime ) - oldIdle;
			double dSys = Li2Double( sti.liKeSystemTime ) - oldSys;
			if ( dSys != 0 ) dIdle = dIdle / dSys;

			// Calculate CPU load
			if ( sbi.bKeNumberProcessors == 0 ) sbi.bKeNumberProcessors = 1;
			cpu = double( 100 ) - ( dIdle * 100 ) / (double)sbi.bKeNumberProcessors + 0.5;
			if ( cpu > 100 ) cpu = 100;

		} // end if
		else bFirst = FALSE;

		// Save old values
		oldIdle = Li2Double( spi.liIdleTime );
		oldSys = Li2Double( sti.liKeSystemTime );

		// Smooth value
//		static double	use = 0;
//		use = ( ( use * 15 ) + cpu ) / 16;

		return cpu;

	} // end else if
				   
	return 0;
}

BOOL CFx::GetFxString(LPFX pfx, LPSTR pStr)
{
	switch( pfx->type )
	{
		case FXTYPE_TEXT :
			strcpy( pStr, (LPCTSTR)pfx->data );
			break;

		case FXTYPE_TIME :
			strcpy( pStr, "< Time >" );
			break;

		case FXTYPE_DATE :
			strcpy( pStr, "< Date >" );
			break;

		case FXTYPE_RECT :
			strcpy( pStr, "< Rectangle >" );
			break;

		case FXTYPE_IMAGE :
			*pStr = 0;
			CWinFile::GetFileNameFromPath( pfx->data, pStr );
			break;

		case FXTYPE_WATERMARK :
			*pStr = 0;
			CWinFile::GetFileNameFromPath( pfx->data, pStr );
			break;

		case FXTYPE_VIDEO :
			strcpy( pStr, "< Video >" );
			break;

		case FXTYPE_SCREENSHOT :
			strcpy( pStr, "< Screen Capture >" );
			break;

		case FXTYPE_URL :
			*pStr = 0;
			CWinFile::GetFileNameFromPath( pfx->data, pStr );
			break;

		case FXTYPE_STEXT :
			strcpy( pStr, "< Special Text >" );
			break;

		default : strcpy( pStr, "Element" ); return FALSE; break;

	} // end switch

	return TRUE;

}

BOOL CFx::GetInternetImage(LPFX pfx)
{
	if ( pfx == NULL ) return FALSE;
	if ( *pfx->data == 0 ) return FALSE;
	
	// Waiting for retry?
	if ( pfx->loadtimeout > GetTickCount() ) return FALSE;

	// Create new object if needed
	if ( pfx->nf == NULL )
	{	pfx->nf = new CNetFile();
		if ( pfx->nf == NULL ) return FALSE;
	} // end if

	// Are we already loading the image?
	if ( pfx->nf->IsWorking() ) return FALSE;

	// Set callback
	pfx->nf->SetCallback( CFx::OnInternetImage, (DWORD)pfx );

	// Download the data
	pfx->nf->Download( pfx->data );

	// Retry every one minute
	pfx->loadtimeout = GetTickCount() + 60000;

	return TRUE;
}

BOOL CFx::OnInternetImage(DWORD dwUser, WPARAM wParam, LPARAM lParam)
{
	LPFX pfx = (LPFX)dwUser;
	if ( pfx == NULL ) return FALSE;

	switch( wParam )
	{
		case NETFILE_DS_DONE :
		{
			CWinImg img;

			// Where is the type info
			LPCTSTR type = pfx->sep;
			if ( *type == 0 ) type = pfx->nf->GetUrl();

			// Decode the internet image
			if ( img.Decode( pfx->nf->GetMem(), pfx->nf->GetDataRead(), type ) )
			{
				// Create new os if needed
				if ( pfx->os == NULL ) pfx->os = new CWinDc();

				// If we got one
				if ( pfx->os != NULL )
				{
					// Create buffer for image
					if ( pfx->os->Create( NULL, img ) )
					{
						// Draw the image into the buffer
						img.Draw( *pfx->os, *pfx->os );

						// Save url
						strcpy( pfx->preloaded, pfx->data );

					} // end if

				} // end if

			} // end if

			// Lose the download data
			pfx->nf->Release();

			return TRUE;

		} break;

	} // end switch

	return TRUE;
}
