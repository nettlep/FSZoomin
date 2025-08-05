// ---------------------------------------------------------------------------------------------------------------------------------
//  ______  _____ ______                      _           _     
// |  ____|/ ____|___  /                     (_)         | |    
// | |__  | (___    / /  ___   ___  _ __ ___  _ _ __     | |__  
// |  __|  \___ \  / /  / _ \ / _ \| '_ ` _ \| | '_ \    | '_ \ 
// | |     ____) |/ /__| (_) | (_) | | | | | | | | | | _ | | | |
// |_|    |_____//_____|\___/ \___/|_| |_| |_|_|_| |_|(_)|_| |_|
//                                                              
//                                                              
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

#ifndef	_H_FSZOOMIN
#define _H_FSZOOMIN

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "resource.h"

// ---------------------------------------------------------------------------------------------------------------------------------

class	FSZoomin : public CWinApp
{
public:
				FSZoomin();

public:
virtual		BOOL		InitInstance();
				DECLARE_MESSAGE_MAP()
};

extern FSZoomin theApp;

#endif // _H_FSZOOMIN
// ---------------------------------------------------------------------------------------------------------------------------------
// FSZoomin.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
