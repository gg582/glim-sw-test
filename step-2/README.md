# MFC Dialog-based Circle Drawer

An MFC Dialog application that draws a circle passing through three user-defined points.
All rendering is performed via pixel-level algorithms (Bresenham) without using
high-level GDI functions such as `Ellipse`, `FillPolygon`, or `DrawPolygon`.

## Features

- **Click to define points**: Up to 3 click points are drawn as filled squares.
- **Dynamic circle**: After the 3rd click, the unique circumcircle is computed and drawn.
- **Draggable points**: Grab any click point and drag to move it; the circle updates in real time.
- **Customizable**: Click radius and edge thickness are configurable via edit boxes.
- **Reset**: Clears all points and the circle.
- **Random Move**: Moves all 3 points to random positions 10 times (2 Hz) on a background thread.

## Requirements

- Windows OS
- Visual Studio 2022 (or compatible) with MFC support
- CMake 3.14+

## Build

Open a **Developer Command Prompt for VS 2022** and run:

```powershell
cmake -B build -S . -G "Visual Studio 17 2022"
cmake --build build --config Release
```

## Run

```powershell
.\build\Release\CircleApp.exe
```

## Controls

| Control | Description |
|---------|-------------|
| **Click Radius** | Radius of the click-point marker |
| **Edge Thickness** | Thickness of the circumcircle outline |
| **Reset** | Clear all drawings |
| **Random Move** | Animate points randomly (background thread) |

## Implementation Notes

- **Bresenham's circle algorithm** is used for the circumcircle outline.
- **3-point circle geometry** solves for the center and radius using perpendicular bisectors.
- **Background thread** for random animation ensures the UI remains responsive.
- All comments are written in English.
