# Installation Instructions

This installation assumes you already have a c++ compiler (e.g., g++) installed on your system.

## Step 1: Obtain the source code

There are two main options to get the source code. The first option is to clone our repository using the command `git clone https://github.com/OSU-CS467-Summer2024-Emulator/8080-emulator.git`.

If you are less familiar with git, you can also download the source code from this GitHub repository. To do this, click the green “Code” button, and then select “Download ZIP”. Once the zipped archive is downloaded, unzip it to a convenient location on your computer.

## Step 2: Verify that CMake is installed

Building the project requires CMake, a cross-platform tool for compiling C++ code. To check if CMake is installed, run the command `cmake --version`. This should output the version of CMake that is installed if it is present. If CMake is not installed, you can download it [here](https://cmake.org/download/). We won’t cover the details of installing CMake, but it is easy to find tutorials online (for example, [this one](https://youtu.be/8_X5Iq9niDE) for Windows).

## Step 3: Build the project

From the command line, make sure your working directory is the root of our project (where the README file is located). To build the project, run the bash script `build.sh` from the command line (execute the command `build.sh` or `bash build.sh` on Windows with bash installed). Alternatively, you can run the following three commands:
```
mkdir build
cmake -S . -B build
cmake --build build
```

## Step 4: Run the executable
After the build finishes, the executable will be placed in the folder `/build/bin` and named `Main` (or `Main.exe` on Windows). Run it with the command `./build/bin/Main`.

## Step 5: Play the game

When the program is started, the Space Invaders splash screen will be shown. To start a game, you must first add credit by pressing the **Up** arrow key. To start a single player game, add at least one credit and then press **Enter**/**Return**. To play a two player game add at least two credits and then press **2**.

When the game starts, move the ship to the left with the **Left** arrow key (or **A** for player 2) and to the right with the **Right** arrow key (or **D** for player 2). Fire at the aliens with the **Space Bar** (or **W** for player 2).

The game plays just like the original arcade machine - the code is exactly the same, we just created the emulator to run and display it.  Enjoy!
