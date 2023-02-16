# Lior and Nave's 3D Animations Final Project (Skinning)

The assignment is to build a 3D Snake game, using a provided engine and utilizing GLFW3, dear ImGUI and papers taught in class to make the game as smooth and fun as possible.

The assignment is coded in the folder [Assignment4](https://github.com/ThatGuyVanquish/3DAnimations_FP/tree/master/tutorial/Assignment4) and in various files we've included in the engine folder, as stated below:   

### 1) Animation Visitor {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/AnimationVisitor.h) | [.cpp](https://github.com/ThatGuyVanquish/3DAnimations_FP/tree/master/tutorial/Assignment4)}

The Animation Visitor is another visitor that's called in the renderer and is used to make the movement smoother and more snake-like.

### 2) Collision Detection {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/CollisionDetection.h) | [.cpp](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/CollisionDetection.cpp)}

The collision detection class holds the methods necessary to add collision detection to all of the entities and the snake itself, using AABBs as shown in [Assignment 2](https://github.com/ThatGuyVanquish/3DAnimation_AS2).

### 3) Texture Coordinates {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/commit/0527a70182d75e1218bcfa4bbec3c98109029901#diff-5094cf97a15dd0544aa388bd12ba949e15f0b6672d2e31a6a590549fbbef06bd)} *[EDIT THE LINK AND INFO]*

### 4) ImGuiOverlay {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/ImGuiOverlay.h) | [.cpp](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/ImGuiOverlay.cpp)}

ImGuiOverlay is our entire implementation of the game's GUI. Using dear ImGUI we've implemented a main menu used to start the game and show a leaderboard, the top of the screen HUD used to display the time and score and more graphics such as a start countdown, level-up and death screens, and so on.

### 5) Leaderboard {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/Leaderboard.h)}

As previously mentioned in ImGuiOverlay, the leaderboard is a class that is used to store given scores locally on the device and display them when queried through the start menu.

### 6) [common.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/common.h)

A header file used for additional general methods and definitions, such as the Entity, entity_data and model_data structs we used throughout the assignment.

### 7) Skinning {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/Skinning.h) | [.cpp](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/Skinning.cpp)}

The class that holds our implementation of the skinning algorithm, using dual quaternion skinning.

We use the methods [*calcWeight*](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/4414a87e9a48c9914e475d7511bb5d9238d89111/engine/Skinning.cpp#L86) to calculate the relevant weight of each joint to each vertex and [*moveModel*](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/4414a87e9a48c9914e475d7511bb5d9238d89111/engine/Skinning.cpp#L115) to animate the snake correctly according to the calculated weights.

### 8) Gameplay {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/Gameplay.h) | [.cpp](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/Gameplay.cpp)}

The Gameplay class handles the entire aspects of the game, from initializing the entities at the loading stage, to handling the result of collisions and game state.

***************************
## BUTTON MAPPINGS

### GAMEPLAY
**W, S** -> Go up and down

**A, D** -> Go left and right

**Q, E** -> Handle the roll of the snake

### CAMERAS
**1** -> Switch to first person view

**3** -> Switch to third person view

**0** -> Switch to top down camera *[SUBJECT TO CHANGES]*

### DEV MODE
#### NOTE THAT MOUSE MOVEMENT IS RESTRICTED TO CAMERAS, AND IS GENERALLY ONLY AVAILABLE IN DEV MODE.

**LN** -> Enters and exits developer mode, used to distinguish a debug state away from the general gameplay and restrict unnecessary actions from the user.

**:arrow_up:, :arrow_down:** -> Increase and decrease the snake's velocity (could be negative), also changese the slerp factor for it's rotation.

**:arrow_left:, :arrow_right:** -> Increases and decreases the amount of lives the player has. Note that lives could be at the negative, player can only die if collided with enemy with 1 heart. *[SUBJECT TO CHANGES]*

**R** -> Reset game back to the main menu.

**G** -> Toggle animation on/off

**T** -> Simulates level-up in the background to get more enemies after next reset.

**J** -> Adds 1000 to the current score

**P** -> Prints camera's position

**H** -> Toggles cylinders and entities collision frame hidden/showing

**C** -> Toggles cylinders hidden/showing

**V** -> Toggles global axis hidden/showing
