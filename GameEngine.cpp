#include "GameEngine.hpp"

GameEngine::GameEngine(float displayWidth, float displayHeight)
{
    meshCube.tris = {
        // collection of triangles that make up square faces | two triangles per face on a 6 sided cube with vertices entered in order clockwise
        // SOUTH
        { 0.0f, 0.0f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 0.0f }, // inits one triangle
        { 0.0f, 0.0f, 0.0f,     1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f },

        // EAST
        { 1.0f, 0.0f, 0.0f,     1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 1.0f },

        // NORTH
        { 1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f,     0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f },

        // WEST
        { 0.0f, 0.0f, 1.0f,     0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f,     0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f },

        // TOP
        { 0.0f, 1.0f, 0.0f,     0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f,     1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f },

        // BOTTOM
        { 1.0f, 0.0f, 1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f,     0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f },
    };
    SDL_Init(SDL_INIT_EVERYTHING);
    fWidth = displayWidth;
    fHeight = displayHeight;
    fAspectRatio = fWidth / fHeight;
    fFovRadians = 1.0f / tanf(fFov * 0.5f / 180.0f * M_PI);

    matProj.m[0][0] = fAspectRatio * fFovRadians;
    matProj.m[1][1] = fFovRadians;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;

    SDL_CreateWindowAndRenderer(fWidth, fHeight, 0, &Window, &Renderer);
    gameLoop();
}

void GameEngine::gameLoop()
{
    SDL_Event e;
    while (isRunning)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            {
                isRunning = false;
            }
        }
        // clear screen
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderClear(Renderer);

        mat4x4 matRotZ, matRotX;
        fTheta += 1.0f * (float)frameRate * 0.001f;

        // z rotation matrix
        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // x rotation matrix
        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fTheta * 0.5f);
        matRotX.m[1][2] = sinf(fTheta * 0.5f);
        matRotX.m[2][1] = -sinf(fTheta * 0.5f);
        matRotX.m[2][2] = cosf(fTheta * 0.5f);
        matRotX.m[3][3] = 1;

        // draw objects
        for (triangle tri : meshCube.tris)
        {
            triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

            multiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
            multiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
            multiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);


            multiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
            multiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
            multiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

            triTranslated = triRotatedZX;
            triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
            triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
            triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

            multiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
            multiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
            multiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

            // scale into view - prev = -1 - 1
            // translate to 0 - 2
            triProjected.p[0].x += 1.0f;    triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f;    triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f;    triProjected.p[2].y += 1.0f;

            // adjust to aspect ratio
            triProjected.p[0].x *= 0.5f * fWidth;   triProjected.p[0].y *= 0.5f * fHeight;
            triProjected.p[1].x *= 0.5f * fWidth;   triProjected.p[1].y *= 0.5f * fHeight;
            triProjected.p[2].x *= 0.5f * fWidth;   triProjected.p[2].y *= 0.5f * fHeight;

            drawTriangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y);
        }

        SDL_RenderPresent(Renderer);
        SDL_Delay(frameRate);
    }
}

void GameEngine::multiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
{
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f)
    {
        o.x /= w; o.y /= w; o.z /= w;
    }
}

void GameEngine::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    SDL_RenderDrawLineF(Renderer, x1, y1, x2, y2);
    SDL_RenderDrawLineF(Renderer, x2, y2, x3, y3);
    SDL_RenderDrawLineF(Renderer, x3, y3, x1, y1);
}