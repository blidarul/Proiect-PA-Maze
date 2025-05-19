#include "camera.h"
#include "raymath.h"
#include <math.h>
#include "raylib.h"
#include "sound.h"
#include "raygui.h"
#include "title.h" // Needed for app_settings
#include "settings.h" // Needed for MIN_PLAYER_HEIGHT_SETTING, MAX_PLAYER_HEIGHT_SETTING, DEFAULT_PLAYER_HEIGHT_SETTING

static float currentStamina = MAX_STAMINA;
static Rectangle bounds = { .x=SCREEN_WIDTH-MINIMAP_DIST_FROM_BORDER-STAMINA_BAR_LENGHT,
                            .y=SCREEN_HEIGHT-MINIMAP_DIST_FROM_BORDER-STAMINA_BAR_HEIGHT,
                            .height=STAMINA_BAR_HEIGHT,
                            .width=STAMINA_BAR_LENGHT};

static inline int MAX(int a, int b)
{
    return a > b ? a : b;
}

static inline int MIN(int a, int b)
{
    return a > b ? b : a;
}

Camera InitializeCamera(void)
{
    Camera camera = {0};
    camera.position = (Vector3){1.5f, app_settings.player_height, 1.5f}; 
    camera.target = (Vector3){0.5f, app_settings.player_height, 0.5f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = PLAYER_FOV;
    camera.projection = CAMERA_PERSPECTIVE;
    return camera;
}

// Helper: returns true if player collides with any wall
bool PlayerCollides(Vector3 playerPos, GameResources resources)
{
    int mapWidth = resources.cubicimage.width;
    int mapHeight = resources.cubicimage.height;

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
            Color pixel = resources.mapPixels[z * mapWidth + x];
            if (pixel.r == 255)
            {
                Rectangle cellRect = {x, z, 1.0f, 1.0f};
                Vector2 playerPos2D = {playerPos.x, playerPos.z};
                if (CheckCollisionCircleRec(playerPos2D, PLAYER_RADIUS, cellRect))
                {
                    collision = true;
                    break;
                }
            }
        }
        if (collision)
            break;
    }
#ifdef DEBUG_COLLISIONS
    TraceLog(LOG_INFO, "[PlayerCollides] Pos:(%.2f,%.2f) Bounds:x[%d-%d] z[%d-%d] Result:%s",
             playerPos.x, playerPos.z, minX, maxX, minZ, maxZ, collision ? "COLLIDE" : "CLEAR");
#endif
    return collision;
}

Vector3 HandleCollisions(Camera *camera, Vector3 localMovement, GameResources resources)
{
    Vector3 originalPos = camera->position;

    Vector3 forward = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera->up));

    Vector3 worldMovement =
        {
            forward.x * localMovement.x + right.x * localMovement.y,
            localMovement.z,
            forward.z * localMovement.x + right.z * localMovement.y};

    Vector3 targetPos = Vector3Add(originalPos, worldMovement);

    if (!PlayerCollides(targetPos, resources))
    {
        return localMovement;
    }

    Vector2 wallNormal = {0};

    Vector3 xTestPos = originalPos;
    xTestPos.x += worldMovement.x;
    bool xCollision = PlayerCollides(xTestPos, resources);

    Vector3 zTestPos = originalPos;
    zTestPos.z += worldMovement.z;
    bool zCollision = PlayerCollides(zTestPos, resources);

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

        wallNormal = Vector2Normalize(wallNormal);
    }
    else
    {
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

    Vector2 movement = {worldMovement.x, worldMovement.z};

    float dotProduct = Vector2DotProduct(movement, wallNormal);

    Vector2 perpendicular = Vector2Scale(wallNormal, dotProduct);

    Vector2 parallel = Vector2Subtract(movement, perpendicular);

    float parallelMagnitude = Vector2Length(parallel);

    if (parallelMagnitude < 0.001f)
    {
        parallel.x = -wallNormal.y;
        parallel.y = wallNormal.x;

        float originalMagnitude = Vector2Length(movement);
        parallel = Vector2Scale(parallel, 0.5f * originalMagnitude);
    }
    else
    {
        parallel = Vector2Scale(parallel, 0.8f);
    }

    Vector3 adjustedWorldMovement =
        {
            parallel.x,
            worldMovement.y,
            parallel.y};

    Vector3 adjustedTargetPos = Vector3Add(originalPos, adjustedWorldMovement);
    if (PlayerCollides(adjustedTargetPos, resources))
    {
        adjustedWorldMovement = Vector3Scale(adjustedWorldMovement, 0.5f);

        adjustedTargetPos = Vector3Add(originalPos, adjustedWorldMovement);
        if (PlayerCollides(adjustedTargetPos, resources))
        {
            return Vector3Zero();
        }
    }

    Vector3 result;

    Vector2 adjustedMovement2D = {adjustedWorldMovement.x, adjustedWorldMovement.z};
    Vector2 forward2D = {forward.x, forward.z};
    Vector2 right2D = {right.x, right.z};

    result.x = Vector2DotProduct(adjustedMovement2D, forward2D);
    result.y = Vector2DotProduct(adjustedMovement2D, right2D);
    result.z = localMovement.z;

    return result;
}

