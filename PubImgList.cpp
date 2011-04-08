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
// PubImgList.cpp: implementation of the CPubImgList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vp.h"
#include "PubImgList.h"

#include "MainFrm.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPubImgList::CPubImgList()
{
	m_dwSel = 0;
}

CPubImgList::~CPubImgList()
{

}

void CPubImgList::Destroy()
{
	// Stop all video
	LPVIDEOINFO pvi = NULL;
	while ( ( pvi = (LPVIDEOINFO)VIDEOLIST().GetNext( pvi ) ) != NULL )
		if ( pvi->cap != NULL ) pvi->cap->SetFrameRate( 0 );

	CLList::Destroy();
}

void CPubImgList::DeleteObject(void * node)
{
	LPPUBIMGINFO ppii = (LPPUBIMGINFO)node;
	if ( ppii == NULL ) return;

	// Release image dialog box
	if ( ppii->imgview != NULL ) 
	{	if ( ::IsWindow( ppii->imgview->GetSafeHwnd() ) )
			ppii->imgview->DestroyWindow();
		delete ppii->imgview;
		ppii->imgview = NULL;
	} // end if

	// Release offscreen memory class
	if ( ppii->os != NULL ) 
	{	delete ppii->os;
		ppii->os = NULL;
	} // end if

	// Release configuration file class
	if ( ppii->cfg != NULL ) 
	{	delete ppii->cfg;
		ppii->cfg = NULL;
	} // end if

	// Release fx class
	if ( ppii->fx != NULL ) 
	{	delete ppii->fx;
		ppii->fx = NULL;
	} // end if

	// Release offscreen memory class
	if ( ppii->bck != NULL ) 
	{	delete ppii->bck;
		ppii->bck = NULL;
	} // end if
	
	// Release offscreen memory class
	if ( ppii->keybck != NULL ) 
	{	delete ppii->keybck;
		ppii->keybck = NULL;
	} // end if
	
	// Release offscreen memory class
	if ( ppii->maskdc != NULL ) 
	{	delete ppii->maskdc;
		ppii->maskdc = NULL;
	} // end if

	// Release lock
	if ( ppii->lock != NULL ) 
	{	delete ppii->lock;
		ppii->lock = NULL;
	} // end if

	// Release memory
	CLList::DeleteObject( node );
}

LPPUBIMGINFO CPubImgList::Add( LPCTSTR pName )
{
	if ( pName == NULL ) return FALSE;

	LPPUBIMGINFO node = FindByName( pName );
	if ( node != NULL ) return node;

	// Allocate memory
	node = (LPPUBIMGINFO)New();
	if ( node == NULL ) return NULL;

	// Save the name
	strcpy_sz( node->name, pName );

	// Allocate offscreen memory class
	node->os = new CWinDc();
	if ( node->os == NULL ) { Delete( node ); return NULL; }

	// Allocate configuration class
	node->cfg = new CCfgFile();
	if ( node->cfg == NULL ) { Delete( node ); return NULL; }

	// Allocate configuration class
	node->fx= new CFx();
	if ( node->fx == NULL ) { Delete( node ); return NULL; }

	// Allocate offscreen memory class
	node->bck = new CWinDc();
	if ( node->bck == NULL ) { Delete( node ); return NULL; }

	// Allocate offscreen memory class
	node->maskdc = new CWinDc();
	if ( node->maskdc == NULL ) { Delete( node ); return NULL; }

	// Allocate memory for view dialog
	node->imgview = new CDlgImgView();
	if ( node->imgview == NULL ) { Delete( node ); return NULL; }

	// Allocate memory for view dialog
	node->lock = new CTlLock();
	if ( node->lock == NULL ) { Delete( node ); return NULL; }

	node->keybck = NULL;
	node->rgbmask = RGB( 255, 255, 255 );

	return node;
}

