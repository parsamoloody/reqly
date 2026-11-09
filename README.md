# Reqly

Reqly is a lightweight, modular API client written in C using GTK 4.

## Current Milestone: Protocol Foundation & Request Bar

This milestone introduces:
- Extensible protocol subsystem (`protocol.h` / `protocol.c`) ready for HTTP and future protocols (WebSocket, gRPC, etc.).
- Standard HTTP methods support (`http.h` / `http.c`) covering `GET`, `POST`, `PUT`, `DELETE`, `PATCH`, `HEAD`, and `OPTIONS`.
- Modern GTK 4 request bar featuring:
  - Protocol selector dropdown
  - HTTP method selector dropdown
  - URL text entry with placeholder and Enter key trigger
  - "Send" action button with visual feedback
- Clean separation between application lifecycle, UI layer, protocol definitions, and HTTP logic.
- Strict compiler warning compliance (`-Wall -Wextra -Wpedantic -std=c11`).

## Project Structure

```text
reqly/
├── Makefile            # Build definitions (targets: all, run, clean)
├── README.md           # Project documentation
├── .gitignore          # Ignored build outputs and editor files
├── include/            # Header files
│   ├── app.h           # Application lifecycle management
│   ├── ui.h            # Main window and layout construction
│   ├── protocol.h      # Extensible protocol registry and interface
│   └── http.h          # HTTP method definitions and conversion utilities
└── src/                # Implementation source files
    ├── main.c          # Application entry point
    ├── app.c           # GtkApplication initialization and event loop
    ├── ui.c            # Window layout, request bar, and event handlers
    ├── protocol.c      # Protocol registry and method query implementations
    └── http.c          # HTTP methods table and conversion routines
```

## Prerequisites

To build and run Reqly, you need a C11 compiler, `make`, `pkg-config`, and GTK 4 development libraries.

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
