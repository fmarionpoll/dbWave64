#include "pch.h"
#include <gtest/gtest.h>
#include "../dbWave64/EmfExportHelper.h"
#include "../dbWave64/EmfLayoutConstants.h"

// Mock DC for testing (uses memory DC)
class TestDC : public CDC
{
public:
	TestDC()
	{
		CreateCompatibleDC(nullptr);
	}
	~TestDC()
	{
		DeleteDC();
	}
};

// Test fixture for EmfExportHelper tests
class EmfExportHelperTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		// Create a memory DC for testing
		m_dc.CreateCompatibleDC(nullptr);
	}

	void TearDown() override
	{
		m_dc.DeleteDC();
	}

	CDC m_dc;
};

TEST_F(EmfExportHelperTests, GetDataRectangle_ReducesByMargins)
{
	CRect full_rect(0, 0, 100, 100);
	CRect data_rect = EmfExportHelper::GetDataRectangle(full_rect);
	
	EXPECT_EQ(data_rect.left, EmfLayout::LEFT_MARGIN_FOR_SCALE);
	EXPECT_EQ(data_rect.right, 100);
	EXPECT_EQ(data_rect.top, 0);
	EXPECT_EQ(data_rect.bottom, 100 - EmfLayout::BOTTOM_MARGIN_FOR_TEXT);
}

TEST_F(EmfExportHelperTests, GetDataRectangle_PreservesTopRight)
{
	CRect full_rect(50, 50, 200, 150);
	CRect data_rect = EmfExportHelper::GetDataRectangle(full_rect);
	
	EXPECT_EQ(data_rect.left, 50 + EmfLayout::LEFT_MARGIN_FOR_SCALE);
	EXPECT_EQ(data_rect.right, 200);
	EXPECT_EQ(data_rect.top, 50);
	EXPECT_EQ(data_rect.bottom, 150 - EmfLayout::BOTTOM_MARGIN_FOR_TEXT);
}

TEST_F(EmfExportHelperTests, CalculateScaleBarLength_ReturnsZeroForInvalidInput)
{
	double out_span = 0.0;
	
	// Zero data span
	int length = EmfExportHelper::CalculateScaleBarLength(0.0, 100, 0.2, out_span);
	EXPECT_EQ(length, 0);
	EXPECT_EQ(out_span, 0.0);
	
	// Negative data span
	length = EmfExportHelper::CalculateScaleBarLength(-1.0, 100, 0.2, out_span);
	EXPECT_EQ(length, 0);
	EXPECT_EQ(out_span, 0.0);
	
	// Zero display size
	length = EmfExportHelper::CalculateScaleBarLength(1.0, 0, 0.2, out_span);
	EXPECT_EQ(length, 0);
	EXPECT_EQ(out_span, 0.0);
}

TEST_F(EmfExportHelperTests, CalculateScaleBarLength_ReturnsReasonableValue)
{
	double out_span = 0.0;
	
	// 1 second span, 1000 pixel display, 20% ratio
	int length = EmfExportHelper::CalculateScaleBarLength(1.0, 1000, 0.2, out_span);
	
	EXPECT_GT(length, EmfLayout::SCALE_BAR_MIN_LENGTH_PX);
	EXPECT_GT(out_span, 0.0);
	EXPECT_LE(out_span, 1.0);  // Should not exceed total span
}

TEST_F(EmfExportHelperTests, CalculateScaleBarLength_RespectsMinimumLength)
{
	double out_span = 0.0;
	
	// Very small span should still return minimum length
	int length = EmfExportHelper::CalculateScaleBarLength(0.001, 1000, 0.2, out_span);
	
	EXPECT_GE(length, EmfLayout::SCALE_BAR_MIN_LENGTH_PX);
}

TEST_F(EmfExportHelperTests, FormatScaleLabel_EmptyWhenBothZero)
{
	CString label = EmfExportHelper::FormatScaleLabel(0.0, 0.0);
	EXPECT_TRUE(label.IsEmpty());
}

TEST_F(EmfExportHelperTests, FormatScaleLabel_VoltsOnly)
{
	CString label = EmfExportHelper::FormatScaleLabel(0.001, 0.0);  // 1 mV
	EXPECT_FALSE(label.IsEmpty());
	EXPECT_TRUE(label.Find(_T("vert")) >= 0);
	EXPECT_TRUE(label.Find(_T("horz")) < 0);
}

TEST_F(EmfExportHelperTests, FormatScaleLabel_TimeOnly)
{
	CString label = EmfExportHelper::FormatScaleLabel(0.0, 0.1);  // 100 ms
	EXPECT_FALSE(label.IsEmpty());
	EXPECT_TRUE(label.Find(_T("horz")) >= 0);
	EXPECT_TRUE(label.Find(_T("vert")) < 0);
}

