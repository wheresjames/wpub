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
// HtmlProperties.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "HtmlProperties.h"

#include "PubThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHtmlProperties

CHtmlProperties::CHtmlProperties()
{
}

CHtmlProperties::~CHtmlProperties()
{
}


BEGIN_MESSAGE_MAP(CHtmlProperties, CPropertiesList)
	//{{AFX_MSG_MAP(CHtmlProperties)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHtmlProperties message handlers

BOOL CHtmlProperties::Show( CReg *pReg)
{
	EndEdit();

	// Destroy properties list
	GetPropList().Destroy();

	// Lose list items
	GetListCtrl().Destroy();

	// Rebuild header
	GetListCtrl().GetHeader().Destroy();
	GetListCtrl().GetHeader().AddHeaderItem( 120, "Name" );
	GetListCtrl().GetHeader().AddHeaderItem( 120, "Value" );

	if ( pReg == NULL ) 
	{	LoadList();
		RedrawWindow();
		return FALSE;
	} // end if

	// Add variables
	AddKey( pReg, pReg->GetKey( "vars" ) );
	AddKey( pReg, pReg->GetKey( "lvars" ) );
	AddKey( pReg, pReg->GetKey( "dvars" ) );

//	AddDynamic( pReg );

	// Loadup the list
	LoadList();

	RedrawWindow();

	return TRUE;
}

BOOL CHtmlProperties::FillCombo(DWORD dwUser, CComboBox *pCombo)
{
	switch( dwUser )
	{
		case 1 :
		{
			{ // Add local server links

				char link[ CWF_STRSIZE ];
				char host[ CWF_STRSIZE ];
				char name[ CWF_STRSIZE ] = { '~', 0x0 };
				DWORD port = 80;
				char strport[ CWF_STRSIZE ];

				// Get port number
				CFG().GetValue( "Settings", "WebServerPort", strport, sizeof( strport ) );
				CFG().GetValue( "Settings", "WebServerPort", &port );

				if ( !gethostname( name, CWF_STRSIZE - 1 ) )
				{
					LPHOSTENT he = gethostbyname( name );
					if ( he != NULL )
					{
						strcpy( host, "http://" );
						strcat( host, inet_ntoa( *(LPIN_ADDR) *( he->h_addr_list ) ) );
						if ( port != 80 ) strcat( host, ":" ), strcat( host, strport );
						strcat( host, "/" );

					} // end if
					else strcpy( host, "http://127.0.0.1/" );

				} // end if
				else strcpy( host, "http://127.0.0.1/" );

				CRKey *pRk = WEBSETTINGS().GetKey( "ImageMap" );
				if ( pRk != NULL )
				{
					LPREGVALUE	prv = NULL;
					while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
					{
						CWinFile::WebBuildPath( link, host, prv->cpkey );
						if ( *link ) pCombo->AddString( link );
					}

					// Add raw image names
					while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
						if ( prv->cpkey != NULL && *prv->cpkey ) 
							pCombo->AddString( prv->cpkey );

				} // end if

			} // end add local server links

			{ // Add publishing links

				LPPUBINFO ppi = NULL;
				char link[ CWF_STRSIZE ];

				while ( ( ppi = (LPPUBINFO)PUBLIST().GetNext( ppi ) ) != NULL )
				{
					if ( ppi->type == PUBTYPE_FTP )
					{
						// Update the filename
						CPubThread::GetFileName( ppi, TRUE );

						// Get HTTP path
						if ( FTPSERVERS().GetValue(	ppi->str, "HttpPath", 
													link, sizeof( link ) ) )
						{
							// Build link to file
							CWinFile::WebBuildPath( link, link, ppi->pub_fname );

							// Add it!
							if ( *link ) pCombo->AddString( link );

						} // end if

					} // end if

					else if ( ppi->type == PUBTYPE_DISK )
					{
						CPubThread::GetFileName( ppi, TRUE );
						CWinFile::WebBuildPath( link, ppi->path, ppi->pub_fname );
						if ( *link ) pCombo->AddString( link );

					} // end else if

				} // end while

			} // end add publishing links

		} break;

		case 2 :
		{
			// Add string values
			for ( DWORD i = 1; i < m_dwParams; i++ )
				pCombo->AddString( m_szParams[ i ] );

		} break;

	} // end switch

	return FALSE;
}

LRESULT CHtmlProperties::GetString( LPPROPINFO ppi, LPSTR str )
{

	return 0;
} 

