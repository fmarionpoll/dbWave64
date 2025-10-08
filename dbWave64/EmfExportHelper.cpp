#include "StdAfx.h"
#include "EmfExportHelper.h"
#include "NiceUnit.h"
#include <algorithm>
#include <cmath>

void EmfExportHelper::DrawAxes(CDC* p_dc, const CRect& rc)
{
	const int saved = p_dc->SaveDC();
	p_dc->SetMapMode(MM_TEXT);
	p_dc->SelectClipRgn(nullptr);
	
	CPen axes_pen(PS_SOLID, EmfLayout::AXES_PEN_WIDTH, EmfLayout::COLOR_AXES);
	const auto old_pen = p_dc->SelectObject(&axes_pen);
	
	// Draw border rectangle
	p_dc->MoveTo(rc.left, rc.top);
	p_dc->LineTo(rc.right, rc.top);
	p_dc->LineTo(rc.right, rc.bottom);
	p_dc->LineTo(rc.left, rc.bottom);
	p_dc->LineTo(rc.left, rc.top);
	
	// Draw tick marks
	for (int i = 1; i <= EmfLayout::TICK_MARK_COUNT; ++i)
	{
		// Horizontal ticks (bottom)
		const int x_tick = rc.left + MulDiv(i, rc.Width(), EmfLayout::TICK_MARK_COUNT + 1);
		p_dc->MoveTo(x_tick, rc.bottom);
		p_dc->LineTo(x_tick, rc.bottom - EmfLayout::TICK_MARK_LENGTH);
		
		// Vertical ticks (left)
		const int y_tick = rc.top + MulDiv(i, rc.Height(), EmfLayout::TICK_MARK_COUNT + 1);
		p_dc->MoveTo(rc.left, y_tick);
		p_dc->LineTo(rc.left + EmfLayout::TICK_MARK_LENGTH, y_tick);
	}
	
	if (old_pen) p_dc->SelectObject(old_pen);
	p_dc->RestoreDC(saved);
}

