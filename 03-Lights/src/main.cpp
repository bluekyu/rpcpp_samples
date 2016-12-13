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
#include <render_pipeline/rppanda/interval/lerp_interval.h>
#include <render_pipeline/rppanda/interval/meta_interval.h>

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/mount_manager.h>
#include <render_pipeline/rpcore/pluginbase/day_manager.h>
#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/util/movement_controller.h>

int main(int argc, char* argv[])
{
	// Setup window size, title and so on
	load_prc_file_data("",
		"window-title Render Pipeline - Lights demo");

	PandaFramework framework;
	framework.open_framework(argc, argv);
	WindowFramework* window = framework.open_window();

    // ------ Begin of render pipeline code ------

	// configure panda3d in program.
	rpcore::RenderPipeline* render_pipeline = new rpcore::RenderPipeline;
	render_pipeline->get_mount_mgr()->set_base_path("../etc/render_pipeline");
	render_pipeline->get_mount_mgr()->set_config_dir("../etc/render_pipeline/config");

	render_pipeline->create(&framework, window);

    // ------ End of render pipeline code, thats it! ------

	// Set time of day
	render_pipeline->get_daytime_mgr()->set_time("5:20");

    float half_energy = 5000;
    float lamp_fov = 70;
    float lamp_radius = 10;

	// Load the scene
	NodePath model = window->load_model(window->get_render(), "../share/render_pipeline/models/03-Lights/Scene.bam");

	// Animate balls, this is for testing the motion blur
	CLerpInterval::BlendType blend_type = CLerpInterval::BT_no_blend;

	{
		NodePath np = model.find("**/MBRotate");

		rppanda::LerpHprInterval::Parameters params;
		params.nodepath = np;
		params.duration = 1.5;
		params.hpr = LVecBase3(360, 360, 0);
		params.start_hpr = LVecBase3(0, 0, 0);
		params.blend_type = blend_type;
		PT(rppanda::LerpHprInterval) lerp = new rppanda::LerpHprInterval(params);
		lerp->loop();
	}

	{
		NodePath np = model.find("**/MBUpDown");
		const LVecBase3& np_pos = np.get_pos() - LVecBase3(0, 0, 2);

		rppanda::LerpPosInterval::Parameters params;
		params.nodepath = np;
		params.duration = 0.15;
		params.pos = np_pos + LVecBase3(0, 0, 6);
		params.start_pos = np_pos;
		params.blend_type = blend_type;
		PT(rppanda::LerpPosInterval) lerp1 = new rppanda::LerpPosInterval(params);

		params.pos = np_pos;
		params.start_pos = np_pos + LVecBase3(0, 0, 6);
		PT(rppanda::LerpPosInterval) lerp2 = new rppanda::LerpPosInterval(params);

		PT(rppanda::Sequence) seq = new rppanda::Sequence({lerp1, lerp2});
		seq->loop();
	}

	{
		NodePath np = model.find("**/MBFrontBack");
		const LVecBase3& np_pos = np.get_pos() - LVecBase3(0, 0, 2);

		rppanda::LerpPosInterval::Parameters params;
		params.nodepath = np;
		params.duration = 0.15;
		params.pos = np_pos + LVecBase3(0, 6, 0);
		params.start_pos = np_pos;
		params.blend_type = blend_type;
		PT(rppanda::LerpPosInterval) lerp1 = new rppanda::LerpPosInterval(params);

		params.pos = np_pos;
		params.start_pos = np_pos + LVecBase3(0, 6, 0);
		PT(rppanda::LerpPosInterval) lerp2 = new rppanda::LerpPosInterval(params);

		PT(rppanda::Sequence) seq = new rppanda::Sequence({lerp1, lerp2});
		seq->loop();
	}

	{
		NodePath np = model.find("**/MBScale");

		rppanda::LerpScaleInterval::Parameters params;
		params.nodepath = np;
		params.duration = 0.15;
		params.scale = LVecBase3(1.5);
		params.start_scale = LVecBase3(1);
		params.blend_type = blend_type;
		PT(rppanda::LerpScaleInterval) lerp1 = new rppanda::LerpScaleInterval(params);

		params.scale = LVecBase3(1);
		params.start_scale = LVecBase3(1.5);
		PT(rppanda::LerpScaleInterval) lerp2 = new rppanda::LerpScaleInterval(params);

		PT(rppanda::Sequence) seq = new rppanda::Sequence({lerp1, lerp2});
		seq->loop();
	}

    // Generate temperature lamps
    // This shows how to procedurally create lamps.In this case, we
    // base the lights positions on empties created in blender.


	render_pipeline->prepare_scene(rpcore::Globals::render);

    rpcore::MovementController* controller =  new rpcore::MovementController(rpcore::Globals::base);
	controller->set_initial_position(
		LVecBase3f(23.9, 42.5, 13.4),
		LVecBase3f(23.8, 33.4, 10.8));
	controller->setup();

	framework.main_loop();
	framework.close_framework();

	delete controller;
	delete render_pipeline;

	return 0;
}
