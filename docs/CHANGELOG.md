# Changelog

## 2026-11-21 (second update) - Reference firmware and build guide

### Added
- `firmware/FireFightingRobot/` (`.ino`, `config.h`, `secrets.h.example`): new reference firmware, a non-blocking state machine. Compiles for the Uno (AVR GCC 7.3.0 + Arduino AVR core 1.8.6, `-Wall -Wextra`, no warnings): 10,306 B flash, 471 B RAM.
- `firmware/BringUpTest/BringUpTest.ino`: serial-menu hardware check (compiles: 7,058 B flash).
- `docs/IMPLEMENTATION_GUIDE.md`: parts, power plan, wiring, bring-up, tuning, system tests, Proteus, troubleshooting.
- `docs/diagrams/`: firmware state machine, reference wiring, power architecture (Mermaid syntax validated; not rendered).
- `LICENSE` (MIT, covering new code and docs; the report is excluded).
- Proposed pin map (table B) in `hardware/pin_mapping.md`.

### Changed
- README, `SOFTWARE_DOCUMENTATION.md`, `firmware/README.md`, limitations, testing, portfolio and interview docs updated to describe the reference firmware as new, compile-checked and not hardware-tested.
- `.gitignore` now ignores `firmware/**/secrets.h`.

### Not done
- No hardware testing; all tests remain Pending.
- The original firmware, pin map and part models are still unknown.

## 2026-11-21 – Portfolio upgrade (branch `professional-portfolio-upgrade`)

### Added
- `docs/PROJECT_AUDIT.md`: hardware/software inventory, evidence status, inconsistencies, missing information.
- `docs/SYSTEM_OVERVIEW.md`, `HARDWARE_DOCUMENTATION.md`, `SOFTWARE_DOCUMENTATION.md`, `LIMITATIONS.md`.
- `docs/TESTING_AND_VALIDATION.md`: 15 test cases, all Pending.
- `docs/PORTFOLIO_TEXT.md`, `docs/INTERVIEW_PREP.md`.
- `docs/diagrams/`: block diagram, flowchart and architecture diagram in Mermaid.
- `hardware/BOM.md`, `hardware/pin_mapping.md` (no pins verified).
- `firmware/README.md`, `simulation/README.md`, `media/README.md` (documenting what is missing).
- `media/images/`: three technical figures extracted from the report.
- `.gitignore`.
- New `README.md`.

### Changed
- Moved `Mini-Project_Report_23-24[1].docx` to `report/original_project_report.docx` (renamed with `git mv`; content unchanged).

### Not done (deliberately)
- No firmware or reference code was written (pins and models unverified).
- No `LICENSE` added; the choice and the permissions of the three co-authors are pending.
- No hardware photos or videos; none were available.
