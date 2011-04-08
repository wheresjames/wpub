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
// RectMover.h: interface for the CRectMover class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECTMOVER_H__BC59A45C_F910_4F55_B999_1A8541B0BFFF__INCLUDED_)
#define AFX_RECTMOVER_H__BC59A45C_F910_4F55_B999_1A8541B0BFFF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CRectMover  
{
public:
	BOOL Draw( HDC hDC, LPRECT pRect, LPRECT pEdit, BOOL bTicks = TRUE );
	HCURSOR GetCursor();
	BOOL Move( LPPOINT point, LPRECT pRect );
	void End();
	void Abort();
	BOOL Start( LPRECT pRect, LPRECT pRef, LPPOINT pt );
	CRectMover();
	virtual ~CRectMover();

	operator LPRECT() { return &m_rect; }
	LPRECT GetRect() { return &m_rect; }

	LPRECT GetEditRect() { return m_pEditRect; }

private:

	LPRECT			m_pEditRect;
	RECT			m_rectEditRef;
	RECT			m_rect;
	RECT			m_ref;
	POINT			m_ptMove;
	POINT			m_ptStart;

	DWORD			m_dwCursor;

};

#endif // !defined(AFX_RECTMOVER_H__BC59A45C_F910_4F55_B999_1A8541B0BFFF__INCLUDED_)
