# Idle Overlay

Small Windows app that shows a full-screen black overlay after the user has been idle for a configurable time. Compatible with OLED screen the most. 

## Build
```powershell
mkdir build
cd build
cmake ..
cmake --build .
```

## Run
```powershell
./idleScreen.exe [idle_timeout_ms]
```
`idle_timeout_ms` is optional. Defaults to 60000 (1 minute).

## Todo List
- [ ]Add a tray icon for quick enable/disable and timeout changes.
- [ ]Persist settings in the registry or a small config file.
- [ ]Add a small GUI for configuration and diagnostics.

## Why?

I made this project because Windows can't provide me a stable way turning off the screen while not making my computer sleep. **** you Microsoft.