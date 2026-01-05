## Mailbox Polling Implementation

### Plan (from `chart.plan.md`)
1. Review Existing Plan & Interfaces — Reconcile `DATAACQ_MAILBOX_PLAN.MD`, confirm `CViewAcqDat` responsibilities, and catalog existing importer hooks in dbWave64.
2. Define View-Level Workflow — Specify `CViewAcqDat` UI (mailbox directory picker, poll interval input, run/stop button) and how it orchestrates polling state.
3. Design Polling & Processing Pipeline — Detail how dbWave64 locates the mailbox file, parses queued filenames, invokes the import routine, and cleans up entries.
4. Plan Robustness & Error Handling — Cover partial writes, duplicate entries, missing files, log/report behavior, and user feedback within the view.
5. Outline Implementation Tasks — Enumerate concrete code changes (view updates, background timer/task, importer wrapper, configuration persistence, tests).
6. Identify Verification Steps — Define manual/automated checks to confirm end-to-end mailbox mirroring and UI controls function correctly.

### Progress Summary
- Implemented a new `CViewAcqDat` form layout: added directory picker, mailbox filename input, poll interval with spin control, and start/stop buttons; status group now displays live messages.
- Persisted mailbox settings (directory, file name, poll interval) using `RegistryManager`; values reload on view initialization and update on change.
- Wired UI logic: controls enable/disable across Run/Stop states, auto-trim inputs, and default filename fallback.
- Added timer-driven polling loop that locks a mailbox file via `.processing` rename, reads entries, normalizes relative paths (Shlwapi), and delegates imports through `CdbWaveDoc::import_file_list`; integrates feedback via status text.
- Included fault handling for busy files, empty mailbox, missing database/document, and restore logic when reads fail; status messages timestamped for traceability.
- Introduced helper utilities (`load_settings`, `save_settings`, `update_controls`, `update_status`, `poll_mailbox_once`, `build_mailbox_file_path`, `start_timer`, `stop_timer`).
- Linked against `Shlwapi.lib` to use filesystem helpers (e.g., `PathCanonicalize`, `PathIsRelative`).

### Next Steps (Manual)
- Build `dbWave64` to ensure `Shlwapi` dependency links correctly.
- In the updated acquisition view, select a mailbox folder/file, start polling, and verify AcqDat64 drops are imported and the mailbox file cleared.
- Confirm state persistence across runs and resilience to malformed mailbox lines.



