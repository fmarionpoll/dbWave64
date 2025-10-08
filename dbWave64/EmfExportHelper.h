#pragma once

#include "EmfLayoutConstants.h"
#include <afxwin.h>  // For CDC, CRect, CString, CFont

/// <summary>
/// Helper class for Enhanced Metafile (EMF) export operations.
/// Centralizes EMF-specific rendering logic to reduce code duplication
/// and provide consistent export behavior across different chart types.
/// </summary>
class EmfExportHelper
{
public:
	// No instances needed - all static utility methods
	EmfExportHelper() = delete;
	~EmfExportHelper() = delete;
	EmfExportHelper(const EmfExportHelper&) = delete;
	EmfExportHelper& operator=(const EmfExportHelper&) = delete;

	/// <summary>
	/// Draws axes (border and tick marks) around a chart area in MM_TEXT mode.
	/// </summary>
	/// <param name="p_dc">Device context for drawing (must be EMF DC)</param>
	/// <param name="rc">Rectangle defining the chart area</param>
	static void DrawAxes(CDC* p_dc, const CRect& rc);

	/// <summary>
	/// Draws an L-shaped scale bar with optional text labels.
	/// The scale bar is positioned at the bottom-left of the rectangle with
	/// appropriate margins as defined in EmfLayoutConstants.
	/// </summary>
	/// <param name="p_dc">Device context for drawing (must be EMF DC)</param>
	/// <param name="rc">Full rectangle including space for scale bar</param>
	/// <param name="dt_seconds">Time span displayed (for horizontal scale)</param>
	/// <param name="px_per_volt">Pixels per volt (for vertical scale)</param>
	/// <param name="out_label">Optional output: formatted label string (e.g., "vert=1 mV, horz=200 ms")</param>
	static void DrawScaleBar(CDC* p_dc, const CRect& rc, double dt_seconds, double px_per_volt, CString* out_label = nullptr);

	/// <summary>
	/// Calculates appropriate scale bar length based on data span and display size.
	/// Uses NiceUnit to generate human-readable scale increments.
	/// </summary>
	/// <param name="data_span">Total span of data (time in seconds or voltage in volts)</param>
	/// <param name="display_size_px">Available display size in pixels</param>
	/// <param name="target_ratio">Desired ratio (e.g., 0.2 for 20% of display)</param>
	/// <param name="out_span">Output: actual span represented by scale bar</param>
	/// <returns>Length of scale bar in pixels</returns>
	static int CalculateScaleBarLength(double data_span, int display_size_px, double target_ratio, double& out_span);

	/// <summary>
	/// Formats scale bar label text with appropriate units.
	/// Converts values to readable units (mV, µV, ms, µs, etc.) using NiceUnit.
	/// </summary>
	/// <param name="span_volts">Voltage span (0 if not applicable)</param>
	/// <param name="span_seconds">Time span (0 if not applicable)</param>
	/// <returns>Formatted label string (e.g., "vert=1 mV, horz=200 ms")</returns>
	static CString FormatScaleLabel(double span_volts, double span_seconds);

	/// <summary>
	/// Creates a standard font for EMF export text.
	/// Uses constants from EmfLayoutConstants for consistency.
	/// </summary>
	/// <param name="p_dc">Device context (for DPI calculation)</param>
	/// <param name="font_size_pt">Font size in points (defaults to standard size)</param>
	/// <returns>CFont object (caller responsible for deletion)</returns>
	static CFont* CreateExportFont(CDC* p_dc, int font_size_pt = EmfLayout::TEXT_FONT_SIZE_PT);

	/// <summary>
	/// Draws text in MM_TEXT mode with standard formatting.
	/// Handles device context save/restore automatically.
	/// </summary>
	/// <param name="p_dc">Device context for drawing</param>
	/// <param name="text">Text to draw</param>
	/// <param name="x">X coordinate (device units)</param>
	/// <param name="y">Y coordinate (device units)</param>
	/// <param name="color">Text color (defaults to black)</param>
	static void DrawText(CDC* p_dc, const CString& text, int x, int y, COLORREF color = EmfLayout::COLOR_TEXT);

	/// <summary>
	/// Calculates the data rectangle (excluding scale bar margins) from full rectangle.
	/// </summary>
	/// <param name="full_rect">Full rectangle including scale bar space</param>
	/// <returns>Rectangle for data display (reduced by margins)</returns>
	static CRect GetDataRectangle(const CRect& full_rect);

	/// <summary>
	/// Validates that a device context is an Enhanced Metafile DC.
	/// </summary>
	/// <param name="p_dc">Device context to check</param>
	/// <returns>True if p_dc is an EMF DC</returns>
	static bool IsEmfDC(CDC* p_dc);
};

