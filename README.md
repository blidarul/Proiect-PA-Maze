# Maze Game
Educational game project that combines 3D maze navigation with challenges in the form of questions and answers. The game features an intuitive graphical interface, visual and auditory elements, and a modular code structure designed for easy maintenance and extension.

* **Note:** Maze is still a work in progress, it is unknown if development will continue afterwards.

## Project Features
* The primary goal is the creation of a 3D maze environment that can be navigated by the user.
* The game implements a question and answer system designed to test the player's knowledge.
* It features an intuitive graphical interface that includes a main menu, a pause menu, and settings.
* The experience is enhanced through the integration of visual and auditory elements.
* The software is built with a modular code structure to ensure easy maintenance and future expansion.
* The raylib library is utilized for handling graphical rendering and user input.
* The game can be compiled and run across multiple platforms, which is configured via the Makefile.

## Team
* Coordinator: Professor Caramihai Dan Mihail.
* Contributors: Blidaru Andrei Eduard, Matei Radu Ștefan, Stan Dragoș Ioan, Chiriac Mihai Ciprian, Șolcă Andrei Rareș, and Delea Robert Stelian.

## Technologies and Architecture
* C Programming Language: Chosen to provide performance and granular control over system resources.
* raylib: A simple C library used for game development, handling 2D/3D graphics, input, and audio.
* raygui: An extension for raylib hosted on GitHub, utilized for creating standard Graphical User Interface (GUI) elements.
* Procedural Maze Generation: Specific algorithms dynamically create the maze structure.
* External Resources: Questions and answers are stored in external text files. This allows the educational content to be modified easily without recompiling the codebase.

## Project Structure
The project is modularized into various .c and .h source file pairs to handle specific component functionalities:
* `main.c`: The application's entry point that initializes the game and runs the main loop.
* `game.c` / `game.h`: Houses the core game logic, player interactions with the maze, and the Q&A system.
* `renderer.c` / `renderer.h`: Manages all screen drawing operations. It initializes the window, draws the 3D scene, manages the camera, and handles 2D GUI elements like the minimap and stamina bar.
* `camera.c` / `camera.h`: Controls the 3D camera, perspective, and movement.
* `maze.c` / `maze.h`: Contains the algorithms for generating and illustrating the maze.
* `questions.c` / `questions.h`: Handles loading questions and answers, displaying them, and validating user input.
* `pause.c` / `pause.h`: Implements the pause menu functionality.
* `settings.c` / `settings.h`: Implements the game's settings menu.
* `sound.c` / `sound.h`: Manages loading and playing background music and sound effects.
* `gameState.c` / `gameState.h`: Administers the current game states, such as being in the main menu, actively playing, paused, or in settings.
* `gameResources.c` / `gameResources.h`: Handles the loading and freeing of game resources, including textures, models, and sounds.
* `title.c` / `title.h`: Manages the game's introduction or title screen.

## Maze Generation Algorithm
The maze generation algorithm is dynamically implemented and inspired by a method created for Minecraft explained in a [youtube video](https://www.youtube.com/watch?v=uctN47p_KVk).
* Every node must point to exactly one neighboring node, except for a single "origin" node that points nowhere.
* This ensures there is always one less connection than the total number of nodes.
* Every node must ultimately connect to the origin node; following the arrows from any node will lead to the origin.
* These rules guarantee that no isolated areas can exist within the maze.
* They also prevent the formation of loops, as the nodes point to each other to form a tree structure.
* The maze remains "perfect"—completely connected without loops—after each generation iteration.
* The algorithm operates by moving the origin node to a new location, removing its pointer, and pointing the previous origin to the new origin.
* Because the maze remains valid after every iteration, it can be played while it is still actively generating and shifting around the player.

## Assets and Resources
All external game resources are located in the `resources` directory:
* `icon.png`: The application icon loaded during window initialization.
* `Q_A`: Contains the `Questions.txt` and `Answers.txt` files utilized by the question system.
* `Sound`: Contains audio files for background music and sound effects. Third-party sounds, including the pop-up sound, correct/wrong answers, music, and a "ding", were altered using the Audacity program.
* `textures`: Holds the image textures used within the game environment.

## Compilation and Execution
The Makefile is an older version of the [default Makefile](https://github.com/raysan5/raylib/blob/master/examples/Makefile) from the raylib library.
* The Makefile is configured to support compilation across various platforms.
* It defines all necessary variables for the compiler, linker, library paths, and platform-specific options.

### Requirements
* A C compiler
* An installation of [raylib](https://www.raylib.com/)
* make

### Building 
1. Clone the repository (`git clone https://github.com/blidarul/Proiect-PA-Maze`).
2. Go to the folder where you cloned the repository.
3. Run `make`.
4. Run the executable compiled as `game` (or `game.exe` if you are on Windows).

## Credits
* Developers of [raylib](https://www.raylib.com/)
* Contributors listed in [LICENSE](https://github.com/blidarul/Proiect-PA-Maze/blob/main/LICENSE)

## License
MIT
