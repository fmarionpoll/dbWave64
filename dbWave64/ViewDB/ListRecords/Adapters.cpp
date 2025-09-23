#include "stdafx.h"
#include "Adapters.h"

// Define the static array and its count here (single definition).
const COLORREF ChartDataRendererAdapter::color_flag_[] =
{
    col_white,
    col_light_grey,
    RGB(246, 133, 17),  // 3
    RGB(15, 181, 174),  // 1
    RGB(197, 198, 198), // 4
    RGB(114, 224, 106), // 7
    RGB(222, 61, 130),  // 5
    RGB(20, 122, 243),  // 8
    RGB(64, 70, 202),   // 2
    RGB(115, 38, 211),  // 9
};

const int ChartDataRendererAdapter::color_flag_count =
    static_cast<int>(sizeof(ChartDataRendererAdapter::color_flag_) / sizeof(ChartDataRendererAdapter::color_flag_[0]));