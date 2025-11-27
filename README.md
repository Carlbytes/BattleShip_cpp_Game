<div align="center">

  <h1>🚢 Battleship C++ Console Game</h1>

  <img src="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white" />
  <img src="https://img.shields.io/badge/C++17-Standard-blue?style=for-the-badge&logo=c%2B%2B" />
  <img src="https://img.shields.io/badge/SFML-8CC445?style=for-the-badge&logo=sfml&logoColor=white" />
  
  <br />

  <img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white" />
  <img src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black" />
  <img src="https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white" />
  <img src="https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white" />
  <img src="https://img.shields.io/badge/git-%23F05033.svg?style=for-the-badge&logo=git&logoColor=white" />

  <br />

  <img src="https://img.shields.io/badge/Networking-TCP%2FIP-blue?style=for-the-badge&logo=socket.io" />
  <img src="https://img.shields.io/badge/Installer-Inno_Setup-orange?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Pattern-Singleton-red?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Tests-MS_Native_Unit-purple?style=for-the-badge" />

  <br />

  <img src="https://img.shields.io/github/repo-size/Carlbytes/BattleShip_cpp_Game?style=flat-square&labelColor=black" />
  <img src="https://img.shields.io/github/last-commit/Carlbytes/BattleShip_cpp_Game?style=flat-square&labelColor=black" />
  <img src="https://img.shields.io/github/issues/Carlbytes/BattleShip_cpp_Game?style=flat-square&labelColor=black" />
  <img src="https://img.shields.io/badge/Maintained%3F-yes-green.svg?style=flat-square" />

</div>

<br />

## 📜 Project Overview
A robust, multiplayer, turn-based Battleship game built in C++ using the SFML library for networking. This project demonstrates a strong understanding of **Object-Oriented Programming (OOP)**, **Network Socket Programming**, and **Software Architecture**.

It features a Client-Server architecture allowing for LAN/WAN play, utilizes the Singleton design pattern for state management, and includes a professional Windows installer.

---

## ✨ Key Features
| Feature | Description |
| :--- | :--- |
| **📡 Networking** | TCP/IP socket connection handling via SFML Network module. |
| **🏗️ OOP Design** | Utilizes **Inheritance** (Ship classes), **Polymorphism**, and **Encapsulation**. |
| **🧠 Design Patterns** | Implements the **Singleton Pattern** for the GameManager to ensure state consistency. |
| **🎮 Gameplay** | Cursor-based aiming, dynamic ship placement, and hit/miss tracking. |
| **📦 Distribution** | Packaged with **Inno Setup** for a one-click install experience. |
| **🧪 Testing** | Code reliability verified via Microsoft Native Unit Test Framework. |

---

## 🛠️ Tech Stack & Tools
* **Language:** C++17
* **Networking/Graphics:** SFML (Simple and Fast Multimedia Library) 3.0.2
* **IDE:** Visual Studio 2022
* **Build Systems:** MSBuild (Visual Studio) & CMake support
* **Version Control:** Git

---

## 🚀 How to Play (Windows)
**The Easy Way:**
1.  Download the latest installer from the **[Releases]** tab on the right.
2.  Run `Battleship_Setup.exe`.
3.  Launch the game from your Desktop shortcut.

---

## ⚙️ Building from Source
If you are a developer and want to build the project yourself:

### Windows (Visual Studio)
1.  Clone the repo: `git clone https://github.com/Carlbytes/BattleShip_cpp_Game.git`
2.  Open `BattleshipGame.sln` in Visual Studio 2022.
3.  Select **Release** and **x64**.
4.  Build Solution (`Ctrl+Shift+B`).

### Linux (CMake)
*Prerequisites: SFML must be installed (`sudo apt-get install libsfml-dev`).*

```bash
mkdir build && cd build
cmake ..
make
./BattleshipGame
