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
// DlgMsgView.cpp : implementation file
//

#include "StdAfx.h"
#include "Vp.h"
#include "DlgMsgView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 54)
#define ListView_SetExtendedListViewStyle(hwndLV, dw)\
		(DWORD)SNDMSG((hwndLV), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dw)

#define LVM_GETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 55)
#define ListView_GetExtendedListViewStyle(hwndLV)\
		(DWORD)SNDMSG((hwndLV), LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0)

#define LVS_EX_GRIDLINES        0x00000001
#define LVS_EX_SUBITEMIMAGES    0x00000002
#define LVS_EX_CHECKBOXES       0x00000004
#define LVS_EX_TRACKSELECT      0x00000008
#define LVS_EX_HEADERDRAGDROP   0x00000010
#define LVS_EX_FULLROWSELECT    0x00000020 // applies to report mode only
#define LVS_EX_ONECLICKACTIVATE 0x00000040

/////////////////////////////////////////////////////////////////////////////
// CDlgMsgView dialog

CDlgMsgView::CDlgMsgView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMsgView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMsgView)
	m_sText = _T("");
	m_bPopup = FALSE;
	m_bPopupWarnings = FALSE;
	//}}AFX_DATA_INIT

	m_log = NULL;
	m_bSave = FALSE;
}

CDlgMsgView::~CDlgMsgView()
{
	DestroyWindow();
}


void CDlgMsgView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMsgView)
	DDX_Control(pDX, IDC_MV_ICON, m_icon);
	DDX_Control(pDX, IDC_MV_LIST, m_list);
	DDX_Text(pDX, IDC_MV_TEXT, m_sText);
	DDX_Check(pDX, IDC_MV_POPUP, m_bPopup);
	DDX_Check(pDX, IDC_MV_POPUPWARNINGS, m_bPopupWarnings);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMsgView, CDialog)
	//{{AFX_MSG_MAP(CDlgMsgView)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_MV_LIST, OnClickMvList)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_MV_CLEAR, OnMvClear)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_MV_SAVE, OnMvSave)
	ON_BN_CLICKED(IDC_MV_POPUP, OnMvPopup)
	ON_BN_CLICKED(IDC_MV_POPUPWARNINGS, OnMvPopupWarnings)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_UPDATELIST, OnUpdateList )
	ON_MESSAGE( WM_INSERTITEM, OnInsertItem )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMsgView message handlers

void CDlgMsgView::Size()
{
	CWnd	*pText = GetDlgItem( IDC_MV_TEXT );
	CWnd	*pList = GetDlgItem( IDC_MV_LIST );
	CWnd	*pClear = GetDlgItem( IDC_MV_CLEAR );
	CWnd	*pSave = GetDlgItem( IDC_MV_SAVE );
	CWnd	*pIcon = GetDlgItem( IDC_MV_ICON );
	CWnd	*pPopup = GetDlgItem( IDC_MV_POPUP );
	CWnd	*pPopupWarnings = GetDlgItem( IDC_MV_POPUPWARNINGS );
	if ( pText == NULL || pList == NULL ) return;

	// Get window params
	POINT	pt = { 0, 0 };
	RECT	rect, text, list, clear, save, icon, popup;
	GetClientRect( &rect );
	ClientToScreen( &pt );

	text.left = rect.left + 5;
	text.top = rect.top + 5;
	text.right = rect.right - 80;
	text.bottom = ( rect.bottom / 2 ) - 22;

	clear.top = text.top;
	clear.bottom = clear.top + 25;
	if ( clear.bottom > text.bottom ) clear.bottom = text.bottom;
	clear.right = rect.right - 5;
	clear.left = text.right + 10;
	// Set the new edit control position
	if ( pClear != NULL )
		pClear->SetWindowPos(	NULL, clear.left, clear.top,
								( clear.right - clear.left ),
								( clear.bottom - clear.top ),
								SWP_NOZORDER | SWP_NOACTIVATE );
	save.top = clear.bottom + 2;
	save.bottom = save.top + 25;
	if ( save.bottom > text.bottom ) save.bottom = text.bottom;
	save.right = rect.right - 5;
	save.left = text.right + 10;
	// Set the new edit control position
	if ( pSave != NULL )
		pSave->SetWindowPos(	NULL, save.left, save.top,
								( save.right - save.left ),
								( save.bottom - save.top ),
								SWP_NOZORDER | SWP_NOACTIVATE );

	icon.top = save.bottom + 2;
	icon.bottom = icon.top + 40;
	if ( icon.bottom > text.bottom ) icon.bottom = text.bottom;
	icon.right = rect.right - 5;
	icon.left = text.right + 10;
	// Set the new edit control position
	if ( ( icon.bottom - icon.top ) < 32 ) 
	{
		if ( pIcon != NULL ) pIcon->ShowWindow( SW_HIDE );
	} // end if
	else if ( pIcon != NULL )
	{
		pIcon->ShowWindow( SW_SHOWNORMAL );
		pIcon->SetWindowPos(	NULL, icon.left, icon.top,
								( icon.right - icon.left ),
								( icon.bottom - icon.top ),
								SWP_NOZORDER | SWP_NOACTIVATE );
	} // end else

	// Set the new edit control position
	if ( pText != NULL )
		pText->SetWindowPos(	NULL, text.left, text.top,
								( text.right - text.left ),
								( text.bottom - text.top ),
								SWP_NOZORDER | SWP_NOACTIVATE );

	list.left = rect.left + 5;
	list.top = ( rect.bottom / 2 ) - 20;
	list.right = rect.right - 5;
	list.bottom = rect.bottom - 25;

	// Set the new list control position
	if ( pList != NULL )
		pList->SetWindowPos(	NULL, list.left, list.top,
								( list.right - list.left ),
								( list.bottom - list.top ),
								SWP_NOZORDER | SWP_NOACTIVATE );

	popup.left = rect.left + 10;
	popup.right = ( rect.right - rect.left ) >> 1;
	popup.top = list.bottom + 5;
	popup.bottom = rect.bottom - 5;
	if ( pPopup != NULL )
		pPopup->SetWindowPos(	NULL, popup.left, popup.top,
								( popup.right - popup.left ),
								( popup.bottom - popup.top ),
								SWP_NOZORDER | SWP_NOACTIVATE );

	popup.left = popup.right + 1;
	popup.right = rect.right - 5;
	if ( pPopupWarnings != NULL )
		pPopupWarnings->SetWindowPos(	NULL, popup.left, popup.top,
										( popup.right - popup.left ),
										( popup.bottom - popup.top ),
										SWP_NOZORDER | SWP_NOACTIVATE );

	// Set the new list control column widths
	m_list.SetColumnWidth( 0, ( list.right - list.left ) / 2 );
	m_list.SetColumnWidth( 1, ( ( list.right - list.left ) / 2 ) - GetSystemMetrics( SM_CXVSCROLL ) - 5 );
}

