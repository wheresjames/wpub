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

// Common defines

#define APPNAME				"WheresJames Webcam Publisher"
#define VERSTR				"2.0 Beta 0015"
#define VERHI				1
#define VERLO				99015
#define REV					0
#define VER					( double( 1.99015 ) )

#define HKEY_SETTINGS		HKEY_CURRENT_USER
#define SZKEY_SETTINGS		"Software\\www_wheresjames_com\\Webcam Publisher Pro"
#define MFCKEY_SETTINGS		"www_wheresjames_com\\Webcam Publisher Pro"

#define PRODUCTID			2
#define UPDATE_LINK			"http://www.wheresjames.com/productinfo/wpub2.inf"

#define DEFJPEGQUALITY		52

#define DEFFRAMERATE		30

#define APP()					( (CVpApp*)AfxGetApp() )
#define FRAME()					( (CMainFrame*)APP()->m_pMainWnd )
#define VIEW()					( (CVpView*)FRAME()->GetActiveView() )
#define EDIT()					( VIEW()->m_winedit )
#define CFG()					( APP()->m_cfg )
#define IMGLIST()				( FRAME()->m_pubimglist )
#define VIDEOLIST()				( FRAME()->m_videolist )
#define SERVERLOG()				( APP()->m_serverlog )
#define WEBSETTINGS()			( APP()->m_websettings )
#define VIDEOSETTINGS()			( APP()->m_videosettings )
#define SCREENSHOT()			( APP()->m_screenshot )
#define WEBUSERS()				( APP()->m_webusers )
#define FTPSERVERS()			( APP()->m_ftpservers )
#define EMAILSERVERS()			( APP()->m_emailservers )
#define CGI()					( APP()->m_cgi )
#define PUBLIST()				( APP()->m_publist )
#define HTMLVARS()				( APP()->m_htmlvars )
#define PLAYSOUND( snd )		( APP()->m_sndthread.Play( snd ) )
#define CHAT()					( APP()->m_chat )
