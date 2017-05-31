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

#include <regex>
#include <queue>
#include <random>

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <texturePool.h>
#include <load_prc_file.h>
#include <nodePathCollection.h>
#include <spotlight.h>
#include <material.h>
#include <genericAsyncTask.h>

#include <render_pipeline/rppanda/showbase/showbase.h>
#include <render_pipeline/rppanda/interval/lerp_interval.h>
#include <render_pipeline/rppanda/interval/meta_interval.h>

#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/mount_manager.h>
#include <render_pipeline/rpcore/pluginbase/day_manager.h>
#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/util/movement_controller.h>
#include <render_pipeline/rpcore/native/rp_spot_light.h>

float half_energy = 5000;
float lamp_fov = 70;
float lamp_radius = 10;
std::vector<PT(rpcore::RPSpotLight)> lights;

/** Update method. */
AsyncTask::DoneStatus update(GenericAsyncTask *task, void *user_data)
{
    double frame_time = rpcore::Globals::clock->get_frame_time();

    // Make the lights glow
    for (size_t i=0, i_end=lights.size(); i < i_end; ++i)
    {
        float brightness = std::sin(0.4 * i + frame_time);
        lights[i]->set_energy(max(0, half_energy / 2.0 + brightness * half_energy));
    }

    return AsyncTask::DS_cont;
}

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
    render_pipeline->get_mount_mgr()->set_base_path("../share/render_pipeline");
    render_pipeline->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");

    render_pipeline->create(&framework, window);

    // ------ End of render pipeline code, thats it! ------

    // Set time of day
    render_pipeline->get_daytime_mgr()->set_time("5:20");

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

        PT(rppanda::Sequence) seq = new rppanda::Sequence({ lerp1, lerp2 });
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

        PT(rppanda::Sequence) seq = new rppanda::Sequence({ lerp1, lerp2 });
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

        PT(rppanda::Sequence) seq = new rppanda::Sequence({ lerp1, lerp2 });
        seq->loop();
    }

    // Generate temperature lamps
    // This shows how to procedurally create lamps.In this case, we
    // base the lights positions on empties created in blender.
    auto light_compare = [](NodePath lhs, NodePath rhs) {
        const std::regex token("LampLum");

        const std::string lhs_name = lhs.get_name();
        const std::string rhs_name = rhs.get_name();
        const std::string lhs_key = std::vector<std::string>(std::sregex_token_iterator(lhs_name.begin(), lhs_name.end(), token, -1), std::sregex_token_iterator()).back();
        const std::string rhs_key = std::vector<std::string>(std::sregex_token_iterator(rhs_name.begin(), rhs_name.end(), token, -1), std::sregex_token_iterator()).back();

        return std::stoi(lhs_key) < std::stoi(rhs_key);
    };

    std::vector<NodePath> lumlamps;
    auto npc = model.find_all_matches("**/LampLum*");
    for (int k=0, k_end=npc.get_num_paths(); k < k_end; ++k)
        lumlamps.push_back(npc.get_path(k));

    std::sort(lumlamps.begin(), lumlamps.end(), light_compare);

    std::random_device rd;
    std::mt19937 reg(rd());
    for (size_t k=0, k_end=lumlamps.size(); k < k_end; ++k)
    {
        NodePath lumlamp = lumlamps[k];

        static const size_t l = std::string("LampLum").length();
        float lum = std::stof(lumlamp.get_name().substr(l));

        PT(rpcore::RPSpotLight) light = new rpcore::RPSpotLight;
        light->set_direction(0, -1.5f, -1.0f);
        light->set_fov(lamp_fov);
        light->set_color_from_temperature(lum * 1000.0f);
        light->set_energy(half_energy);
        light->set_pos(lumlamp.get_pos(rpcore::Globals::render));
        light->set_radius(lamp_radius);
        light->set_casts_shadows(false);
        light->set_shadow_map_resolution(256);
        render_pipeline->add_light(light);

        // Put Pandas on the edges
        if (k < 2 || k >= k_end - 2)
        {
            NodePath panda = window->load_model(window->get_render(), "../share/render_pipeline/models/03-Lights/panda");
            panda.reparent_to(rpcore::Globals::render);
            PT(Material) panda_mat = new Material("default");
            panda_mat->set_emission(0);
            panda.set_material(panda_mat);
            panda.set_pos(light->get_pos());
            panda.set_z(0.65);

            std::uniform_int_distribution<int> uni(-60, 60);
            panda.set_h(180 + uni(reg));
            panda.set_scale(0.2);
            panda.set_y(panda.get_y() - 3.0);
        }

        lights.push_back(light);
    }

    render_pipeline->prepare_scene(rpcore::Globals::render);

    rpcore::MovementController* controller =  new rpcore::MovementController(rpcore::Globals::base);
    controller->set_initial_position(
        LVecBase3f(23.9, 42.5, 13.4),
        LVecBase3f(23.8, 33.4, 10.8));
    controller->setup();

    rpcore::Globals::base->add_task(update, nullptr, "update");

    framework.main_loop();
    framework.close_framework();

    delete controller;
    delete render_pipeline;

    return 0;
}