BOOL CDlgMsgView::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RECT	rect;
	while ( m_list.DeleteColumn( 0 ) );
	m_list.GetWindowRect( &rect );
	m_list.InsertColumn( 0, "Messages", LVCFMT_LEFT, 
						 ( ( rect.right - rect.left ) / 2 ) );
	m_list.InsertColumn( 1, "Time", LVCFMT_LEFT, 
						 ( ( rect.right - rect.left ) / 2 ) - GetSystemMetrics( SM_CXVSCROLL ) - 5 );
	
	ListView_SetExtendedListViewStyle( m_list.GetSafeHwnd(), LVS_EX_FULLROWSELECT );
	
	// IDI_APPLICATION Default application icon. 
	// IDI_ASTERISK Same as IDI_INFORMATION. 
	// IDI_ERROR Hand-shaped icon. 
	// IDI_EXCLAMATION Same as IDI_WARNING. 
	// IDI_HAND Same as IDI_ERROR.  
	// IDI_INFORMATION Asterisk icon. 
	// IDI_QUESTION Question mark icon. 
	// IDI_WARNING Exclamation point icon. 
	// IDI_WINLOGO Windows logo icon. 

	// Create an image list
	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );

	m_images.Add( ::LoadIcon( NULL, IDI_APPLICATION ) );	// 0
	m_images.Add( ::LoadIcon( NULL, IDI_ASTERISK ) );		// 1
	m_images.Add( ::LoadIcon( NULL, IDI_ERROR ) );			// 2
	m_images.Add( ::LoadIcon( NULL, IDI_EXCLAMATION ) );	// 3
	m_images.Add( ::LoadIcon( NULL, IDI_HAND ) );			// 4
	m_images.Add( ::LoadIcon( NULL, IDI_INFORMATION ) );	// 5
	m_images.Add( ::LoadIcon( NULL, IDI_QUESTION ) );		// 6
	m_images.Add( ::LoadIcon( NULL, IDI_WARNING ) );		// 7
	m_images.Add( ::LoadIcon( NULL, IDI_WINLOGO ) );		// 8

	m_list.SetImageList( &m_images, LVSIL_NORMAL );
	m_list.SetImageList( &m_images, LVSIL_SMALL );
	
	Size();
	
	m_icon.SetIcon( AfxGetApp()->LoadIcon( IDR_MAINFRAME ) );

	// Get popup state
	m_bPopup = AfxGetApp()->GetProfileInt( "MsgView", "Popup", 1 ) ? 1 : 0;
	m_bPopupWarnings = AfxGetApp()->GetProfileInt( "MsgView", "PopupWarnings", 1 ) ? 1 : 0;

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMsgView::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	Size();	

	SavePosition();
}


