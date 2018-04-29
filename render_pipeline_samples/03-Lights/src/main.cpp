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

#include <regex>
#include <queue>
#include <random>

#include <load_prc_file.h>
#include <nodePathCollection.h>
#include <material.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/interval/lerp_interval.hpp>
#include <render_pipeline/rppanda/interval/meta_interval.hpp>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/native/rp_spot_light.h>
#include <render_pipeline/rpcore/loader.hpp>

float half_energy = 5000;
float lamp_fov = 70;
float lamp_radius = 10;
std::vector<PT(rpcore::RPSpotLight)> lights;

/** Update method. */
AsyncTask::DoneStatus update(rppanda::FunctionalTask* task)
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

    // ------ Begin of render pipeline code ------

    // configure panda3d in program.
    auto render_pipeline = std::make_unique<rpcore::RenderPipeline>(argc, argv);

    {
        render_pipeline->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
        render_pipeline->create();

        // ------ End of render pipeline code, thats it! ------

        // Set time of day
        render_pipeline->get_daytime_mgr()->set_time("5:20");

        // Load the scene
        NodePath model = rpcore::RPLoader::load_model("../share/render_pipeline/models/03-Lights/Scene.bam");
        model.reparent_to(rpcore::Globals::render);

        // Animate balls, this is for testing the motion blur
        CLerpInterval::BlendType blend_type = CLerpInterval::BT_no_blend;

        NodePath np = model.find("**/MBRotate");
        PT(rppanda::LerpHprInterval) lerp = new rppanda::LerpHprInterval(
            np, 1.5f, LVecBase3(360, 360, 0), LVecBase3(0, 0, 0), {}, {}, blend_type);
        lerp->loop();

        np = model.find("**/MBUpDown");
        LVecBase3 np_pos = np.get_pos() - LVecBase3(0, 0, 2);
        PT(rppanda::LerpPosInterval) lerp_updown_begin = new rppanda::LerpPosInterval(
            np, 0.15f, np_pos + LVecBase3(0, 0, 6), np_pos, {}, blend_type);
        PT(rppanda::LerpPosInterval) lerp_updown_end = new rppanda::LerpPosInterval(
            np, 0.15f, np_pos, np_pos + LVecBase3(0, 0, 6), {}, blend_type);
        PT(rppanda::Sequence) updown_seq = new rppanda::Sequence({ lerp_updown_begin, lerp_updown_end });
        updown_seq->loop();

        np = model.find("**/MBFrontBack");
        np_pos = np.get_pos() - LVecBase3(0, 0, 2);
        PT(rppanda::LerpPosInterval) lerp_fronback_begin = new rppanda::LerpPosInterval(
            np, 0.15f, np_pos + LVecBase3(0, 6, 0), np_pos, {}, blend_type);
        PT(rppanda::LerpPosInterval) lerp_frontback_end = new rppanda::LerpPosInterval(
            np, 0.15f, np_pos, np_pos + LVecBase3(0, 6, 0), {}, blend_type);
        PT(rppanda::Sequence) front_backseq = new rppanda::Sequence({ lerp_fronback_begin, lerp_frontback_end });
        front_backseq->loop();

        np = model.find("**/MBScale");
        PT(rppanda::LerpScaleInterval) lerp_scale_begin = new rppanda::LerpScaleInterval(
            np, 0.15f, LVecBase3(1.5f), LVecBase3(1), {}, blend_type);
        PT(rppanda::LerpScaleInterval) learp_scale_end = new rppanda::LerpScaleInterval(
            np, 0.15f, LVecBase3(1), LVecBase3(1.5f), {}, blend_type);
        PT(rppanda::Sequence) scale_seq = new rppanda::Sequence({ lerp_scale_begin, learp_scale_end });
        scale_seq->loop();

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
                NodePath panda = rpcore::RPLoader::load_model("panda");
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

        auto controller = std::make_unique<rpcore::MovementController>(rpcore::Globals::base);
        controller->set_initial_position(
            LVecBase3f(23.9, 42.5, 13.4),
            LVecBase3f(23.8, 33.4, 10.8));
        controller->setup();

        rpcore::Globals::base->add_task(std::bind(&update, std::placeholders::_1), "update");

        render_pipeline->run();

        // release resources out of scope
    }

    // release explicitly
    render_pipeline.reset();

    return 0;
}
