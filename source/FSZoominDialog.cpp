// ---------------------------------------------------------------------------------------------------------------------------------
//  ______  _____ ______                      _       _____  _       _                                  
// |  ____|/ ____|___  /                     (_)     |  __ \(_)     | |                                 
// | |__  | (___    / /  ___   ___  _ __ ___  _ _ __ | |  | |_  __ _| | ___   __ _      ___ _ __  _ __  
// |  __|  \___ \  / /  / _ \ / _ \| '_ ` _ \| | '_ \| |  | | |/ _` | |/ _ \ / _` |    / __| '_ \| '_ \ 
// | |     ____) |/ /__| (_) | (_) | | | | | | | | | | |__| | | (_| | | (_) | (_| | _ | (__| |_) | |_) |
// |_|    |_____//_____|\___/ \___/|_| |_| |_|_|_| |_|_____/|_|\__,_|_|\___/ \__, |(_) \___| .__/| .__/ 
//                                                                            __/ |        | |   | |    
//                                                                           |___/         |_|   |_|    
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

#include "stdafx.h"
#include "FSZoomin.h"
#include "FSZoominDialog.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(FSZoominDialog, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_CBN_SELCHANGE(IDC_FREQ_COMBO, OnCbnSelchangeFreqCombo)
	ON_CBN_SELCHANGE(IDC_ZOOM_COMBO, OnCbnSelchangeZoomCombo)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_GETMINMAXINFO()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// ---------------------------------------------------------------------------------------------------------------------------------

static const	int	UpdateTimer = 12345;

