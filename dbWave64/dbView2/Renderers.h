#pragma once

#include <afxwin.h>
#include "DisplaySettings.h"
#include "DbWaveDataProvider.h"

class IDataRenderer
{
public:
	virtual ~IDataRenderer() = default;
	virtual CBitmap createBitmap(const DisplaySettings& settings, const RowMeta& meta) = 0;
};

class ISpikeRenderer
{
public:
	virtual ~ISpikeRenderer() = default;
	virtual CBitmap createBitmap(const DisplaySettings& settings, const RowMeta& meta) = 0;
};

class EmptyBitmapRenderer
{
public:
	static CBitmap create(const int width, const int height)
	{
		CBitmap bmp;
		CDC dc;
		CWindowDC screenDC(nullptr);
		dc.CreateCompatibleDC(&screenDC);
		bmp.CreateBitmap(width, height, screenDC.GetDeviceCaps(PLANES), screenDC.GetDeviceCaps(BITSPIXEL), nullptr);
		return bmp;
	}
};


