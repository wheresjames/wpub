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
// MenuItemList.cpp: implementation of the CMenuItemList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuItemList.h"
#include "WinMenu.h"

#include "WndToolbar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuItemList::CMenuItemList()
{

}

CMenuItemList::~CMenuItemList()
{	Destroy();
}

void CMenuItemList::DeleteObject( void *node )
{
	LPMITEMINFO pmi = (LPMITEMINFO)node;
	if ( pmi == NULL ) return;

	// Delete sub menu if any
	if ( pmi->submenu != NULL )
	{	delete pmi->submenu;
		pmi->submenu = NULL;
	} // end if

	// Delete toolbar if any
	if ( pmi->toolbar != NULL )
	{	delete pmi->toolbar;
		pmi->toolbar = NULL;
	} // end if

	CLList::DeleteObject( node );
}

