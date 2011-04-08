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
#if !defined(AFX_PGPUBEXTRA_H__B126CFAF_37F9_49FC_839E_FF432EAB0CF0__INCLUDED_)
#define AFX_PGPUBEXTRA_H__B126CFAF_37F9_49FC_839E_FF432EAB0CF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgPubExtra.h : header file
//

typedef struct tagCODECINFO : LLISTINFO
{
	DWORD	fourCC;
	BOOL	bConfig;

} CODECINFO, *LPCODECINFO;

/////////////////////////////////////////////////////////////////////////////
// CPgPubExtra dialog

class CPgPubExtra : public CDialog
{
// Construction
public:
	void SelectCodec( DWORD fourCC );
	void FillCodecs();
	long GetSlidePosition( long frames, long seconds );
	double GetFrameRate( long frames, long seconds );
	void DoEnable();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgPubExtra(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgPubExtra)
	enum { IDD = IDD_PUBEXTRA };
	CComboBox	m_comboCodec;
	CSliderCtrl	m_slidePbScale;
	CSliderCtrl	m_slideCapScale;
	CString	m_sCapFrames;
	CString	m_sCapSeconds;
	CString	m_sPbFrames;
	CString	m_sPbSeconds;
	BOOL	m_bCompress;
	BOOL	m_bCapMotion;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgPubExtra)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgPubExtra)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCapFrames();
	afx_msg void OnChangeCapSeconds();
	afx_msg void OnChangePbFrames();
	afx_msg void OnChangePbSeconds();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSettings();
	afx_msg void OnSelchangeCodec();
	afx_msg void OnCompress();
	afx_msg void OnCapmotion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CLList			m_codecs;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGPUBEXTRA_H__B126CFAF_37F9_49FC_839E_FF432EAB0CF0__INCLUDED_)
