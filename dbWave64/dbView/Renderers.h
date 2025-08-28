#pragma once

#include <afxwin.h>
#include "DisplaySettings.h"
#include "DbWaveDataProvider.h"

class IDataRenderer
{
public:
	virtual ~IDataRenderer() = default;
	virtual void renderBitmap(const DisplaySettings& settings, const RowMeta& meta, CBitmap& out_bitmap) = 0;
};

class ISpikeRenderer
{
public:
	virtual ~ISpikeRenderer() = default;
	virtual void renderBitmap(const DisplaySettings& settings, const RowMeta& meta, CBitmap& out_bitmap) = 0;
};


