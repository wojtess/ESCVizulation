#pragma once
#include "raylib.h"
#include <memory>
#include "imgui.h"
#include <string>

struct Window_context {
    ImVec2 size;
};

class Renderer {
    public:
        virtual void render(Window_context) = 0;
};

class Window {
private:
    //texture is lazy loaded, check render()
    RenderTexture texture;
    ImVec2 size;
    std::string name;
public:
    std::unique_ptr<Renderer> renderer;
    Window(std::unique_ptr<Renderer>, std::string);
    void render();
    ~Window();
};