BOOL CDlgMsgView::UpdateList()
{
	m_list.DeleteAllItems();

	if ( m_log == NULL ) return FALSE;

	LPLOGMSGINFO pmi = NULL;

	while ( ( pmi = m_log->GetNext( pmi ) ) != NULL )
	{
		// MB_ICONEXCLAMATION, 
		// MB_ICONWARNING An exclamation-point icon appears in the message box. 
		// MB_ICONINFORMATION, MB_ICONASTERISK	An icon consisting of a lowercase 
		//										letter i in a circle appears in the 
		//										message box. 
		// MB_ICONQUESTION A question-mark icon appears in the message box. 
		// MB_ICONSTOP, 
		// MB_ICONERROR, 
		// MB_ICONHAND A stop-sign icon appears in the message box. 

		int icon;
		if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONEXCLAMATION )		icon = 3;
		else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONWARNING )	icon = 7;
		else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONQUESTION )	icon = 6;
		else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONSTOP )		icon = 4;
		else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONERROR )		icon = 2;
		else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONHAND )		icon = 4;
		else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONASTERISK )	icon = 1;
		else icon = 5;

		int n = m_list.InsertItem( 0, pmi->szTitle, icon );
		m_list.SetItemText( n, 1, pmi->szTime );
		m_list.SetItemData( n, (DWORD)pmi ); 

		SetIcon( ::LoadIcon( NULL, IDI_EXCLAMATION ), TRUE );
		SetIcon( ::LoadIcon( NULL, IDI_EXCLAMATION ), FALSE );

	} // end while

	// Select the first item
	m_list.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );

	ShowMessage();

	return TRUE;
}

BOOL CDlgMsgView::ShowMessage()
{
	m_sText = "";
	m_icon.SetIcon( ::LoadIcon( NULL, IDI_INFORMATION ) );
	UpdateData( FALSE );	

	SetIcon( ::LoadIcon( NULL, IDI_INFORMATION ), FALSE );
	SetIcon( ::LoadIcon( NULL, IDI_INFORMATION ), TRUE );
//	SetWindowText( "Event Log" );

	UINT count = (UINT)m_list.GetItemCount();
	if ( count == 0 ) return FALSE;

	// Search for the first selected output
	for ( DWORD i = 0; i < count; i++ )
		if ( m_list.GetItemState( i, LVIS_SELECTED ) & LVIS_SELECTED )
		{
			LPLOGMSGINFO pmi = (LPLOGMSGINFO)m_list.GetItemData( i );
			if ( m_log == NULL || !m_log->IsValidMsgInfo( pmi ) ) return FALSE;

			m_sText = "\r\n";
			if ( *pmi->szMessage != NULL )
			{
//				m_sText += "Message:\r\n";
				m_sText += pmi->szMessage;
			} // end if
			if ( *pmi->szLocation != NULL )
			{
				if ( !m_sText.IsEmpty() ) m_sText += "\r\n\r\n";
//				m_sText += "Location:\r\n";
				m_sText += pmi->szLocation;
			} // end if

			UpdateData( FALSE );
			
			int icon;
			if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONEXCLAMATION )		icon = 3;
			else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONWARNING )	icon = 7;
			else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONQUESTION )	icon = 6;
			else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONSTOP )		icon = 4;
			else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONERROR )		icon = 2;
			else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONHAND )		icon = 4;
			else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONASTERISK )	icon = 1;
			else icon = 5;

			m_icon.SetIcon( GetIcon( icon ) );

//			SetWindowText( pmi->szTitle );
			SetIcon( GetIcon( icon ), TRUE );
			SetIcon( GetIcon( icon ), FALSE );

			return TRUE;
		} // end if

	return FALSE;
}

void CDlgMsgView::OnClickMvList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	ShowMessage();	
	*pResult = 0;
}

void CDlgMsgView::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);

	SavePosition();
}

void CDlgMsgView::OnMvClear() 
{
	if ( m_log != NULL ) m_log->Destroy();
	UpdateList();	
}

