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
#if !defined(AFX_WZFRAME_H__0BDD2B86_8CA4_40F1_BC46_4453F19BE0ED__INCLUDED_)
#define AFX_WZFRAME_H__0BDD2B86_8CA4_40F1_BC46_4453F19BE0ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WzFrame.h : header file
//

#include "TitleWnd.h"
#include "BlankWnd.h"

#define CWZ_MAXPAGES		32

#define CWZ_SAVE			( WM_USER + 701 )
#define CWZ_RESTORE			( WM_USER + 702 )
#define CWZ_APPLY			( WM_USER + 703 )
#define CWZ_ENAPPLY			( WM_USER + 704 )

#define CWZ_NEXT			( WM_USER + 706 )
#define CWZ_BACK			( WM_USER + 707 )
#define CWZ_ENABLEBACK		( WM_USER + 708 )
#define CWZ_ENABLENEXT		( WM_USER + 709 )
#define CWZ_ENABLEFINISH	( WM_USER + 710 )
#define CWZ_ENABLEHELP		( WM_USER + 711 )
#define CWZ_SETPAGE			( WM_USER + 712 )
#define CWZ_SETNEXTPAGE		( WM_USER + 713 )
#define CWZ_SETFINISHTEXT	( WM_USER + 715 )
#define CWZ_GETCURRENTPAGE	( WM_USER + 716 )

#define CWZ_PAGECHG			( WM_USER + 720 )
#define CWZ_REFRESH			( WM_USER + 721 )
#define CWZ_GETDATA			( WM_USER + 722 )
#define CWZ_PAGECLOSE		( WM_USER + 723 )

/////////////////////////////////////////////////////////////////////////////
// CWzFrame dialog

class CWzFrame : public CDialog
{
// Construction
public:
	BOOL SetPage( DWORD pg );
	CWzFrame(UINT idd, CWnd* pParent = NULL);   // standard constructor
	~CWzFrame();

	void RemoveAllPages() { Destroy(); }

// Dialog Data
	//{{AFX_DATA(CWzFrame)
	CBlankWnd	m_blank;
	CTitleWnd	m_title;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWzFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWzFrame)
	virtual BOOL OnInitDialog();
	afx_msg void OnNext();
	afx_msg void OnBack();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnHelpbtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	LRESULT OnGetData( WPARAM wParam, LPARAM lParam );
	LRESULT OnEnableBack( WPARAM wParam, LPARAM lParam );
	LRESULT OnEnableNext( WPARAM wParam, LPARAM lParam );
	LRESULT OnEnableFinish( WPARAM wParam, LPARAM lParam );
	LRESULT OnEnableHelp( WPARAM wParam, LPARAM lParam );
	LRESULT OnBackMsg( WPARAM wParam, LPARAM lParam );
	LRESULT OnNextMsg( WPARAM wParam, LPARAM lParam );
	LRESULT OnSetPage( WPARAM wParam, LPARAM lParam );
	LRESULT OnSetFinishText( WPARAM wParam, LPARAM lParam );
	LRESULT OnSetNextPage(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetCurrentPage(WPARAM wParam, LPARAM lParam);
	static void* GetData( HWND hPage );
	static void EnableBack( HWND hPage, BOOL enable );
	static void EnableNext( HWND hPage, BOOL enable );
	static void EnableFinish( HWND hPage, BOOL enable );
	static void EnableHelp( HWND hPage, BOOL enable, LPCTSTR pLink );
	static void Next( HWND hPage );
	static void Back( HWND hPage );
	static void SetPage( HWND hPage, DWORD page );
	static void SetFinishText( HWND hPage, LPCTSTR pText, BOOL bCancel = FALSE );
	static void SetNextPage( HWND hPage, DWORD pg );
	static DWORD GetCurrentPage( HWND hPage );

	void Destroy();
	BOOL SetPage( DWORD pg, CDialog *dlg, DWORD idd, LPCTSTR name, LPCTSTR help = NULL, DWORD dwNext = MAXDWORD, DWORD dwBack = MAXDWORD )
	{	if ( pg < CWZ_MAXPAGES ) 
		{ 
			m_pg[ pg ] = dlg; 
			m_idd[ pg ] = idd; 
			if ( name != NULL ) strcpy( m_name[ pg ], name ); 
			if ( help != NULL ) strcpy( m_help[ pg ], help ); 			
			m_next[ pg ] = dwNext;
			m_back[ pg ] = dwBack;
			return TRUE;
		} 
		return FALSE;
	}
	CDialog* GetPage( DWORD pg )
	{	if ( pg < CWZ_MAXPAGES ) return m_pg[ pg ]; return NULL; }

	void SetHelpRoot( LPCTSTR ptr ) 
	{	if ( ptr == NULL ) *m_helproot = 0; else strcpy( m_helproot, ptr ); }

	void* SetData( void *ptr ) { return ( m_pData = ptr ); }

private:

	char		m_helproot[ CWF_STRSIZE ];

	DWORD		m_dwPg;
	DWORD		m_dwLastPage;
	DWORD		m_dwNextPage;
	CDialog		*m_pg[ CWZ_MAXPAGES ];
	DWORD		m_idd[ CWZ_MAXPAGES ];
	char		m_name[ CWZ_MAXPAGES ][ 64 ];
	char		m_help[ CWZ_MAXPAGES ][ CWF_STRSIZE ];
	BOOL		m_next[ CWZ_MAXPAGES ];
	BOOL		m_back[ CWZ_MAXPAGES ];

	BOOL		m_bNext;
	BOOL		m_bBack;
	BOOL		m_bFinish;

	void		*m_pData;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZFRAME_H__0BDD2B86_8CA4_40F1_BC46_4453F19BE0ED__INCLUDED_)
