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

#include <render_pipeline/rppanda/showbase/showbase.h>
#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/mount_manager.h>
#include <render_pipeline/rpcore/pluginbase/day_manager.h>
#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/util/movement_controller.h>

int main(int argc, char* argv[])
{
	// Setup window size, title and so on
	load_prc_file_data("",
		"window-title Render Pipeline - Shading Models Demo");

	PandaFramework framework;
	framework.open_framework(argc, argv);
	WindowFramework* window = framework.open_window();

	// configure panda3d in program.
	rpcore::RenderPipeline* render_pipeline = new rpcore::RenderPipeline;
	render_pipeline->get_mount_mgr()->set_base_path("../etc/render_pipeline");
	render_pipeline->get_mount_mgr()->set_config_dir("../etc/render_pipeline/config");
	render_pipeline->create(&framework, window);

	// Set time of day
	render_pipeline->get_daytime_mgr()->set_time(0.769f);

	// Load the scene
	NodePath model = window->load_model(window->get_render(), "../share/render_pipeline/models/07-Shading-Models/TestScene.bam");
	render_pipeline->prepare_scene(model);

	// Init movement controller
	std::shared_ptr<rpcore::MovementController> controller = std::make_shared<rpcore::MovementController>(rpcore::Globals::base);
	controller->set_initial_position_hpr(
		LVecBase3f(6.6f, -18.8f, 4.5f),
		LVecBase3f(4.7f, -16.7f, 3.4f));
	controller->setup();

	framework.main_loop();
	framework.close_framework();

	return 0;
}
