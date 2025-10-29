## Chart Display Review

- **Inheritance & Responsibilities**
  - `ChartWnd` provides the MFC window shell: cursor management, drawing surface prep, ruler attachment, and message reflection hooks for derived charts. Its virtuals (`plot_data_to_dc`, `zoom_data`, export helpers, physical extent queries) define the extensibility points for specialized visualizations.
  - `ChartData` overrides the rendering/export hooks to translate acquisition buffers into polylines, manages channel lists, scaling, zooming, and clipboard export. It layers data access on top of the base UI cycle and adds event handlers for tags and curve tracking.
  - `ChartDataAD` extends `ChartData` for streaming A/D buffers (triggered capture) by slicing incoming sample blocks across the chart surface, optionally via an off-screen DIB.
  - No `ChartSpike*` classes exist in the current tree; legacy references point to removed spike tooling and would reappear as additional `ChartWnd` derivatives if restored.

- **Supporting Types & Collaboration**
  - `CChanlistItem` couples an abscissa envelope with an ordinate envelope plus display metadata (gain, offsets, pens, comments). `ChartData` owns an array of these to draw each trace, update gain, and serialize settings.
  - `CEnvelope` stores max/min (or raw) samples per pixel interval; it supplies export helpers so `ChartData` can transform the envelope into device coordinates and supports serialization of processed data.
  - `Scale` maps document indices to pixel buckets via `CWordArray`/`CDWordArray`, enabling consistent zoom/scroll calculations across the chart.
  - `Ruler` models numeric axes; `ChartWnd` hosts embedded rulers and can delegate to `RulerBar` windows. `RulerBar` attaches to a `ChartData` window and forwards zoom gestures back through `ChartWnd::zoom_data`.
  - Tagging (`TagList` within `ChartWnd`) and ruler updates (`update_x_ruler`, `update_y_ruler`) integrate with channel data to support cursors and overlays.

- **x64 Readiness Observations**
  - Indexing relies heavily on 32-bit `long` for sample positions, file offsets, and paging. On x64, `long` stays 32-bit; acquisitions exceeding ~2 billion samples will overflow, truncate tag positions, or break zoom math.
  - Several notifications compress values with `MAKELONG`, limiting payloads to 16-bit halves—fine for control IDs but unsafe for larger data.
  - `Scale` stores interval widths in a `CWordArray`, so buckets wider than 65,535 samples wrap; combined with 32-bit totals, very long recordings risk mis-scaling in 64-bit builds.
  - Pointer/handle usage follows MFC idioms with balanced GDI resource lifetimes. Aside from data-size assumptions, no x64-specific API incompatibilities surfaced.

## Next Steps

- Promote offsets/counts to 64-bit types (and update serialization/message packing) if datasets can exceed 32-bit ranges.
- Reintroduce spike-specific chart derivatives only if those features return.
- Revisit message payloads (e.g., custom structs or wider WPARAM/LPARAM usage) before finalizing x64 migration.