LPPUBIMGINFO CPubImgList::FindByName(LPCTSTR pName)
{
	if ( pName == NULL ) return FALSE;

	LPPUBIMGINFO ppii = NULL;

	// Search for this name
	while( ( ppii = (LPPUBIMGINFO)GetNext( ppii ) ) != NULL )
		if ( !strcmpi( ppii->name, pName ) ) return ppii;

	return NULL;
}


BOOL CPubImgList::Draw(HDC hDC, LPRECT pRect, DWORD i)
{
	LPPUBIMGINFO node = GetImage( i );
	if ( node == NULL ) return FALSE;

	// Draw the image
	return node->os->Draw( hDC, pRect );
}

LPPUBIMGINFO CPubImgList::GetImage(DWORD i)
{
	LPPUBIMGINFO node = (LPPUBIMGINFO)GetByIndex( i );
	if ( node == NULL ) return NULL;
	if ( node->os == NULL ) return NULL;
	if ( node->cfg == NULL ) return NULL;

	return node;
}

LPPUBIMGINFO CPubImgList::GetSelImage()
{
	return GetImage( m_dwSel );
}

BOOL CPubImgList::Update(LPPUBIMGINFO pPii, BOOL bAll)
{
	if ( pPii == NULL ) return FALSE;

	// Aquire lock
	CTlLocalLock ll( pPii->lock, 3000 );
	if ( !ll.IsLocked() ) return FALSE;

	// Draw the background
	DWORD type = 0;
	pPii->cfg->GetValue( "Settings", "Type", &type );

	if ( type == 0 )
	{
		// Ensure valid offscreen space
		if ( !pPii->os->Create( NULL, &pPii->rect ) )
			return FALSE;

		CGrDC::FillSolidRect( *pPii->os, *pPii->os, pPii->rgbbck );

	} // en dif

	else if ( type == 1 )
	{
		DWORD driver = 0;
		pPii->cfg->GetValue( "Settings", "Driver", &driver );

		LPVIDEOINFO pvi = VIDEOLIST().Add( driver );

		// Regrab video if needed
		if ( pvi != NULL && bAll )
		{
			pvi->cap->GrabFrame();
//			pvi->cap->WaitCapture();
		} // end if

		// Get video size if needed
		if ( ( pPii->f1 & PIIF1_CUSTOMSIZE ) == 0 )
		{
			if ( pvi != NULL && pvi->os->GetSafeHdc() != NULL )
			{
				// Ensure rect matches video
				if ( !EqualRect( &pPii->rect, *pvi->os ) )
					CopyRect( &pPii->rect, *pvi->os );
			} // end if

		} // end if

		// Ensure valid offscreen space
		if ( !pPii->os->Create( NULL, &pPii->rect ) )
			return FALSE;

		// Get dc pointer
		CWinDc *os = pvi->os;

		// Used averaged frame if needed
		if ( ( pvi->f1 & VIF1_FRAMEAVG ) != 0 && pvi->tavg->GetSafeHdc() != NULL )
			os = pvi->tavg;
		
		// Use motion frame if needed
//		if ( pvi->motion.bEnable && pvi->mos->GetSafeHdc() != NULL ) os = pvi->mos;

		// Draw the video frame
		if ( os != NULL && os->GetSafeHdc() != NULL ) 
		{
			if (	( pPii->f1 & PIIF1_CHROMAKEY ) != 0 && 
					DrawChromaBck( pPii->os, pPii ) )
			{	os->SetTransparency( TRUE );
				os->SetTransparentColor( pPii->rgbkey );
				os->SetChromaErr( pPii->cpos, pPii->cneg );
				os->SetAntiAlias( ( pPii->f1 & PIIF1_NOSMOOTHING ) == 0 );
			} // end if
			else os->SetTransparency( FALSE );
			os->ReleaseMask();

			// Draw the video
			os->Draw( *pPii->os, *pPii->os );

			// Turn transparency back off
			os->SetTransparency( FALSE );

		} // end if

		// Draw solid background if no video
		else CGrDC::FillSolidRect( *pPii->os, *pPii->os, RGB( 40, 40, 40 ) );
		
	} // end if

	else if ( type == 2 )
	{
		// Ensure valid offscreen space
		if ( !pPii->os->Create( NULL, &pPii->rect ) )
			return FALSE;

		char bck[ CWF_STRSIZE ] = { 0 };
		pPii->cfg->GetValue( "Settings", "BckImage", bck, sizeof( bck ) );

		// Load image from disk if needed
		if ( pPii->bck->GetSafeHdc() == NULL || strcmpi( pPii->bckimgname, bck ) )
		{	
			CWinImg img;
			if ( img.Load( bck ) )
			{
				// Ensure offscreen buffer
				pPii->bck->Create( NULL, &pPii->rect );

				// Draw the image
				img.Draw( *pPii->bck, *pPii->bck );

				// Save the image name
				strcpy( pPii->bckimgname, bck );

			} // end if

		} // end if

		if (	( pPii->f1 & PIIF1_CHROMAKEY ) != 0 && 
				DrawChromaBck( pPii->os, pPii ) )
		{	pPii->bck->SetTransparency( TRUE );
			pPii->bck->SetTransparentColor( pPii->rgbkey );
			pPii->bck->SetChromaErr( pPii->cpos, pPii->cneg );
			pPii->bck->SetAntiAlias( ( pPii->f1 & PIIF1_NOSMOOTHING ) == 0 );
		} // end if
		else pPii->bck->SetTransparency( FALSE );
		pPii->bck->ReleaseMask();

		// Draw the image
		if ( !pPii->bck->Draw( *pPii->os, *pPii->os ) )
			CGrDC::FillSolidRect( *pPii->os, *pPii->os, RGB( 40, 40, 40 ) );

		pPii->bck->SetTransparency( FALSE );

	} // end else if

	// Do Effects
	BOOL ret = pPii->fx->Draw( *pPii->os, *pPii->os, TRUE, bAll );

	if ( ( pPii->f1 & PIIF1_MASK ) != 0 )
	{
		// Load image from disk if needed
		if (	pPii->maskdc->GetSafeHdc() == NULL || 
				( pPii->f1 & PIIF1_MASKIMG ) != ( pPii->fc1 & PIIF1_MASKIMG ) ||				
				strcmpi( pPii->mask, pPii->maskloaded ) )
		{	
			CWinImg img;
			if ( img.Load( pPii->mask ) )
			{	
				// Ensure offscreen buffer
				pPii->maskdc->Create( NULL, &pPii->rect );
				pPii->maskdc->ReleaseMask();

				// Grayscale image if mask
				if ( ( pPii->f1 & PIIF1_MASKIMG ) != 0 ) img.GrayScale();

				// Draw the image
				img.Draw( *pPii->maskdc, *pPii->maskdc );

				// Set flags
				MATCHBIT( pPii->fc1, pPii->f1, PIIF1_MASKIMG );

				// Save the image name
				strcpy( pPii->maskloaded, pPii->mask );

			} // end if

		} // end if

		// Mask the image
		if ( pPii->maskdc->GetSafeHdc() != NULL )			
		{
			char fname[ CWF_STRSIZE ];
			CWinFile::GetFileNameFromPath( pPii->mask, fname );
			if ( ( pPii->f1 & PIIF1_MASKIMG ) != 0 )
				pPii->os->Mask( pPii->maskdc, NULL, NULL, pPii->rgbmask );
			else 
			{	pPii->maskdc->SetTransparency( TRUE );
//				pPii->maskdc->SetChromaErr( 20, 20 );
				pPii->maskdc->SetTransparentColor( RGB( 255, 255, 255 ) );
				pPii->maskdc->Draw( *pPii->os, *pPii->os );
			} // end if
		} // end if

	} // end if

	return ret;
}

