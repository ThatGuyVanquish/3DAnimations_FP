# Lior and Nave's 3D Animations Final Project (Skinning)

The assignment is to build a 3D Snake game, using a provided engine and utilizing GLFW3, dear ImGUI and papers taught in class to make the game as smooth and fun as possible.

The assignment is coded in the folder [Assignment4](https://github.com/ThatGuyVanquish/3DAnimations_FP/tree/master/tutorial/Assignment4) and in various files we've included in the engine folder, as stated below:   

### 1) Animation Visitor {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/AnimationVisitor.h) | [.cpp](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/AnimationVisitor.cpp)}

The Animation Visitor is another visitor that's called in the renderer and is used to make the movement smoother and more snake-like:
* <span style="color:orange"> Snaky Locomotion </span> - for cylinder kind models, this visitor is calculating the angle from the cylinder to its X and Y axis, and rotating it slightly to the axis direction in each visit. 
* <span style="color:orange"> Snaky Locomotion </span> - in addition, when the player's controls are not in action, this visitor is responsible on making the snake move in a snake-like movement with rotations to left and right of the snake.
* <span style="color:orange"> Bouncing Objects </span> - for entity kind models, the visitor will move the model dynamically in the game space, under a predefined range.

### 2) Collision Detection {[.cpp](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/CollisionDetection.cpp)}

The collision detection class holds the methods necessary to add collision detection to all the entities and the snake itself, using AABBs as shown in [Assignment 2](https://github.com/ThatGuyVanquish/3DAnimation_AS2). And with [Gameplay::checkForCollision()](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/3b88b154858cab81be747d8511da25302c085e2a/engine/Gameplay.cpp#L293) different kinds of collisions are checked frame by frame and followed by a proper response:
* <span style="color:orange"> Detection of Collision With Objects </span> - collision between the head of the snake and any kind of entity is followed by a bonus, extra point of loss of life.
* <span style="color:orange"> Self Collision of the Snake </span> - if the head of the snake is colliding with its body it will cause a loss of life.

### 3) Texture Coordinates {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/TextureCoordinates.h)}

TextureCoordinates is our implementation for calculating the snake texture coordinates, which is crucial to make the snake model look more snake-like with an awesome snake skin texture. This functionality is based on [libigl-tutorial-503](https://github.com/libigl/libigl/blob/main/tutorial/503_ARAPParam/main.cpp) for <span style="color:orange"> "As-rigid-as-possible Parametrization" algorithm </span>. 

### 4) ImGuiOverlay {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/ImGuiOverlay.h) | [.cpp](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/ImGuiOverlay.cpp)}

<span style="color:orange"> Interactive User Interface using ImGui </span> ImGuiOverlay is our entire implementation of the game's GUI. Using dear ImGUI we've implemented a main menu used to start the game and show a leaderboard, the top of the screen HUD used to display the time and score and more graphics such as a start countdown, level-up and death screens, and so on.

### 5) Leaderboard {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/Leaderboard.h)}

As previously mentioned in ImGuiOverlay, the <span style="color:orange"> Leaderboard </span> is a class that is used to store given scores locally on the device and display them when queried through the start menu.


### 6) [common.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/common.h)

A header file used for additional general methods and definitions, such as the Entity, entity_data and model_data structs we used throughout the assignment.

### 7) Skinning {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/Skinning.h) | [.cpp](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/Skinning.cpp)}

The class that holds our implementation of the skinning algorithm, using dual quaternion skinning.

<span style="color:orange"> Dual Quaternion Skinning : </span>

We use the methods [*calcWeight*](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/4414a87e9a48c9914e475d7511bb5d9238d89111/engine/Skinning.cpp#L86) to calculate the relevant weight of each joint to each vertex and [*moveModel*](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/4414a87e9a48c9914e475d7511bb5d9238d89111/engine/Skinning.cpp#L115) to animate the snake correctly according to the calculated weights. The implementation for the wights calculation for each snake vertex is considering the 2 closets joints where the weight for each joint is based on its distance from the vertex and the joint z coordinate.  

### 8) Gameplay {[.h](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/Gameplay.h) | [.cpp](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/Gameplay.cpp)}

The Gameplay class handles the entire aspects of the game, from initializing the entities at the loading stage, to handling the result of collisions and game state.

### 9) Audio {[callPythonScript](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/engine/common.h#L101)}
<span style="color:orange"> Sound :</span>

We've used threads to execute python scripts on a different thread. These python scripts run an audio file either once for a set amount of time (adlibs) or in a loop (the background music). Python scripts are [play_sound.py](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/tutorial/Assignment4/resources/scripts/play_sound.py) and [bgm.py](https://github.com/ThatGuyVanquish/3DAnimations_FP/blob/master/tutorial/Assignment4/resources/scripts/bgm.py)
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
