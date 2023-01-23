#include "raylib.h"
struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float update_time;
    float running_time;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return (data.pos.y >= windowHeight - data.rec.height);
}

AnimData updateRunningTime(AnimData scarfyData, float dT, int maxFrame)
{
    // update running_time
    scarfyData.running_time += dT;
    if (scarfyData.running_time >= scarfyData.update_time)
    {
        scarfyData.running_time = 0.0;
        // update animation frame
        scarfyData.rec.x = scarfyData.frame * scarfyData.rec.width;
        scarfyData.frame++;
        if (scarfyData.frame > maxFrame)
        {
            scarfyData.frame = 0;
        }
    }
    return scarfyData;
}

AnimData initNebula(AnimData nebula, Texture2D neb, int i, int wind_dim)
{
    nebula.rec.x = 0.0;
    nebula.rec.y = 0.0;
    nebula.rec.width = (float)(neb.width / 8);
    nebula.rec.height = (float)(neb.height / 8);
    nebula.pos.x = (float)(wind_dim + i * 400);
    nebula.pos.y = (float)(wind_dim - neb.height / 8);
    nebula.frame = 0;
    nebula.update_time = 1.0 / (12.0 + i * 4);
    nebula.running_time = 0;
    return nebula;
}
AnimData initScarfy(Texture2D scarfy, int width, int height)
{
    AnimData scarfyData;
    scarfyData.rec.x = 0.0;
    scarfyData.rec.y = 0.0;
    scarfyData.rec.width = (float)(scarfy.width / 6);
    scarfyData.rec.height = (float)(scarfy.height);
    scarfyData.pos.x = (float)(width / 2 - scarfyData.rec.width);
    scarfyData.pos.y = (float)(height - scarfyData.rec.height);
    scarfyData.frame = 0;
    scarfyData.update_time = 1.0 / 12.0;
    scarfyData.running_time = 0;
    return scarfyData;
}

int main()
{
    // Window dimensions
    int window_dimensions[2] = {512, 380};

    // initialize the window
    InitWindow(window_dimensions[0], window_dimensions[1], "runner");

    // initialize audio deveice
    InitAudioDevice();
    Sound game_over_sound = LoadSound("./sound/game_over.mp3");
    Sound jump_sound = LoadSound("./sound/jump.mp3");
    Sound win_sound = LoadSound("./sound/win.mp3");

    int win = 0, lose = 0;
    int velocity{0};
    // jum velocity (pixels/second)
    const int jumpVel{-600};
    bool isInAire{true};

    // accelaration due to gravity (pixels/fram)/frame
    const int gravity{1'000};

    // nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    AnimData Tnebula[6];
    const int n = 6;

    // initialise nebula
    for (int i = 0; i < n; i++)
    {
        Tnebula[i] = initNebula(Tnebula[i], nebula, i + 1, window_dimensions[1]);
    }

    float finish_line{Tnebula[n - 1].pos.x};
    // nebula X velocity (pixels/second)
    const int nebVel{-200};

    // scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");

    AnimData scarfyData = initScarfy(scarfy, window_dimensions[0], window_dimensions[1]);

    // bg
    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float bgX = 0;
    float mgX = 0;
    float fgX = 0;
    bool collision{};
    // audio
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {

        // delta time(time since last frame)
        const float dT{GetFrameTime()};
        // start drawwing
        BeginDrawing();
        ClearBackground(WHITE);

        // draw background
        Vector2 bgPos{bgX, 0.0};
        Vector2 bg2Pos{bgX + 2 * background.width, 0.0};
        DrawTextureEx(background, bgPos, 0.0, 2.0, WHITE);
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);
        bgX -= 20 * dT;
        if (bgX <= -background.width * 2)
        {
            bgX = 0.0;
        }

        // draw midground
        Vector2 midground1Pos{mgX, 0.0};
        Vector2 midground2Pos{mgX + midground.width * 2, 0.0};
        DrawTextureEx(midground, midground1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(midground, midground2Pos, 0.0, 2.0, WHITE);
        mgX -= 40 * dT;
        if (mgX <= -midground.width * 2)
        {
            mgX = 0.0;
        }

        // draw foreground
        Vector2 foreground1Pos{fgX, 0};
        Vector2 foreground2Pos{fgX + midground.width * 2, 0.0};
        DrawTextureEx(foreground, foreground1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(foreground, foreground2Pos, 0.0, 2.0, WHITE);
        fgX -= 80 * dT;
        if (fgX <= -midground.width * 2)
        {
            fgX = 0.0;
        }

        if (isOnGround(scarfyData, window_dimensions[1]))
        {
            // stop gravity
            velocity = 0;
            isInAire = false;
        }
        else
        {
            // Apply gravity
            velocity += gravity * dT;
            isInAire = true;
        }

        // check for jumpping
        if (IsKeyPressed(KEY_SPACE) && !isInAire)
        {
            // play the  song
            PlaySoundMulti(jump_sound);
            velocity += jumpVel;
        }

        // updte scarfy position
        scarfyData.pos.y += velocity * dT;

        // update scanfy's animation frame
        if (!isInAire)
        {
            scarfyData = updateRunningTime(scarfyData, dT, 5);
        }

        for (int i = 0; i < n; i++)
        {
            // update nebula position
            Tnebula[i].pos.x += nebVel * dT;
            // update nebula animation frame
            Tnebula[i] = updateRunningTime(Tnebula[i], dT, 7);
        }
        // update finish ligne
        finish_line += nebVel * dT;
        for (AnimData nebula : Tnebula)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad};
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height};
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }

        if (collision)
        {
            if (lose == 0)
            {
                // play the game over song
                PlaySoundMulti(game_over_sound);
                lose = 1;
            }

            //  lose the game
            DrawText("Game Over", window_dimensions[0] / 4, window_dimensions[1] / 2, 30, RED);
        }
        else if (scarfyData.pos.x > finish_line)
        {
            if (win == 0)
            {
                // play the sound
                PlaySoundMulti(win_sound);
                win = 1;
            }

            // win the game
            DrawText("You win", window_dimensions[0] / 2, window_dimensions[1] / 2, 25, RED);
        }
        else
        {
            // draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }

        // draw nebula
        for (int i = 0; i < n; i++)
        {
            DrawTextureRec(nebula, Tnebula[i].rec, Tnebula[i].pos, RED);
        }

        // stop drawwing
        EndDrawing();
    }

    StopSoundMulti();

    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);

    UnloadSound(game_over_sound);
    UnloadSound(jump_sound);
    UnloadSound(win_sound);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}