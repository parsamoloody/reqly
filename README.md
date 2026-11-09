# Reqly

Reqly is a lightweight API client written in C using GTK 4.

## Current Milestone: Minimal Foundation

This milestone establishes the initial foundation of the project:
- Clean, modular C architecture separating application lifecycle from UI.
- Standard GTK 4 application entry point with header bar and minimal window layout.
- Makefile supporting incremental compilation with strict warnings enabled (`-Wall -Wextra -Wpedantic`).

## Project Structure

```text
reqly/
├── Makefile          # Build definitions (targets: all, run, clean)
├── README.md         # Project documentation
├── .gitignore        # Ignored files and directories
├── include/          # Public header files
│   ├── app.h         # Application lifecycle management
│   └── ui.h          # Window and UI component construction
└── src/              # Implementation source files
    ├── main.c        # Minimal program entry point
    ├── app.c         # GtkApplication initialization and event loop
    └── ui.c          # Main window and layout setup
```

## Prerequisites

To build and run Reqly, you need a C11 compiler, `make`, `pkg-config`, and the GTK 4 development libraries.

### Debian / Ubuntu
```bash
sudo apt update
sudo apt install -y build-essential pkg-config libgtk-4-dev
```

### Fedora
```bash
sudo dnf install -y gcc make pkgconf-pkg-config gtk4-devel
```

### Arch Linux
```bash
sudo pacman -S --needed base-devel pkgconf gtk4
```

## Building and Running

### Build
Compile the application:
```bash
make
```
The compiled binary will be placed at `build/reqly`.

### Run
Build (if needed) and run the application:
```bash
make run
```

### Clean
Remove all build artifacts:
```bash
make clean
```

