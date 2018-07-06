/**
 * MIT License
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <load_prc_file.h>
#include <virtualFileSystem.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/loader.hpp>

class MainApp : public rppanda::ShowBase
{
public:
    MainApp(int argc, char* argv[]) : ShowBase()
    {
        // Setup window size, title and so on
        load_prc_file_data("",
            "window-title Render Pipeline - Car Demo");

        // configure panda3d in program.
        render_pipeline_ = std::make_unique<rpcore::RenderPipeline>();

        render_pipeline_->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
        VirtualFileSystem::get_global_ptr()->mount("../share/rpcpp_samples/06-Car", "/$$app", 0);

        render_pipeline_->create(argc, argv, this);

        // Set time of day
        render_pipeline_->get_daytime_mgr()->set_time("20:08");

        // Load the scene
        NodePath model = rpcore::RPLoader::load_model("/$$app/scene/scene.bam");
        model.reparent_to(rpcore::Globals::render);

        render_pipeline_->prepare_scene(model);

        // Init movement controller
        controller_ = new rpcore::MovementController(this);
        controller_->set_initial_position(
            LVecBase3f(-7.5f, -5.3f, 1.8f),
            LVecBase3f(-5.9f, -4.0f, 1.6f));
        controller_->setup();

        accept("l", [&](const Event*) { tour(); });
    }

    virtual ~MainApp() = default;

    ALLOC_DELETED_CHAIN(MainApp);

    void reload_shaders()
    {
        render_pipeline_->reload_shaders();
        render_pipeline_->prepare_scene(rpcore::Globals::render);
    }

    void tour()
    {
        rpcore::MovementController::MotionPathType mopath = {
            { LVecBase3(-10.8645000458, 9.76458263397, 2.13306283951), LVecBase3(-133.556228638, -4.23447799683, 0.0) },
            { LVecBase3(-10.6538448334, -5.98406457901, 1.68028640747), LVecBase3(-59.3999938965, -3.32706642151, 0.0) },
            { LVecBase3(9.58458328247, -5.63625621796, 2.63269257545), LVecBase3(58.7906494141, -9.40668964386, 0.0) },
            { LVecBase3(6.8135137558, 11.0153560638, 2.25509500504), LVecBase3(148.762527466, -6.41223621368, 0.0) },
            { LVecBase3(-9.07093334198, 3.65908527374, 1.42396306992), LVecBase3(245.362503052, -3.59927511215, 0.0) },
            { LVecBase3(-8.75390911102, -3.82727789879, 0.990055501461), LVecBase3(296.090484619, -0.604830980301, 0.0) },
        };
        controller_->play_motion_path(mopath, 3.0);
    }

private:
    std::unique_ptr<rpcore::RenderPipeline> render_pipeline_;
    PT(rpcore::MovementController) controller_;
};

int main(int argc, char* argv[])
{
    PT(MainApp) app = new MainApp(argc, argv);
    app->run();

    return 0;
}
