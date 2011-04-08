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
// HtmlBuilder.cpp: implementation of the CHtmlBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "HtmlBuilder.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// _main=^%w*%[(1)%]\n(1:_data)
// _data=(name):(type):(interf)=(value)

//-------------------------------------------------
//     |     |     |     |
//-------------------------------------------------



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHtmlBuilder::CHtmlBuilder()
{

}

CHtmlBuilder::~CHtmlBuilder()
{

}

BOOL CHtmlBuilder::CreatePage(LPCTSTR pSource, LPCTSTR pPath, CReg *pReg)
{														 
	// Sanity check
	if ( pSource == NULL || pPath == NULL || pReg == NULL ) return FALSE;

	char src[ CWF_STRSIZE ];
	CWinFile::GetPathFromFileName( pSource, src );

	// Create target directory
	CWinFile::DeleteDirectory( pPath );
	CWinFile::CreateDirectory( pPath );

	// Get variables
	CRKey vars;
	vars.Copy( pReg->GetKey( "vars" ) );
	vars.Copy( pReg->GetKey( "lvars" ), TRUE );
	vars.Copy( pReg->GetKey( "dvars" ), TRUE );

	// from to file buffers
	char from[ CWF_STRSIZE ], to[ CWF_STRSIZE ];

	// Copy variable files
	LPREGVALUE prv = NULL;
	char type[ CWF_STRSIZE ];
	CRKey *pType = pReg->GetKey( "type" );
	if ( pType != NULL )
		while ( ( prv = (LPREGVALUE)vars.GetNext( prv ) ) != NULL )
			if ( pType->Get( prv->cpkey, type, sizeof( type ) ) )
				if ( !strcmpi( type, "diskimg" ) )
				{ 
					// Build paths to files
					strcpy( from, prv->pcdata );
					CWinFile::GetFileNameFromPath( from, prv->pcdata );
					CWinFile::BuildPath( to, pPath, prv->pcdata );

					// Copy the file
					CopyFile( from, to, FALSE );

				} // end if			

	// Copy files
	CRKey *pRk = pReg->GetKey( "files" );
	if ( pRk != NULL )
	{	LPREGVALUE prv = NULL;
		while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
		{	
			CWinFile::BuildPath( from, src, prv->cpkey );
			CWinFile::BuildPath( to, pPath, prv->cpkey );

			// Is it an HTML file?
			if ( CWinFile::CmpExt( prv->cpkey, "htm" ) )
			{
				// Replace tokens
				vars.Replace( from, to );			

			} // end if

			// Just copy the file
			else CopyFile( from, to, FALSE );

		} // end while
	} // end if

	return TRUE;
}
										