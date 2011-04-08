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
#if !defined(AFX_DLGTREEFRAME_H__1A550BEF_EC7B_484F_A508_BA5EE9039F55__INCLUDED_)
#define AFX_DLGTREEFRAME_H__1A550BEF_EC7B_484F_A508_BA5EE9039F55__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgTreeFrame.h : header file
//

#define DTF_STRSIZE		1024

#ifndef WM_PG_MSGS
#define WM_PG_MSGS

#define WM_PG_SAVE		( WM_USER + 701 )
#define WM_PG_RESTORE	( WM_USER + 702 )
#define WM_PG_APPLY		( WM_USER + 703 )
#define WM_PG_ENAPPLY	( WM_USER + 704 )

#define WM_PG_CHANGE	( WM_USER + 720 )
#define WM_PG_REFRESH	( WM_USER + 721 )
#define WM_PG_GETDATA	( WM_USER + 722 )

#endif

typedef struct tagTREEPAGEINFO
{
	DWORD		index;
	HICON		icon;
	DWORD		iindex;
	CDialog		*pg;
	DWORD		idd;
	char		name[ DTF_STRSIZE ];
	char		help[ DTF_STRSIZE ];
	char		hlink[ DTF_STRSIZE ];	

	// Sub pages
	DWORD				dwSize;
	tagTREEPAGEINFO		*pHead;
	tagTREEPAGEINFO		*pTail;

	tagTREEPAGEINFO		*pParent;
	tagTREEPAGEINFO		*pNext;
	tagTREEPAGEINFO		*pPrev;

} TREEPAGEINFO; // end typedef struct
typedef TREEPAGEINFO*		LPTREEPAGEINFO;
typedef LPTREEPAGEINFO		HTPAGE;


/////////////////////////////////////////////////////////////////////////////
// CDlgTreeFrame dialog

class CDlgTreeFrame : public CDialog
{
// Construction
public:
	BOOL SetPageByIndex( DWORD i, HTPAGE hParent = NULL );
	BOOL Size();
	BOOL SetPage( HTPAGE hpg );
	BOOL RemovePage( HTPAGE hParent, HTPAGE node );
	HTPAGE AddPage( HTPAGE hParent, HICON hIcon, CDialog *pg, DWORD idd, LPCTSTR name, LPCTSTR help = NULL, LPCTSTR hlink = NULL );
	void DeleteTreePageList( LPTREEPAGEINFO node );
	void Destroy();
	CDlgTreeFrame(CWnd* pParent = NULL);   // standard constructor
	~CDlgTreeFrame();

// Dialog Data
	//{{AFX_DATA(CDlgTreeFrame)
	enum { IDD = IDD_SETTINGS };
	CTreeCtrl	m_tree;
	CStatic	m_blank;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTreeFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTreeFrame)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnApply();
	afx_msg void OnPleasehelp();
	afx_msg void OnItemexpandedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	DWORD GetCurPageIndex();
	LRESULT OnEnableApply( WPARAM wParam, LPARAM lParam );
	static void EnableApply( HWND hWnd );
	void SendPageMessage( UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L, LPTREEPAGEINFO pParent = NULL );
	HTREEITEM MakeTreeEntry( LPCTSTR pText, HTREEITEM hParent, DWORD dwImage );
	BOOL FillTree(HTPAGE hParent = NULL, HTREEITEM hItemParent = NULL );

	void SetHelpRoot( LPCTSTR ptr ) 
	{	if ( ptr == NULL ) *m_helproot = 0; else strcpy_sz( m_helproot, ptr ); }

	DWORD GetSize() { return m_dwSize; }
	DWORD GetNumPages() { return GetSize(); }

	HTPAGE GetNext( HTPAGE hParent, HTPAGE ptr )
	{	if ( ptr != NULL ) return ptr->pNext; 
		if ( hParent != NULL ) return hParent->pHead;
		return m_pg.pHead; 
	}

	void SetStartPage( DWORD pg ) { m_dwStartPage = pg; }
	DWORD GetLastPage() { return m_dwLastPage; }

private:

	DWORD				m_dwIcon;
	DWORD				m_dwStartPage;
	DWORD				m_dwLastPage;

	char				m_helproot[ DTF_STRSIZE ];

	DWORD				m_dwIndex;
	DWORD				m_dwSize;
	TREEPAGEINFO		m_pg;

	HTPAGE				m_hpg;

	CImageList			m_images;

	BOOL				m_bApply;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTREEFRAME_H__1A550BEF_EC7B_484F_A508_BA5EE9039F55__INCLUDED_)
