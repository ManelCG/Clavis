## [2.1.0-1] - 2026-06-23

- Add release tooling: deploy.sh, AUR packaging, changelog
- Add Windows and macOS support; i18n StrArg refactor
- Settings window
- Fix password security issues
- Fix git errors
- Export gpg keys
- Recursive search style
- git
- Import clav workflow and bugfixes
- Export clav workflow
- Recursive search
- Bug fixes and hotkeys
- asd
- Fixed timeout dispatcher use after free
- MacOS dmg creation
- MacOS .app bundle done
- Style enhancements
- Fix pinentry on MacOS
- MacOS definitions
- Merge branch 'develop' of github.com:ManelCG/Clavis into develop
- nproc
- Dep
- Mac support
- Merge branch 'develop' of github.com:ManelCG/Clavis into develop
- Mac support
- Fixed script
- Make with target
- Fix archlinux build
- Fix
- Empty commit for testing
- Submodules
- Better styleable entries!
- Escape to close all palettes. Escape for no and Return for yes in dual choice palettes
- QoL
- Improved Ubuntu install

# Changelog

All notable changes to Clavis are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and the project uses `version-pkgrel` release tags (e.g. `v2.1.0-1`).

## [2.1.0-1] - 2026-06-23

### Added
- Windows support: bundled GnuPG binaries and runtime DLLs, automatic GnuPG home,
  engine and pinentry (loopback) configuration, and a Windows installer build via
  `./make.sh windows-installer` (MSYS2 + Inno Setup 6).
- macOS support: `.app` bundle and `.dmg` packaging, with pinentry-mac configuration.
- Import and export of the full Clavis workflow (passwords and configuration).
- Export of GPG keys, including from the first-run wizard.
- Recursive search across the password store.
- Keyboard shortcuts and assorted quality-of-life improvements.
- Settings window.
- Email validation when creating a new GPG key.

### Changed
- Internationalization: the `_()` translation macro now accepts
  `std::filesystem::path` and string arguments through a new `StrArg` type.
- More styleable password entries; Escape closes palettes and Enter/Escape map to
  yes/no in confirmation dialogs.
- Improved Ubuntu installation flow and dependency setup.

### Fixed
- Password security hardening.
- Git synchronization errors.
- Use-after-free in the GUI timeout dispatcher.
- Windows process spawning: arguments are now quoted and child consoles are hidden.
- Renamed the `ERROR` state and error enums (`ERROR_STATE`,
  `CLAVIS_ERROR_FILE_NOT_FOUND`) to avoid clashing with the Windows `ERROR` macro.

[2.1.0-1]: https://github.com/ManelCG/clavis/releases/tag/v2.1.0-1