BOOL CPubImgList::Update(DWORD i, BOOL bAll )
{
	if ( i == MAXDWORD ) i = GetCurSel();

	// Get image object
	LPPUBIMGINFO ppii = GetImage( i );
	if ( ppii == NULL ) return FALSE;

	return Update( ppii, bAll );
}

BOOL CPubImgList::Update(LPCTSTR pImg, BOOL bAll)
{
	if ( pImg == NULL ) return FALSE;

	// Get image object
	LPPUBIMGINFO ppii = FindByName( pImg );
	if ( ppii == NULL ) return FALSE;

	return Update( ppii, bAll );
}

BOOL CPubImgList::Scale(DWORD i, LPRECT pRect )
{
	// Sanity check
	if ( pRect == NULL ) return FALSE;

	// Get node
	if ( i == MAXDWORD ) i = GetCurSel();
	LPPUBIMGINFO node = GetImage( i );
	if ( node == NULL ) return FALSE;

	// Is there an image?
	if ( node->os->GetSafeHdc() == NULL ) return FALSE;

	long width = pRect->right - pRect->left;
	long height = pRect->bottom - pRect->top;
	long w = node->os->GetWidth();
	long h = node->os->GetHeight();

	// No dividing by zero!
	if ( w == 0 || h == 0 ) return FALSE;

//	double r = double( node->os->GetHeight() ) / double( node->os->GetWidth() );

	// Scale axis
	if ( h > w ) pRect->right = pRect->left + ( ( height * w ) / h );
	else pRect->bottom = pRect->top + ( ( width * h ) / w );

	return TRUE;
}

