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
// PropertiesList.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "PropertiesList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_COMBOCALLBACK		( WM_USER + 701 )
#define WM_REFRESHCALLBACK		( WM_USER + 702 )
#define WM_SETCOMBODATA			( WM_USER + 703 )

/////////////////////////////////////////////////////////////////////////////
// CPropertiesList

CPropertiesList::CPropertiesList()
{
	m_bInit = FALSE;

	// Want autosizing header
	GetListCtrl().GetHeader().Autosize( TRUE );
	GetListCtrl().MultiSelect( FALSE );
	GetListCtrl().SetDefaultItemHeight( 22 );

}

CPropertiesList::~CPropertiesList()
{
}


BEGIN_MESSAGE_MAP(CPropertiesList, C3DList)
	//{{AFX_MSG_MAP(CPropertiesList)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_SETCOMBODATA, OnSetComboData )
	ON_MESSAGE( WM_COMBOCALLBACK, OnFillCombo )
	ON_MESSAGE( WM_REFRESHCALLBACK, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesList message handlers

BOOL CPropertiesList::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL bRet = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	return bRet;	
}

BOOL CPropertiesList::OnEraseBkgnd(CDC* pDC) 
{
	if ( !m_bInit ) Init();
	
	return CStatic::OnEraseBkgnd(pDC);
}

void CPropertiesList::Init()
{
	m_bInit = TRUE;

	// Rebuild header
	GetListCtrl().GetHeader().Destroy();
	GetListCtrl().GetHeader().AddHeaderItem( 120, "Name" );
	GetListCtrl().GetHeader().AddHeaderItem( 120, "Value" );
}


void CPropertiesList::Update()
{
	// Sanity check
	if ( !IsWindow( GetSafeHwnd() ) || GetListCtrl().Size() == 0 )
		return;

	LPPROPINFO	ppi = NULL;

	for ( DWORD n = 0; n < GetListCtrl().Size(); n++ )
	{
		ppi = (LPPROPINFO)GetListCtrl().GetItemData( n );
		if ( ppi != NULL )
		{
			char str[ CWF_STRSIZE ] = { 0 };

			// Get string to show
			if ( ppi->user == 0 || !GetString( ppi, str ) )
				m_pl.GetStr( ppi, str, sizeof( str ) );

			if ( ppi->type == PROPTYPE_GROUP );
			
			else
			{
				if ( ppi->type == PROPTYPE_FILE || ppi->type == PROPTYPE_DIR )
					CWinFile::GetFileNameFromPath( str, str );

				GetListCtrl().SetSubItemText( n, 1, str );

				switch( ppi->type )
				{

					case PROPTYPE_RGB : 
					{
						COLORREF rgb = *( (LPCOLORREF)ppi->data );

						GetListCtrl().SetSubItemBckColor( n, 1, rgb );
						if ( CGrDC::IsLightColor( rgb ) )
							GetListCtrl().SetSubItemTextColor( n, 1, RGB( 0, 0, 0 ) );
						else GetListCtrl().SetSubItemTextColor( n, 1, RGB( 255, 255, 255 ) );

					} break;

					case PROPTYPE_RGBSTR : 
					{
						COLORREF rgb = CGrDC::HtmlToRgb( (char*)ppi->data );

						GetListCtrl().SetSubItemBckColor( n, 1, rgb );
						if ( CGrDC::IsLightColor( rgb ) )
							GetListCtrl().SetSubItemTextColor( n, 1, RGB( 0, 0, 0 ) );
						else GetListCtrl().SetSubItemTextColor( n, 1, RGB( 255, 255, 255 ) );

					} break;

				} // end switch

			} // end if

		} // end if

	} // end while 

	RedrawWindow();
}

