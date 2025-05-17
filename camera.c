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
    camera.position = (Vector3){ 1.5f, PLAYER_HEIGHT, 1.5f };
    camera.target = (Vector3){ 0.5f, PLAYER_HEIGHT, 0.5f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = PLAYER_FOV;
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
    Vector3 forward = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera->up));
    
    Vector3 worldMovement =
    {
        forward.x * localMovement.x + right.x * localMovement.y,
        localMovement.z,
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
    if (xCollision && !zCollision)
    {
        wallNormal.x = (worldMovement.x > 0) ? -1.0f : 1.0f;
        wallNormal.y = 0.0f;
    }
    else if (!xCollision && zCollision)
    {
        wallNormal.x = 0.0f;
        wallNormal.y = (worldMovement.z > 0) ? -1.0f : 1.0f;
    }
    else if (xCollision && zCollision)
    {
        wallNormal.x = (worldMovement.x > 0) ? -1.0f : 1.0f;
        wallNormal.y = (worldMovement.z > 0) ? -1.0f : 1.0f;
        
        // Normalize this diagonal normal
        wallNormal = Vector2Normalize(wallNormal);
    }
    else
    {
        // Strange case - targetPos collides but neither axis alone does
        if (fabs(worldMovement.x) > fabs(worldMovement.z))
        {
            wallNormal.x = (worldMovement.x > 0) ? -1.0f : 1.0f;
            wallNormal.y = 0.0f;
        }
        else
        {
            wallNormal.x = 0.0f;
            wallNormal.y = (worldMovement.z > 0) ? -1.0f : 1.0f;
        }
    }
    
    // Handle collision response (sliding along walls)
    Vector2 movement = { worldMovement.x, worldMovement.z };
    
    // Calculate dot product for projection
    float dotProduct = Vector2DotProduct(movement, wallNormal);
    
    // Calculate perpendicular component (against the wall)
    Vector2 perpendicular = Vector2Scale(wallNormal, dotProduct);
    
    // Calculate parallel component (along the wall)
    Vector2 parallel = Vector2Subtract(movement, perpendicular);
    
    // Check if we have a meaningful parallel component
    float parallelMagnitude = Vector2Length(parallel);
    
    if (parallelMagnitude < 0.001f)
    {
        // Almost no parallel component, create a perpendicular slide direction
        parallel.x = -wallNormal.y;
        parallel.y = wallNormal.x;
        
        // Scale based on original movement magnitude
        float originalMagnitude = Vector2Length(movement);
        parallel = Vector2Scale(parallel, 0.5f * originalMagnitude);
    }
    else
    {
        // Scale down parallel component to prevent sliding into corners
        parallel = Vector2Scale(parallel, 0.8f);
    }
    
    // Create the adjusted world movement
    Vector3 adjustedWorldMovement =
    {
        parallel.x,
        worldMovement.y, // Keep vertical movement
        parallel.y
    };
    
    // Test if the adjusted movement is collision-free
    Vector3 adjustedTargetPos = Vector3Add(originalPos, adjustedWorldMovement);
    if (PlayerCollides(adjustedTargetPos, resources))
    {
        // Try a smaller movement (half the magnitude)
        adjustedWorldMovement = Vector3Scale(adjustedWorldMovement, 0.5f);
        
        adjustedTargetPos = Vector3Add(originalPos, adjustedWorldMovement);
        if (PlayerCollides(adjustedTargetPos, resources))
        {
            return Vector3Zero(); // Still colliding, cancel movement
        }
    }
    
    // Convert the world movement back to camera-relative coordinates
    Vector3 result;
    
    // Create a 2D version of the adjusted movement for projection
    Vector2 adjustedMovement2D = { adjustedWorldMovement.x, adjustedWorldMovement.z };
    Vector2 forward2D = { forward.x, forward.z };
    Vector2 right2D = { right.x, right.z };
    
    // Project onto forward and right vectors
    result.x = Vector2DotProduct(adjustedMovement2D, forward2D);
    result.y = Vector2DotProduct(adjustedMovement2D, right2D);
    result.z = localMovement.z;
    
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
    bool SHIFT = IsKeyDown(KEY_LEFT_SHIFT);
    if(SHIFT) 
    {
        straightMovementSpeed*=PLAYER_ACCELERATION;
        diagonalMovementSpeed*=PLAYER_ACCELERATION;
    }

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