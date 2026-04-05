# Idle Overlay

Small Windows app that shows a fullscreen black overlay after the user has been idled for a
configurable time. Compatible with OLED screen the most. 


## Build
```powershell
mkdir build
cd build
cmake ..
cmake --build .
```

## Run
```powershell
./idleScreen.exe [idle_timeout_min]
```
`idle_timeout_min` is optional. Defaults to 1 minute.

## Todo List
- [x] Add a tray icon for quick enable/disable and timeout changes.
- [ ] Implement Linux version

## Why?

I made this project because Windows can't provide me a stable way turning off the screen 
while not making my computer sleep. **** you Macrohard.

## Knowledge

This is just a practice project for me. I will try implement full function of displaying
black window at top after specific time idling under 2 platforms (should be 3, but I don't
have a mac) and adapt it into different WM in linux. 

Windows version is completed (hope so.) Unless I want to figure out how to actually turn
off the monitor, or display some customizable pictures instead of whole black, there will
be no major updates.