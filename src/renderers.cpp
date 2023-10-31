#include "renderers.h"
#include "raylib.h"
#include "raymath.h"

#include <cmath>

ThreePhaseRenderer::ThreePhaseRenderer() {
    this->phase_A = std::make_shared<std::atomic<float>>();
    this->phase_B = std::make_shared<std::atomic<float>>();
    this->phase_C = std::make_shared<std::atomic<float>>();
}

void ThreePhaseRenderer::render(Window_context context) {
    ClearBackground(WHITE);

    float phaseA = this->phase_A->load();
    float phaseB = this->phase_B->load();;
    float phaseC = this->phase_C->load();

    Vector2 phaseAVector = Vector2{(float)(cos(0) * phaseA), (float)(sin(0) * phaseA)};
    Vector2 phaseBVector = Vector2{(float)(cos(120 * PI/180.0) * phaseB), (float)(sin(120 * PI/180.0) * phaseB)};
    Vector2 phaseCVector = Vector2{(float)(cos(240 * PI/180.0) * phaseC), (float)(sin(240 * PI/180.0) * phaseC)};
    
    Vector2 phaseAll = Vector2{(phaseAVector.x + phaseBVector.x + phaseCVector.x), (phaseAVector.y + phaseBVector.y + phaseCVector.y)};

    DrawText("purple is first phase, blue is second\ngreen is third, red line is all them combined", 200, (float)context.size.y - 80, 20, RED);

        char buf[6];

        DrawRectangleRec(Rectangle{0,(float)context.size.y - 80, 50, 80}, PURPLE);
        DrawRectangle(0,context.size.y - 80, 50, (1 - (phaseA + 1) / 2) * 80, WHITE);
        snprintf(buf, sizeof(buf), "%+.2f", phaseA);
        DrawText(buf, 0, context.size.y - 100, 20, PURPLE);

        DrawRectangleRec(Rectangle{50 ,(float)context.size.y - 80, 50, 80}, BLUE);
        DrawRectangle(50 ,context.size.y - 80, 50, (1 - (phaseB + 1) / 2) * 80, WHITE);
        snprintf(buf, sizeof(buf), "%+.2f", phaseB);
        DrawText(buf, 50, context.size.y - 100, 20, BLUE);

        DrawRectangleRec(Rectangle{100 ,(float)context.size.y - 80, 50, 80}, GREEN);
        DrawRectangle(100 ,context.size.y - 80, 50, (1 - (phaseC + 1) / 2) * 80, WHITE);
        snprintf(buf, sizeof(buf), "%+.2f", phaseC);
        DrawText(buf, 100, context.size.y - 100, 20, GREEN);

        DrawLineEx(Vector2{0,(float)context.size.y - 40}, Vector2{150, (float)context.size.y - 40}, 2, Color{230, 41, 55, 240});


        BeginScissorMode(0, 0, (float)context.size.x, (float)context.size.y - 100);
            DrawRectangleLinesEx(Rectangle{0, 0, (float)context.size.x, (float)context.size.y - 100}, 4.0f, RED);

            int x = context.size.x / 2;
            int y = (context.size.y - 100) / 2;
            int r = std::min(x,y) - 10;
            DrawCircleLines(x, y, r, RED);
            Vector2 startingPoint = Vector2{(float)x,(float)y};
            
            DrawLineEx(startingPoint, Vector2Add(startingPoint, Vector2{(float)r * phaseAll.x, (float)r * phaseAll.y}), 2.0, RED);

            DrawLineEx(startingPoint, Vector2Add(startingPoint, Vector2{r * phaseAVector.x, r * phaseAVector.y}), 2.0, PURPLE);
            DrawLineEx(startingPoint, Vector2Add(startingPoint, Vector2{r * phaseBVector.x, r * phaseBVector.y}), 2.0, BLUE);
            DrawLineEx(startingPoint, Vector2Add(startingPoint, Vector2{r * phaseCVector.x, r * phaseCVector.y}), 2.0, GREEN);
        EndScissorMode();
}

void ChartRenderer::render(Window_context context) {
    ClearBackground(WHITE);
    auto border = 10;
    auto width = context.size.x - border;
    auto height = context.size.y - border;
    for(auto buffer:this->buffers) {
        auto data = buffer.buffer->getVector();
        
        for(int i = 1; i < (int)data.size(); i++) {
            DrawLineEx(Vector2{(float)(i - 1)/(int)data.size() * width + border / 2, context.size.y - (data[i-1] * height + border / 2)}, Vector2{(float)i/(int)data.size() * width + border / 2, context.size.y - (data[i] * height + border / 2)}, 1.0, buffer.color);
        }
    }
}

ChartRenderer::ChartRenderer(std::vector<toRender> buffers) {
    this->buffers = buffers;
}