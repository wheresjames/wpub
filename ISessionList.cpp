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
// ISessionList.cpp: implementation of the CISessionList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "ISessionList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CISessionList::CISessionList()
{
	m_fCreate = NULL;
	m_fDelete = NULL;
}

CISessionList::~CISessionList()
{

}

void CISessionList::DeleteObject(void *node)
{
	LPISESSIONINFO	pisi = (LPISESSIONINFO)node;
	if ( pisi == NULL ) return;

	// Delete winsock object
	if ( pisi->ws != NULL )
	{	if ( m_fDelete != NULL ) m_fDelete( pisi->ws );
		pisi->ws = NULL;
	} // end if

	// Complete the delete
	CLList::DeleteObject( node );
}

CAsyncWinSock* CISessionList::Add()
{
	// Can we create objects?
	if ( m_fCreate == NULL ) return NULL;

	// Allocate memory
	LPISESSIONINFO node = (LPISESSIONINFO)New();
	if ( node == NULL ) return NULL;

	// Create winsock 
	node->ws = m_fCreate();
	if ( node->ws == NULL ) { DeleteObject( node ); return NULL; }

	return node->ws;
}

void CISessionList::Cleanup()
{

	// Remove disconnected nodes
	LPISESSIONINFO	pisi = NULL;
	while ( ( pisi = (LPISESSIONINFO)GetNext( pisi ) ) != NULL )
		
		// Is this node connected?
		if ( pisi->ws != NULL && !pisi->ws->IsConnected() )
		{	
			// Point to next node
			LPISESSIONINFO del = pisi;
			pisi = (LPISESSIONINFO)GetNext( pisi );

			// Delete this node
			Delete( del );

		} // end if
}


/*
class session
{
	virtual void ProcessData() = 0;

	// ...
};

class mysession : public session
{
public:
	virtual void ProcessData() {}
};

class sessionlist
{
public:
	session* NewSession()
	{
		return AddToList( new mysession() );
	}

	void RemoveSession( mysession *s )
	{
		RemoveFromList( s );
		delete s;
	} 

	session* AddToList( session* s ) { return s; }
	void RemoveFromList( session* s ){}

	// ...
};

class server
{
public:
	sessionlist  sl;

	void OnAccept()
	{
		Accept( sl.NewSession() );
	}

	void OnDissconnect( session *s )
	{

	}

	void Accept( session* s ) {}
	// ...
};
*/

/*
class session
{
public:

	virtual ~session();

	virtual void ProcessData() = 0;

	virtual session* clone() { return NULL; }

	static session* create( session* ptr ) { return ptr->clone(); }
};

class mysession : public session
{
public:

	virtual void ProcessData() {}

	virtual session* clone() { return new mysession(); }
};

void aloof( session *tmpl )
{
	session *ptr = session::create( tmpl );

	// ...

	delete ptr;
}


void f()
{
	aloof( &mysession() );
}
*/