# Changelog
All notable changes to this project will be documented in this file.

## Unreleased

### Fixed
- QML Connections deprecation warning (#123).
- Automatic profiles are not activated using the default wine packages in Pop!_OS (#161).

### Changed
- Require Qt 5.15
- Replace file headers license section with [SPDX unique license identifiers](https://spdx.dev/ids/).

### Added
- Implement resizable graph and sensors regions. Both regions are configurable by dragging their split handles and the sizes persist between sessions (#309).


## CoreCtrl 1.3.1 (2022-10-08)

### Fixed
- Incorrect alignment of state voltage labels when automatic voltage is selected on the frequency & voltage control.
- CPU performance scaling mode not being restored from file (#322).
- Compilation issues with Linux 6.0 API headers (#325).

### Changed
- Explicitly require Qt 5.9 (or compatible versions).
- Install application binary and library files into GNU standard installation directories (#324).


## CoreCtrl 1.3.0 (2022-09-18)

### Fixed
- Profiles not loading consistently between reboots when using multiple GPUs (#300). If you are affected by this issue and your GPUs supports unique IDs (Vega and later models), re-save your profiles. More info on the [Known Issues](https://gitlab.com/corectrl/corectrl/-/wikis/Known-issues#profiles-reset-randomly-between-reboots-when-using-multiple-gpus) Wiki page.
- Active profile is not deactivated after changing its name or executable name.
- Profile icons and other displayed images not preserving their aspect ratio (#310).
- Crash on application exit when running under some window managers (like IceWM).
- Crash when applying specific fan curves in some circumstances (#164). Now, only constant and ascending curves can be created and used as fan curves.
- `corectrl_helper` blocking when running on kernels in which the option `CONFIG_PROC_EVENTS` has not been set (#71). The automatic profiles won't work when running on such kernels, though.

### Changed
- Replace KAuth dependency with Polkit, D-Bus and Qt5::DBus.
- Replace KArchive dependency with QuaZip.
- Update Russian translation. Thanks to OlesyaGerasimenko (!35).

### Added
- Display the GPU unique ID in the GPU info tab. Available on Vega and later GPU models.
- Make custom profile icons optional in the profile dialog. The user can now select a custom icon or simply use the default one in their profiles (#127).
- Manual profiles. The user can now create profiles to partially or completely override  the settings applied by the global and per-application profiles. They can be toggled from the system tray or through the main GUI (#49).
- System tray menu entry to hide and show the main window (#280).
- New command line options:
  - `--minimize-systray`: starts the application minimized either to the system tray (when available) or to the taskbar. When an instance of the application is already running, the action will be applied to its main window (#78).
  - `--toggle-window-visibility`: when an instance of the application is already running, it will toggle the main window visibility showing or minimizing it, either to the taskbar or to system tray.
  - `m`, `--toggle-manual-profile`: when an instance of the application is already running, it will toggle the given manual profile (#224).
- Option to save the window geometry (#312). The saved geometry is always restored at application startup. Restoring some window properties, such as window position, can be unreliable under Wayland.
- German translation. Thanks to Rüdiger Arp, polyphase and SE. (#104).


## CoreCtrl 1.2.7 (2022-08-09)

### Fixed
- Fix power profiles parsing on sienna cichlid asics (#305).

### Added
- Workaround for a [long-standing driver bug](https://gitlab.freedesktop.org/drm/amd/-/issues/1706) which affects the RX6000 series. It allows the user to control the maximum memory frequency when using these cards. Turned out that this was not the cause of the low memory frequency issue (#304), so it should be safe to restore this functionality.


## CoreCtrl 1.2.6 (2022-08-06)

### Removed
- Workaround for a [long-standing driver bug](https://gitlab.freedesktop.org/drm/amd/-/issues/1706) affecting the RX6000 series. This workaround allowed the user to control the maximum memory frequency on those models. Unfortunately, this functionality triggers another memory related driver bug that locks the memory frequency to a low value (#304).


## CoreCtrl 1.2.5 (2022-08-02)

### Added
- Add a workaround for a [long-standing driver bug](https://gitlab.freedesktop.org/drm/amd/-/issues/1706) which affects the RX6000 series. It allows the user to control the maximum memory frequency when using these cards.
- Support power profile mode on asics without heuristics settings (#303).


## CoreCtrl 1.2.4 (2022-07-17)

### Fixed
- Fix clangd not picking the project C++ standard level.
- Fix deprecated calls to QProcess::start.
- Fix warning on FindBotan.cmake.
- Explicitly disable QApplication quit on last window closed.
- Fix voltage offset not being restored during the initialization phase.

### Added
- Add support for unix paths to .exe files on wine launch cmdline.
- Add Czech translation. Thanks to viktorp (!32).
- Add Dutch translation. Thanks to Heimen Stoffels (!33).


## CoreCtrl 1.2.3 (2022-02-23)

### Fixed
- Use QObject parent/child ownership to manage system tray icon lifetime (#76).
- Parse kernel versions without patch number (#254).


## CoreCtrl 1.2.2 (2021-11-14)

### Fixed
- Various fixes for some wine apps not being recognized when launched:
  - Using custom compatibility tools on Steam.
  - Having uppercase extensions and launched from a case sensitive file system.
- Fixed missing profile icon after editing the profile executable name.
- Fixed profile not being monitored after editing the profile executable name.

### Changed
- Session code improvements.


## CoreCtrl 1.2.1 (2021-09-19)

### Fixed
- Use tension instead of voltage on french translation. Thanks to DarkeoX ABYSSION Tar Valantinir (!31).
- Fixed system hang on some models of RX 6000 XT series (#213).
- Fixed ignored disabled states on overdrive frequency + voltage controls (#218).
- Rework command generation and handling. The number of submitted commands has been reduced and now commands writing to different files won't be interleaved, thus improving the stability of the system. The previous implementation might trigger unexpected driver behavior and bugs that might hang the system, as reported on both #217 and #218.

### Changed
- Rework driver quirks handling. Now, it's done in a more granular way, per overdrive control. As a consequence of this, the fallback fixed frequency control is now only used when `pp_od_clk_voltage` is missing or empty (typically, when the user has not set `amdgpu.ppfeaturemask`).
- Reduce the amount of events sent to the process monitor. Process thread events are now filtered out.


## CoreCtrl 1.2.0 (2021-09-05)

### Added
- Added GPU memory and junction sensors (#36).
- Added GPU voltage sensor.
- Added support for voltage offset (#139). With this RX 6XXX are fully supported. Use with caution, as no voltage range is provided by the driver.
- Added noop control. This control can be used to hand over control of certain components to other programs. More info in the Wiki: [Do not control mode](https://gitlab.com/corectrl/corectrl/-/wikis/How-profiles-works#do-not-control-mode), [Controlling parts of your system with external applications](https://gitlab.com/corectrl/corectrl/-/wikis/How-profiles-works#controlling-parts-of-your-system-with-external-applications).
- Modularized overdrive controls. This adds clock controls to RX 6XXX (#139).

### Changed
- Sensor color is no longer stored in profiles.
- Group sensor's colors by type.


## CoreCtrl 1.1.5 (2021-08-22)

### Fixed
- Fixed wrong VRAM usage shown (#92). Thanks to Dmitry Fateev (!30).
- Fixed incorrect amount of VRAM detected (#83). GPU video ram is now read from ioctl.
- Fixed advanced performance settings not being cleaned when switching to other performance modes (#204).

### Changed
- Minor code refactorings.
- Move translations and QT_STYLE_OVERRIDE messages to INFO level.


## CoreCtrl 1.1.4 (2021-07-25)

### Changed
- Code cleanups.
- Remove unsafe flags on arch installation instructions (#181).

### Added
- Rework AMD fan sensors creation logic (#184).


## CoreCtrl 1.1.3 (2021-05-09)

### Fixed
- Fix tests compilation (#178).

### Added
- Make path to pci.ids configurable at build time using -DWITH_PCI_IDS_PATH=<path-to-pci.ids-file>. Thanks to Alexander Kapshuna (!26).


## CoreCtrl 1.1.2 (2021-05-02)

### Fixed
- Fixed theme by ignoring QT_STYLE_OVERRIDE environment variable (#106).
- Fixed header width of mode selector controls (#176).

### Changed
- Use KDEInstallDirs instead of hardcoded paths to install resources. Thanks to Alexander Kapshuna (!27).


## CoreCtrl 1.1.1 (2020-08-19)

### Fixed
- Fixed kernel version parsing. Thanks to Norbert Preining (#82).
- Added svg Qt component as required dependency (#61).
- Fixed incorrect executable file name on profile dialog (#94).
- Added a workaround for bogus values on critical temperature (#103).
- Fixed settings are not being applied with some wine executables (#121).

### Added
- Added Bulgarian translation. Thanks to Xrey274 (!17).
- Added Russian translation to launcher.desktop comment. Thanks to Harry Kane (!24).


## CoreCtrl 1.1.0 (2020-05-31)

### Fixed
- Fixed compilation with newer versions of gcc (#54, #62).
- Reworked vega20 (navi... and newer hardware) advanced power management controls (#37).

### Added
- Added French translation. Thanks to DarkeoX ABYSSION Tar Valantinir (#30).
- Added Catalan translation. Thanks to bla6 (#72).
- Added workaround for high cpu load on some hardware (#29).
- Added navi support (#41).


## CoreCtrl 1.0.9 (2020-04-19)

### Fixed
- Fix compilation on some systems (#62).


## CoreCtrl 1.0.8 (2020-03-15)

### Fixed
- Fix compilation with gcc 10 (#54).


## CoreCtrl 1.0.7 (2019-11-23)

### Fixed
- Missing character on profile menu icon.
- Skip empty GPU info vendor, device and subdevice names.
- Add deferred system tray icon support (#43).

### Changed
- Code cleanups.
- Removed Patreon info.


## CoreCtrl 1.0.6 (2019-08-08)

### Fixed
- Missing controls when using Qt 5.9 (#17).
- UI performance issues (#28).

### Changed
- Update russian translation. Thanks to Schwonder Reismus (!13).
- Do not create controls and sensors when data sources have invalid data.
- Check and try to fix the state of some controls and all profiles after importing wrong values.
- Fan curve control scale the curve into valid range.
- Reduce CPU usage when the sensors graph is not visible.

### Added
- Restore previous hardware state after init.


## CoreCtrl 1.0.5 (2019-07-22)

### Fixed
- Remove qca build dependency.
- Fix wrong icon size. If you installed CoreCtrol from source, you may want to uninstall it before instaling this version. This will remove no longer used files from your computer. You can also remove them manually:

  ```
  sudo rm /usr/share/applications/CoreCtrl.desktop /usr/share/icons/hicolor/196x196/apps/corectrl.svg
  ```

  If `/usr/share/icons/hicolor/196x196` directory only contains `apps/corectrl.svg` remove it with:

  ```
  sudo rm /usr/share/applications/CoreCtrl.desktop
  sudo rm -rf /usr/share/icons/hicolor/196x196
  ```

### Added
- Add Russian translation.
- Add AppStream metadata file.

### Changed
- Improve default cpu frequency scaling governor selection.
- Follow freedesktop naming conventions for the .desktop file.


## CoreCtrl 1.0.4 (2019-07-17)

### Fixed
- Removed an unused qml module import.

### Changed
- CoreCtrl now does not start minimized on system tray by default.


## CoreCtrl 1.0.3 (2019-07-15)

### Fixed
- Fixed application crash when using Qt 5.9.

### Added
- Add uninstall target to CMake.


## CoreCtrl 1.0.2 (2019-07-15)

### Fixed
- Require CMake version 3.12 instead 3.3.
- Launcher doesn't show its icon due to installed icons having a wrong name.


## CoreCtrl 1.0.1 (2019-07-14)

### Fixed
- Compilation failed due to a wrong symbolic link.
- Some typos.


## CoreCtrl 1.0.0 (2019-07-14)

### Added
- Automatic application of profiles for native and Windows programs.
- Full AMD GPUs controls (for both old and new models).
- Basic CPU controls.
