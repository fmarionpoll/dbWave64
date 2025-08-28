#include "pch.h"
#define DBVIEW2_TEST
#include "../dbWave64/dbView2/Adapters.h"

TEST(RendererAdapters, DataRendererCreatesBitmapEvenIfMissingFile)
{
	ChartDataRendererAdapter renderer;
	DisplaySettings s; s.image_width = 100; s.image_height = 30;
	RowMeta m; m.cs_datafile_name = _T("");
	CBitmap bmp;
	renderer.renderBitmap(s, m, bmp);
	EXPECT_NE(bmp.m_hObject, nullptr);
}

TEST(RendererAdapters, SpikeRendererCreatesBitmapEvenIfMissingFile)
{
	ChartSpikeRendererAdapter renderer(nullptr);
	DisplaySettings s; s.image_width = 100; s.image_height = 30;
	RowMeta m; m.cs_spike_file_name = _T("");
	CBitmap bmp;
	renderer.renderBitmap(s, m, bmp);
	EXPECT_NE(bmp.m_hObject, nullptr);
}
