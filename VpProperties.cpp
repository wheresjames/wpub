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
// VpProperties.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "VpProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CVpProperties

CVpProperties::CVpProperties()
{
}

CVpProperties::~CVpProperties()
{
}


BEGIN_MESSAGE_MAP(CVpProperties, CPropertiesList)
	//{{AFX_MSG_MAP(CVpProperties)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVpProperties message handlers

BOOL CVpProperties::Show(LPFX pfx)
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

	// Is there an object?
	if ( pfx == NULL ) 
	{	RedrawWindow();
		return FALSE;
	} // end if

	switch( pfx->type )
	{
		case FXTYPE_TEXT:

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Text", NULL, 0 );

			// Add properties
			GetPropList().Add(	PROPTYPE_STR, PROPEDIT_EDIT,
								0, "Text", 
								pfx->data, sizeof( pfx->data ) );

			break;

		case FXTYPE_TIME:

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Time", NULL, 0 );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Style", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_ANALOG, "Analog", "Digital" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Time Zone", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_GMT, "GMT", "Local" );

			GetPropList().Add(	PROPTYPE_STR, PROPEDIT_EDIT,
								0, "Format", 
								&pfx->sep, sizeof( pfx->sep ) );

			break;

		case FXTYPE_DATE:

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Date", NULL, 0 );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Style", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_ANALOGCAL, "Analog", "Digital" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Time Zone", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_GMT, "GMT", "Local" );

			GetPropList().Add(	PROPTYPE_STR, PROPEDIT_EDIT,
								0, "Format String", 
								&pfx->sep, sizeof( pfx->sep ) );
			break;

		
		case FXTYPE_SCREENSHOT :

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Screenshot", NULL, 0 );

			GetPropList().Add(	PROPTYPE_STR, PROPEDIT_COMBO,
								1, "Window Name", 
								&pfx->data, sizeof( pfx->data ),
								0, 0, 0, NULL, NULL, TRUE );
			break;

		case FXTYPE_URL :

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Internet Image", NULL, 0 );

			GetPropList().Add(	PROPTYPE_STR, PROPEDIT_EDIT,
								1, "URL", 
								&pfx->data, sizeof( pfx->data ),
								0, 0, 0, NULL, NULL, TRUE );
			break;

		case FXTYPE_VIDEO :

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Video", NULL, 0 );

			GetPropList().Add(	PROPTYPE_DWORD, PROPEDIT_COMBO,
								2, "Video Driver", 
								&pfx->driver, sizeof( pfx->driver ),
								0, 0, 0, NULL, NULL, TRUE );
			break;

		case FXTYPE_IMAGE:
		case FXTYPE_WATERMARK:

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Image", NULL, 0 );

			GetPropList().Add(	PROPTYPE_FILE, PROPEDIT_FILE, 
								0, "Image", 
								pfx->data, sizeof( pfx->data ) );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Real Size", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_REALSIZE, "True", "False" );

			break;

		case FXTYPE_RECT:

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Rectangle", NULL, 0 );

			GetPropList().Add(	PROPTYPE_RGB, PROPEDIT_RGB, 
								0, "Background Color", 
								&pfx->bck, sizeof( pfx->bck ) );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Border", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_BORDER, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Border Type", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_FOUNDERED, "Foundered", "Normal" );

			break;

	} // end switch

	switch( pfx->type )
	{

		case FXTYPE_IMAGE:
		case FXTYPE_WATERMARK:

		case FXTYPE_VIDEO :
		case FXTYPE_SCREENSHOT:

		case FXTYPE_URL :

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Image Properties", NULL, 0 );

			GetPropList().Add(	PROPTYPE_DWORD, PROPEDIT_SLIDER, 
								0, "Translucency", 
								&pfx->translucency, sizeof( pfx->translucency ), 0, 10000 );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Transparency", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_TRANSPARENCY, "True", "False" );

			GetPropList().Add(	PROPTYPE_DWORD, PROPEDIT_SLIDER, 
								0, "Chroma (+)", 
								&pfx->cpos, sizeof( pfx->cpos ), 0, 255 );
			
			GetPropList().Add(	PROPTYPE_DWORD, PROPEDIT_SLIDER,  
								0, "Chroma (--)", 
								&pfx->cneg, sizeof( pfx->cneg ), 0, 255 );

			GetPropList().Add(	PROPTYPE_RGB, PROPEDIT_RGB, 
								0, "Chroma Key", 
								&pfx->trans, sizeof( pfx->trans ) );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Aspect Ratio", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_ASPECT, "Maintain", "Ignore" );


			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Border", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_BORDER, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Border Type", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_FOUNDERED, "Foundered", "Normal" );

			
			break;


		case FXTYPE_TIME:
		case FXTYPE_DATE:
		case FXTYPE_STEXT:
		case FXTYPE_TEXT:

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Image Properties", NULL, 0 );

			GetPropList().Add(	PROPTYPE_DWORD, PROPEDIT_SLIDER, 
								0, "Translucency", 
								&pfx->translucency, sizeof( pfx->translucency ), 0, 10000 );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Background", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_BCK, "True", "False" );

			GetPropList().Add(	PROPTYPE_RGB, PROPEDIT_RGB, 
								0, "Background Color", 
								&pfx->bck, sizeof( pfx->bck ) );

			GetPropList().Add(	PROPTYPE_RGB, PROPEDIT_RGB, 
								0, "Foreground", 
								&pfx->rgb, sizeof( pfx->rgb ) );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Border", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_BORDER, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Border Type", 
								&pfx->f1, sizeof( pfx->f1 ), 0, 0,
								FXF1_FOUNDERED, "Foundered", "Normal" );

			// Add text group
			GetPropList().Add( PROPTYPE_GROUP, 0, 0, "Text Properties", NULL, 0 );

			// Add properties
			GetPropList().Add(	PROPTYPE_FONT, PROPEDIT_FONT, 
								0, "Font", 
								&pfx->lf, sizeof( pfx->lf ) );

			GetPropList().Add(	PROPTYPE_LONG, PROPEDIT_SLIDER, 
								0, "Shadow", 
								&pfx->shadow, sizeof( pfx->shadow ), -12, 12 );

			GetPropList().Add(	PROPTYPE_LONG, PROPEDIT_EDIT, 
								0, "Rotation", 
								&pfx->lf.lfEscapement, sizeof( pfx->lf.lfEscapement ), 0, 3600 );


			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Single Line", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								DT_SINGLELINE, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Word Break", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								DT_WORDBREAK, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Multiline Center", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								EDT_MULTICENTER, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Escapement Center", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								EDT_ESCAPEMENTCENTER, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "No Clipping", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								DT_NOCLIP, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Path Ellipsis", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								DT_PATH_ELLIPSIS, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Like Edit Ctrl", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								DT_EDITCONTROL, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "End Ellipsis", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								DT_END_ELLIPSIS, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Expand Tabs", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								DT_EXPANDTABS, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "External Leading", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								DT_EXTERNALLEADING, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "No Prefix", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								DT_NOPREFIX, "True", "False" );

			GetPropList().Add(	PROPTYPE_FLAG, PROPEDIT_CHECK,
								0, "Right To Left", 
								&pfx->flags, sizeof( pfx->flags ), 0, 0,
								DT_RTLREADING, "True", "False" );

			break;


	} // end switch

	// Loadup the list
	LoadList();

	RedrawWindow();

	return TRUE;
}

