// ---------------------------------------------------------------------------------------------------------------------------------
//  ______  _____ ______                      _                            
// |  ____|/ ____|___  /                     (_)                           
// | |__  | (___    / /  ___   ___  _ __ ___  _ _ __       ___ _ __  _ __  
// |  __|  \___ \  / /  / _ \ / _ \| '_ ` _ \| | '_ \     / __| '_ \| '_ \ 
// | |     ____) |/ /__| (_) | (_) | | | | | | | | | | _ | (__| |_) | |_) |
// |_|    |_____//_____|\___/ \___/|_| |_| |_|_|_| |_|(_) \___| .__/| .__/ 
//                                                            | |   | |    
//                                                            |_|   |_|    
//
// Description:
//
//   Simple app for zooming in on the desktop - main application
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   12/04/2001 by Paul Nettle: Original creation
//
// Originally released under a custom license.
// This historical re-release is provided under the MIT License.
// See the LICENSE file in the repo root for details.
//
// https://github.com/nettlep
//
// Copyright 2002, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "FSZoomin.h"
#include "FSZoominDialog.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(FSZoomin, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// ---------------------------------------------------------------------------------------------------------------------------------

FSZoomin theApp;

// ---------------------------------------------------------------------------------------------------------------------------------

	FSZoomin::FSZoomin()
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

BOOL	FSZoomin::InitInstance()
{
	CWinApp::InitInstance();

	// "This is who we are" (Millenium fans go nutz here :)

	SetRegistryKey("Fluid Studios");
	free(const_cast<void *>(static_cast<const void *>(theApp.m_pszProfileName)));
	theApp.m_pszProfileName = _tcsdup(_T("Fluid Studios desktop zoom utility"));

	// Run the dialog

	FSZoominDialog dlg;
	dlg.DoModal();

	return FALSE;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// FSZoomin.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
