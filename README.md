# Reqly

A fast, lightweight, and distraction-free desktop API client.

## What is Reqly?

Reqly is a minimalist API client designed for developers who want to test and debug APIs quickly without friction. It gives you an intuitive, clean interface to compose requests, test endpoints, and inspect responses without the bloat and complexity of modern API tools.

## Why Reqly?

- **Instant Startup & Tiny Footprint**: Launches immediately and consumes minimal system resources, avoiding the heavy memory and CPU overhead of Electron-based clients.
- **Zero Distractions**: No mandatory account creation, no cloud sync nag screens, and no enterprise clutter. It's just you and your APIs.
- **Frictionless Workflow**: Simple, direct controls for protocols, methods, and URLs so you can test endpoints in seconds.
- **Native Desktop Integration**: A smooth, responsive interface tailored for developers who value speed and simplicity.

## Getting Started

### Prerequisites

#### Debian / Ubuntu
```bash
sudo apt update
sudo apt install -y build-essential pkg-config libgtk-4-dev
```

#### Fedora
```bash
sudo dnf install -y gcc make pkgconf-pkg-config gtk4-devel
```

#### Arch Linux
```bash
sudo pacman -S --needed base-devel pkgconf gtk4
```

### Building and Running

Compile the application:
```bash
make
```

Run the application:
```bash
make run
```

Clean build artifacts:
```bash
make clean
```