BOOL CPubImgList::Save()
{
	// Anything to save?
	if ( Size() <= 0 ) return TRUE;

	char exe[ CWF_STRSIZE ];
	char path[ CWF_STRSIZE ];

	// Where are the overlays?
	if ( GetModuleFileName( NULL, exe, sizeof( exe ) ) )
		CWinFile::GetPathFromFileName( exe, exe );
	else GetCurrentDirectory( sizeof( exe ), exe );

	LPPUBIMGINFO ppii = NULL;

	// Search for this index
	while( ( ppii = (LPPUBIMGINFO)GetNext( ppii ) ) != NULL )
	{
		// Build file name
		wsprintf( path, "%s.llo", ppii->name );
		CWinFile::BuildPath( path, exe, path );

		// Add name and position
		ppii->cfg->SetValue( "Settings", "Name", ppii->name );
		ppii->cfg->SetValue( "Settings", "Rect", &ppii->rect, sizeof( ppii->rect ) );
		ppii->cfg->SetValue( "Settings", "BckColor", ppii->rgbbck );
		ppii->cfg->SetValue( "Settings", "Flags1", ppii->f1 );
		ppii->cfg->SetValue( "Settings", "CPos", ppii->cpos );
		ppii->cfg->SetValue( "Settings", "CNeg", ppii->cneg );
		ppii->cfg->SetValue( "Settings", "rgbKey", ppii->rgbkey );
		ppii->cfg->SetValue( "Settings", "KeyImg", ppii->keyimg );
		ppii->cfg->SetValue( "Settings", "KeyDriver", ppii->keydriver );

		ppii->cfg->SetValue( "Settings", "MaskFile", ppii->mask );
		ppii->cfg->SetValue( "Settings", "MaskColor", ppii->rgbmask );

		// Save the settings
		ppii->cfg->Save( path );

		// Save the overlay
		wsprintf( path, "%s.ovl", ppii->name );
		CWinFile::BuildPath( path, exe, path );
		ppii->fx->Save( path );

	} // end while

	return TRUE;
}

