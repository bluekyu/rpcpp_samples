/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016, Center of human-centered interaction for coexistence.
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

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <texturePool.h>
#include <load_prc_file.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>

void tour(const Event* ev, void* user_data)
{
    rpcore::MovementController* mc = reinterpret_cast<rpcore::MovementController*>(user_data);
    rpcore::MovementController::MotionPathType mopath ={
        { LVecBase3(-10.8645000458, 9.76458263397, 2.13306283951), LVecBase3(-133.556228638, -4.23447799683, 0.0) },
        { LVecBase3(-10.6538448334, -5.98406457901, 1.68028640747), LVecBase3(-59.3999938965, -3.32706642151, 0.0) },
        { LVecBase3(9.58458328247, -5.63625621796, 2.63269257545), LVecBase3(58.7906494141, -9.40668964386, 0.0) },
        { LVecBase3(6.8135137558, 11.0153560638, 2.25509500504), LVecBase3(148.762527466, -6.41223621368, 0.0) },
        { LVecBase3(-9.07093334198, 3.65908527374, 1.42396306992), LVecBase3(245.362503052, -3.59927511215, 0.0) },
        { LVecBase3(-8.75390911102, -3.82727789879, 0.990055501461), LVecBase3(296.090484619, -0.604830980301, 0.0) },
    };
    mc->play_motion_path(mopath, 3.0);
}

int main(int argc, char* argv[])
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "window-title Render Pipeline - Car Demo");

    // configure panda3d in program.
    rpcore::RenderPipeline* render_pipeline = new rpcore::RenderPipeline(argc, argv);
    render_pipeline->get_mount_mgr()->set_base_path("../share/render_pipeline");
    render_pipeline->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
    render_pipeline->create();

    render_pipeline->get_daytime_mgr()->set_time("20:08");

    // Load the scene
    NodePath model = rpcore::Globals::base->get_window_framework()->load_model(rpcore::Globals::render,
        "../share/render_pipeline/models/06-Car/scene.bam");
    render_pipeline->prepare_scene(rpcore::Globals::render);

    // Init movement controller
    std::shared_ptr<rpcore::MovementController> controller = std::make_shared<rpcore::MovementController>(rpcore::Globals::base);
    controller->set_initial_position(
        LVecBase3f(-7.5f, -5.3f, 1.8f),
        LVecBase3f(-5.9f, -4.0f, 1.6f));
    controller->setup();

    render_pipeline->get_showbase()->accept("l", tour, controller.get());

    render_pipeline->run();

    delete render_pipeline;

    return 0;
}