void EmfExportHelper::DrawScaleBar(CDC* p_dc, const CRect& rc, const double dt_seconds, const double px_per_volt, CString* out_label)
{
	const int saved = p_dc->SaveDC();
	p_dc->SetMapMode(MM_TEXT);
	p_dc->SelectClipRgn(nullptr);
	
	// Calculate L-mark origin
	const int x0 = rc.left + EmfLayout::SCALE_BAR_MARGIN_LEFT;
	const int y0 = rc.bottom - EmfLayout::SCALE_BAR_MARGIN_BOTTOM;
	
	// Calculate horizontal scale bar with nice span
	CString unit_time;
	float scale_time = 1.0f;
	int nice_time_value = 0;
	double nice_time_span = 0.0;
	int horiz_px = 0;
	
	if (dt_seconds > 0.0)
	{
		// Get target span (1/5 of total)
		const float target_time = static_cast<float>(dt_seconds * EmfLayout::SCALE_BAR_TIME_RATIO);
		
		// Ask NiceUnit for a nice value
		const float scaled_t = NiceUnit::change_unit(target_time, &unit_time, &scale_time);
		nice_time_value = NiceUnit::nice_unit(scaled_t);
		nice_time_span = static_cast<double>(nice_time_value) * static_cast<double>(scale_time);
		nice_time_span = std::min(nice_time_span, dt_seconds);
		
		// Calculate bar length based on the NICE span (not the target)
		horiz_px = std::max(EmfLayout::SCALE_BAR_MIN_LENGTH_PX,
		                    static_cast<int>(std::lround(nice_time_span / dt_seconds * static_cast<double>(rc.Width()))));
	}
	
	// Calculate vertical scale bar with nice span
	CString unit_volts;
	float scale_volts = 1.0f;
	int nice_volt_value = 0;
	double nice_volt_span = 0.0;
	int vert_px = 0;
	
	if (px_per_volt > 0.0)
	{
		const double view_volts = static_cast<double>(rc.Height()) / px_per_volt;
		
		// Get target span (1/5 of total)
		const float target_volts = static_cast<float>(view_volts * EmfLayout::SCALE_BAR_VOLT_RATIO);
		
		// Ask NiceUnit for a nice value
		const float scaled_v = NiceUnit::change_unit(target_volts, &unit_volts, &scale_volts);
		nice_volt_value = NiceUnit::nice_unit(scaled_v);
		nice_volt_span = static_cast<double>(nice_volt_value) * static_cast<double>(scale_volts);
		nice_volt_span = std::min(nice_volt_span, view_volts);
		
		// Calculate bar length based on the NICE span (not the target)
		vert_px = std::max(EmfLayout::SCALE_BAR_MIN_LENGTH_PX,
		                   static_cast<int>(std::lround(nice_volt_span / view_volts * static_cast<double>(rc.Height()))));
	}
	
	// Limit vertical bar to not exceed available height
	vert_px = std::min(vert_px, rc.Height() - EmfLayout::SCALE_BAR_MAX_HEIGHT_OFFSET);
	
	// Draw L-mark as polyline for proper corner joining
	CPen scale_pen(PS_SOLID, EmfLayout::SCALE_BAR_PEN_WIDTH, EmfLayout::COLOR_SCALE_BAR);
	const auto old_pen = p_dc->SelectObject(&scale_pen);
	
	if (vert_px > 0 && horiz_px > 0)
	{
		CPoint pts[3];
		pts[0] = CPoint(x0, y0 - vert_px);  // top of vertical bar
		pts[1] = CPoint(x0, y0);             // corner
		pts[2] = CPoint(x0 + horiz_px, y0);  // end of horizontal bar
		p_dc->Polyline(pts, 3);
	}
	else if (vert_px > 0)
	{
		p_dc->MoveTo(x0, y0);
		p_dc->LineTo(x0, y0 - vert_px);
	}
	else if (horiz_px > 0)
	{
		p_dc->MoveTo(x0, y0);
		p_dc->LineTo(x0 + horiz_px, y0);
	}
	
	if (old_pen) p_dc->SelectObject(old_pen);
	
	// Generate and draw label using the nice values we already calculated
	if (out_label || (nice_volt_value > 0 || nice_time_value > 0))
	{
		CString label;
		
		// Format voltage
		if (nice_volt_value > 0)
			label.AppendFormat(_T("vert=%d %sV"), nice_volt_value, unit_volts);
		
		// Format time
		if (nice_time_value > 0)
		{
			if (!label.IsEmpty())
				label.Append(_T(", "));
			label.AppendFormat(_T("horz=%d %ss"), nice_time_value, unit_time);
		}
		
		if (out_label)
			*out_label = label;
		
		if (!label.IsEmpty())
		{
			DrawText(p_dc, label, 
			        x0 + horiz_px + EmfLayout::TEXT_OFFSET_FROM_SCALE_X,
			        y0 - EmfLayout::TEXT_OFFSET_FROM_SCALE_Y,
			        EmfLayout::COLOR_TEXT);
		}
	}
	
	p_dc->RestoreDC(saved);
}

int EmfExportHelper::CalculateScaleBarLength(const double data_span, const int display_size_px, const double target_ratio, double& out_span)
{
	if (data_span <= 0.0 || display_size_px <= 0)
	{
		out_span = 0.0;
		return 0;
	}
	
	// Calculate target span based on desired ratio
	const float target_span = static_cast<float>(data_span * target_ratio);
	
	// Use NiceUnit to round to human-readable increment
	CString unit;
	float scale = 1.0f;
	const float scaled = NiceUnit::change_unit(target_span, &unit, &scale);
	const int nice_int = NiceUnit::nice_unit(scaled);
	
	// Calculate actual span
	out_span = static_cast<double>(nice_int) * static_cast<double>(scale);
	out_span = std::min(out_span, data_span);
	
	// Convert to pixels
	const int length_px = std::max(EmfLayout::SCALE_BAR_MIN_LENGTH_PX,
	                               static_cast<int>(std::lround(out_span / data_span * static_cast<double>(display_size_px))));
	
	return length_px;
}

