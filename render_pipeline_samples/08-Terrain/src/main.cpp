/**
 * MIT License
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
 * Copyright (c) 2018 Younguk Kim
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
#include <shaderTerrainMesh.h>
#include <virtualFileSystem.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/showbase/messenger.hpp>
#include <render_pipeline/rppanda/showbase/loader.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>

#include <rpplugins/env_probes/environment_probe.hpp>

class Application : public rppanda::ShowBase
{
public:
    Application(int argc, char* argv[]) : ShowBase()
    {
        // Setup window size, title and so on
        load_prc_file_data("",
            "window-title Render Pipeline - Shading Models Demo\n"
            "stm-max-chunk-count 2048\n"
            "gl-coordinate-system default\n"
            "stm-max-views 20\n"
            "notify-level-linmath error\n"
        );

        // configure panda3d in program.
        render_pipeline_ = std::make_unique<rpcore::RenderPipeline>();

        render_pipeline_->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
        VirtualFileSystem::get_global_ptr()->mount("../share/rpcpp_samples/08-Terrain", "/$$app", 0);
        render_pipeline_->create(argc, argv, this);

        // Set time of day
        render_pipeline_->get_daytime_mgr()->set_time("10:25");

        // Add some environment probe to provide better reflections
        auto probe = render_pipeline_->add_environment_probe();
        probe->set_pos({0, 0, 600});
        probe->set_scale({8192 * 2, 8192 * 2, 1000});

        terrain_np_ = get_render().attach_new_node("terrain");

        auto heightfield = get_loader()->load_texture("/$$app/resources/heightfield.png");

        for (int x = 0; x < 3; ++x)
        {
            for (int y = 0; y < 3; ++y)
            {
                PT(ShaderTerrainMesh) terrain_node = new ShaderTerrainMesh;
                terrain_node->set_heightfield(heightfield);
                terrain_node->set_target_triangle_width(6.0f);
                terrain_node->generate();

                auto terrain_n = terrain_np_.attach_new_node(terrain_node);
                terrain_n.set_scale(8192, 8192, 600);
                terrain_n.set_pos(-4096 + (x - 1) * 8192, -4096 + (y - 1) * 8192, 0);
            }
        }

        // Init movement controller
        controller_ = std::make_unique<rpcore::MovementController>(rpcore::Globals::base);
        controller_->set_initial_position(
            LVecBase3f(-12568, -11736, 697),
            LVecBase3f(-12328, -11357, 679));
        controller_->setup();

        rpcore::Globals::base->accept("l", [this](const Event*) { tour(); });
        rpcore::Globals::base->accept("r", [this](const Event*) { reload_shaders(); });
        reload_shaders();
    }

    virtual ~Application() = default;

    void reload_shaders()
    {
        render_pipeline_->reload_shaders();

        // Set the terrain effect
        render_pipeline_->set_effect(terrain_np_, "/$$rp/effects/terrain-effect.yaml", {}, 100);
    }

    void tour()
    {
        rpcore::MovementController::MotionPathType control_points = {
            { LVecBase3f(2755.62084961, 6983.76708984, 506.219055176), LVecBase3f(-179.09147644, 4.30751991272, 0.0) },
            { LVecBase3f(3153.70068359, 5865.30859375, 560.780822754), LVecBase3f(-225.239028931, 3.43641376495, 0.0) },
            { LVecBase3f(2140.57080078, 5625.22753906, 598.345031738), LVecBase3f(-196.022613525, 1.91196918488, 0.0) },
            { LVecBase3f(2598.85961914, 3820.56958008, 627.692993164), LVecBase3f(-272.410125732, 3.32752752304, 0.0) },
            { LVecBase3f(1894.64526367, 3597.39257812, 647.455078125), LVecBase3f(-198.227630615, 0.605303645134, 0.0) },
            { LVecBase3f(1998.48425293, 1870.05358887, 639.38458252), LVecBase3f(-121.682502747, 0.169744253159, 0.0) },
            { LVecBase3f(3910.55297852, 2370.70922852, 518.356567383), LVecBase3f(-130.42376709, 3.65418696404, -0.000583928485867) },
            { LVecBase3f(4830.80761719, 1542.30749512, 501.019073486), LVecBase3f(21.7212314606, 8.00974178314, 0.0) },
            { LVecBase3f(4324.81982422, 2259.2409668, 713.095458984), LVecBase3f(-19.1500263214, -1.68137300014, 0.0) },
            { LVecBase3f(4584.38720703, 3417.75073242, 612.79510498), LVecBase3f(-39.4675331116, -4.94804239273, 0.0) },
            { LVecBase3f(4569.68261719, 4840.80908203, 540.924926758), LVecBase3f(-83.4887771606, 4.08973407745, 0.0) },
            { LVecBase3f(5061.6484375, 5490.93896484, 694.142578125), LVecBase3f(-135.778717041, 0.605291008949, 0.0) },
            { LVecBase3f(6273.03222656, 6343.10546875, 769.735290527), LVecBase3f(-200.826278687, -1.35470712185, 0.0) },
            { LVecBase3f(7355.02832031, 6314.39892578, 1164.65527344), LVecBase3f(-262.645324707, -12.2435855865, 0.0) },
            { LVecBase3f(6321.42431641, 4453.68310547, 898.895202637), LVecBase3f(-322.574157715, -2.22581481934, 0.0) },
            { LVecBase3f(4820.63916016, 4637.07128906, 793.165222168), LVecBase3f(-330.291748047, 0.931967377663, 0.0) },
            { LVecBase3f(2621.3125, 4790.99072266, 691.223144531), LVecBase3f(-423.059020996, 3.10974740982, 0.0) },
            { LVecBase3f(2166.64697266, 5683.75195312, 685.415039062), LVecBase3f(-497.871612549, -1.57248008251, 0.0) },
            { LVecBase3f(2426.64135742, 7216.48974609, 993.247558594), LVecBase3f(-524.17388916, -6.69025611877, 0.0) },
            { LVecBase3f(5014.83398438, 6712.60253906, 963.192810059), LVecBase3f(-596.072387695, -3.31469583511, 0.00446693599224) },
            { LVecBase3f(6107.69970703, 5460.03662109, 820.63104248), LVecBase3f(-650.567199707, 1.04085958004, 0.0) },
        };
        controller_->play_motion_path(control_points, 3.0);
    }

private:
    std::unique_ptr<rpcore::RenderPipeline> render_pipeline_;
    std::unique_ptr<rpcore::MovementController> controller_;

    NodePath terrain_np_;
};

int main(int argc, char* argv[])
{
    Application(argc, argv).run();

    return 0;
}
