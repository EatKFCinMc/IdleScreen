# Black Screen Idle Watcher

Small Windows app that shows a full-screen black overlay after the user has been idle for a configurable duration. Built with CMake and Win32 APIs; designed to stay tiny while leaving room to grow (tray icon, service, etc.).

## Build
```powershell
cmake -S . -B build -G "Ninja"
cmake --build build --config Release
```
You can swap `Ninja` for another generator you prefer (e.g., "Visual Studio 17 2022").

## Run
```powershell
./build/Release/BlackScreenIdleWatcher.exe [idle_timeout_ms]
```
`idle_timeout_ms` is optional; defaults to 60000 (1 minute).

## Todo List
- [ ]Add a tray icon for quick enable/disable and timeout changes.
- [ ]Persist settings in the registry or a small config file.
- [ ]Add a small GUI for configuration and diagnostics.
