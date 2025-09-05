# Orange Game
Orange Game is a console-based game developed in C, exclusively for Windows. It provides an interactive text-based gaming experience that is lightweight and easy to run. The game is designed to run without administrative privileges—running it as an administrator will cause it to fail.
**Table of Contents**

Features
Requirements
Installation
Usage
Building from Source
Important Notes
Contributing
License

Features

Fully text-based gameplay in the Windows console
Engaging and interactive mechanics
No external dependencies required
Portable executable for easy distribution

Requirements

Windows operating system (Windows 10 or later recommended)
A C compiler (e.g., GCC via MinGW or MSVC)
Standard Windows console (Command Prompt or PowerShell)

Installation

Download the latest release from the Releases page.
Extract the orange_game.exe file to your desired directory.
Ensure you have a Windows environment (Command Prompt or PowerShell).

Usage

Open a Command Prompt or PowerShell.
Navigate to the directory containing orange_game.exe.
Run the executable:orange_game.exe


Follow the on-screen instructions to play the game.

Important: Do not run the executable as an administrator (e.g., via "Run as Administrator"). Doing so will cause the game to fail.
Building from Source
To build the game from source, follow these steps:

Clone the repository:git clone https://github.com/yourusername/orange-game.git


Navigate to the project directory:cd orange-game


Compile the source code using a C compiler (e.g., GCC):gcc -o orange_game main.c


Run the generated orange_game.exe as described in the Usage section.

Dependencies

No external libraries are required.
Ensure you have a C compiler installed (e.g., MinGW or MSVC).

Important Notes

The game is Windows-only and will not work on other operating systems.
Running the executable with administrator privileges will cause it to fail. Always run it as a standard user.
The game uses the standard Windows console for input and output.

Contributing
Contributions are welcome! To contribute:

Fork the repository.
Create a new branch (git checkout -b feature/your-feature).
Make your changes and commit them (git commit -m "Add your feature").
Push to your branch (git push origin feature/your-feature).
Open a pull request.

Please ensure your code follows the project's coding style and includes appropriate comments.
License
This project is licensed under the MIT License. See the LICENSE file for details.
