#include <SDL2/SDL.h>
#include <vector>
#include <cmath>

struct vec3d
{
    float x, y, z;
};

struct triangle
{
    vec3d p[3];
};

struct mesh
{
    std::vector<triangle> tris;
};

struct mat4x4
{
    float m[4][4] = { 0 };
};

class GameEngine
{
    public:
        GameEngine(float displayWidth, float displayHeight);
    private:
        // variables needed to project 3d coordinates to 2d screen
        int frameRate { 23 };
        float fWidth { }, fHeight { }, fAspectRatio { }, fNear { 0.1f }, fFar { 1000.0f }, fFov { 90.0f }, fFovRadians { }, fTheta { };

        // matrix that converts 3d coordinates to 2d coordinates when multiplied
        mat4x4 matProj;

        SDL_Window* Window { nullptr };
        SDL_Renderer* Renderer { nullptr };

        bool isRunning { true };
        void gameLoop();
        
        // functions
        void multiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m);
        void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3);

        // objects
        mesh meshCube;
};