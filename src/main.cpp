#include <string>
#include "rlImGui.h"
#include "imgui.h"
#include "window.h"
#include "renderers.h"
#include "engine.h"
#include "implot.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    Window phasesWindow(std::make_unique<ThreePhaseRenderer>(), "phases");

    auto threePhaseRenderer = ((ThreePhaseRenderer*)phasesWindow.renderer.get());

    Engine engine;
    engine.phase_A.get()->currentData = threePhaseRenderer->phase_A;
    engine.phase_B.get()->currentData = threePhaseRenderer->phase_B;
    engine.phase_C.get()->currentData = threePhaseRenderer->phase_C;
    engine.startThread();

    std::vector<ChartRenderer::toRender> buffers;
    std::shared_ptr<Buffer<float>> bufer = std::make_shared<Buffer<float>>(500);

    buffers.push_back(ChartRenderer::toRender{engine.phase_A->historicData, RED});
    buffers.push_back(ChartRenderer::toRender{engine.phase_A_PWM->historicData, BLUE});
    // buffers.push_back(engine.phase_B->historicData);
    // buffers.push_back(engine.phase_C->historicData);
    Window phasesChart(std::make_unique<ChartRenderer>(buffers), "chart");

    SetConfigFlags(ConfigFlags::FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(ConfigFlags::FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "3phase");
    SetTargetFPS(120);
    rlImGuiSetup(true);
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    //--------------------------------------------------------------------------------------
    
    bool rawData;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLANK);
            rlImGuiBegin();

            phasesWindow.render();
            // phasesChart.render();

            if(ImGui::Begin("controls")) {
                float speed = engine.target_rpm->load();
                ImGui::SliderFloat("rpm", &speed, 0.1f, 200.0f, "%.2f");
                engine.target_rpm->store(speed);

                int size = engine.getBufferSize();
                ImGui::SliderInt("buffer size", &size, 0, 5000);
                engine.setBufferSize(size);

                int f = engine.getFreqnecyFetch();
                ImGui::SliderInt("freqnecy", &f, 0, 500);
                engine.setFreqnecyFetch(f);

                ImGui::Checkbox("rawData", &rawData);
            }
            ImGui::End();

            if(ImGui::Begin("all phases plot")) {
                
                if(ImPlot::BeginPlot("plot", ImVec2(-1,-1))) {
                    ImPlot::SetupAxes(NULL, NULL);
                    {
                        std::vector<float> values;
                        if(rawData) {
                            values = engine.phase_A->historicData->getRawData();
                        } else {
                            values = engine.phase_A->historicData->getVector();
                        }
                        float* xValues = new float[values.size()];
                        float* yValues = new float[values.size()];
                        for (int i = 0; i < values.size(); i++) {
                            xValues[i] = values[i];
                            yValues[i] = (float) i;
                        }
                        ImPlot::PlotLine("sinA", yValues, xValues, values.size());
                        delete xValues;
                        delete yValues;
                    }
                    {
                        std::vector<float> values;
                        if(rawData) {
                            values = engine.phase_A_PWM->historicData->getRawData();
                        } else {
                            values = engine.phase_A_PWM->historicData->getVector();
                        }
                        float* xValues = new float[values.size()];
                        float* yValues = new float[values.size()];
                        for (int i = 0; i < values.size(); i++) {
                            xValues[i] = values[i];
                            yValues[i] = (float) i;
                        }
                        ImPlot::PlotLine("pwmA", yValues, xValues, values.size());
                        delete xValues;
                        delete yValues;
                    }
                    {
                        std::vector<float> values;
                        if(rawData) {
                            values = engine.phase_B->historicData->getRawData();
                        } else {
                            values = engine.phase_B->historicData->getVector();
                        }
                        float* xValues = new float[values.size()];
                        float* yValues = new float[values.size()];
                        for (int i = 0; i < values.size(); i++) {
                            xValues[i] = values[i];
                            yValues[i] = (float) i;
                        }
                        ImPlot::PlotLine("sinB", yValues, xValues, values.size());
                        delete xValues;
                        delete yValues;
                    }
                    {
                        std::vector<float> values;
                        if(rawData) {
                            values = engine.phase_B_PWM->historicData->getRawData();
                        } else {
                            values = engine.phase_B_PWM->historicData->getVector();
                        }
                        float* xValues = new float[values.size()];
                        float* yValues = new float[values.size()];
                        for (int i = 0; i < values.size(); i++) {
                            xValues[i] = values[i];
                            yValues[i] = (float) i;
                        }
                        ImPlot::PlotLine("pwmB", yValues, xValues, values.size());
                        delete xValues;
                        delete yValues;
                    }
                    {
                        std::vector<float> values;
                        if(rawData) {
                            values = engine.phase_C->historicData->getRawData();
                        } else {
                            values = engine.phase_C->historicData->getVector();
                        }
                        float* xValues = new float[values.size()];
                        float* yValues = new float[values.size()];
                        for (int i = 0; i < values.size(); i++) {
                            xValues[i] = values[i];
                            yValues[i] = (float) i;
                        }
                        ImPlot::PlotLine("sinC", yValues, xValues, values.size());
                        delete xValues;
                        delete yValues;
                    }
                    {
                        std::vector<float> values;
                        if(rawData) {
                            values = engine.phase_C_PWM->historicData->getRawData();
                        } else {
                            values = engine.phase_C_PWM->historicData->getVector();
                        }
                        float* xValues = new float[values.size()];
                        float* yValues = new float[values.size()];
                        for (int i = 0; i < values.size(); i++) {
                            xValues[i] = values[i];
                            yValues[i] = (float) i;
                        }
                        ImPlot::PlotLine("pwmC", yValues, xValues, values.size());
                        delete xValues;
                        delete yValues;
                    }
                    {
                        std::vector<float> values;
                        if(rawData) {
                            values = engine.phase_first->historicData->getRawData();
                        } else {
                            values = engine.phase_first->historicData->getVector();
                        }
                        float* xValues = new float[values.size()];
                        float* yValues = new float[values.size()];
                        for (int i = 0; i < values.size(); i++) {
                            xValues[i] = values[i];
                            yValues[i] = (float) i;
                        }
                        ImPlot::PlotLine("first phase", yValues, xValues, values.size());
                        delete xValues;
                        delete yValues;
                    }
                    {
                        std::vector<float> values;
                        if(rawData) {
                            values = engine.phase_second->historicData->getRawData();
                        } else {
                            values = engine.phase_second->historicData->getVector();
                        }
                        float* xValues = new float[values.size()];
                        float* yValues = new float[values.size()];
                        for (int i = 0; i < values.size(); i++) {
                            xValues[i] = values[i];
                            yValues[i] = (float) i;
                        }
                        ImPlot::PlotLine("second phase", yValues, xValues, values.size());
                        delete xValues;
                        delete yValues;
                    }
                    {
                        std::vector<float> values;
                        if(rawData) {
                            values = engine.phase_third->historicData->getRawData();
                        } else {
                            values = engine.phase_third->historicData->getVector();
                        }
                        float* xValues = new float[values.size()];
                        float* yValues = new float[values.size()];
                        for (int i = 0; i < values.size(); i++) {
                            xValues[i] = values[i];
                            yValues[i] = (float) i;
                        }
                        ImPlot::PlotLine("third phase", yValues, xValues, values.size());
                        delete xValues;
                        delete yValues;
                    }
                    // ImPlot::SetupAxisLimits(ImAxis_X1, yValues[0], yValues[0] + values.size(), ImGuiCond_Always);
                }
                ImPlot::EndPlot();
            }
            ImGui::End();

            rlImGuiEnd();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}