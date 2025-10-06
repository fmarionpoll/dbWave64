### EMF/MM_ANISOTROPIC CLIPBOARD MAPPING DEBUG NOTES

- Context: `ViewDbTable -> CopyAsEmfToClipboard -> ViewSpikeDetection::render_for_export -> ChartData::print`
- Goal: Draw data and a test frame inside the page‑rebased `p_rect` when exporting to EMF and copying to clipboard.

### Current Known Good State
- Rectangles passed to `ChartData::print` are page‑rebased (not screen coords).
- Mapping in `ChartData::print` right after `adjust_display_rect(p_rect)`:
  - `MM_TEXT -> MM_ANISOTROPIC` (1 -> 8)
  - `WindowOrg=(0,0)`, `WindowExt=(W,H)`
  - `ViewportOrg=(L, T + H/2)`, `ViewportExt=(W, -H)`
  - DP/LP checks match expected:
    - `LP(0, +H/2) -> DP(L, T)`
    - `LP(W, 0) -> DP(R, T + H/2)`
    - `DP(L, T) -> LP(0, +H/2)`
    - `DP(R, T + H/2) -> LP(W, 0)`
- Device caps during EMF recording: `TECH=DT_RASDISPLAY`, DPI ≈ (96,96), desktop RES reported.
- EMF page target now non‑trivial (e.g., 1038 × 873 pixels).
- Clip after mapping is logical and matches our logical rect (we also clear any prior clip).

### Remaining Symptom
- Only a tiny colored square shows near top‑left; the large test frame does not appear inside the target rectangle.

### Likely Causes To Validate
1) Hairline stroke visibility in EMF playback
   - EMF viewers can cull or thin hairlines. Use `PS_SOLID`, width 1+, and/or fill areas to validate.

2) Clipping still constraining drawing
   - Even after `SelectClipRgn(nullptr)` and `IntersectClipRect(logical)`, confirm the effective clip (we log `CLIP(after)`).

3) Order of operations or DC state overridden elsewhere
   - Ensure the test drawing happens after mapping is set and before any `RestoreDC` or external drawing alters the DC.

### What We Already Log In `ChartData::print`
- Map mode before/after
- Window/Viewport Org/Ext
- Device caps: TECHNOLOGY, LOGPIXELSX/Y, HORZRES/VERTRES, RASTERCAPS
- `GetClipBox` before and after explicit clip reset/intersect
- Sanity LP<->DP transforms for corners and centerline

### Next Checks (Minimal, High Signal)
1) Draw a filled test band in logical space to prove visibility
   - After mapping and clip:
     - Fill a horizontal band around Y=0 in logical coords: `y ∈ [-8..+8]`, `x ∈ [0..W]`.
     - Then draw the frame with `PS_SOLID`, width 2.

2) Also draw the same frame in MM_TEXT using device coords
   - Compute device corners via `LPtoDP` (already demonstrated by the logs) and draw a `Rectangle` after `SaveDC/SetMapMode(MM_TEXT)`.
   - If MM_TEXT rectangle appears perfectly while the logical one does not, it’s a stroke/clip interaction under MM_ANISOTROPIC.

3) Confirm no state reset between calls
   - Break at the test draw and check `GetMapMode()==MM_ANISOTROPIC` right there.
   - Ensure `SelectClipRgn(nullptr)` is not re‑applied after the clip we set.

### Quick Formulas (for reference)
- General mapping per axis: `DP = VO + (LP − WO) × VE / WE`; `LP = WO + (DP − VO) × WE / VE`.
- With our current mapping: `WO=(0,0)`, `WE=(W,H)`, `VO=(L,T+H/2)`, `VE=(W,−H)`.
  - `LP→DP`: `x_dp = L + x_lp`, `y_dp = (T + H/2) − y_lp`.
  - `DP→LP`: `x_lp = x_dp − L`, `y_lp = (T + H/2) − y_dp`.

### Concrete Actions To Try Next
- Use a thicker pen and insets to avoid boundary‑clipping:
  - `inset = 1;` draw logical frame: `[inset, +H/2−inset, W−inset, −H/2+inset]` with `pen width=2`.
- Fill to disambiguate stroke issues:
  - Draw a filled rectangle band `(x=[0..W], y=[−8..+8])` in logical space.
- Device‑space validation:
  - Switch to `MM_TEXT` (temporary `SaveDC/SetMapMode`), draw `Rectangle(L+1, T+1, R−1, B−1)`. If this appears, mapping/clip is fine; issue is stroke rendering under MM_ANISOTROPIC.
- Re‑verify `GetClipBox` after the clip set (we log this as `CLIP(after)`). It should show `[0, +H/2]..[W, −H/2]` in logical coords, possibly inset.

### If Still Failing
- Temporarily bypass `erase_background` and any other drawing to isolate.
- Ensure no subsequent drawing overwrites the test frame (place a breakpoint after `LineTo`).
- Try a very thick pen (e.g., width 6) to rule out viewer rasterization of 1‑pixel lines.

### Useful Snippets (pseudo‑calls you can paste in place)
```cpp
// 1) Logical filled band around center
const int band = 8; // logical units
CRect logical_band(0, -band, display_rect_.Width(), +band);
p_dc->FillSolidRect(&logical_band, RGB(255, 200, 0));

// 2) Logical frame with non‑hairline pen
CPen framePen; framePen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
auto oldPen = p_dc->SelectObject(&framePen);
const int inset = 1;
p_dc->Rectangle(inset, +h_dev/2 - inset, w_dev - inset, -h_dev/2 + inset);
p_dc->SelectObject(oldPen);

// 3) Device‑space frame (MM_TEXT) using DP corners
CPoint dpLT(display_rect_.left+1, display_rect_.top+1);
CPoint dpRB(display_rect_.right-1, display_rect_.bottom-1);
const int saved = p_dc->SaveDC();
p_dc->SetMapMode(MM_TEXT);
p_dc->Rectangle(dpLT.x, dpLT.y, dpRB.x, dpRB.y);
p_dc->RestoreDC(saved);
```

### Current Logs (examples you provided)
- Rect #1:
  - `display_rect=[L=162 T=0 R=1038 B=352] size=[876 x 352]`
  - `WindowOrg=(0,0) WindowExt=(876,352)`
  - `ViewportOrg=(162,176) ViewportExt=(876,-352)`
  - `LP/DP` sanity checks: match
  - `PAGE target HxV = (1038 x 873)`
  - `CLIP(after)= [L=0 T=176 R=876 B=-176]` (logical)
- Rect #2: analogous, size `[876 x 364]`; sanity checks also match.

### Bottom Line
- The mapping math and clip setup are correct; EMF page is now appropriately sized.
- The missing frame is likely due to stroke rendering/clip at playback. The next steps above (filled band, thicker pen, device‑space rectangle) will pinpoint whether this is a stroke‑visibility issue versus any lingering state.


