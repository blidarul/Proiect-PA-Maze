#include "camera.h"
#include "raymath.h"
#include <math.h>
#include "raylib.h"

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
    camera.position = (Vector3){ 1.5f, 0.4f, 1.5f };
    camera.target = (Vector3){ 0.5f, 0.4f, 0.5f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    return camera;
}

// Helper: returns true if player collides with any wall
bool PlayerCollides(Vector3 playerPos, GameResources resources)
{
    int mapWidth = resources.cubicimage.width;
    int mapHeight = resources.cubicimage.height;
    Color *pixels = LoadImageColors(resources.cubicimage);

    int minX = (int)floorf(playerPos.x - PLAYER_RADIUS);
    int maxX = (int)ceilf(playerPos.x + PLAYER_RADIUS);
    int minZ = (int)floorf(playerPos.z - PLAYER_RADIUS);
    int maxZ = (int)ceilf(playerPos.z + PLAYER_RADIUS);

    minX = (minX < 0) ? 0 : minX;
    minZ = (minZ < 0) ? 0 : minZ;
    maxX = (maxX >= mapWidth) ? mapWidth - 1 : maxX;
    maxZ = (maxZ >= mapHeight) ? mapHeight - 1 : maxZ;

    bool collision = false;
    for (int x = minX; x <= maxX; x++)
    {
        for (int z = minZ; z <= maxZ; z++)
        {
            Color pixel = pixels[z * mapWidth + x];
            if (pixel.r == 255)
            {
                Rectangle cellRect = { x, z, 1.0f, 1.0f };
                Vector2 playerPos2D = { playerPos.x, playerPos.z };
                if (CheckCollisionCircleRec(playerPos2D, PLAYER_RADIUS, cellRect))
                {
                    collision = true;
                    break;
                }
            }
        }
        if (collision) break;
    }
    UnloadImageColors(pixels);

    TraceLog(LOG_INFO, "[PlayerCollides] Pos:(%.2f,%.2f) Bounds:x[%d-%d] z[%d-%d] Result:%s",
        playerPos.x, playerPos.z, minX, maxX, minZ, maxZ, collision ? "COLLIDE" : "CLEAR");

    return collision;
}

Vector3 HandleCollisions(Camera *camera, Vector3 localMovement, GameResources resources)
{
    // Store the original position
    Vector3 originalPos = camera->position;
    
    // Convert camera-relative movement to world-space movement
    // First, calculate forward and right vectors
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera->up));
    
    Vector3 worldMovement = {
        forward.x * localMovement.x + right.x * localMovement.y,
        localMovement.z, // Keep vertical movement if any
        forward.z * localMovement.x + right.z * localMovement.y
    };
    
    // Simulate the new position
    Vector3 targetPos = Vector3Add(originalPos, worldMovement);
    
    // Check if target position collides
    if (!PlayerCollides(targetPos, resources))
    {
        // No collision, return the original movement
        return localMovement;
    }
    
    // We have a collision, determine the wall normal more accurately
    Vector2 wallNormal = {0};
    
    // Test X-axis movement only
    Vector3 xTestPos = originalPos;
    xTestPos.x += worldMovement.x;
    bool xCollision = PlayerCollides(xTestPos, resources);
    
    // Test Z-axis movement only
    Vector3 zTestPos = originalPos;
    zTestPos.z += worldMovement.z;
    bool zCollision = PlayerCollides(zTestPos, resources);
    
    // Set wall normal based on which axis collides
    if (xCollision && !zCollision) {
        // Wall blocks X movement
        wallNormal.x = (worldMovement.x > 0) ? -1.0f : 1.0f;
        wallNormal.y = 0.0f;
    }
    else if (!xCollision && zCollision) {
        // Wall blocks Z movement
        wallNormal.x = 0.0f;
        wallNormal.y = (worldMovement.z > 0) ? -1.0f : 1.0f;
    }
    else {
        // Both axes collide or we have a corner case
        // Use the dominant movement direction to determine primary blocking axis
        if (fabs(worldMovement.x) > fabs(worldMovement.z)) {
            wallNormal.x = (worldMovement.x > 0) ? -1.0f : 1.0f;
            wallNormal.y = 0.0f;
        }
        else {
            wallNormal.x = 0.0f;
            wallNormal.y = (worldMovement.z > 0) ? -1.0f : 1.0f;
        }
    }
    
    // Handle collision response (sliding along walls)
    Vector2 movement = {
        worldMovement.x,
        worldMovement.z
    };
    
    float dotProduct = movement.x * wallNormal.x + movement.y * wallNormal.y;
    
    Vector2 perpendicular = {
        wallNormal.x * dotProduct,
        wallNormal.y * dotProduct
    };
    
    Vector2 parallel = {
        movement.x - perpendicular.x,
        movement.y - perpendicular.y
    };
    
    // Scale down the parallel component to prevent sliding into corners
    const float slideScale = 0.8f;
    parallel.x *= slideScale;
    parallel.y *= slideScale;
    
    // Create the adjusted world movement
    Vector3 adjustedWorldMovement = {
        parallel.x,
        worldMovement.y, // Keep vertical movement
        parallel.y
    };
    
    // Test if the adjusted movement is collision-free
    Vector3 adjustedTargetPos = Vector3Add(originalPos, adjustedWorldMovement);
    if (PlayerCollides(adjustedTargetPos, resources))
    {
        // Still colliding, cancel movement in this direction
        return (Vector3){0, 0, 0};
    }
    
    // Convert the world movement back to camera-relative coordinates
    Vector3 result = {
        Vector3DotProduct(forward, adjustedWorldMovement),
        Vector3DotProduct(right, adjustedWorldMovement),
        localMovement.z
    };
    
    return result;
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
    localMovement = HandleCollisions(camera, localMovement, resources);
    
    // Update camera using the (potentially modified) localMovement
    UpdateCameraPro(camera, localMovement, rotation, zoom);
}