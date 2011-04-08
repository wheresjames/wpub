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
#if !defined(AFX_SLIDER_H__95A45112_1CC5_11D2_A487_000000000000__INCLUDED_)
#define AFX_SLIDER_H__95A45112_1CC5_11D2_A487_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Slider.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSlider window

class CSlider : public CSliderCtrl
{
// Construction
public:
	CSlider();

	void UpdateText();

	CString& GetText() { return m_sText; }

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetDword( DWORD dwPos );
	CString m_sText;
	virtual ~CSlider();

	void SetMessageTarget( HWND hWnd, UINT uMsg )
	{	m_hMsgWnd = hWnd; m_uMsg = uMsg; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CSlider)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	HWND	m_hMsgWnd;
	UINT	m_uMsg;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDER_H__95A45112_1CC5_11D2_A487_000000000000__INCLUDED_)
