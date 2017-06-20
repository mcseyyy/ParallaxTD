# ParallaxTD
#### Bringing phisicality to Tower Defence
![game-screenshot](https://cloud.githubusercontent.com/assets/9435724/10560156/10ec42d8-74fa-11e5-8abb-50ed98a7244b.png)

[![yt-video](https://cloud.githubusercontent.com/assets/9435724/10560153/10e9000a-74fa-11e5-88ad-d9304e33e07f.png)](https://www.youtube.com/watch?v=oYYZJd8W8i8)

Parallax TD is a “Tower Wars” type game, in which players compete directly against each other; each player must simultaneously build towers to defend themselves and summon creeps to attack the opponent team. It is by default a three-versus-three game, in which players on the same team share an arena – divided into zones owned by individual players – and hitpoints.

Each player is given a set of tiles (made out of wood) at the start of the game, which may be placed anywhere on the projection area, within that player’s zone; this is detected by a camera (which is attached to the ceiling) connected to our Parallax Vision software and a tower is built/projected on the tile. This results in a unique experience, where players can creatively build their own maze out of our Parallax Tower-Tiles.

To the players, the main interface to the game – beyond building the towers – is the **Android App**, which functions as a controller. From this app, players may select a username, a level of experience and join games. When playing a game the app provides a UI, with real-time feedback on the status of the player and their team. It also accepts user input, allowing players to spawn various types of creeps to attack their enemies. The app is also used  by players in the construction of towers; players select a type of tower using the app, and towers they build subsequently will be of that type.

The nature of the gameplay is fast, competitive, and tactical. Players must focus on defending against their enemies while simultaneously working together to bring them down. Some of the key strategies in the game are building complex mazes to stall creeps as long as possible, coordinating attacks with teammates to cause the most damage, and combining special abilities of the towers to maximise both offensive and defensive potential.

PS: The repo does not contain the Android App and the tile detection system.

#### Tools

- The core of the game was developed using **Unreal Engine** with C++, though some of the basic functionalities were implemented using Blueprins.
- The tile detection system was developed with **OpenCV** and C++.
- The Android App was created using the standard **Andorid SDK**.
- Networking was done using socket libraries.
- Since Unreal Engine did not support synchronised audio playback, this was done using **irrKlang**.

## Features
#### Tile Detection
The tile detection system is very reliable and works perfectly(i.e. no false positives and negatives) in various lighting conditions. It also includes a perspective transform so that only the projected screen is used for detection instead of the whole frames recorded by the camera. The program is also able to find the projected screen in the initial frames and therefore the perspective transform is calibrated automatically before every round of the game.

![tile-detection](https://cloud.githubusercontent.com/assets/9435724/10560157/10f08f6e-74fa-11e5-89d4-cc68bb1ad49e.png)
On the left: Camera feed with the recognised tiles;<br>
On the right: Thresholded camera feed;
#### Android App
Every player used an Android device as their controller for the game.

![android-app](https://cloud.githubusercontent.com/assets/9435724/10560154/10ea1684-74fa-11e5-8729-a14bd70715cc.png)

From the app, the players can:
- select the type of towers that they want to build;
- send creeps to attack to opponent;
- get information about their stats (money, health)
- be informed when their base is attacked through haptick feedback

#### Procedurally arranced music
The background music was created by adding/removing audio layers to a base melodic line. Each team hears a slightly different background music which is dependent to the HP of their team.

In the following image, each column corresponds to a HP interval of a team; e.g.: if a team's HP was between 0-11% they would hear only the audio layers from the last column
![audio](https://cloud.githubusercontent.com/assets/9435724/10560155/10ea6ecc-74fa-11e5-80c1-d0d846567723.png)

## Contributors
The developing team was formed by:

- Andrei Ilisei
- Milan Zolota
- Nikolay Nikolov
- Patrick Johnston
- Shayan Chaudhary
- Stephen Livermore-Tozer

## How to run the game
Due to the nature of the setup (projection on the floor, camera attached to the ceiling etc.) the game cannot be actually played properly. You can use the executables in the /Fake Clients directory to simulate the players and the camera feed and you also need Unreal Engine installed on the machine.

Note: Most of the code actually written by us is in \Source folder. The Blueprint classes (with the attached models) can be found in /Content/Bluebrints/.
