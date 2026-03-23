# Idle Overlay

Small Windows app that shows a fullscreen black overlay after the user has been idled for a configurable time. Compatible with OLED screen the most. 

It's finally usable 

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
- [x] Add a tray icon for quick enable/disable and timeout changes.
- [ ] Persist settings in the registry or a small config file.

## Why?

I made this project because Windows can't provide me a stable way turning off the screen 
while not making my computer sleep. **** you Macrohard.

## Knowledge

This is just a practice project for me. I will try implement full function of displaying
black window at top after specific time idling under 2 platforms (should be 3, but I don't
have a mac) and adapt it into different WM in linux. 