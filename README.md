# Platformer Game (Raylib C)

This project is a platformer game developed in C using **Raylib**. It includes an animated player, platforms, collectible diamonds, moving enemies, a dynamic camera system, a life system, and a camera shake effect when taking damage.



https://github.com/user-attachments/assets/8de11f4c-433e-4b06-ac0f-de6d6f8f386e



---

## 🎮 Features

* **Full player animations**: idle, run, jump, fall, hit.
* **Smooth movement**: jumping, left/right movement.
* **Realistic gravity**.
* **Multiple platforms** with collision detection.
* **Moving enemies** (Spikeheads) with vertical patrol patterns.
* **Collectible items** (animated diamonds).
* **Score system**.
* **Life system**.
* **Camera follow system** + **Camera Shake** on damage.

---

## 📦 Requirements

* **Raylib** installed
* C compiler (GCC/Clang/MinGW)

Install Raylib on Windows (MinGW):

```bash
winget install raylib
```

---

## ▶️ Building & Running

### Windows (MinGW)

```bash
gcc main.c -o game -lraylib -lopengl32 -lgdi32 -lwinmm
./game
```
---

## 🎨 Recommended Folder Structure

```
project/
│-- main.c
│-- Sprites/
│   │-- 01-KingHuman/
│   │   │-- idle.png
│   │   │-- run.png
│   │   │-- jump.png
│   │   │-- fall.png
│   │   │-- hit.png
│   │-- Background/Blue.png
│   │-- ground.png
│   │-- platform.png
│   │-- Diamond.png
│   │-- enemy/idle.png
```

---

## ⌨️ Controls

| Key     | Action     |
| ------- | ---------- |
| `A`     | Move left  |
| `D`     | Move right |
| `SPACE` | Jump       |

---

## ❤️ Life System

The player starts with **3 lives**. Touching a Spikehead:

* removes 1 life
* plays the hit animation
* triggers a knockback
* activates **camera shake**

---

## 💎 Objective

Collect **all diamonds** placed in the level.

The score is shown in the top-left corner:

```
Score: X / 7
```

---

## 📌 To Improve (TODO)

* Add a win screen.
* Add a game over screen.
* Add audio (jump, damage, collectibles).
* Add background music.
* Add more levels.
