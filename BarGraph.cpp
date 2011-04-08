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
// BarGraph.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "BarGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBarGraph

CBarGraph::CBarGraph()
{
}

CBarGraph::~CBarGraph()
{
}


BEGIN_MESSAGE_MAP(CBarGraph, CStatic)
	//{{AFX_MSG_MAP(CBarGraph)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarGraph message handlers

BOOL CBarGraph::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CBarGraph::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rect;
	GetClientRect( &rect );

	// Create offscreen buffer
	if ( m_os.Create( dc.GetSafeHdc(), &rect ) ) 
	{
		// Draw the graph
		m_graph.Draw( m_os, m_os );

		// Draw from the offscreen buffer
		m_os.Draw( dc.GetSafeHdc(), &rect );

	} // end if

	// Just skip the offscreen buffer
	else m_graph.Draw( dc.GetSafeHdc(), &rect );

}	


void CBarGraph::OnSysColorChange() 
{
	CStatic::OnSysColorChange();

	m_graph.m_bck.Destroy();
	
}
