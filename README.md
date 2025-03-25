---

## Networked Checkers Game

A graphical checkers (draughts) game written in C++ with support for **local multiplayer** and **online play** over TCP using **SDL2**, **SDL_image**, and **SDL_net**.

###  Features
- Local two-player mode
- Network play (host as server or join as client)
- King promotion
- Capture detection (including multi-capture)
- Animated board with textures

---

## Requirements

Make sure you have the following libraries installed:

- **SDL2**
- **SDL2_image**
- **SDL2_net**

On Debian/Ubuntu-based systems:
```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-net-dev
```

On macOS (using Homebrew):
```bash
brew install sdl2 sdl2_image sdl2_net
```

On Windows:
- Download development libraries from [https://www.libsdl.org/](https://www.libsdl.org/) and set up include/lib paths in your IDE (e.g., Visual Studio or Code::Blocks).

---

## Assets

The game expects the following image files in the **same directory** as the executable:

```
board.png
white_piece.png
black_piece.png
white_piece_s.png
black_piece_s.png
white_king.png
black_king.png
white_king_s.png
black_king_s.png
```

Make sure these images are present and properly named.

---

##  Build Instructions

### Linux / macOS

Using `g++`:

```bash
g++ -o checkers main.cpp $(pkg-config --cflags --libs sdl2 SDL2_image SDL2_net)
```

Then run:

```bash
./checkers
```

### Windows (MinGW)

```bash
g++ main.cpp -o checkers.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_net
checkers.exe
```

> Make sure `.dll` files for SDL2 are in the same folder or in your system path.

---

## How to Play

### Launch Options:
On startup, you'll be prompted to choose a mode:

```
1 - Host Game (Server)
2 - Join Game (Client)
3 - Local Two-Player
```

---

##  Networking

- Server opens port `12345`
- Client connects using the server's IP
- Movement is synchronized in real time

---

## Controls

- **Left-click** to select and move pieces
- Regular rules of checkers apply (mandatory captures, king movement, etc.)

---

##  Authors

1. Artemijs Stanko
2. Rolands Timonovs

---
