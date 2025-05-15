#include "camera.h"
#include "raymath.h"
#include <math.h>

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

void UpdatePlayerMovement(Camera* camera, Vector3* oldCamPos, Vector3* oldCamTarget)
{
    // Store current position and target
    *oldCamPos = camera->position;
    *oldCamTarget = camera->target;
    
    // Update camera (handles input)
    UpdateCamera(camera, CAMERA_FIRST_PERSON);
}

void HandleCollisions(Camera* camera, Vector3 oldCamPos, Vector3 oldCamTarget, 
                     GameResources resources, int playerCellX, int playerCellY)
{
    // Calculate position delta
    Vector3 positionDelta = {
        camera->position.x - oldCamPos.x,
        camera->position.y - oldCamPos.y,
        camera->position.z - oldCamPos.z
    };
    
    // Calculate direction change separately
    Vector3 oldDirection = {
        oldCamTarget.x - oldCamPos.x,
        oldCamTarget.y - oldCamPos.y,
        oldCamTarget.z - oldCamPos.z
    };
    
    Vector3 newDirection = {
        camera->target.x - camera->position.x,
        camera->target.y - camera->position.y,
        camera->target.z - camera->position.z
    };
    
    // Increase precision for collision testing
    const int numSteps = 5;
    bool collision = false;
    Vector2 wallNormal = {0};
    
    // Check intermediate positions for smoother collision detection
    for (int step = 1; step <= numSteps && !collision; step++) {
        // Test at intermediate positions
        float t = (float)step / numSteps;
        Vector3 testPos = {
            oldCamPos.x + positionDelta.x * t,
            oldCamPos.y + positionDelta.y * t, 
            oldCamPos.z + positionDelta.z * t
        };
        
        Vector2 playerPos = { testPos.x, testPos.z };
        
        // Slightly larger radius to prevent slipping through walls
        const float extendedRadius = PLAYER_RADIUS * 1.2f;
        
        // Check surrounding cells in a wider area
        for (int y = playerCellY - 3; y <= playerCellY + 3 && y < resources.cubicmap.height && !collision; y++) {
            for (int x = playerCellX - 3; x <= playerCellX + 3 && x < resources.cubicmap.width && !collision; x++) {
                if (y < 0 || x < 0) continue;
                
                if ((resources.mapPixels[y*resources.cubicmap.width + x].r == 255) &&
                    (CheckCollisionCircleRec(playerPos, extendedRadius,
                    (Rectangle){ resources.mapPosition.x - 0.5f + x*1.0f, 
                                resources.mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f })))
                {
                    collision = true;
                    
                    // Calculate wall normal
                    float wallCenterX = resources.mapPosition.x - 0.5f + x*1.0f + 0.5f;
                    float wallCenterZ = resources.mapPosition.z - 0.5f + y*1.0f + 0.5f;
                    
                    float dx = playerPos.x - wallCenterX;
                    float dz = playerPos.y - wallCenterZ;
                    
                    if (fabs(dx) > fabs(dz)) {
                        wallNormal.x = (dx > 0) ? 1.0f : -1.0f;
                        wallNormal.y = 0.0f;
                    } else {
                        wallNormal.x = 0.0f;
                        wallNormal.y = (dz > 0) ? 1.0f : -1.0f;
                    }
                }
            }
        }
    }
    
    if (collision)
    {
        // Handle collision response (sliding along walls, etc.)
        // ...existing collision handling code...
        
        // Calculate movement vector
        Vector2 movement = { 
            positionDelta.x,
            positionDelta.z 
        };
        
        // Project movement along the wall
        float dotProduct = movement.x * wallNormal.x + movement.y * wallNormal.y;
        
        Vector2 perpendicular = {
            wallNormal.x * dotProduct,
            wallNormal.y * dotProduct
        };
        
        // Only the parallel component remains
        Vector2 parallel = {
            movement.x - perpendicular.x,
            movement.y - perpendicular.y
        };
        
        // Scale down the parallel movement slightly for safety
        parallel.x *= 0.9f;
        parallel.y *= 0.9f;
        
        // Apply rotation changes regardless of collision
        Vector3 rotationChange = {
            newDirection.x - oldDirection.x,
            newDirection.y - oldDirection.y,
            newDirection.z - oldDirection.z
        };
        
        // Reset position to old position
        camera->position = oldCamPos;
        
        // Apply the parallel movement (sliding along wall)
        camera->position.x += parallel.x;
        camera->position.z += parallel.y;
        
        // Update target based on our new position and the rotation change
        camera->target.x = camera->position.x + oldDirection.x + rotationChange.x;
        camera->target.y = camera->position.y + oldDirection.y + rotationChange.y;
        camera->target.z = camera->position.z + oldDirection.z + rotationChange.z;
        
        // Extra safety check - if our new position collides with any wall, retreat completely
        Vector2 newPos = { camera->position.x, camera->position.z };
        bool secondaryCollision = false;
        
        // Safety check code...
        for (int y = playerCellY - 2; y <= playerCellY + 2 && y < resources.cubicmap.height; y++) {
            for (int x = playerCellX - 2; x <= playerCellX + 2 && x < resources.cubicmap.width; x++) {
                if (y < 0 || x < 0) continue;
                
                if ((resources.mapPixels[y*resources.cubicmap.width + x].r == 255) &&
                    (CheckCollisionCircleRec(newPos, PLAYER_RADIUS * 1.1f,
                    (Rectangle){ resources.mapPosition.x - 0.5f + x*1.0f, 
                                resources.mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f })))
                {
                    secondaryCollision = true;
                    break;
                }
            }
            if (secondaryCollision) break;
        }
        
        // If still in wall, revert to original position completely
        if (secondaryCollision) {
            camera->position = oldCamPos;
        }
    }
}