### Practical EMF requirements for CorelDRAW and PowerPoint

- **Use EMF (not only EMF+)**
  - Prefer classic GDI EMF records; EMF+ may rasterize or degrade in PowerPoint. If you use GDI+, also emit fallback GDI where possible.

- **Create the metafile with a correct frame**
  - Use `CreateEnhMetaFile` with a non-null reference HDC and a valid FRAME rectangle in 0.01 mm units.
  - Match the FRAME aspect to your intended page size; incorrect bounds cause scaling in PowerPoint.

- **Set a deterministic mapping early**
  - `SetMapMode(MM_ANISOTROPIC)` with `SetWindowExt/SetViewportExt` before any drawing.
  - Avoid changing map mode mid-stream; PPT sometimes ignores late transforms.

- **Pen and stroke visibility**
  - Avoid hairline pens; use `PS_SOLID` with width ≥ 1 device pixel equivalent in logical units.
  - Specify joins/caps explicitly via `ExtCreatePen` (miter limit ~4–10) to match CorelDRAW and PPT rendering.

- **Clipping**
  - Clear clip with `SelectClipRgn(NULL)` and set one logical clip region if needed; nested complex clips can be misinterpreted.
  - Intersect only once after mapping is established.

- **Fills and transparency**
  - EMF (GDI) alpha is limited; prefer solid fills or EMF gradient primitives (`EMR_GRADIENTFILL`).
  - For semi-transparent effects, consider flattening to a high-DPI bitmap.

- **Bezier/paths vs polylines**
  - Use `PolyBezier`, `PolyPolyline` for smooth geometry; avoid excessively fragmented paths.
  - Keep path segments under a few thousand points to prevent importer slowdowns.

- **Text fidelity**
  - Create TrueType fonts with explicit charset and height in logical units; set `GM_ADVANCED` if using rotated text.
  - For guaranteed appearance, convert critical text to outlines (paths); CorelDRAW imports text well, PowerPoint may substitute fonts.

- **Colors and palettes**
  - Use RGB colors; avoid palette-indexed brushes. Ensure no device-dependent palette reliance.

- **Bitmaps/rasters**
  - When embedding images, use `StretchDIBits` with DIBSECTION, 24/32bpp; set correct DPI in the metafile frame for crisp scaling in PPT.

- **Transforms**
  - Prefer world transformations encoded via mapping mode and extents rather than `ModifyWorldTransform`; PPT often ignores complex world transforms.

- **Arcs/ellipses**
  - Use `Ellipse`, `ArcTo` consistently; mixing start/end directions can import flipped. Test in both apps.

- **Header consistency**
  - Ensure EMF header bounds match the draw area; keep all drawing within bounds to avoid clipping on import.

- **Clipboard vs file**
  - Clipboard: use `SetClipboardData(CF_ENHMETAFILE, hEmf)`; include a small `CF_DIB` preview for apps that show thumbnails.
  - File: save `.emf` from `CloseEnhMetaFile`; verify size/aspect in both apps.

- **PowerPoint-specific quirks**
  - Tends to thin 1px strokes; use ≥ 1.2 px equivalent.
  - May ignore late map-mode changes and some world transforms; set mapping first and draw afterwards.
  - Group complex drawings logically; PPT ungrouping works best with simpler path sets.

- **CorelDRAW-specific notes**
  - Imports EMF geometry richly; supports joins/caps, gradients, Beziers.
  - If appearance deviates, use outlines for text and expand complex brushes.

- **Diagnostics to bake in**
  - Log EMF DPI, frame (0.01 mm), map mode, window/viewport extents, and clip box.
  - Test: filled bands plus framed rectangles in both logical and `MM_TEXT` device space to catch visibility issues.

- **Fallback strategy**
  - If a feature doesn’t round-trip cleanly, provide an alternate EMF export mode: “vector-pure” (no alpha, solid pens) and “raster-safe” (flatten effects to bitmap within the correct frame).

Summary:
- Set `MM_ANISOTROPIC` early, provide a correct EMF frame, avoid hairlines/alpha, and keep geometry/text device-independent. PowerPoint is stricter about transforms and stroke widths; CorelDRAW is more forgiving but benefits from explicit joins/caps and outlines for critical text.



