// ---------------------------------------------------------------------------------------------------------------------------------
//  _   _   _ _       _____  _____ ____                       
// | | | | | (_)     |  __ \|_   _|  _ \                      
// | | | | | |_ _ __ | |  | | | | | |_) |     ___ _ __  _ __  
// | | | | | | | '_ \| |  | | | | |  _ <     / __| '_ \| '_ \ 
// |  V _ V  | | | | | |__| |_| |_| |_) | _ | (__| |_) | |_) |
//  \__/ \__/|_|_| |_|_____/|_____|____/ (_) \___| .__/| .__/ 
//                                               | |   | |    
//                                               |_|   |_|    
//
// Generic windows DIBsection class for quick and painless output to a windows window
//
// ---------------------------------------------------------------------------------------------------------------------------------
//
// Originally released under a custom license.
// This historical re-release is provided under the MIT License.
// See the LICENSE file in the repo root for details.
//
// https://github.com/nettlep
//
// ---------------------------------------------------------------------------------------------------------------------------------
// Originally created on 12/06/2000 by Paul Nettle
//
// Copyright 2000, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h" // If this line gives you an error, comment it out
#include <math.h>
#include "winDIB.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// --------------------------------------------------------------------------------------------------------------------------------

static	unsigned char	blendTable8[256][256];
static	bool		blendTableInitialized;

// --------------------------------------------------------------------------------------------------------------------------------

static	inline		unsigned int	scaleAlpha(const unsigned int color, const int scale)
{
	// alpha * scale, where scale is a scalar (0...255) -- only color's alpha is modified
	unsigned int	a = ((color >> 8) & 0x00ff0000) * scale;
	return	(a & 0xff000000) | (color & 0x00ffffff);
}

// --------------------------------------------------------------------------------------------------------------------------------

static	inline		unsigned int	alphaBlend(const unsigned int dst, const unsigned int src)
{
	// src * alpha + dst * (1-alpha), where alpha is src's alpha, resulting alpha is dst alpha
	unsigned int	a = src >> 24;
	unsigned int	rb = (((src & 0x00ff00ff) * a) + ((dst & 0x00ff00ff) * (0xff - a))) & 0xff00ff00;
	unsigned int	g  = (((src & 0x0000ff00) * a) + ((dst & 0x0000ff00) * (0xff - a))) & 0x00ff0000;
	return (src & 0xff000000) | ((rb | g) >> 8);
}