void CPropertiesList::LoadList()
{
	LPPROPINFO	ppi = NULL;

	DWORD dwLine = GetListCtrl().GetCurLine();

	// Lose list items
	GetListCtrl().Destroy();

	while ( ( ppi = (LPPROPINFO)m_pl.GetNext( ppi ) ) != NULL )
	{
		if ( ppi->type == PROPTYPE_GROUP )
		{
			DWORD n = GetListCtrl().AddItem( LIF_GROUP );
			if ( n != MAXDWORD ) GetListCtrl().AddSubItem( n, ppi->name );
		} // end if

		else 
		{
			DWORD n = GetListCtrl().AddItem();

			if ( n != MAXDWORD )
			{
	//			if ( ppi->type == PROPTYPE_FONT )
	//				GetListCtrl().AddSubItem( n, ppi->name, NULL, SIT_TEXT, 0, MAXDWORD, MAXDWORD, (LPLOGFONT)ppi->data );
				GetListCtrl().AddSubItem( n, ppi->name );
				GetListCtrl().SetItemData( n, (DWORD)ppi );

				char str[ CWF_STRSIZE ] = { 0 };

				// Get string to show
				if ( ppi->user == 0 || !GetString( ppi, str ) )
					m_pl.GetStr( ppi, str, sizeof( str ) );

				if ( ppi->type == PROPTYPE_FILE || ppi->type == PROPTYPE_DIR )
					CWinFile::GetFileNameFromPath( str, str );

				if ( ppi->type == PROPTYPE_FONT )
					GetListCtrl().AddSubItem( n, str, NULL, SIT_TEXT, 0, MAXDWORD, MAXDWORD, (LPLOGFONT)ppi->data );
				
				else GetListCtrl().AddSubItem( n, str );

				switch( ppi->type )
				{
					case PROPTYPE_RGB : 
					{
						COLORREF rgb = *( (LPCOLORREF)ppi->data );

						GetListCtrl().SetSubItemBckColor( n, 1, rgb );
						if ( CGrDC::IsLightColor( rgb ) )
							GetListCtrl().SetSubItemTextColor( n, 1, RGB( 0, 0, 0 ) );
						else GetListCtrl().SetSubItemTextColor( n, 1, RGB( 255, 255, 255 ) );

					} break;

					case PROPTYPE_RGBSTR : 
					{
						COLORREF rgb = CGrDC::HtmlToRgb( (char*)ppi->data );

						GetListCtrl().SetSubItemBckColor( n, 1, rgb );
						if ( CGrDC::IsLightColor( rgb ) )
							GetListCtrl().SetSubItemTextColor( n, 1, RGB( 0, 0, 0 ) );
						else GetListCtrl().SetSubItemTextColor( n, 1, RGB( 255, 255, 255 ) );

					} break;

				} // end switch

			} // end if

		} // end if

	} // end while

	GetListCtrl().SetCurLine( dwLine );
}

void CPropertiesList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	GetListCtrl().NoDraw( 1 );

	C3DList::OnLButtonDown(nFlags, point);

	// Lose the window
	m_editparam.DestroyWindow();

	// Get first selected item
	DWORD sel = GetListCtrl().GetFirstSelectedItem();
	if ( sel == MAXDWORD ) { RedrawWindow(); return; }

	// Get property info
	LPPROPINFO ppi = (LPPROPINFO)GetListCtrl().GetItemData( sel );
	if ( ppi == NULL ) { RedrawWindow(); return; }

	RECT rect;
	GetClientRect( &rect );

	// Where should we show the edit window
	GetListCtrl().GetItemRect( sel, &rect );

	// Find the exact spot
	rect.top += 1;
	rect.left += GetListCtrl().GetHeader().GetHeaderItemWidth( 0 );
	rect.right = rect.left + GetListCtrl().GetHeader().GetHeaderItemWidth( 1 ) - 2;

	// Set callback
	m_editparam.SetCallbackWnd( GetSafeHwnd(), WM_COMBOCALLBACK, WM_REFRESHCALLBACK, WM_SETCOMBODATA );

	// Edit the thing
	m_editparam.Edit( this, &rect, ppi );

	RedrawWindow();

}

void CPropertiesList::OnSize(UINT nType, int cx, int cy) 
{
	EndEdit();	

	C3DList::OnSize(nType, cx, cy);	
}

void CPropertiesList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	EndEdit();
	
	C3DList::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CPropertiesList::OnFillCombo( WPARAM wParam, LPARAM lParam )
{	return FillCombo( (DWORD)wParam, (CComboBox*)lParam );
}

LRESULT CPropertiesList::OnSetComboData( WPARAM wParam, LPARAM lParam )
{	return SetComboData( (DWORD)wParam, (LPPROPCALLBACKINFO)lParam );
}

LRESULT CPropertiesList::OnGetString(WPARAM wParam, LPARAM lParam)
{	return GetString( (LPPROPINFO)wParam, (LPSTR)lParam );
}

LRESULT CPropertiesList::OnRefresh( WPARAM wParam, LPARAM lParam )
{	if ( ::IsWindow( GetSafeHwnd() ) ) Update();
	DoRefreshCallback( wParam, lParam );
	return Refresh( (DWORD)wParam, (LPPROPINFO)lParam );
}

