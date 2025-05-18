#ifndef CONFIG_H
#define CONFIG_H

// Screen settings
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

#define MAZE_SIZE 20

// Mini-map settings
#define MINIMAP_SCALE 7.0f
#define PLAYER_COLOR RED
#define MINIMAP_DIST_FROM_BORDER 20.0f

//player related
#define PLAYER_RADIUS 0.2f
#define DEFAULT_MOUSE_SENSITIVITY 0.1f
#define PLAYER_SPEED 2.0f
#define PLAYER_ACCELERATION 1.8f //For sprinting 
#define PLAYER_FOV 65.0f
#define PLAYER_HEIGHT 0.5f


//sound related
#define STEP_SOUND_COUNT 4
#define STEP_COOLDOWN 1.0f / PLAYER_SPEED

#endif // CONFIG_H