void CDlgMsgView::SavePosition()
{
	if ( !m_bSave ) return;
	
	RECT	rect;
	GetWindowRect( &rect );

	// Verify rect
	if ( rect.left < 0 || rect.left > GetSystemMetrics( SM_CXSCREEN ) ) return;
	if ( rect.right < 0 || rect.right > GetSystemMetrics( SM_CXSCREEN ) ) return;
	if ( rect.top < 0 || rect.top > GetSystemMetrics( SM_CYSCREEN ) ) return;
	if ( rect.bottom < 0 || rect.bottom > GetSystemMetrics( SM_CYSCREEN ) ) return;

	AfxGetApp()->WriteProfileInt( "MsgView", "MsgBoxPos_X", rect.left );
	AfxGetApp()->WriteProfileInt( "MsgView", "MsgBoxPos_Y", rect.top );
	AfxGetApp()->WriteProfileInt( "MsgView", "MsgBoxSize_X", rect.right - rect.left );
	AfxGetApp()->WriteProfileInt( "MsgView", "MsgBoxSize_Y", rect.bottom - rect.top );

}

void CDlgMsgView::OnClose() 
{
	ShowWindow( SW_HIDE );
	
//	CDialog::OnClose();
}


#define FILE_WRITE( file, str ) file.Write( str, strlen( str ) )
void CDlgMsgView::OnMvSave() 
{
	char filename[ MAX_PATH ] = { 0 };

	if ( m_log == NULL ) return;

	// Get user filename
	if ( !CWinFile::GetSaveFileName( filename, "Save Messages", 
										"Error Messages (*.txt)\x0*.txt\x0"
										"All Files (*.*)\x0*.*\x0",
										"txt", NULL ) ) return;
	try
	{
		CWinFile file;
		if ( !file.OpenNew( filename, GENERIC_WRITE ) ) return;

		FILE_WRITE( file, "; Msg View Errors\r\n;\r\n" );
		FILE_WRITE( file, "; Filename : " );
		FILE_WRITE( file, filename );
		FILE_WRITE( file, "\r\n;\r\n;\r\n" );

		LPLOGMSGINFO pmi = NULL;
		while ( ( pmi = m_log->GetNext( pmi ) ) != NULL )
		{
			m_log->SaveToFile( &file, pmi );
		
		} // end while
	} // end try
	catch( ... )
	{
		return;
	} // end catch

	return;
}

HICON CDlgMsgView::GetIcon(DWORD i)
{
	switch( i )
	{
		case 0 : return ::LoadIcon( NULL, IDI_APPLICATION ); break;
		case 1 : return ::LoadIcon( NULL, IDI_ASTERISK ); break;
		case 2 : return ::LoadIcon( NULL, IDI_ERROR ); break;
		case 3 : return ::LoadIcon( NULL, IDI_EXCLAMATION ); break;
		case 4 : return ::LoadIcon( NULL, IDI_HAND ); break;
		case 5 : return ::LoadIcon( NULL, IDI_INFORMATION ); break;
		case 6 : return ::LoadIcon( NULL, IDI_QUESTION ); break;
		case 7 : return ::LoadIcon( NULL, IDI_WARNING ); break;
		case 8 : return ::LoadIcon( NULL, IDI_WINLOGO ); break;

	} // end switch

	return ::LoadIcon( NULL, IDI_INFORMATION );
}

LRESULT CDlgMsgView::OnUpdateList(WPARAM wParam, LPARAM lParam)
{
	UpdateList();
	return 0;
}

void CDlgMsgView::OnMvPopup() 
{
	UpdateData( TRUE );
	AfxGetApp()->WriteProfileInt( "MsgView", "Popup", m_bPopup ? 1 : 0 );
}

void CDlgMsgView::OnMvPopupWarnings()
{
	UpdateData( TRUE );
	AfxGetApp()->WriteProfileInt( "MsgView", "PopupWarnings", m_bPopupWarnings ? 1 : 0 );
}

LRESULT CDlgMsgView::OnInsertItem(WPARAM wParam, LPARAM lParam)
{
	LPLOGMSGINFO pmi = (LPLOGMSGINFO)lParam;
	if ( pmi == NULL ) return 0;

	int icon;
	if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONEXCLAMATION )		icon = 3;
	else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONWARNING )	icon = 7;
	else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONQUESTION )	icon = 6;
	else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONSTOP )		icon = 4;
	else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONERROR )		icon = 2;
	else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONHAND )		icon = 4;
	else if ( ( pmi->dwFlags & 0x000000F0L ) == MB_ICONASTERISK )	icon = 1;
	else icon = 5;

	int n = m_list.InsertItem( 0, pmi->szTitle, icon );
	m_list.SetItemText( n, 1, pmi->szTime );
	m_list.SetItemData( n, (DWORD)pmi ); 

	m_list.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );

	SetIcon( ::LoadIcon( NULL, IDI_EXCLAMATION ), TRUE );
	SetIcon( ::LoadIcon( NULL, IDI_EXCLAMATION ), FALSE );

	ShowMessage();

	return 0;
}