TEST_F(EmfExportHelperTests, FormatScaleLabel_BothComponents)
{
	CString label = EmfExportHelper::FormatScaleLabel(0.001, 0.1);
	EXPECT_FALSE(label.IsEmpty());
	EXPECT_TRUE(label.Find(_T("vert")) >= 0);
	EXPECT_TRUE(label.Find(_T("horz")) >= 0);
	EXPECT_TRUE(label.Find(_T(",")) >= 0);  // Should have comma separator
}

TEST_F(EmfExportHelperTests, CreateExportFont_ReturnsValidFont)
{
	CFont* font = EmfExportHelper::CreateExportFont(&m_dc);
	ASSERT_NE(font, nullptr);
	
	// Verify font properties
	LOGFONT lf;
	font->GetLogFont(&lf);
	EXPECT_EQ(lf.lfWeight, EmfLayout::FONT_WEIGHT);
	EXPECT_EQ(lf.lfCharSet, EmfLayout::FONT_CHARSET);
	EXPECT_STREQ(lf.lfFaceName, EmfLayout::FONT_FACE);
	
	delete font;
}

TEST_F(EmfExportHelperTests, CreateExportFont_CustomSize)
{
	CFont* font1 = EmfExportHelper::CreateExportFont(&m_dc, 12);
	CFont* font2 = EmfExportHelper::CreateExportFont(&m_dc, 8);
	
	ASSERT_NE(font1, nullptr);
	ASSERT_NE(font2, nullptr);
	
	LOGFONT lf1, lf2;
	font1->GetLogFont(&lf1);
	font2->GetLogFont(&lf2);
	
	// Different point sizes should have different heights
	EXPECT_NE(lf1.lfHeight, lf2.lfHeight);
	
	delete font1;
	delete font2;
}

TEST_F(EmfExportHelperTests, IsEmfDC_ReturnsFalseForMemoryDC)
{
	EXPECT_FALSE(EmfExportHelper::IsEmfDC(&m_dc));
}

TEST_F(EmfExportHelperTests, IsEmfDC_ReturnsFalseForNull)
{
	EXPECT_FALSE(EmfExportHelper::IsEmfDC(nullptr));
}

TEST_F(EmfExportHelperTests, DrawAxes_DoesNotCrash)
{
	CRect rect(0, 0, 100, 100);
	
	// Should not crash or throw
	EXPECT_NO_THROW({
		EmfExportHelper::DrawAxes(&m_dc, rect);
	});
}

TEST_F(EmfExportHelperTests, DrawScaleBar_DoesNotCrash)
{
	CRect rect(0, 0, 100, 100);
	CString label;
	
	// Should not crash or throw
	EXPECT_NO_THROW({
		EmfExportHelper::DrawScaleBar(&m_dc, rect, 1.0, 100.0, &label);
	});
	
	// Label should be generated
	EXPECT_FALSE(label.IsEmpty());
}

TEST_F(EmfExportHelperTests, DrawScaleBar_WithoutLabel)
{
	CRect rect(0, 0, 100, 100);
	
	// Should work without output label
	EXPECT_NO_THROW({
		EmfExportHelper::DrawScaleBar(&m_dc, rect, 1.0, 100.0, nullptr);
	});
}

TEST_F(EmfExportHelperTests, DrawText_DoesNotCrash)
{
	CString text = _T("Test");
	
	// Should not crash or throw
	EXPECT_NO_THROW({
		EmfExportHelper::DrawText(&m_dc, text, 10, 10);
	});
}

TEST_F(EmfExportHelperTests, DrawText_EmptyString)
{
	// Should handle empty string gracefully
	EXPECT_NO_THROW({
		EmfExportHelper::DrawText(&m_dc, _T(""), 10, 10);
	});
}

// Integration test with actual rectangle calculations
TEST_F(EmfExportHelperTests, Integration_FullExportSequence)
{
	CRect full_rect(0, 0, 400, 300);
	
	// Get data rectangle
	CRect data_rect = EmfExportHelper::GetDataRectangle(full_rect);
	EXPECT_LT(data_rect.Width(), full_rect.Width());
	EXPECT_LT(data_rect.Height(), full_rect.Height());
	
	// Draw axes on data rectangle
	EXPECT_NO_THROW({
		EmfExportHelper::DrawAxes(&m_dc, data_rect);
	});
	
	// Draw scale bar on full rectangle
	CString label;
	EXPECT_NO_THROW({
		EmfExportHelper::DrawScaleBar(&m_dc, full_rect, 1.0, 100.0, &label);
	});
	
	EXPECT_FALSE(label.IsEmpty());
}

// Performance test - ensure calculations are fast
TEST_F(EmfExportHelperTests, Performance_ScaleBarCalculation)
{
	double out_span = 0.0;
	
	auto start = std::chrono::high_resolution_clock::now();
	
	// Run calculation many times
	for (int i = 0; i < 1000; ++i)
	{
		EmfExportHelper::CalculateScaleBarLength(1.0, 1000, 0.2, out_span);
	}
	
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	
	// Should complete in reasonable time (< 100ms for 1000 iterations)
	EXPECT_LT(duration.count(), 100);
}







