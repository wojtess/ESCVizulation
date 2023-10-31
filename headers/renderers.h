#include "window.h"
#include <atomic>
#include "helpers.h"
#include <vector>
#include <memory>
#include "raylib.h"

class ThreePhaseRenderer: public Renderer {
    public:
        std::shared_ptr<std::atomic<float>> phase_A;
        std::shared_ptr<std::atomic<float>> phase_B;
        std::shared_ptr<std::atomic<float>> phase_C;
        void render(Window_context);
        ThreePhaseRenderer();
};


class ChartRenderer: public Renderer {
    public:

        struct toRender {
            std::shared_ptr<Buffer<float>> buffer;
            Color color;
        };

        std::vector<toRender> buffers;

        ChartRenderer(std::vector<toRender>);
        void render(Window_context);
};