BOOL CHtmlProperties::AddKey(CReg *pReg, CRKey *pRk)
{
	CRKey *pType = pReg->GetKey( "type" );

	if ( pRk != NULL )
	{
		char type[ CWF_STRSIZE ];
		LPREGVALUE prv = NULL;
		while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
		{		
			// Get type
			if ( pType == NULL ) m_dwParams = 0, m_szParams[ 0 ][ 0 ] = 0;

			// Get variable type
			else if ( pType->Get( prv->cpkey, type, sizeof( type ) ) )
				GetParams( type );

			// No params
			else m_dwParams = 0, m_szParams[ 0 ][ 0 ] = 0;
											
			// Image
			if ( m_dwParams > 0 )
			{
				// Check for group name
				if ( *prv->cpkey == '*' )
					GetPropList().Add( PROPTYPE_GROUP, 0, 0, &prv->cpkey[ 1 ], NULL, 0 );

				else if ( !strcmpi( m_szParams[ 0 ], "image" ) )
					GetPropList().Add(	PROPTYPE_FILE, PROPEDIT_COMBOEDIT, 1,
										prv->cpkey, prv->data, prv->size );

				else if ( !strcmpi( m_szParams[ 0 ], "diskimg" ) )
					GetPropList().Add(	PROPTYPE_FILE, PROPEDIT_FILE, 0,
										prv->cpkey, prv->data, prv->size );

				else if ( !strcmpi( m_szParams[ 0 ], "rgb" ) )
					GetPropList().Add(	PROPTYPE_RGBSTR, PROPEDIT_RGB, 0,
										prv->cpkey, prv->data, prv->size );

				else if ( !strcmpi( m_szParams[ 0 ], "slider" ) )
				{
					if ( m_dwParams < 2 ) m_lParams[ 1 ] = 0;
					if ( m_dwParams < 3 ) m_lParams[ 2 ] = 100;

					GetPropList().Add(	PROPTYPE_STR, PROPEDIT_SLIDER, 0,
										prv->cpkey, prv->data, prv->size,
										m_lParams[ 1 ], m_lParams[ 2 ] );
				} // end else if

				else if ( !strcmpi( m_szParams[ 0 ], "combo" ) )
					GetPropList().Add(	PROPTYPE_STR, PROPEDIT_COMBO, 2,
										prv->cpkey, prv->data, prv->size );


				// String
				else GetPropList().Add(	PROPTYPE_STR, PROPEDIT_EDIT, 0,
										prv->cpkey, prv->data, prv->size );
			
			} // end if

			// Check for group name
			else if ( *prv->cpkey == '*' )
				GetPropList().Add( PROPTYPE_GROUP, 0, 0, &prv->cpkey[ 1 ], NULL, 0 );

			else if ( *prv->pcdata == '#' )
			{
				GetPropList().Add(	PROPTYPE_RGBSTR, PROPEDIT_RGB, 0,
									prv->cpkey, prv->data, prv->size );
			} // end if

			// Assume string
			else GetPropList().Add(	PROPTYPE_STR, PROPEDIT_EDIT, 0,
									prv->cpkey, prv->data, prv->size );

		} // end while

	} // end if

	return TRUE;

/*	if ( pRk == NULL ) return FALSE;

	LPREGVALUE prv = NULL;
	while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
	{		
		if ( prv->data != NULL )
		{
			// Check for group name
			if ( *prv->cpkey == '*' )
				GetPropList().Add( PROPTYPE_GROUP, 0, 0, &prv->cpkey[ 1 ], NULL, 0 );

			else if ( *prv->pcdata == '#' )
			{
				GetPropList().Add(	PROPTYPE_RGBSTR, PROPEDIT_RGB, 0,
									prv->cpkey, prv->data, prv->size );
			} // end if

			// Add the property
			else GetPropList().Add(	PROPTYPE_STR, PROPEDIT_EDIT, 0,
									prv->cpkey, prv->data, prv->size );
		} // end if
		
	} // end while

	return TRUE;
*/
}


DWORD CHtmlProperties::GetParams( LPCTSTR str )
{
	m_dwParams = 0;

	// Sanity checks
	if ( str == NULL ) return FALSE;

	// Read in each token
	DWORD skip = 0, i = 0;
	while ( m_dwParams < 32 &&
			( skip = CReg::GetToken( (LPBYTE)m_szParams[ m_dwParams ], 256, (LPBYTE)&str[ i ], 256, ',' ) ) )
	{
		// Skip token
		i += skip; if ( str[ i ] == ',' ) i++;

		// Convert to integer
		m_lParams[ m_dwParams ] = strtol( m_szParams[ m_dwParams ], NULL, 10 );

		// Next token
		m_dwParams++;

	} // end while

	return m_dwParams;
}
