
# 🏁 SDL2 Checkers Game

A networked and local multiplayer checkers (draughts) game built with **C++**, **SDL2**, and **SDL_net**.

---

## 📦 Features

- 🧠 Classic checkers rules with kings and multi-captures
- 🎮 Play locally or over the network (server/client)
- 🖼️ SDL2-based rendering with textures
- ⚡ Lightweight and fast to build with Makefile

---

## ✅ Dependencies

To build and run the project, make sure the following libraries are installed:

- `g++` or another C++11-compatible compiler
- [SDL2](https://libsdl.org)
- `SDL2_image`
- `SDL2_net`
- `make`

### Ubuntu / Debian installation

```bash
sudo apt update
sudo apt install g++ make libsdl2-dev libsdl2-image-dev libsdl2-net-dev
```

## 🔧 Building the Game

1. Clone or download the project:
   ```bash
   git clone https://github.com/your-username/sdl2-checkers.git
   cd sdl2-checkers
   ```

2. Build and run the game using `make`:
   ```bash
   make run
   ```

   This will:
   - Compile the source code in `src/`
   - Produce the `Checkers` executable
   - Immediately launch the game

3. Alternatively, just build without launching:
   ```bash
   make
   ```

4. To clean the build:
   ```bash
   make clean
   ```

---

## 🕹️ How to Play

On launch, choose a mode:

- `1`: Start as **Server** (white pieces, listens for connection)
- `2`: Start as **Client** (black pieces, connect to server by IP)
- `3`: Local 2-player game on the same device

**Mouse Controls**:  
- Click to select a piece  
- Click again to move it (if the move is valid)

---

## 📁 Project Structure

```
.
├── src/                   # Source code
│   ├── main.cpp
│   ├── Game.h / Game.cpp
│   ├── Board.h / Board.cpp
│   ├── NetworkManager.h / NetworkManager.cpp
├── assets/                # Textures (board, pieces)
├── makefile
└── README.md
```

---

## 🧊 Notes

- Ensure that all required textures (`board.png`, `white_piece.png`, etc.) are available in the working directory or adjusted in code.
- Works on Linux. For Windows, update the makefile or use a compatible environment like MinGW or WSL.

---

## 🚀 Authors

Artemijs Stanko, Rolands Timonovs

```

---

Let me know if you want me to generate this `README.md` file for you directly or adjust it for CMake or Windows support too.