// ---------------------------------------------------------------------------------------------------------------------------------

		winDIB::winDIB(CDC &dc)
		:_dc(dc), _frameBuffer(NULL), _zBuffer(NULL)
{
	_bmi[0].bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	_bmi[0].bmiHeader.biPlanes = 1;
	_bmi[0].bmiHeader.biBitCount = 0;
	_bmi[0].bmiHeader.biXPelsPerMeter = 72;
	_bmi[0].bmiHeader.biYPelsPerMeter = 72;
	_bmi[0].bmiHeader.biClrUsed = 0;
	_bmi[0].bmiHeader.biClrImportant = 0;

	// Setup the blend table (only once)

	if (!blendTableInitialized)
	{
		for (unsigned int i = 0; i < 256; i++)
		{
			for (unsigned int j = 0; j < 256; j++)
			{
				blendTable8[i][j] = static_cast<unsigned char>((i*j) >> 8);
			}
		}

		blendTableInitialized = true;
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void		winDIB::depth(const WORD bitdepth)
{
	unsigned int	bytesPerPixel;
	DWORD		*colors = reinterpret_cast<DWORD *>(_bmi[0].bmiColors);

	switch(bitdepth)
	{
		case 8:
			bytesPerPixel = 1;
			break;

		case 15:
			_bmi[0].bmiHeader.biCompression = BI_BITFIELDS;
			_bmi[0].bmiHeader.biBitCount = 16;
			colors[0] = 0x7C00;
			colors[1] = 0x03E0;
			colors[2] = 0x001F;
			bytesPerPixel = 2;
			break;

		case 16:
			_bmi[0].bmiHeader.biCompression = BI_BITFIELDS;
			_bmi[0].bmiHeader.biBitCount = 16;
			colors[0] = 0xF800;
			colors[1] = 0x07E0;
			colors[2] = 0x001F;
			bytesPerPixel = 2;
			break;

		case 24:
			_bmi[0].bmiHeader.biCompression = BI_RGB;
			_bmi[0].bmiHeader.biBitCount = 24;
			bytesPerPixel = 3;
			break;

		case 32:
			_bmi[0].bmiHeader.biCompression = BI_RGB;
			_bmi[0].bmiHeader.biBitCount = 32;
			bytesPerPixel = 4;
			break;

		default:
			bytesPerPixel = 0;
			_bmi[0].bmiHeader.biBitCount = 0;
			break;
	}

	_bmi[0].bmiHeader.biSizeImage = width() * height() * bytesPerPixel;
}

// ---------------------------------------------------------------------------------------------------------------------------------

unsigned char	*winDIB::frameBuffer(unsigned char *fb, float *zb, const LONG width, const LONG height, const WORD bitdepth)
{
	_bmi[0].bmiHeader.biWidth = width;
	_bmi[0].bmiHeader.biHeight = -height;
	_frameBuffer = fb;
	_zBuffer = zb;
	if (bitdepth)	depth(bitdepth);
	return _frameBuffer;
}

// ---------------------------------------------------------------------------------------------------------------------------------

unsigned int	winDIB::stretchToDisplay()
{
	int	srcX = _srcRect.left;
	int	srcY = _srcRect.top;
	int	srcW = _srcRect.Width();
	int	srcH = _srcRect.Height();
	int	dstX = _dstRect.left;
	int	dstY = _dstRect.top;
	int	dstW = _dstRect.Width();
	int	dstH = _dstRect.Height();
	return StretchDIBits(dc().GetSafeHdc(), dstX, dstY, dstW, dstH, srcX, srcY, srcW, srcH, _frameBuffer, _bmi, DIB_RGB_COLORS, SRCCOPY);
}

// ---------------------------------------------------------------------------------------------------------------------------------

unsigned int	winDIB::copyToDisplay()
{
	int	srcX = _srcRect.left;
	int	srcY = _srcRect.top;
	int	dstX = _dstRect.left;
	int	dstY = _dstRect.top;
	int	dstW = _dstRect.Width();
	int	dstH = _dstRect.Height();
	return SetDIBitsToDevice(dc().GetSafeHdc(), dstX, dstY, dstW, dstH, srcX, srcY, 0, dstH, _frameBuffer, _bmi, DIB_RGB_COLORS);
}

// ---------------------------------------------------------------------------------------------------------------------------------

const	bool	winDIB::clipLine(float &x0, float &y0, float &x1, float &y1, const bool clipForAntialiasing) const
{
	// Our clipping rect -- note that the right/bottom of the rect describe the column/row beyond the screen, we do this because
	// we clip to the edge of those pixels.

	float	lClip = static_cast<float>(srcRect().left);
	float	rClip = static_cast<float>(srcRect().right) - (clipForAntialiasing ? 0.001f:1);
	float	tClip = static_cast<float>(srcRect().top);
	float	bClip = static_cast<float>(srcRect().bottom) - (clipForAntialiasing ? 0.001f:1);

	// Gurarntee top->bottom

	if (y0 > y1)
	{
		float	temp;
		temp = x0; x0 = x1; x1 = temp;
		temp = y0; y0 = y1; y1 = temp;
	}
	
	// Off-screen?

	if (y1 < tClip || y0 >= bClip) return false;

	// Top-clip

	if (y0 < tClip)
	{
		float	clipDelta = (tClip - y0) / (y1 - y0);
		x0 += (x1 - x0) * clipDelta;
		y0 = tClip;
	}

	// Bottom-clip

	if (y1 > bClip)
	{
		float	clipDelta = (y1 - bClip) / (y1 - y0);
		x1 -= (x1 - x0) * clipDelta;
		y1 = bClip;
	}

	// Gurarntee left->right

	if (x0 > x1)
	{
		float	temp;
		temp = x0; x0 = x1; x1 = temp;
		temp = y0; y0 = y1; y1 = temp;
	}
	
	// Off-screen?

	if (x1 < lClip || x0 >= rClip) return false;

	// Left-clip

	if (x0 < lClip)
	{
		float	clipDelta = (lClip - x0) / (x1 - x0);
		x0 = lClip;
		y0 += (y1 - y0) * clipDelta;
	}

	// Right-clip

	if (x1 > rClip)
	{
		float	clipDelta = (x1 - rClip) / (x1 - x0);
		x1 = rClip;
		y1 -= (y1 - y0) * clipDelta;
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void		winDIB::line(const int x0, const int y0, const int x1, const int y1, const unsigned int color)
{
	// For fixed-point stuff

	enum		{F_BITS = 16};
	enum		{F_RANGE = (1 << F_BITS) - 1};
	enum		{F_HALF = 1 << (F_BITS - 1)};
	enum		{F_ONE  = 1 << F_BITS};

	// Make sure we have a frame buffer

	if (!frameBuffer()) return;

	// Clip the line

	float	cx0 = static_cast<float>(x0);
	float	cx1 = static_cast<float>(x1);
	float	cy0 = static_cast<float>(y0);
	float	cy1 = static_cast<float>(y1);
	if (!clipLine(cx0, cy0, cx1, cy1)) return;

	// Fixed-point precision

	int	ix0 = static_cast<int>(floor(cx0));
	int	ix1 = static_cast<int>(floor(cx1));
	int	iy0 = static_cast<int>(floor(cy0));
	int	iy1 = static_cast<int>(floor(cy1));

	// Enforce top->bottom orientation

	if (iy0 > iy1)
	{
		int	temp;
		temp = ix0; ix0 = ix1; ix1 = temp;
		temp = iy0; iy0 = iy1; iy1 = temp;
	}

	// |deltas|

	int	absDX = ix1>ix0 ? ix1-ix0:ix0-ix1;
	int	absDY = iy1>iy0 ? iy1-iy0:iy0-iy1;

	// The line starts here

	int	pitch = width();

	unsigned int	*fb = reinterpret_cast<unsigned int *>(frameBuffer()) + iy0 * pitch + ix0;

	// Floating point precision

	int	fx0 = ix0 << F_BITS;
	int	fx1 = ix1 << F_BITS;
	int	fy0 = iy0 << F_BITS;
	int	fy1 = iy1 << F_BITS;

	// Primarily horizontal

	if (absDX > absDY)
	{
		// Step in Y

		int	dy = 0;
		if (absDX) dy = (absDY << F_BITS) / absDX;
		fy0 = F_HALF;

		// X direction

		if (ix0 < ix1)
		{
			while(ix0 <= ix1)
			{
				// Step (with error correction)

				if (fy0 > F_ONE)
				{
					fy0 -= F_ONE;
					fb += pitch;
				}

				// Draw the pixel

				*fb = color;
				fb++;
				fy0 += dy;
				ix0++;
			}
		}
		else
		{
			while(ix1 <= ix0)
			{
				// Step (with error correction)

				if (fy0 > F_ONE)
				{
					fy0 -= F_ONE;
					fb += pitch;
				}

				// Draw the pixel

				*fb = color;
				fb--;
				fy0 += dy;
				ix1++;
			}
		}
	}

	// Primarily vertical

	else
	{
		// Step in X

		int	dx = 0;
		if (absDY) dx = (absDX << F_BITS) / absDY;
		fx0 = F_HALF;

		// X direction

		if (ix0 < ix1)
		{
			while(iy0 <= iy1)
			{
				// Step (with error correction)

				if (fx0 > F_ONE)
				{
					fx0 -= F_ONE;
					fb++;
				}

				// Draw the pixel

				*fb = color;
				fb += pitch;
				fx0 += dx;
				iy0++;
			}
		}
		else
		{
			while(iy0 <= iy1)
			{
				// Step (with error correction)

				if (fx0 > F_ONE)
				{
					fx0 -= F_ONE;
					fb--;
				}

				// Draw the pixel

				*fb = color;
				fb += pitch;
				fx0 += dx;
				iy0++;
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// Modified Wu anti-aliased line drawing.  Performance oriented, uses Wu combined with Bresenham, with modifications to allow the
// line to start on non-exact pixel boundaries.
// ---------------------------------------------------------------------------------------------------------------------------------

void		winDIB::aaLine(const float x0, const float y0, const float x1, const float y1, const unsigned int color)
{
	// For fixed-point stuff

	enum		{F_BITS  = 8};
	enum		{F_ONE   = 1 << F_BITS};
	enum		{F_RANGE = F_ONE - 1};
	enum		{F_HALF  = F_ONE >> 1};

	// Make sure we have a frame buffer

	if (!frameBuffer()) return;

	// Clip the line

	float	cx0 = x0;
	float	cx1 = x1;
	float	cy0 = y0;
	float	cy1 = y1;
	if (!clipLine(cx0, cy0, cx1, cy1, true)) return;

	// Fixed-point precision

	int	fx0 = static_cast<int>(floor(cx0 * static_cast<float>(F_ONE)));
	int	fx1 = static_cast<int>(floor(cx1 * static_cast<float>(F_ONE)));
	int	fy0 = static_cast<int>(floor(cy0 * static_cast<float>(F_ONE)));
	int	fy1 = static_cast<int>(floor(cy1 * static_cast<float>(F_ONE)));

	// Integer precision

	int	ix0 = fx0 >> F_BITS;
	int	ix1 = fx1 >> F_BITS;
	int	iy0 = fy0 >> F_BITS;
	int	iy1 = fy1 >> F_BITS;

	// |deltas|

	int	absDX = ix1>ix0 ? ix1-ix0:ix0-ix1;
	int	absDY = iy1>iy0 ? iy1-iy0:iy0-iy1;

	// Pitch

	unsigned int	pitch = width();

	// Primarily horizontal

	if (absDX > absDY)
	{
		// Enforce right->left orientation

		if (fx0 > fx1)
		{
			int	temp;
			temp = fx0; fx0 = fx1; fx1 = temp;
			temp = fy0; fy0 = fy1; fy1 = temp;
			temp = ix0; ix0 = ix1; ix1 = temp;
			temp = iy0; iy0 = iy1; iy1 = temp;
		}

		unsigned int	*fb = reinterpret_cast<unsigned int *>(frameBuffer()) + iy0 * pitch + ix0;

		// Fractional values (0...1)

		int	fracX = fx0 & F_RANGE;
		int	fracY = fy0 & F_RANGE;

		// Delta Y

		int	dy = 0;
		if (fx1-fx0) dy = ((fy1-fy0)<<F_BITS) / (fx1-fx0);

		// First pixel requires special care

		int	distXHalf = fracX > F_HALF ? fracX-F_HALF:F_HALF-fracX;
		int	distYHalf = fracY > F_HALF ? fracY-F_HALF:F_HALF-fracY;

		if (fracY < F_HALF)
		{
			if (fracX < F_HALF)
			{
				if (ix0 > 0)
				{
					if (iy0 > 0)	*(fb-pitch-1) = alphaBlend(*(fb-pitch-1), scaleAlpha(color, blendTable8[distXHalf][        distYHalf]));
							*(fb-1)       = alphaBlend(*(fb-1),       scaleAlpha(color, blendTable8[distXHalf][F_RANGE-distYHalf]));
				}

				fracY += (dy * (F_HALF-fracX)) >> F_BITS;
			}
			else
			{
					if (iy0 > 0)	*(fb-pitch) = alphaBlend(*(fb-pitch), scaleAlpha(color, blendTable8[F_RANGE-distXHalf][        distYHalf]));
							*fb         = alphaBlend(*fb,         scaleAlpha(color, blendTable8[F_RANGE-distXHalf][F_RANGE-distYHalf]));

				fracY += (dy * (F_HALF+(F_ONE-fracX))) >> F_BITS;
				fb++;
				fx0 += F_ONE;
			}
		}
		else
		{
			if (fracX < F_HALF)
			{
				if (ix0 > 0)
				{
									*(fb-1)       = alphaBlend(*(fb-1),       scaleAlpha(color, blendTable8[distXHalf][F_RANGE-distYHalf]));
					if (iy0 < (int) height()-1)	*(fb+pitch-1) = alphaBlend(*(fb+pitch-1), scaleAlpha(color, blendTable8[distXHalf][        distYHalf]));
				}

				fracY += (dy * (F_HALF-fracX)) >> F_BITS;
			}
			else
			{
									*fb         = alphaBlend(*fb,         scaleAlpha(color, blendTable8[F_RANGE-distXHalf][F_RANGE-distYHalf]));
					if (iy0 < (int) height()-1)	*(fb+pitch) = alphaBlend(*(fb+pitch), scaleAlpha(color, blendTable8[F_RANGE-distXHalf][        distYHalf]));

				fracY += (dy * (F_HALF+(F_ONE-fracX))) >> F_BITS;
				fb++;
				fx0 += F_ONE;
			}
		}

		// Move the pixel forward to the next full pixel center

		fx0 = (fx0 & (~F_RANGE)) + F_HALF;

		// Top-down

		if (dy >= 0)
		{
			// Draw the pixels

			while(fx0 < fx1)
			{
				// Step (with error correction)

				if (fracY > F_ONE)
				{
					fracY -= F_ONE;
					fb += pitch;
					iy0++;
				}

				if (fracY < F_HALF)
				{
					int	distYHalf = F_HALF - fracY;
					if (iy0 > 0)			*(fb-pitch) = alphaBlend(*(fb-pitch), scaleAlpha(color,         distYHalf));
					if (iy0 < (int) height())	*(fb)       = alphaBlend(*(fb),       scaleAlpha(color, F_RANGE-distYHalf));
				}
				else
				{
					int	distYHalf = fracY - F_HALF;
					if (iy0 < (int) height())	*(fb)       = alphaBlend(*(fb),       scaleAlpha(color, F_RANGE-distYHalf));
					if (iy0 < (int) height()-1)	*(fb+pitch) = alphaBlend(*(fb+pitch), scaleAlpha(color,         distYHalf));
				}
				fb++;
				fracY += dy;
				fx0 += F_ONE;
			}

		}

		// Bottom-up

		else
		{
			// Draw the pixels

			while(fx0 < fx1)
			{
				// Step (with error correction)

				if (fracY < 0)
				{
					fracY += F_ONE;
					fb -= pitch;
					iy0--;
				}

				if (fracY < F_HALF)
				{
					int	distYHalf = F_HALF - fracY;
					if (iy0 > 0)	*(fb-pitch) = alphaBlend(*(fb-pitch), scaleAlpha(color,	        distYHalf));
					if (iy0 >= 0)	*(fb)       = alphaBlend(*(fb),       scaleAlpha(color, F_RANGE-distYHalf));
				}
				else
				{
					int	distYHalf = fracY - F_HALF;
					if (iy0 >= 0)			*(fb)       = alphaBlend(*(fb),       scaleAlpha(color, F_RANGE-distYHalf));
					if (iy0 < (int) height()-1)	*(fb+pitch) = alphaBlend(*(fb+pitch), scaleAlpha(color,         distYHalf));
				}
				fb++;
				fracY += dy;
				fx0 += F_ONE;
			}
		}

		// Last pixel requires special care

		if (ix1 < (int) width())
		{
			fracX = fx1 & F_RANGE;
			fracY = fy1 & F_RANGE;
			distXHalf = fracX > F_HALF ? fracX-F_HALF:F_HALF-fracX;
			distYHalf = fracY > F_HALF ? fracY-F_HALF:F_HALF-fracY;
			fb = reinterpret_cast<unsigned int *>(frameBuffer()) + iy1 * pitch + ix1;

			if (fracY < F_HALF)
			{
				if (fracX > F_HALF)
				{
					if (ix1 < (int)width()-1)
					{
								*(fb+1)       = alphaBlend(*(fb+1),       scaleAlpha(color, blendTable8[distXHalf][F_RANGE-distYHalf]));
						if (iy1 > 0)	*(fb-pitch+1) = alphaBlend(*(fb-pitch+1), scaleAlpha(color, blendTable8[distXHalf][        distYHalf]));
					}
				}
				else
				{
								*(fb)       = alphaBlend(*(fb),       scaleAlpha(color, blendTable8[F_RANGE-distXHalf][F_RANGE-distYHalf]));
						if (iy1 > 0)	*(fb-pitch) = alphaBlend(*(fb-pitch), scaleAlpha(color, blendTable8[F_RANGE-distXHalf][        distYHalf]));
				}
			}
			else
			{
				if (fracX > F_HALF)
				{
					if (ix1 < (int)width()-1)
					{
										*(fb+1)       = alphaBlend(*(fb+1),       scaleAlpha(color, blendTable8[distXHalf][F_RANGE-distYHalf]));
						if (iy1 < (int) height()-1)	*(fb+pitch+1) = alphaBlend(*(fb+pitch+1), scaleAlpha(color, blendTable8[distXHalf][        distYHalf]));
					}
				}
				else
				{
										*fb         = alphaBlend(*(fb),       scaleAlpha(color, blendTable8[F_RANGE-distXHalf][F_RANGE-distYHalf]));
						if (iy1 < (int) height()-1)	*(fb+pitch) = alphaBlend(*(fb+pitch), scaleAlpha(color, blendTable8[F_RANGE-distXHalf][        distYHalf]));
				}
			}
		}
	}

	// Primarily vertical

	else
	{
		// Enforce top->bottom orientation

		if (fy0 > fy1)
		{
			int	temp;
			temp = fx0; fx0 = fx1; fx1 = temp;
			temp = fy0; fy0 = fy1; fy1 = temp;
			temp = ix0; ix0 = ix1; ix1 = temp;
			temp = iy0; iy0 = iy1; iy1 = temp;
		}

		unsigned int	*fb = reinterpret_cast<unsigned int *>(frameBuffer()) + iy0 * pitch + ix0;

		// Fractional values (0...1)

		int	fracX = fx0 & F_RANGE;
		int	fracY = fy0 & F_RANGE;

		// Delta X

		int	dx = 0;
		if (fy1-fy0) dx = ((fx1-fx0)<<F_BITS) / (fy1-fy0);

		// First pixel requires special care

		int	distXHalf = fracX > F_HALF ? fracX-F_HALF:F_HALF-fracX;
		int	distYHalf = fracY > F_HALF ? fracY-F_HALF:F_HALF-fracY;

		if (fracX < F_HALF)
		{
			if (fracY < F_HALF)
			{
				if (iy0 > 0)
				{
					if (ix0 > 0)	*(fb-pitch-1) = alphaBlend(*(fb-pitch-1), scaleAlpha(color, blendTable8[        distXHalf][distYHalf]));
							*(fb-pitch)   = alphaBlend(*(fb-pitch),   scaleAlpha(color, blendTable8[F_RANGE-distXHalf][distYHalf]));
				}

				fracX += (dx * (F_HALF-fracY)) >> F_BITS;
			}
			else
			{
					if (ix0 > 0)	*(fb-1) = alphaBlend(*(fb-1), scaleAlpha(color, blendTable8[        distXHalf][F_RANGE-distYHalf]));
							*fb     = alphaBlend(*fb,     scaleAlpha(color, blendTable8[F_RANGE-distXHalf][F_RANGE-distYHalf]));

				fracX += (dx * (F_HALF+(F_ONE-fracY))) >> F_BITS;
				fb += pitch;
				fy0 += F_ONE;
			}
		}
		else
		{
			if (fracY < F_HALF)
			{
				if (iy0 > 0)
				{
									*(fb-pitch)   = alphaBlend(*(fb-pitch),   scaleAlpha(color, blendTable8[F_RANGE-distXHalf][distYHalf]));
					if (ix0 < (int) width()-1)	*(fb-pitch+1) = alphaBlend(*(fb-pitch+1), scaleAlpha(color, blendTable8[        distXHalf][distYHalf]));
				}

				fracX += (dx * (F_HALF-fracY)) >> F_BITS;
			}
			else
			{
									*fb     = alphaBlend(*fb,     scaleAlpha(color, blendTable8[F_RANGE-distXHalf][F_RANGE-distYHalf]));
					if (ix0 < (int) width()-1)	*(fb+1) = alphaBlend(*(fb+1), scaleAlpha(color, blendTable8[        distXHalf][F_RANGE-distYHalf]));

				fracX += (dx * (F_HALF+(F_ONE-fracY))) >> F_BITS;
				fb += pitch;
				fy0 += F_ONE;
			}
		}

		// Move the pixel forward to the next full pixel center

		fy0 = (fy0 & (~F_RANGE)) + F_HALF;

		// Left->Right

		if (dx >= 0)
		{
			// Draw the pixels

			while(fy0 < fy1)
			{
				// Step (with error correction)

				if (fracX > F_ONE)
				{
					fracX -= F_ONE;
					fb++;
					ix0++;
				}

				if (fracX < F_HALF)
				{
					int	distXHalf = F_HALF - fracX;
					if (ix0 > 0)			*(fb-1) = alphaBlend(*(fb-1), scaleAlpha(color,         distXHalf));
					if (ix0 < (int) width())	*(fb)   = alphaBlend(*(fb),   scaleAlpha(color, F_RANGE-distXHalf));
				}
				else
				{
					int	distXHalf = fracX - F_HALF;
					if (ix0 < (int) width())	*(fb)   = alphaBlend(*(fb),   scaleAlpha(color, F_RANGE-distXHalf));
					if (ix0 < (int) width()-1)	*(fb+1) = alphaBlend(*(fb+1), scaleAlpha(color,         distXHalf));
				}
				fb += pitch;
				fracX += dx;
				fy0 += F_ONE;
			}

		}

		// Right->Left

		else
		{
			// Draw the pixels

			while(fy0 < fy1)
			{
				// Step (with error correction)

				if (fracX < 0)
				{
					fracX += F_ONE;
					fb--;
					ix0--;
				}

				if (fracX < F_HALF)
				{
					int	distXHalf = F_HALF - fracX;
					if (ix0 > 0)	*(fb-1) = alphaBlend(*(fb-1), scaleAlpha(color,	        distXHalf));
					if (ix0 >= 0)	*(fb)   = alphaBlend(*(fb),   scaleAlpha(color, F_RANGE-distXHalf));
				}
				else
				{
					int	distXHalf = fracX - F_HALF;
					if (ix0 >= 0)			*(fb)   = alphaBlend(*(fb),   scaleAlpha(color, F_RANGE-distXHalf));
					if (ix0 < (int) height()-1)	*(fb+1) = alphaBlend(*(fb+1), scaleAlpha(color,         distXHalf));
				}
				fb += pitch;
				fracX += dx;
				fy0 += F_ONE;
			}
		}

		// Last pixel requires special care

		if (iy1 < (int) height())
		{
			fracX = fx1 & F_RANGE;
			fracY = fy1 & F_RANGE;
			distXHalf = fracX > F_HALF ? fracX-F_HALF:F_HALF-fracX;
			distYHalf = fracY > F_HALF ? fracY-F_HALF:F_HALF-fracY;
			fb = reinterpret_cast<unsigned int *>(frameBuffer()) + iy1 * pitch + ix1;

			if (fracX < F_HALF)
			{
				if (fracY > F_HALF)
				{
					if (iy1 < (int)height()-1)
					{
								*(fb+pitch)   = alphaBlend(*(fb+pitch),   scaleAlpha(color, blendTable8[F_RANGE-distXHalf][distYHalf]));
						if (ix1 > 0)	*(fb+pitch-1) = alphaBlend(*(fb+pitch-1), scaleAlpha(color, blendTable8[        distXHalf][distYHalf]));
					}
				}
				else
				{
								*(fb)   = alphaBlend(*(fb),   scaleAlpha(color, blendTable8[F_RANGE-distXHalf][F_RANGE-distYHalf]));
						if (ix1 > 0)	*(fb-1) = alphaBlend(*(fb-1), scaleAlpha(color, blendTable8[        distXHalf][F_RANGE-distYHalf]));
				}
			}
			else
			{
				if (fracY > F_HALF)
				{
					if (iy1 < (int)height()-1)
					{
										*(fb+pitch)   = alphaBlend(*(fb+pitch),   scaleAlpha(color, blendTable8[F_RANGE-distXHalf][distYHalf]));
						if (ix1 < (int) width()-1)	*(fb+pitch+1) = alphaBlend(*(fb+pitch+1), scaleAlpha(color, blendTable8[        distXHalf][distYHalf]));
					}
				}
				else
				{
										*fb     = alphaBlend(*(fb),   scaleAlpha(color, blendTable8[F_RANGE-distXHalf][F_RANGE-distYHalf]));
						if (ix1 < (int) width()-1)	*(fb+1) = alphaBlend(*(fb+1), scaleAlpha(color, blendTable8[        distXHalf][F_RANGE-distYHalf]));
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	winDIB::fillRect(const int left, const int top, const int right, const int bottom, const unsigned int color)
{
	if (!frameBuffer()) return;

	// Clip

	int	t = top;
	int	l = left;
	int	r = right;
	int	b = bottom;

	// Totally off-screen?

	if (l >= static_cast<int>(width()) || r < 0 || t >= static_cast<int>(height()) || b < 0) return;

	// Clip it to the screen

	if (l < 0) l = 0;
	if (t < 0) t = 0;

	if (r >= static_cast<int>(width())) r = width() - 1;
	if (b >= static_cast<int>(height())) b = height() - 1;

	// Make sure it's ordered properly

	if (l > r || t > b) return;

	// Fill it

	int	x0 = l;
	int	y0 = t;
	int	x1 = r + 1;
	int	y1 = b + 1;

	// The alpha component

	unsigned int	alpha = color >> 24;

	// Full alpha?

	if (alpha == 0xff)
	{
		unsigned int	*fbRow = reinterpret_cast<unsigned int *>(frameBuffer()) + y0 * width() + x0;
		for (int y = y0; y < y1; y++, fbRow += width())
		{
			unsigned int	*fb = fbRow;
			for (int x = x0; x < x1; x++, fb++)
			{
				*fb = color;
			}
		}
	}
	else
	{
		unsigned int	*fbRow = reinterpret_cast<unsigned int *>(frameBuffer()) + y0 * width() + x0;
		for (int y = y0; y < y1; y++, fbRow += width())
		{
			unsigned int	*fb = fbRow;
			for (int x = x0; x < x1; x++, fb++)
			{
				unsigned int	dst = *fb;
				unsigned int	rb = (((color & 0x00ff00ff) * alpha) + ((dst & 0x00ff00ff) * (0xff - alpha))) & 0xff00ff00;
				unsigned int	g  = (((color & 0x0000ff00) * alpha) + ((dst & 0x0000ff00) * (0xff - alpha))) & 0x00ff0000;
				*fb = (rb | g) >> 8;
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------
// WinDIB.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

