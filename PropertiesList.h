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
#if !defined(AFX_PROPERTIESLIST_H__DB5003BE_4125_4D27_B5D3_D9025A6DC04D__INCLUDED_)
#define AFX_PROPERTIESLIST_H__DB5003BE_4125_4D27_B5D3_D9025A6DC04D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertiesList.h : header file
//

#include "3DList.h"
#include "PropList.h"

#include "DlgEditParam.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertiesList window

class CPropertiesList : public C3DList
{
// Construction
public:
	CPropertiesList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertiesList)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Init();
	virtual ~CPropertiesList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertiesList)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:

	void LoadList();
	void Update();

	CPropList& GetPropList() { return m_pl; }

	void EmptyList() { m_pl.Destroy(); }

	void EndEdit() { m_editparam.DestroyWindow(); }

	void SetCallbackWnd( HWND hWnd, UINT uCombo, UINT uRefresh, UINT uSetCombo ) 
	{	m_editparam.SetCallbackWnd( hWnd, uCombo, uRefresh, uSetCombo ); }

	LRESULT OnGetString( WPARAM wParam, LPARAM lParam );
	virtual LRESULT GetString( LPPROPINFO ppi, LPSTR str )
	{	return FALSE; }

	LRESULT OnFillCombo( WPARAM wParam, LPARAM lParam );
	virtual BOOL FillCombo( DWORD dwUser, CComboBox *pCombo )
	{	return FALSE; }

	LRESULT OnSetComboData( WPARAM wParam, LPARAM lParam );
	virtual BOOL SetComboData( DWORD dwUser, LPPROPCALLBACKINFO pPci )
	{	return FALSE; }

	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	virtual BOOL Refresh( DWORD dwAll, LPPROPINFO ppi )
	{	return FALSE; }

	void SetRefreshCallback( HWND hWnd, UINT uMsg )
	{	m_hWndRefresh = hWnd; m_uMsgRefresh = uMsg; }

	LRESULT DoRefreshCallback( WPARAM wParam, LPARAM lParam )
	{	if ( m_hWndRefresh != NULL && ::IsWindow( m_hWndRefresh ) )
			return ::SendMessage( m_hWndRefresh, m_uMsgRefresh, wParam, lParam ); 
		else return 0;
	}

private:

	BOOL		m_bInit;

	CPropList	m_pl;

	HWND	m_hWndRefresh;
	UINT	m_uMsgRefresh;

private:

	CDlgEditParam		m_editparam;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTIESLIST_H__DB5003BE_4125_4D27_B5D3_D9025A6DC04D__INCLUDED_)