void DrawStaminaBar()
{
    // Store the original color of the progress bar's filled part
    Color originalColor = GetColor(GuiGetStyle(PROGRESSBAR, BASE_COLOR_PRESSED));

    // Set the progress bar's filled part color to yellow
    GuiSetStyle(PROGRESSBAR, BASE_COLOR_PRESSED, ColorToInt(GREEN));

    GuiProgressBar(bounds, NULL, NULL, &currentStamina, 0.0f, MAX_STAMINA);

    // Restore the original color so other progress bars are not affected
    GuiSetStyle(PROGRESSBAR, BASE_COLOR_PRESSED, ColorToInt(originalColor));
}

void UpdatePlayerMovement(Camera *camera, GameResources resources)
{
    camera->position.y = app_settings.player_height;

    const float sqrt2 = 1.41421356237f;
    float zoom = 0.0f;
    Vector3 rotation = {0, 0, 0};
    Vector3 localMovement = {0, 0, 0};

    float delta = GetFrameTime();

    float effectiveMouseSensitivity = (app_settings.sensitivity / 50.0f) * DEFAULT_MOUSE_SENSITIVITY;

    rotation.x = GetMouseDelta().x * effectiveMouseSensitivity;
    rotation.y = GetMouseDelta().y * effectiveMouseSensitivity;

    float straightMovementSpeed = PLAYER_SPEED * delta;
    float diagonalMovementSpeed = straightMovementSpeed / sqrt2;

    bool W = IsKeyDown(KEY_W);
    bool A = IsKeyDown(KEY_A);
    bool S = IsKeyDown(KEY_S);
    bool D = IsKeyDown(KEY_D);
    bool SHIFT = IsKeyDown(KEY_LEFT_SHIFT);

    if (SHIFT && (W || A || S || D) && currentStamina > 0.0f)
    {
        straightMovementSpeed *= PLAYER_ACCELERATION;
        diagonalMovementSpeed *= PLAYER_ACCELERATION;

        currentStamina -= STAMINA_DRAIN_RATE * delta;
        if (currentStamina < 0.0f)
            currentStamina = 0.0f;

    }
    else
    {
        if (currentStamina < MAX_STAMINA)
        {
            currentStamina += (STAMINA_RECOVERY * delta);

            if (currentStamina > MAX_STAMINA)
                currentStamina = MAX_STAMINA;
        }
    }

    #ifdef DEBUG_STAMINA
        TraceLog(LOG_INFO,"Player stamina: %.2f/%.2f",currentStamina,MAX_STAMINA);
    #endif

    if (W)
    {
        if (A || D)
            localMovement.x += diagonalMovementSpeed;
        else
            localMovement.x += straightMovementSpeed;
    }
    if (S)
    {
        if (A || D)
            localMovement.x -= diagonalMovementSpeed;
        else
            localMovement.x -= straightMovementSpeed;
    }
    if (A)
    {
        if (W || S)
            localMovement.y -= diagonalMovementSpeed;
        else
            localMovement.y -= straightMovementSpeed;
    }
    if (D)
    {
        if (W || S)
            localMovement.y += diagonalMovementSpeed;
        else
            localMovement.y += straightMovementSpeed;
    }

    localMovement = HandleCollisions(camera, localMovement, resources);

    if (localMovement.x || localMovement.z)
    {
        UpdateStepSounds();
    }

    UpdateCameraPro(camera, localMovement, rotation, zoom);

}