CString EmfExportHelper::FormatScaleLabel(const double span_volts, const double span_seconds)
{
	CString text;
	
	// Format voltage component
	if (span_volts > 0.0)
	{
		CString unit_v;
		float scale_v = 1.0f;
		const float scaled_v = NiceUnit::change_unit(static_cast<float>(span_volts), &unit_v, &scale_v);
		const int nice_v = NiceUnit::nice_unit(scaled_v);
		
		// Display the nice integer value directly with the unit NiceUnit selected
		// e.g., if span_volts=0.0007V, NiceUnit might return nice_v=700, unit='µ', so display "700 µV"
		text.AppendFormat(_T("vert=%d %sV"), nice_v, unit_v);
	}
	
	// Format time component
	if (span_seconds > 0.0)
	{
		if (!text.IsEmpty())
			text.Append(_T(", "));
		
		CString unit_t;
		float scale_t = 1.0f;
		const float scaled_t = NiceUnit::change_unit(static_cast<float>(span_seconds), &unit_t, &scale_t);
		const int nice_t = NiceUnit::nice_unit(scaled_t);
		
		// Display the nice integer value directly with the unit NiceUnit selected
		// e.g., if span_seconds=0.4s, NiceUnit might return nice_t=400, unit='m', so display "400 ms"
		text.AppendFormat(_T("horz=%d %ss"), nice_t, unit_t);
	}
	
	return text;
}

CFont* EmfExportHelper::CreateExportFont(CDC* p_dc, const int font_size_pt)
{
	LOGFONT lf{};
	lf.lfHeight = -MulDiv(font_size_pt, p_dc->GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfWeight = EmfLayout::FONT_WEIGHT;
	lf.lfCharSet = EmfLayout::FONT_CHARSET;
	lf.lfOutPrecision = OUT_TT_PRECIS;       // Use TrueType for better Unicode support
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;            // High quality rendering
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	lstrcpyn(lf.lfFaceName, EmfLayout::FONT_FACE, LF_FACESIZE);
	
	auto* font = new CFont();
	if (!font->CreateFontIndirect(&lf))
	{
		delete font;
		return nullptr;
	}
	
	return font;
}

void EmfExportHelper::DrawText(CDC* p_dc, const CString& text, const int x, const int y, const COLORREF color)
{
	if (text.IsEmpty())
		return;
	
	const int saved = p_dc->SaveDC();
	p_dc->SetMapMode(MM_TEXT);
	p_dc->SelectClipRgn(nullptr);
	p_dc->SetBkMode(TRANSPARENT);
	p_dc->SetTextColor(color);
	
	// Ensure proper Unicode text output for special characters like µ
	p_dc->SetTextCharacterExtra(0);
	p_dc->SetTextAlign(TA_LEFT | TA_TOP | TA_NOUPDATECP);
	
	CFont* font = CreateExportFont(p_dc);
	CFont* old_font = nullptr;
	if (font)
		old_font = p_dc->SelectObject(font);
	
	// Use TextOutW (wide character version) to ensure Unicode support
	const int length = text.GetLength();
	::TextOutW(p_dc->GetSafeHdc(), x, y, text, length);
	
	if (old_font && font)
		p_dc->SelectObject(old_font);
	if (font)
	{
		font->DeleteObject();
		delete font;
	}
	
	p_dc->RestoreDC(saved);
}

CRect EmfExportHelper::GetDataRectangle(const CRect& full_rect)
{
	CRect data_rect = full_rect;
	data_rect.left += EmfLayout::LEFT_MARGIN_FOR_SCALE;
	data_rect.bottom -= EmfLayout::BOTTOM_MARGIN_FOR_TEXT;
	return data_rect;
}

bool EmfExportHelper::IsEmfDC(CDC* p_dc)
{
	if (!p_dc || !p_dc->GetSafeHdc())
		return false;
	
	return ::GetObjectType(p_dc->GetSafeHdc()) == OBJ_ENHMETADC;
}

