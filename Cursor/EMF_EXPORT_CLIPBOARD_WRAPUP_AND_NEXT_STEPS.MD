## EMF EXPORT (CLIPBOARD) – STATUS, FINDINGS, AND NEXT STEPS (2025-10-06)

### What Works Now
- EMF frame sizing is correct at creation time:
  - TargetPx is set before CreateEnhanced: 1038 × 873 px
  - HIMETRIC computed at 96 DPI ≈ 27464 × 23098
  - PowerPoint shows a large placeholder (≈ 19.05 × 22.79 cm), confirming a non-1×1 EMF page.

### Current Symptoms
- CorelDraw: tiny object (previously ~8×10 px) reporting ~188 objects inside (so content recorded, but visually tiny/clustered).
- PowerPoint: large rectangle appears but mostly blank; when zooming massively, some elements visible (background, a bit of signal, 2 ruler lines, spike bars superimposed).
- Our DEBUG traces show:
  - begin_aniso (top-level) sets MM_ANISOTROPIC with 1:1 logical mapping over a 1038×873 logical page; CLIP=[0,0,1038,873].
  - ChartData::print_data_to_dc applies its own MM_ANISOTROPIC mapping per sub-rect (876×352, 876×364), Y flipped, with clip boxes consistent with logical [-H/2..+H/2].

### Likely Root Causes (ranked)
1) EMF content visibility/scale: GDI hairlines and/or nested mappings still render too thin/clustered in EMF playback viewers.
   - We added a width-2 pen guard for polylines in EMF, but the overall scene still looks empty/tiny in viewers.
2) Diagnostic fills/frames not visible in EMF render path.
   - Even unconditional background/frames (DEBUG) did not surface in viewers; this suggests either the diagnostic block didn’t execute at runtime or subsequent DC state changes or clips occluded them.
   - However, top-level begin_aniso outputs consistent mapping/clip, so clipping should be permissive.
3) Nested mappings inside ChartData (SetMapMode/Viewport/Window) recorded into EMF may interact with the outer mapping in ways that certain viewers (Corel/PowerPoint) replay oddly (e.g., resulting in a tiny transform stack).

### Evidence Summary
- EMF page size now matches TargetPx; the tiny-object symptom in Corel implies transforms applied to content, not page size.
- ChartData sets viewport origin to an inset inside the export page (e.g., VO=(162,176) for a sub-rect) with negative VE for Y-flip; math is consistent, but some viewers may compose nested transforms differently.
- CLIP after ChartData is logical [0, +H/2]..[W, -H/2]; this is expected for Y-flipped mapping.

### Next Session – Diagnostic Plan (Minimal, High Signal)
1) Prove basic EMF drawing is visible BEFORE view code runs:
   - In GraphicsExport::CopyAsEmfToClipboard, immediately after CreateEnhanced and before calling draw_fn, draw a filled rectangle (full-page) and a thick frame in MM_TEXT. Repeat the same AFTER draw_fn. This isolates whether the page is visible independent of nested mappings.

2) Save the EMF to disk for inspection:
   - Temporarily change CopyAsEmfToClipboard to also save the HENHMETAFILE to a file (via CopyEnhMetaFile) for inspection in external EMF viewers.

3) Bypass nested mappings for export (one chart first):
   - Add an export flag to ChartData::print_data_to_dc to skip SetMapMode/Viewport/Window and draw using precomputed DP points in MM_TEXT (LPtoDP then MM_TEXT draw). Verify curve visibility; then re-enable elements incrementally (grid, tags, spikes, bars).

4) Disable ruler/margins influence explicitly in export:
   - Ensure b_nice_grid = FALSE and scope clip fully reset before ChartData drawing (already attempted; re-verify effective clip via GetClipBox and visualize w/ filled bands).

5) Independent playback sanity:
   - Add a small test harness that records a known scene to EMF with only one mapping (MM_TEXT, rectangles/lines), then paste into Corel/PowerPoint to confirm environment path is sound.

### Concrete Implementation Steps
- GraphicsExport::CopyAsEmfToClipboard (temporary diagnostics):
  - After CreateEnhanced:
    - SaveDC; SetMapMode(MM_TEXT); FillSolidRect(0,0,TargetPx.w,TargetPx.h, pale yellow); Rectangle with width=6 red; RestoreDC.
  - Call draw_fn.
  - Repeat the same MM_TEXT background+frame (different color) to prove post-state visibility.

- ChartData::print_data_to_dc (export flag path):
  - If export_mode:
    - Skip SetMapMode/Viewport/Window; compute DP from logical points with LPtoDP (under outer mapping) OR precompute DP with known rect math and draw in MM_TEXT.
    - Use width≥2 pens in EMF.

- ViewSpikeDetection::render_for_export:
  - Keep client-based union for bounds; pass sub-rects rebased to (0,0).
  - Ensure diagnostics draw BEFORE any chart code and again AFTER it.

### Notes on Viewer Behavior
- EMF viewers differ: CorelDraw often shows hairline issues and can cluster objects if world transforms are stacked unexpectedly. PowerPoint sometimes scales an EMF to fit placeholders; absolute page size is governed by HIMETRIC at creation and DPI assumptions.

### Open Questions
- Why diagnostic fills/frames don’t appear despite top-level mapping and permissive clipping:
  - Possibility: later code restores/clobbers DC state before recording completes or diagnostic block compiled out (configuration). We will move diagnostics straight into GraphicsExport and make them unconditional.

### Action Items (Next Session)
- [ ] Add unconditional pre/post-MM_TEXT diagnostic fills/frames in GraphicsExport::CopyAsEmfToClipboard.
- [ ] Add optional on-disk EMF save for debugging (CopyEnhMetaFile to %TEMP%).
- [ ] Implement export-only MM_TEXT path in ChartData::print_data_to_dc guarded by a flag.
- [ ] Re-test in Corel/PowerPoint; capture new traces and screenshots.

### Key Traces (for reference)
- copy_as_emf_to_clipboard: TargetPx=(1038,873)
- CopyAsEmfToClipboard: DPI=(96,96) HIMETRIC=(27464,23098) TargetPx=(1038,873)
- begin_aniso: WO=(0,0) WE=(1038,873) VO=(0,0) VE=(1038,873); CLIP=[0,0,1038,873]
- ChartData::print_data_to_dc (example sub-rect):
  - DR=[L=162 T=0 R=1038 B=352] -> WE=(876,352) VO=(162,176) VE=(876,-352); CLIP logical [0, +H/2]..[W, -H/2]

---
Prepared for next session; we will start at GraphicsExport to prove EMF page visibility independently, then constrain ChartData to a single mapping path for export.

