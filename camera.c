#include "camera.h"
#include "raymath.h"
#include <math.h>

int MAX(int a, int b)
{
    return a > b ? a : b;
}

int MIN(int a, int b)
{
    return a > b ? b : a;
}

Camera InitializeCamera(void)
{
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.2f, 0.4f, 0.2f };
    camera.target = (Vector3){ 0.2f, 0.4f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    return camera;
}

void UpdatePlayerMovement(Camera* camera, GameResources resources)
{
    const float sqrt2 = 1.41421356237f;
    
    float zoom = 0.0f;
    Vector3 rotation = {0, 0, 0};
    Vector3 localMovement = {0, 0, 0};

    rotation.x = GetMouseDelta().x * DEFAULT_MOUSE_SENSITIVITY;
    rotation.y = GetMouseDelta().y * DEFAULT_MOUSE_SENSITIVITY;

    float straightMovementSpeed = PLAYER_SPEED * GetFrameTime();
    float diagonalMovementSpeed = straightMovementSpeed / sqrt2;
    bool W = IsKeyDown(KEY_W);
    bool A = IsKeyDown(KEY_A);
    bool S = IsKeyDown(KEY_S);
    bool D = IsKeyDown(KEY_D);

    // Calculate camera-local movement based on input
    if (W)
    {
        if (A || D) localMovement.x += diagonalMovementSpeed;
        else localMovement.x += straightMovementSpeed;
    }
    if (S) // Assuming S is backward
    {
        if (A || D) localMovement.x -= diagonalMovementSpeed;
        else localMovement.x -= straightMovementSpeed;
    }

    if (A)
    {
        if (W || S) localMovement.y -= diagonalMovementSpeed;
        else localMovement.y -= straightMovementSpeed;
    }
    if (D)
    {
        if (W || S) localMovement.y += diagonalMovementSpeed;
        else localMovement.y += straightMovementSpeed;
    }
    
    // Handle collision detection - modifies localMovement if necessary
    HandleCollisions(camera, &localMovement, resources);

    // Update camera using the (potentially modified) localMovement and rotation
    UpdateCameraPro(camera, localMovement, rotation, zoom);
}

void HandleCollisions(Camera *camera, Vector3 *localMovement, GameResources resources)
{
    
}