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

void HandleCollisions(Camera *camera, Vector3 *localMovement, GameResources resources, Vector3 oldCamPos)
{
    // Calculate position delta
    Vector3 positionDelta =
    {
        camera->position.x - oldCamPos.x,
        camera->position.y - oldCamPos.y,
        camera->position.z - oldCamPos.z
    };

    // Increase precision for collision testing
    const int numSteps = 5;
    bool collision = false;
    Vector2 wallNormal = {0};

    Vector3 testPos = oldCamPos;

    // Check intermediate positions for smoother collision detection
    for (int step = 1; step <= numSteps && !collision; step++)
    {
        float t = (float)step / numSteps;
        testPos.x = oldCamPos.x + positionDelta.x * t;
        testPos.y = oldCamPos.y + positionDelta.y * t;
        testPos.z = oldCamPos.z + positionDelta.z * t;

        // Use PlayerCollides for collision detection
        if (PlayerCollides(testPos, resources))
        {
            collision = true;

            // Approximate wall normal (simple: direction of movement)
            float dx = positionDelta.x;
            float dz = positionDelta.z;
            if (fabs(dx) > fabs(dz))
            {
                wallNormal.x = (dx > 0) ? 1.0f : -1.0f;
                wallNormal.y = 0.0f;
            }
            else
            {
                wallNormal.x = 0.0f;
                wallNormal.y = (dz > 0) ? 1.0f : -1.0f;
            }
        }
    }

    if (collision)
    {
        // Handle collision response (sliding along walls, etc.)
        Vector2 movement =
        {
            positionDelta.x,
            positionDelta.z
        };

        float dotProduct = movement.x * wallNormal.x + movement.y * wallNormal.y;

        Vector2 perpendicular =
        {
            wallNormal.x * dotProduct,
            wallNormal.y * dotProduct
        };

        Vector2 parallel =
        {
            movement.x - perpendicular.x,
            movement.y - perpendicular.y
        };

        // Reset position to old position
        camera->position = oldCamPos;

        // Apply the parallel movement (sliding along wall)
        camera->position.x += parallel.x;
        camera->position.z += parallel.y;

        // Extra safety check - if our new position collides with any wall, retreat completely
        Vector3 newPos = camera->position;
        if (PlayerCollides(newPos, resources))
        {
            camera->position = oldCamPos;
        }
    }
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
    
    Vector3 oldCamPos = camera->position;

    // Update camera using the (potentially modified) localMovement and rotation
    UpdateCameraPro(camera, localMovement, rotation, zoom);

    // Handle collision detection - modifies localMovement if necessary
    HandleCollisions(camera, &localMovement, resources, oldCamPos);
}