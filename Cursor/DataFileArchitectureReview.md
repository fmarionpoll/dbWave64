## Data File Architecture Review

### Loader Pattern
- `CDataFileX` defines a shared interface (open, sniff, stream) for data files. Derived classes override `check_file_type`, `read_data_infos`, and tag loaders to parse format-specific headers and populate `CWaveBuf`.
- `AcqDataDoc::open_acq_file` iterates DOCTYPEs but passes the loop index, so only AWAVE and ATLAB instantiate correctly; other formats fall back to `CDataFileX`, losing metadata.

### Risks & Limitations
- Header parsing relies on pointer reinterpretation, assumed struct packing, and manual endian fixes; fragile across compilers and hard to test.
- Buffer math and offsets still use 32-bit types; very large recordings risk overflow on x64.
- Minimal error handling in readers; partial failures can leave `x_file` inconsistent.
- Tight coupling to MFC (`CFile`, `CString`) and Win32 APIs makes cross-platform work difficult.

### Document & Buffer Cascade
- `AcqDataDoc` orchestrates one active reader, maintains streaming windows, and delegates data access to `CWaveBuf`.
- `CWaveBuf` bundles raw/transform buffers plus `CWaveFormat`, `CWaveChanArray`, and tags; lifetime managed via manual `malloc/free`.
- Collaboration is coherent but pointer-heavy; RAII and clearer buffer contracts would help maintainability.

### Modular Alternatives
- Introduce a registration/factory so readers self-register by format ID or sniffers; enables testing and future plugin loading.
- For DLL/so plugins, expose a small C ABI (`create_reader`, `get_supported_doctypes`) and load dynamically per platform.
- Replace MFC-specific types with standard equivalents or wrap them to ease portability.

### Recommendations
1. Fix reader instantiation (`instantiate_data_file_object(data_types_array[id])`) and add regression tests per format.
2. Centralize format detection via a registry/factory; let each reader implement a `bool sniff(Stream&)` to indicate support.
3. Adopt RAII (`std::unique_ptr`, `std::vector`) for readers and buffers; encapsulate buffer sizing logic.
4. Promote counters/offsets to 64-bit, consolidate endian handling, and validate read sizes.
5. Map a path toward platform-neutral I/O (standard library, filesystem) if cross-platform support is desired.

### Next Steps
- Implement the instantiation fix and unit tests for at least AWAVE, ATLAB, ASD, MCID sample files.
- Prototype the reader registry in a branch; measure impact on existing code.
- Audit large-file behavior under x64 builds and adjust buffer math as needed.
- Outline dependencies on MFC/Win32 as preparation for portability work.
