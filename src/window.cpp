#include "window.h"
#include <cstdlib>
#include "rlImGui.h"
#include <algorithm>

Window::Window(std::unique_ptr<Renderer> renderer, std::string name) {
    this->renderer = std::move(renderer);
    this->size = {0,0};
    this->name = name;
}

void Window::render() {
    if(ImGui::Begin(this->name.c_str())) {
        //lazyload texture(size)
        auto windowSize = ImGui::GetContentRegionAvail();
        if(abs(size.x - windowSize.x) > 0 || abs(size.y - windowSize.y) > 0) {
            float resolution_multiplayer = 16.0/9.0;

            float y;
            float x;
            float calculated_y = windowSize.y * resolution_multiplayer;
            float calculated_x = windowSize.x * 1.0/resolution_multiplayer;
            
            if(std::max(calculated_x, windowSize.x) < std::max(calculated_y, windowSize.y)) {
                y = windowSize.x * 1.0/resolution_multiplayer;
                x = windowSize.x;
            } else if(std::max(calculated_x, windowSize.x) > std::max(calculated_y, windowSize.y)) {
                y = windowSize.y;
                x = windowSize.y * resolution_multiplayer;
            } else {
                x = windowSize.x;
                y = windowSize.y;
            }

            if(y != size.y && x != size.x) {
                //resize
                UnloadRenderTexture(this->texture);
                this->texture = LoadRenderTexture(x, y);
                this->size = ImVec2{x, y};
            }
        }

        //render on texture
        BeginTextureMode(this->texture);
            this->renderer->render(Window_context{
                size
            });
        EndTextureMode();

        //show texture in ImGui window
        Rectangle viewRect = { 0 };
        viewRect.x = this->texture.texture.width / 2 - size.x / 2;
        viewRect.y = this->texture.texture.height / 2 - size.y / 2;
        viewRect.width = size.x;
        viewRect.height = -size.y;

        rlImGuiImageRect(&this->texture.texture, (int)size.x, (int)size.y, viewRect);

        
    }
    ImGui::End();
}

Window::~Window() {
    UnloadRenderTexture(this->texture);
}