BOOL CVpProperties::FillCombo(DWORD dwUser, CComboBox *pCombo)
{
	if ( pCombo == NULL ) return FALSE;

	switch( dwUser )
	{
		case 1 :
		{
			// Add the desktop window
			pCombo->SetCurSel( pCombo->AddString( "< desktop >" ) );
			pCombo->AddString( "< active window >" );

			HWND hWnd = ::GetDesktopWindow();
			if ( !::IsWindow( hWnd ) ) return FALSE;

			// Get first child window
			hWnd = ::GetWindow( hWnd, GW_CHILD );

			do
			{
				char text[ 256 ];
				if( ::GetWindowText( hWnd, text, sizeof( text ) ) && *text != 0 )
					if ( pCombo->FindString( -1, text ) == CB_ERR )
						pCombo->AddString( text );
						
			} while ( ::IsWindow( hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) ) );

		} break;

		case 2 :
		{
			CCapture	cap;
			DWORD		drivers = cap.GetNumDrivers();

			// For each driver
			for ( DWORD i = 0; i < drivers; i++ )
			{
				char				show[ CWF_STRSIZE ];
				CAPTUREDRIVERINFO	ci;

				if ( cap.GetDriverInfo( i, &ci ) )
				{
					sprintf( show, "%s (%s)", ci.szName, ci.szVer );

					int index = pCombo->AddString( show );
					if ( index != LB_ERR ) pCombo->SetItemData( index, ci.index );
				} // end if

			} // end for
		} break;
		
	} // end switch

	return TRUE;
}

LRESULT CVpProperties::GetString( LPPROPINFO ppi, LPSTR str )
{
	if ( ppi == NULL || str == NULL ) return 0;

	switch( ppi->user )
	{
		case 2 :
		{
			CCapture	cap;
			DWORD		i = (DWORD)CPropList::GetNum( ppi );
			DWORD		drivers = cap.GetNumDrivers();

			CAPTUREDRIVERINFO	ci;
			if ( cap.GetDriverInfo( i, &ci ) )
				sprintf( str, "%s (%s)", ci.szName, ci.szVer );

		} break;		
	
		default : return 0;

	} // end switch

	return 1;
}

BOOL CVpProperties::SetComboData( DWORD dwUser, LPPROPCALLBACKINFO pPci )
{
	if ( pPci == NULL ) return 0;

/*	switch( dwUser )
	{
		case 3 :
		{ 
//			DWORD num = (DWORD)CPropList::GetNum( pPci->pPi );

			switch( pPci->dwData )
			{
				case 0 : CPropList::SetStr( pPci->pPi, "%w, %b %D, %Y" );
				case 1 : CPropList::SetStr( pPci->pPi, "%W, %B %D, %Y" );
				case 2 : CPropList::SetStr( pPci->pPi, "%c-%d-%Y" );
				case 3 : CPropList::SetStr( pPci->pPi, "%c-%d-%y" );
				case 4 : CPropList::SetStr( pPci->pPi, "%d-%c-%Y" );
				case 5 : CPropList::SetStr( pPci->pPi, "%d-%c-%y" );
				case 6 : CPropList::SetStr( pPci->pPi, "%Y.%c.%d" );

			} // end switch

		} break;

	} // end switch
*/
	return 1;
}

		
BOOL CVpProperties::Refresh( DWORD dwAll, LPPROPINFO ppi )
{	APP()->ReGen( dwAll ); return TRUE;
}