BOOL CPubImgList::Restore()
{
	// Lose current overlays
	Destroy();

	char exe[ CWF_STRSIZE ];
	char path[ CWF_STRSIZE ];
	char name[ CWF_STRSIZE ];

	// Where are the overlays?
	if ( GetModuleFileName( NULL, exe, sizeof( exe ) ) )
		CWinFile::GetPathFromFileName( exe, exe );
	else GetCurrentDirectory( sizeof( exe ), exe );
	
	WIN32_FIND_DATA		wfd;
	char				search[ CWF_STRSIZE ];
	HANDLE				hFind;

	// Get first file
	CWinFile::BuildPath( search, exe, "*.llo" );

	// Punt if empty
	hFind = FindFirstFile( search, &wfd );
	if ( hFind == INVALID_HANDLE_VALUE ) return FALSE;
	
	do
	{
		// Do we need to copy this file?
		if (	*wfd.cFileName != NULL &&
				strcmp( wfd.cFileName, "." ) &&
				strcmp( wfd.cFileName, ".." ) && 
				( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
		{
			// Build full path to file
			CWinFile::BuildPath( path, exe, wfd.cFileName );

			// Get the name
			CWinFile::GetFileNameFromPath( wfd.cFileName, name, TRUE );

			LPPUBIMGINFO ppii = FindByName( name );
			if ( ppii == NULL )
			{
				ppii = Add( name );
				if ( ppii != NULL && ppii->cfg->Load( path ) )
				{
					// Read in the position
					ppii->cfg->GetValue( "Settings", "Name", ppii->name, sizeof( ppii->name ) );
					ppii->cfg->GetValue( "Settings", "Rect", &ppii->rect, sizeof( ppii->rect ) );
					ppii->cfg->GetValue( "Settings", "BckColor", &ppii->rgbbck );
					ppii->cfg->GetValue( "Settings", "Flags1", &ppii->f1 );
					ppii->cfg->GetValue( "Settings", "CPos", &ppii->cpos );
					ppii->cfg->GetValue( "Settings", "CNeg", &ppii->cneg );
					ppii->cfg->GetValue( "Settings", "rgbKey", &ppii->rgbkey );
					ppii->cfg->GetValue( "Settings", "KeyImg", ppii->keyimg, sizeof( ppii->keyimg ) );
					ppii->cfg->GetValue( "Settings", "KeyDriver", &ppii->keydriver );

					ppii->cfg->GetValue( "Settings", "MaskFile", ppii->mask, sizeof( ppii->mask ) );
					ppii->cfg->GetValue( "Settings", "MaskColor", &ppii->rgbmask );

					// Restore the overlay
					CWinFile::BuildPath( path, exe, name );
					strcat( path, ".ovl" );

					// Load the overlay
					ppii->fx->Load( path );

					// Update the image
					Update( ppii );

					// Connect video
//					ConnectVideo( ppii );
					
				} // end if

			} // end if

		} // end if

	// Get the next file or directory
	} while ( FindNextFile( hFind, &wfd ) );
	
	// Close the find handle
	FindClose( hFind );

	return TRUE;
}

void CPubImgList::Delete(void *ptr, BOOL bDelete)
{
	LPPUBIMGINFO ppii = (LPPUBIMGINFO)ptr;
	if ( ppii == NULL ) return;

	char exe[ CWF_STRSIZE ];
	char path[ CWF_STRSIZE ];

	// Where are the overlays?
	if ( GetModuleFileName( NULL, exe, sizeof( exe ) ) )
		CWinFile::GetPathFromFileName( exe, exe );
	else GetCurrentDirectory( sizeof( exe ), exe );

	// Delete image settings
	wsprintf( path, "%s.llo", ppii->name );
	CWinFile::BuildPath( path, exe, path );
	CWinFile::Delete( path );

	// Delete overlay
	wsprintf( path, "%s.ovl", ppii->name );
	CWinFile::BuildPath( path, exe, path );
	CWinFile::Delete( path );

	// Do the actual delete
	CLList::Delete( ptr, bDelete );
}

BOOL CPubImgList::DeactivateUnusedVideo()
{
	LPVIDEOINFO		pvi = NULL;

	// For each driver
	while ( ( pvi = (LPVIDEOINFO)VIDEOLIST().GetNext( pvi ) ) != NULL )

		// Disable video update
		if ( pvi->cap != NULL && !pvi->motion.bEnable )
		{
			// Deactivate video
			if ( !IsDlg( pvi->index ) ) 
				pvi->active = 0, pvi->cap->SetFrameRate( 0 );

		} // end if

	return TRUE;
}
/*
BOOL CPubImgList::ActivateVideo(DWORD i)
{
	if ( i == MAXDWORD ) i = GetCurSel();
	LPPUBIMGINFO ppii = GetImage( i );
	if ( ppii == NULL ) return FALSE;

	return ActivateVideo( ppii );
}

/*
BOOL CPubImgList::ActivateVideo(LPPUBIMGINFO pPii)
{
	// Draw the background
	DWORD type = 0;
	pPii->cfg->GetValue( "Settings", "Type", &type );

	// Enable background video if needed
	if ( type == 1 ) 
	{
		DWORD driver = 0;
		LPVIDEOINFO pvi = NULL;
		if ( pPii->cfg->GetValue( "Settings", "Driver", &driver ) )
		{
			VIDEOLIST().Add( driver );
			VIDEOLIST().SetFrameRate( driver, DEFFRAMERATE );
		} // end if
	} // end if

	// Enable video in fx 
	LPFX pfx = NULL;	
	while ( ( pfx = pPii->fx->GetNext( pfx ) ) != NULL )
		if ( pfx->type == FXTYPE_VIDEO )
		{	VIDEOLIST().Add( pfx->driver );
			VIDEOLIST().SetFrameRate( pfx->driver, DEFFRAMERATE );
		} // end if

	return TRUE;
}
*/

void CPubImgList::ConnectVideo( LPPUBIMGINFO pPii )
{
	LPVIDEOINFO		pvi = NULL;

	// For each driver
	while ( ( pvi = (LPVIDEOINFO)VIDEOLIST().GetNext( pvi ) ) != NULL )
	{
		// Mark video as inactive
		if ( ( pvi->motion.bEnable ) != 0 || IsDlg( pvi->index ) ) pvi->active = 1;
		else pvi->active = 0;

	} // end while
	
	// Draw the background
	DWORD type = 0;
	pPii->cfg->GetValue( "Settings", "Type", &type );

	// Enable background video if needed
	if ( type == 1 ) 
	{
		DWORD driver = pPii->cfg->GetDword( "Settings", "Driver", 0 );

		// Get driver object
		LPVIDEOINFO pvi = VIDEOLIST().Add( driver );
		
		// Activate video
		if ( pvi != NULL ) pvi->active = 1;

	} // end if

	// Enable video in fx 
	LPFX pfx = NULL;	
	while ( ( pfx = pPii->fx->GetNext( pfx ) ) != NULL )
	{	if ( pfx->type == FXTYPE_VIDEO )
		{	pvi = VIDEOLIST().Add( pfx->driver );
			if ( pvi != NULL && pvi->active == 0 ) pvi->active = 1;
		} // end if
	} // end while

	// Activate Drivers
	VIDEOLIST().SetFrameRates( MAXDWORD );
}

BOOL CPubImgList::ConnectVideo(DWORD i)
{
	if ( i == MAXDWORD ) i = GetCurSel();
	LPPUBIMGINFO ppii = GetImage( i );
	if ( ppii == NULL ) return FALSE;

	ConnectVideo( ppii );

	return TRUE;
}

BOOL CPubImgList::GetImageName(DWORD i, LPSTR pName)
{
	LPPUBIMGINFO ppii = GetImage( i );
	if ( ppii == NULL ) return FALSE;

	strcpy( pName, ppii->name );

	return TRUE;
}

BOOL CPubImgList::GetImage(DWORD i, CWinImg *pImg)
{
	// Sanity check
	if ( pImg == NULL ) return FALSE;

	// Use selected image if needed
	if ( i == MAXDWORD ) i = IMGLIST().GetCurSel();

	// Get the image object
	LPPUBIMGINFO ppii = GetImage( i );
	if ( ppii == NULL ) return FALSE;
	if ( ppii->os == NULL || ppii->os->GetSafeHdc() == NULL )
		return FALSE;

	// Aquire lock
	CTlLocalLock ll( ppii->lock, 3000 );
	if ( !ll.IsLocked() ) return FALSE;

	// Create the image object
	pImg->CreateFromHBITMAP( *ppii->os );	

	// Return the image
	return pImg->IsValid();
}

BOOL CPubImgList::GetImage(LPCTSTR pImgName, CWinImg *pImg, long w, long h)
{
	// Sanity check
	if ( pImg == NULL ) return FALSE;

	// Get the image object
	LPPUBIMGINFO ppii = FindByName( pImgName );
	if ( ppii == NULL ) return FALSE;
	if ( ppii->os == NULL || ppii->os->GetSafeHdc() == NULL )
		return FALSE;

	// Lock the image
	CTlLocalLock ll( ppii->lock, 3000 );
	if ( !ll.IsLocked() ) return FALSE;
	
	// Do we need to stretch
	if ( w > 0 && h > 0 )
	{
		CWinDc os;

		// Stretch the image
		os.Create( NULL, 0, 0, w, h );
		os.Copy( *ppii->os, *ppii->os );

		pImg->CreateFromHBITMAP( os );	

	} // end if

	// Create the image object
	else pImg->CreateFromHBITMAP( *ppii->os );	

	// Return the image
	return pImg->IsValid();
}


BOOL CPubImgList::DrawChromaBck(CWinDc *pDC, LPPUBIMGINFO pPii)
{
	if ( pDC == NULL || pPii == NULL ) return FALSE;

	if ( ( pPii->f1 & PIIF1_KEYIMG ) != 0 )
	{
		// Allocate key offscreen memory class
		if ( pPii->keybck == NULL )
		{
			pPii->keybck = new CWinDc();
			if ( pPii->keybck == NULL ) return FALSE;
		} // end if

		// Load bck if not already
		if (	pPii->keybck->GetSafeHdc() == NULL || 
				!EqualRect( *pPii->keybck, *pDC ) ||
				strcmpi( pPii->keyimgloaded, pPii->keyimg ) )
		{
			CWinImg img;
			if ( !img.Load( pPii->keyimg ) ) return FALSE;
			if ( !pPii->keybck->Create( *pDC, *pDC ) ) return FALSE;
			img.Draw( *pPii->keybck, *pPii->keybck );
			strcpy( pPii->keyimgloaded, pPii->keyimg );

		} // end if

		return pPii->keybck->Draw( *pDC, *pDC );

	} // end if

	// Key to other image panel?
	else if ( ( pPii->f1 & PIIF1_KEYDYNAMIC ) != 0 )
	{
		LPPUBIMGINFO ppii = FindByName( pPii->keyimg );
		if ( ppii != NULL && ppii != pPii )
			return ppii->os->Draw( *pDC, *pDC ); 

	} // end else if

	// Keying to video?
	else if ( ( pPii->f1 & PIIF1_KEYVIDEO ) != 0 )
	{	LPVIDEOINFO pvi = VIDEOLIST().Add( pPii->keydriver );
		if ( pvi != NULL ) return pvi->os->Draw( *pDC, *pDC );
	} // end else if

	return FALSE;
}

BOOL CPubImgList::IsVideoInImage(DWORD driver, LPPUBIMGINFO ppii)
{
	if ( ppii == NULL ) return FALSE;
	
	// Is this the main driver?
	if ( 	ppii->cfg->GetDword( "Settings", "Type", MAXDWORD ) == 1 &&
			ppii->cfg->GetDword( "Settings", "Driver", 0 ) == driver )
		return TRUE;
	
	// Is it in any of the fx?
	LPFX pfx = NULL;	
	while ( ( pfx = ppii->fx->GetNext( pfx ) ) != NULL )
		if ( pfx->type == FXTYPE_VIDEO && pfx->driver == driver ) 
			return TRUE;
			
	return FALSE;
}


BOOL CPubImgList::IsDlg(DWORD driver)
{
	BOOL bIsDlg = FALSE;
	LPPUBIMGINFO ppii = NULL;
	while ( !bIsDlg && ( ppii = (LPPUBIMGINFO)GetNext( ppii ) ) != NULL )
		if (	::IsWindow( ppii->imgview->GetSafeHwnd() ) &&
				IsVideoInImage( driver, ppii ) ) bIsDlg = TRUE;
	return bIsDlg;
}
