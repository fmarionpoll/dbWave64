### DBVIEW REFACTORING AND TESTING SUGGESTIONS

Scope: `dbWave64/dbView` — `ViewdbWave`, `DataListCtrl`, `DataListCtrl_Row`.

### Key Risks and Pain Points
- **Tight coupling to parent/document**: `DataListCtrl` directly casts parent to `ViewdbWave` and manipulates the document’s current record, coupling UI, navigation, and data access.
- **Raw owning pointers and shallow copy**: `DataListCtrl_Row` owns multiple raw pointers and defines a copy-assignment that shallow-copies them (double-delete risk).
- **GDI resource handling**: DCs and selected GDI objects aren’t consistently released; offscreen rendering relies on hidden child windows per row.
- **Mixed responsibilities**: Row cache management, DB access, rendering, and UI live together, making changes risky and tests hard.
- **Magic numbers and flags**: Display modes, transform modes, and spike plot modes use integers; intent isn’t self-documenting.
- **Serialization hazards**: UI pointers are serialized/used without clear construction guarantees.

### Specific Findings (code citations)
```185:232:dbWave64/dbView/DataListCtrl.cpp
// ...
const auto pdb_doc = static_cast<ViewdbWave*>(GetParent())->GetDocument();
// ...
```
```60:66:dbWave64/dbView/DataListCtrl_Row.cpp
// shallow copy includes owning pointers
p_chart_data_wnd = arg.p_chart_data_wnd;
p_chart_spike_wnd = arg.p_chart_spike_wnd;
p_data_doc = arg.p_data_doc;
p_spike_doc = arg.p_spike_doc;
```
```133:140:dbWave64/dbView/DataListCtrl_Row.cpp
// Deserialize calls through pointers that may be null
p_chart_data_wnd->Serialize(ar);
p_chart_spike_wnd->Serialize(ar);
```
```252:266:dbWave64/dbView/DataListCtrl_Row.cpp
// GetDC without ReleaseDC; similar in spikes path
const auto p_dc = p_chart_data_wnd->GetDC();
CDC mem_dc;
VERIFY(mem_dc.CreateCompatibleDC(p_dc));
// ...
```
```364:372:dbWave64/dbView/DataListCtrl.cpp
// update_cache temporarily changes document state
BOOL b_success = db_wave_doc->db_set_current_record_position(index_current_file);
```

### Refactoring Recommendations
1) Decouple UI from data/document
   - Introduce an interface `IDbWaveDataProvider` consumed by `DataListCtrl` with methods:
     - `int getRecordsCount()`
     - `RowMeta getRowMeta(int index)` (fields for insect id, stimuli, spike counts, etc.)
     - `AcqDataRef openData(int index)` (lightweight handle; no global doc mutation)
     - `SpikeDataRef openSpikes(int index)`
   - Pass the provider (and `DisplaySettings`) into `DataListCtrl::init(...)`. Remove direct `static_cast<ViewdbWave*>` usage and any document state mutations.

2) Separate cache mechanics from UI
   - Extract a non-MFC `RowCache` that owns a vector of row view-models and implements:
     - `ensureRange(first, last)` (build/shift logic currently in `update_cache`, `cache_shift_rows_positions`, `cache_build_rows`)
     - Boundary correction currently in `cache_adjust_boundaries`.
   - `DataListCtrl` becomes an adapter: forwards visible range changes to `RowCache` and pushes images/texts to the control.

3) Replace per-row hidden windows with offscreen renderers
   - Create `IDataRenderer` and `ISpikeRenderer` that render to an offscreen bitmap (HBITMAP or DIB section) without owning a window. Methods return a bitmap handle/buffer that `DataListCtrl` places into the `CImageList`.
   - Benefits: no HWND per row, simpler lifecycle, deterministic unit tests.

4) Strong types for modes/settings
   - Replace ints with scoped enums:
     - `enum class DisplayMode { None=0, Data=1, Spikes=2 }`
     - `enum class SpikePlotMode { AllClasses=0, OneClass=1 }`
     - `enum class DataTransform { None=0, MedianFilter=13 }`
   - Rename `DataListCtrlInfos` to `DisplaySettings` and make fields const where possible. Pass by const reference.

5) Make `DataListCtrl_Row` a pure data carrier
   - Remove ownership of UI objects (`ChartData*`, `ChartSpikeBar*`), and remove serialization of UI state. Keep only row metadata and file paths.
   - Delete copy/move as needed or use value semantics with no owning raw pointers.

6) RAII and resource safety
   - Use guards for DCs and selected GDI objects; always `ReleaseDC` and re-select previous objects.
   - Prefer `std::unique_ptr` for any remaining heap objects.

7) Persisted layout consolidation
   - Replace scattered static arrays with a single `Columns` descriptor (header, format, initial width). Load/save widths through a single helper.

8) Event flow simplification
   - `DataListCtrl` should expose callbacks (std::function) for navigation requests instead of sending parent window messages directly.
   - Keep sorting decisions in `ViewdbWave`, but move “SQL column name resolution” into a reusable map.

9) Error handling and logging
   - Replace `ASSERT`-only paths for file failures with readable error states and testable outcomes (e.g., explicit “data unavailable” rendering).

### Testability Plan (GoogleTest)
- Unit tests (headless, no MFC window):
  - `RowCacheTests`: window size changes, scroll up/down, bounds correction, reuse vs. rebuild counts.
  - `DisplaySettingsTests`: enum conversions, validation of time/span logic.
  - `RendererTests`: given stub `AcqDataRef`/`SpikeDataRef`, verify produced bitmaps’ sizes and basic content (non-empty vs. empty, class filtering).
  - `RowMetaMappingTests`: DB fields mapped correctly to displayed strings (insect id, stimuli, concentrations, spike counts).
- Fakes/mocks:
  - `FakeDbWaveDataProvider`: returns fixed-size datasets; files optional.
  - `MockImageListAdapter`: records Replace/Add calls and indices for verification (similar to existing CGraphImageList tests).
- Light integration tests:
  - `DataListCtrl` with fake provider and renderer: ensure `LVN_GETDISPINFO` path populates text and images for a small dataset without touching real docs.

### Incremental Adoption Strategy
1. Introduce `DisplaySettings` and enums; adapt callers with minimal change.
2. Add `RowCache` and redirect current cache logic; leave rendering as-is temporarily.
3. Introduce provider and renderer interfaces; add adapter implementations that wrap current `CdbWaveDoc`, `ChartData`, and `ChartSpikeBar`.
4. Switch `DataListCtrl_Row` to pure data; migrate serialization accordingly.
5. Remove hidden per-row windows; use offscreen renderer.
6. Add unit tests at each step to lock behavior and prevent regressions.

### Success Criteria
- No regressions in paging, selection, sorting, and rendering.
- Headless unit tests cover cache behavior and rendering decisions.
- GDI/resource leaks eliminated (verified via debug CRT or tooling).
- Measurable reduction in coupling (no direct `static_cast<ViewdbWave*>` in `DataListCtrl`).

### Notes
- Keep `boolean` usage consistent with existing codebase unless a broader rename is planned.
- Use adapter layers to avoid churn in existing `ChartData`/`ChartSpikeBar` until offscreen rendering is ready.


