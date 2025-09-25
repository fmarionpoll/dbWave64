## MM_ANISOTROPIC Mapping: Logical ↔ Physical and Print/Export Tips

### What MM_ANISOTROPIC Does
- You define a linear mapping from logical to device coordinates:
  - device = scale × (logical − windowOrigin) + viewportOrigin
- Scale is set by `SetWindowExt()` and `SetViewportExt()`:
  - scaleX = ViewportExt.cx / WindowExt.cx
  - scaleY = ViewportExt.cy / WindowExt.cy
- Origins are set by `SetWindowOrg()` and `SetViewportOrg()`.
- Y direction is controlled by the sign of ViewportExt.cy (negative flips Y downwards like screen pixels).

### Device Metrics You’ll Use
- `HORZRES`, `VERTRES`: device width/height in pixels
- `HORZSIZE`, `VERTSIZE`: device width/height in millimeters
- `LOGPIXELSX`, `LOGPIXELSY`: DPI (pixels per inch)
- Pixels per millimeter:
  - px_per_mm_x = HORZRES / HORZSIZE
  - px_per_mm_y = VERTRES / VERTSIZE

### Two Practical Mapping Setups

1) 1 logical unit = 1 device pixel (fills the rect exactly)
```cpp
// Fill rect [left, top, right, bottom] exactly, Y grows down
dc->SetMapMode(MM_ANISOTROPIC);
dc->SetWindowExt(rect.Width(), rect.Height());
dc->SetWindowOrg(0, 0);
dc->SetViewportOrg(rect.left, rect.top + rect.Height());
dc->SetViewportExt(rect.Width(), -rect.Height());
```

2) Logical units = millimeters (physically consistent sizing on paper)
```cpp
dc->SetMapMode(MM_ANISOTROPIC);
const int px_per_mm_x = dc->GetDeviceCaps(HORZRES) / std::max(1, dc->GetDeviceCaps(HORZSIZE));
const int px_per_mm_y = dc->GetDeviceCaps(VERTRES) / std::max(1, dc->GetDeviceCaps(VERTSIZE));

const int w_mm = desired_width_mm;
const int h_mm = desired_height_mm;

dc->SetWindowExt(w_mm, h_mm);       // logical units in mm
dc->SetWindowOrg(0, 0);
dc->SetViewportOrg(rect.left, rect.top + px_per_mm_y * h_mm);
dc->SetViewportExt(px_per_mm_x * w_mm, -px_per_mm_y * h_mm);
```

### Why EMF (Vector) Looks Good Scaled, But Paper Can Look Off
- With the “1 logical = 1 pixel” approach, sampling and strokes are at device pixel granularity.
- EMF preserves geometry and scales later in apps like Corel/Illustrator, so it stays crisp.
- Printers have very high DPI (e.g., 600–1200), so 1‑pixel hairlines can be too thin; dense sampling can look faint.

Recommendations for paper:
- Consider a mm-based mapping for physical consistency; or
- Keep pixel mapping but use geometric pens with logical widths (e.g., 0.2 mm) so the output is not hairline‑thin regardless of DPI.

### Ensuring Curves Span the Intended Rectangle
- If using pixel mapping, pass the exact target width in pixels into your sampling (e.g., `resize_channels(display_rect_.Width(), data_len)`) so the envelope fills the width.
- If using mm mapping, compute the sampling width from the post‑mapping viewport width in pixels (`abs(ViewportExt.cx)`) and use that to size the envelope.

### Clipping, Rulers, and Margins
- If rulers/margins are active, your content rect (`display_rect_`) might be smaller than the frame; disable ruler margins (e.g., `b_nice_grid = FALSE`) for export/printing when you want the content to fill the whole target rect.
- Clip only after mapping if needed; otherwise clear clips (`SelectClipRgn(nullptr)`) to avoid accidental content loss.

### Save/Restore DC
- Always `SaveDC()` before setting mapping and `RestoreDC()` after to avoid leaking transforms or clip regions into subsequent drawing.

### Quick Checklist
- Choose mapping (pixels vs mm) based on target medium.
- Set mapping (Window/Viewport Ext/Org) before drawing.
- Size the envelope to the mapped width so it spans the rect.
- Use geometric pens for stable physical line widths on printers.
- Disable ruler margins for full‑rect export; manage clipping explicitly.
- Bracket each section with `SaveDC()`/`RestoreDC()`.


