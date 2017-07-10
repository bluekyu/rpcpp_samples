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

#include <load_prc_file.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/loader.hpp>

int main(int argc, char* argv[])
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "window-title Render Pipeline - Material Sample");

    // configure panda3d in program.
    rpcore::RenderPipeline* render_pipeline = new rpcore::RenderPipeline(argc, argv);
    render_pipeline->get_mount_mgr()->set_base_path("../share/render_pipeline");
    render_pipeline->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
    render_pipeline->create();

    // Set time of day
    render_pipeline->get_daytime_mgr()->set_time("19:17");

    // Load the scene
    NodePath model = rpcore::RPLoader::load_model("../share/render_pipeline/models/01-Material-Demo/TestScene.bam");
    model.reparent_to(rpcore::Globals::render);
    render_pipeline->prepare_scene(rpcore::Globals::render);

    // Enable parallax mapping on the floor
    render_pipeline->set_effect(model.find("**/FloorPlane"),
        "effects/default.yaml",
        { { "parallax_mapping", true } }, 100);

    rpcore::MovementController* controller =  new rpcore::MovementController(rpcore::Globals::base);
    controller->set_initial_position_hpr(
        LVecBase3f(-17.2912578583, -13.290019989, 6.88211250305),
        LVecBase3f(-39.7285499573, -14.6770210266, 0.0));
    controller->setup();

    render_pipeline->run();

    delete controller;
    delete render_pipeline;

    return 0;
}
