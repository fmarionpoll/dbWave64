#pragma once

// EMF Export Layout Constants
// Centralized constants for Enhanced Metafile export layout and styling
// Used across chart export functions to maintain consistency

namespace EmfLayout {
	// Margins for scale bars within chart rectangles
	constexpr int LEFT_MARGIN_FOR_SCALE = 20;      // Space reserved on left for vertical scale bar
	constexpr int BOTTOM_MARGIN_FOR_TEXT = 20;     // Space reserved at bottom for text labels
	
	// Scale bar positioning (within reserved margin space)
	constexpr int SCALE_BAR_MARGIN_LEFT = 5;       // Distance from left edge to L-mark origin
	constexpr int SCALE_BAR_MARGIN_BOTTOM = 5;     // Distance from bottom edge to L-mark origin
	constexpr int SCALE_BAR_MAX_HEIGHT_OFFSET = 25; // Max vertical extent to prevent overlap
	
	// Scale bar sizing
	constexpr int SCALE_BAR_MIN_LENGTH_PX = 12;    // Minimum length for scale bars
	constexpr double SCALE_BAR_TIME_RATIO = 0.2;   // Target: 20% of view width (1/5)
	constexpr double SCALE_BAR_VOLT_RATIO = 0.2;   // Target: 20% of view height (1/5)
	
	// Text formatting
	constexpr int TEXT_FONT_SIZE_PT = 8;           // Font size in points (all EMF text)
	constexpr int TEXT_OFFSET_FROM_SCALE_X = 6;    // Horizontal offset from scale bar end
	constexpr int TEXT_OFFSET_FROM_SCALE_Y = 10;   // Vertical offset from baseline
	
	// Pen and line styles
	constexpr int SCALE_BAR_PEN_WIDTH = 2;         // Width of L-mark lines
	constexpr int AXES_PEN_WIDTH = 1;              // Width of axis border lines
	constexpr int TICK_MARK_LENGTH = 6;            // Length of axis tick marks
	constexpr int TICK_MARK_COUNT = 4;             // Number of tick marks per side
	
	// Colors
	constexpr COLORREF COLOR_SCALE_BAR = RGB(0, 0, 0);      // Black for scale bars
	constexpr COLORREF COLOR_AXES = RGB(100, 100, 100);     // Gray for axes
	constexpr COLORREF COLOR_TEXT = RGB(0, 0, 0);           // Black for text
	
	// Font specification
	constexpr WCHAR FONT_FACE[] = L"Arial";
	constexpr int FONT_WEIGHT = FW_NORMAL;
	constexpr BYTE FONT_CHARSET = ANSI_CHARSET;  // ANSI for better compatibility with µ character
}