// ---------------------------------------------------------------------------------------------------------------------------------

	FSZoominDialog::FSZoominDialog(CWnd* pParent /*=NULL*/)
	: CDialog(FSZoominDialog::IDD, pParent), _dib(NULL), _captured(false), _desktop(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAME, frame);
	DDX_Control(pDX, IDC_ZOOM_COMBO, zoomCombo);
	DDX_Control(pDX, IDC_FREQ_COMBO, frequencyCombo);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnDestroy()
{
	CDialog::OnDestroy();

	cleanup();
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 326;
	lpMMI->ptMinTrackSize.y = 286;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// ---------------------------------------------------------------------------------------------------------------------------------

BOOL	FSZoominDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	frequencyCombo.AddString("No update");
	frequencyCombo.AddString("1000ms");
	frequencyCombo.AddString("500ms");
	frequencyCombo.AddString("250ms");
	frequencyCombo.AddString("100ms");
	frequencyCombo.AddString("50ms");
	frequencyCombo.AddString("10ms");
	frequencyCombo.SetCurSel(0);

	zoomCombo.AddString("1x");
	zoomCombo.AddString("2x");
	zoomCombo.AddString("4x");
	zoomCombo.AddString("8x");
	zoomCombo.AddString("16x");
	zoomCombo.AddString("32x");
	zoomCombo.AddString("64x");
	zoomCombo.AddString("128x");
	zoomCombo.AddString("256x");
	zoomCombo.SetCurSel(1);

	CRect	wr;
	GetWindowRect(wr);
	lastPoint() = wr.TopLeft();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		if (!dib()) setSize();
		dib()->stretchToDisplay();

		CDialog::OnPaint();
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (frame.m_hWnd)
	{
		CRect	r;
		GetClientRect(r);
		r.top += 44;
		r.left += 12;
		r.right -= 12;
		r.bottom -= 12;

		frame.MoveWindow(r);
		setSize();
		zoom(lastPoint());
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

HCURSOR	FSZoominDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	captured() = true;
	SetCapture();
	CDialog::OnLButtonDown(nFlags, point);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	CDialog::OnLButtonUp(nFlags, point);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	if (captured())
	{
		lastPoint() = point;
		zoom(lastPoint());
	}

	CDialog::OnMouseMove(nFlags, point);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnCaptureChanged(CWnd *pWnd)
{
	captured() = false;
	CDialog::OnCaptureChanged(pWnd);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnCbnSelchangeFreqCombo()
{
	KillTimer(UpdateTimer);

	switch (frequencyCombo.GetCurSel())
	{
		case 1: SetTimer(UpdateTimer, 1000, NULL); break;
		case 2: SetTimer(UpdateTimer, 500, NULL); break;
		case 3: SetTimer(UpdateTimer, 250, NULL); break;
		case 4: SetTimer(UpdateTimer, 100, NULL); break;
		case 5: SetTimer(UpdateTimer, 50, NULL); break;
		case 6: SetTimer(UpdateTimer, 10, NULL); break;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == UpdateTimer)
	{
		zoom(lastPoint());
	}

	CDialog::OnTimer(nIDEvent);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::OnCbnSelchangeZoomCombo()
{
	setSize();
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::setSize()
{
	cleanup();

	if (!frame.m_hWnd) return;

	// Allocate a new dib and stuff

	dib() = new winDIB(*frame.GetDC());

	CRect	r;
	frame.GetClientRect(r);

	// Set the dest rect

	dib()->dstRect() = r;

	// Allocate the desktop RAM

	desktop() = new unsigned int[r.Width() * r.Height()];

	// Resize the source rect

	int	nw = r.Width() >> zoomCombo.GetCurSel(); if (!nw) nw = 1;
	int	nh = r.Height() >> zoomCombo.GetCurSel();	if (!nh) nh = 1;
	r.right = r.left + nw;
	r.bottom = r.top + nh;
	dib()->srcRect() = r;

	// Set the frame buffer

	dib()->frameBuffer((unsigned char *) desktop(), NULL, r.Width(), r.Height(), 32);

	// Update the display

	zoom(lastPoint());
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::cleanup()
{
	if (desktop())
	{
		delete[] desktop();
		desktop() = NULL;
	}

	if (dib())
	{
		frame.ReleaseDC(&dib()->dc());
		delete dib();
		dib() = NULL;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	FSZoominDialog::zoom(const CPoint & p)
{
	// Convert the point to scree-coordinates

	CPoint	point = p;
	MapWindowPoints(NULL, &point, 1);

	// Source size

	int	width = dib()->srcRect().Width();
	int	height = dib()->srcRect().Height();

	// Offset the point so that it's centered in the source rect

	point.x -= width/2;
	point.y -= height/2;

	// Our window rect

	CRect	r;
	frame.GetClientRect(r);
	r.left = point.x;
	r.top = point.y;
	r.right = r.left + width;
	r.bottom = r.top + height;

	// Our screen DC

	CDC	srcDC;
	srcDC.CreateDC("DISPLAY", NULL, NULL, NULL);

	CDC	memDC;
	memDC.CreateCompatibleDC(&srcDC);

	CBitmap	compatBitmap;
	compatBitmap.CreateCompatibleBitmap(&srcDC, width, height);

	CBitmap	*oldBitmap = (CBitmap *) memDC.SelectObject(compatBitmap);

	memDC.BitBlt(0, 0, width, height, &srcDC, r.left, r.top, SRCCOPY);

	// For some reason, oldBitmap is always garbage (but NOT NULL)... so this causes a crash.
	//   These kinds of Windows problems can really get under your skin. Oh well... I just
	//   commented it out.
	//
	//memDC.SelectObject(oldBitmap);

	BITMAPINFO	bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = (LONG) -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = dib()->depth();
	bmi.bmiHeader.biCompression = BI_RGB;

	GetDIBits(srcDC.GetSafeHdc(), (HBITMAP) compatBitmap.GetSafeHandle(), 0, height, desktop(), &bmi, DIB_RGB_COLORS);

	memDC.DeleteDC();
	srcDC.DeleteDC();
	compatBitmap.DeleteObject();

	dib()->stretchToDisplay();
}

// ---------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// ---------------------------------------------------------------------------------------------------------------------------------

void	CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

// ---------------------------------------------------------------------------------------------------------------------------------
// FSZoominDialog.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
