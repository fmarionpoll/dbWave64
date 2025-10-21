## dbWave64 ⇄ dataAcq split: minimal mailbox protocol (portable)

### Current status
- Fixed version info bug by loading `VS_VERSION_INFO` from the running module in `DlgAbout.cpp`.
- Agreed to decouple acquisition into a separate `dataAcq` app.
- Keep existing acqdatadoc files; dbWave64 will reuse `ChildFrame::on_tools_import_data_files` to import.
- Minimal, portable handoff via a per‑session mailbox directory (polling 1 s).

### Session folder (all OS)
- `Documents/dbwave/acquisition/<YYYY>/<YYYY-MM-DD>/session-<uuid>/`

### Minimal messages (JSON sidecar files)
- Ready (one or many files):
  - Name: `<session>.ready.<seq>.notify` (write as `.partial`, then atomic rename)
  - Body: `{ "type":"ready", "session":"<uuid>", "sequence":<n>, "files":["rec_0001.acqdatadoc", ...], "created_utc":"ISO8601" }`
- Done:
  - Name: `<session>.end.notify`
  - Body: `{ "type":"done", "session":"<uuid>", "created_utc":"ISO8601" }`
- No “wait” message for now (can add later).

### Launch contract (CLI)
- Required: `--session <uuid> --session_dir <absolute_path>`
- Device/amp and detailed acquisition parameters are deferred; tool may use its own UI/defaults.

### dbWave64 behavior
- Discover or pick acquisition tool (persist the choice).
- On Acquire:
  - Create/select session folder and `session_id`.
  - Launch dataAcq with `--session` and `--session_dir`.
  - Open a non‑modal “Acquisition Monitor”.
- Monitor (1 s poll):
  - On each `.notify` of type `ready`, call the existing directory importer programmatically on the session folder, then archive/delete the `.notify`.
  - On `done` (or process exit), run a final scan, then stop.

### Where to hook in code
- `dbWave64\ChildFrame.cpp`
  - Replace `ID_VIEW_ACQUIRE_DATA` handler (currently `replaceview` → `ViewADcontinuous`) to:
    - Launch dataAcq (process handle kept),
    - Start the monitor,
    - Stop and finalize on exit/done.
- Reuse importer:
  - Factor a thin `ImportDirectorySilently(const CString& dir)` that invokes existing `on_tools_import_data_files` logic without UI.

### Data semantics
- Gain, filters, and other metadata remain in acqdatadoc headers (dbWave64 reads them on import). Notify files carry no domain details.

### Defaults (user‑space)
- Same structure on Windows/macOS/Linux (under the user’s Documents). Poll interval = 1 s (configurable later).

### Next steps (checklist)
- [ ] Implement cross‑OS session folder creation under Documents with unified naming
- [ ] Implement minimal dataAcq CLI (`--session`, `--session_dir`)
- [ ] Ensure writer uses `.partial` → final atomic rename (data files and `.notify`)
- [ ] Add NotifyWriter emitting `ready` (files[]) and `done`
- [ ] Add `ImportDirectorySilently(dir)` wrapper over existing importer
- [ ] Implement 1 s NotificationMonitor for the session folder
- [ ] Update `ID_VIEW_ACQUIRE_DATA` to launch tool and open monitor
- [ ] Persist base path and poll interval in settings
- [ ] Add simulated device for cross‑platform testing
- [ ] Test partial files, duplicates, crash recovery, large files

### Open items (later)
- Optional native file watchers (Win/macOS/Linux) behind abstraction
- Richer control/messaging (pause/wait), device/amp configuration via CLI

