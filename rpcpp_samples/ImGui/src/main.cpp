#include <load_prc_file.h>

#include <imgui.h>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/manager.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/globals.hpp>

#include "main.hpp"

World::World(int argc, char* argv[])
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "window-title Render Pipeline - Test");

    // configure panda3d in program.
    render_pipeline_ = std::make_unique<rpcore::RenderPipeline>();

    render_pipeline_->get_mount_mgr()->set_base_path("../share/render_pipeline");
    render_pipeline_->get_mount_mgr()->set_config_dir("../etc/rpsamples/imgui");
    render_pipeline_->create(argc, argv, this);

    // Set time of day
    render_pipeline_->get_daytime_mgr()->set_time(0.619f);

    // Init movement controller
    controller_ = std::make_unique<rpcore::MovementController>(this);
    controller_->set_initial_position(
        LVecBase3f(6.6f, -18.8f, 4.5f),
        LVecBase3f(4.7f, -16.7f, 3.4f));
    controller_->setup();

    auto plugin_mgr = render_pipeline_->get_plugin_mgr();
    if (plugin_mgr->is_plugin_enabled("imgui"))
    {
        rppanda::Messenger::get_global_instance()->send(
            "imgui-setup-context",
            EventParameter(new rppanda::FunctionalTask([this](rppanda::FunctionalTask* task) {
                ImGui::SetCurrentContext(std::static_pointer_cast<ImGuiContext>(task->get_user_data()).get());
                accept("imgui-new-frame", [this](auto) { on_imgui_new_frame(); });
                return AsyncTask::DS_done;
            }, "World::setup-imgui"))
        );
    }
    else
    {
        rpcore::RPObject::global_error("World", "imgui plugin is not enabled.");
    }
}

World::~World() = default;

void World::on_imgui_new_frame()
{
    static bool show_demo_window = true;
    static bool show_another_window = false;
    static LVecBase3f clear_color = LVecBase3f(0);

    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
    {
        static float f = 0.0f;
        static int counter = 0;
        ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
        ImGui::ColorEdit3("clear color", (float*)&clear_color[0]); // Edit 3 floats representing a color

        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
    if (show_demo_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        ImGui::ShowDemoWindow(&show_demo_window);
    }
}

int main(int argc, char* argv[])
{
    World(argc, argv).run();

    return 0;
}
