// ---------------------------------------------------------------------------------------------------------------------------------
//  ______  _____ ______                      _       _____  _       _                 _     
// |  ____|/ ____|___  /                     (_)     |  __ \(_)     | |               | |    
// | |__  | (___    / /  ___   ___  _ __ ___  _ _ __ | |  | |_  __ _| | ___   __ _    | |__  
// |  __|  \___ \  / /  / _ \ / _ \| '_ ` _ \| | '_ \| |  | | |/ _` | |/ _ \ / _` |   | '_ \ 
// | |     ____) |/ /__| (_) | (_) | | | | | | | | | | |__| | | (_| | | (_) | (_| | _ | | | |
// |_|    |_____//_____|\___/ \___/|_| |_| |_|_|_| |_|_____/|_|\__,_|_|\___/ \__, |(_)|_| |_|
//                                                                            __/ |          
//                                                                           |___/           
//
// Description:
//
//   Handles the zoom utility's actual functionality
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

#ifndef	_H_FSZOOMINDIALOG
#define _H_FSZOOMINDIALOG

// ---------------------------------------------------------------------------------------------------------------------------------
// Module setup (required includes, macros, etc.)
// ---------------------------------------------------------------------------------------------------------------------------------

#include "winDIB.h"
#include "afxwin.h"

// ---------------------------------------------------------------------------------------------------------------------------------

class	FSZoominDialog : public CDialog
{
public:
		enum		{ IDD = IDD_FSZOOMIN_DIALOG };
				FSZoominDialog(CWnd* pParent = NULL);

	// Implementaion

virtual		BOOL		OnInitDialog();
afx_msg 	void		OnSysCommand(UINT nID, LPARAM lParam);
afx_msg 	void		OnPaint();
afx_msg 	HCURSOR		OnQueryDragIcon();
afx_msg 	void		OnLButtonDown(UINT nFlags, CPoint point);
afx_msg 	void 		OnMouseMove(UINT nFlags, CPoint point);
afx_msg 	void 		OnLButtonUp(UINT nFlags, CPoint point);
afx_msg 	void 		OnCaptureChanged(CWnd *pWnd);
afx_msg 	void 		OnCbnSelchangeFreqCombo();
afx_msg 	void 		OnCbnSelchangeZoomCombo();
afx_msg 	void 		OnSize(UINT nType, int cx, int cy);
afx_msg 	void 		OnDestroy();
afx_msg 	void 		OnGetMinMaxInfo(MINMAXINFO* lpMMI);
afx_msg 	void 		OnTimer(UINT nIDEvent);
virtual		void		setSize();
virtual		void		cleanup();
virtual		void		zoom(const CPoint & p);

	// Accessors

inline		winDIB *&	dib()			{return _dib;}
inline	const	winDIB *	dib() const		{return _dib;}
inline		bool &		captured()		{return _captured;}
inline	const	bool		captured() const	{return _captured;}
inline		unsigned int *&	desktop()		{return _desktop;}
inline	const	unsigned int *	desktop() const		{return _desktop;}
inline		CPoint &	lastPoint()		{return _lastPoint;}
inline	const	CPoint &	lastPoint() const	{return _lastPoint;}

private:
		winDIB *	_dib;
		bool		_captured;
		unsigned int *	_desktop;
		CPoint		_lastPoint;

protected:
		HICON		m_hIcon;
		CStatic		frame;
		CComboBox	zoomCombo;
		CComboBox	frequencyCombo;

				DECLARE_MESSAGE_MAP()
virtual		void		DoDataExchange(CDataExchange* pDX);
};

// ---------------------------------------------------------------------------------------------------------------------------------

class	CAboutDlg : public CDialog
{
public:
		enum		{ IDD = IDD_ABOUTBOX };

				CAboutDlg() : CDialog(CAboutDlg::IDD) {}

protected:
virtual		void		DoDataExchange(CDataExchange* pDX);
				DECLARE_MESSAGE_MAP()
public:
};

#endif // _H_FSZOOMINDIALOG
// ---------------------------------------------------------------------------------------------------------------------------------
// FSZoominDialog.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
