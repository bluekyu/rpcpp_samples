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
#include <nodePathCollection.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/showbase/messenger.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/loader.hpp>

class MainApp : public rppanda::ShowBase
{
public:
    MainApp() : ShowBase(true)
    {
        // Setup window size, title and so on
        load_prc_file_data("",
            "window-title Render Pipeline - Shading Models Demo");

        // configure panda3d in program.
        render_pipeline_ = std::make_unique<rpcore::RenderPipeline>();

        render_pipeline_->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
        VirtualFileSystem::get_global_ptr()->mount("../share/rpcpp_samples/07-Shading-Models", "/$$app", 0);

        render_pipeline_->create(this);

        // Set time of day
        render_pipeline_->get_daytime_mgr()->set_time(0.769f);

        // Load the scene
        NodePath model = rpcore::RPLoader::load_model("/$$app/scene/TestScene.bam");
        model.reparent_to(rpcore::Globals::render);

        // Geoms of TRANSPARENT_MODEL have none transparency attrib in current TestScene.bam file.
        // So, remove it in code level.
        remove_transparency_attrib(model);

        render_pipeline_->prepare_scene(model);

        // Init movement controller
        controller_ = std::make_unique<rpcore::MovementController>(rpcore::Globals::base);
        controller_->set_initial_position(
            LVecBase3f(6.6f, -18.8f, 4.5f),
            LVecBase3f(4.7f, -16.7f, 3.4f));
        controller_->setup();

        accept("l", [this](const Event*) { tour(); });
        accept("r", [this](const Event*) { reload_shaders(); });
    }

    ~MainApp() = default;

    void reload_shaders()
    {
        render_pipeline_->reload_shaders();
        render_pipeline_->prepare_scene(rpcore::Globals::render);
    }

    void tour()
    {
        rpcore::MovementController::MotionPathType mopath = {
            { LVecBase3(3.97601628304, -15.5422525406, 1.73230814934), LVecBase3(49.2462043762, -11.7619161606, 0.0) },
            { LVecBase3(4.37102460861, -6.52981519699, 2.84148645401), LVecBase3(138.54864502, -15.7908058167, 0.0) },
            { LVecBase3(-5.88968038559, -13.9816446304, 2.44033527374), LVecBase3(302.348571777, -13.2863616943, 0.0) },
            { LVecBase3(5.23844909668, -18.1897411346, 4.54698801041), LVecBase3(402.91229248, -14.7019147873, 0.0) },
            { LVecBase3(-7.27328443527, -0.466051012278, 3.30696845055), LVecBase3(607.032165527, -19.6019115448, 0.0) },
            { LVecBase3(5.33415555954, 1.92750489712, 2.53945565224), LVecBase3(484.103546143, -11.9796953201, 0.0) },
            { LVecBase3(-0.283608138561, -6.86583900452, 1.43702816963), LVecBase3(354.63848877, -4.79302883148, 0.0) },
            { LVecBase3(-11.7576808929, 7.0855755806, 3.40899515152), LVecBase3(272.73840332, -12.959692955, 0.0) },
            { LVecBase3(7.75462722778, 13.220041275, 3.97876667976), LVecBase3(126.342140198, -19.4930171967, 0.0) },
            { LVecBase3(-2.10827493668, 4.78230571747, 1.27567899227), LVecBase3(-40.1353683472, -5.77301359177, 0.0) },
            { LVecBase3(8.67115211487, 16.9084873199, 3.72598099709), LVecBase3(89.5658569336, -10.9996757507, 0.0) },
            { LVecBase3(-8.1254825592, 26.6411190033, 3.21335697174), LVecBase3(268.092102051, -12.1974525452, 0.0) },
            { LVecBase3(7.89382314682, 45.8911399841, 4.47727441788), LVecBase3(498.199554443, -11.3263425827, 0.0) },
            { LVecBase3(-2.33054184914, 43.8977775574, 1.86498868465), LVecBase3(551.198242188, 13.6092195511, 0.0) },
            { LVecBase3(4.80335664749, 36.9664497375, 6.16300296783), LVecBase3(810.128417969, -10.6730031967, 0.0) },
            { LVecBase3(45.0654678345, 7.54712438583, 22.2645874023), LVecBase3(808.238586426, -17.5330123901, 0.0) },
            { LVecBase3(5.99377584457, -12.3760728836, 4.53536558151), LVecBase3(806.978820801, -2.39745855331, 0.0) },
            { LVecBase3(6.05853939056, -1.72227275372, 4.53848743439), LVecBase3(809.65637207, -2.39745855331, 0.0) },
            { LVecBase3(4.81568479538, 8.28769683838, 4.48393821716), LVecBase3(809.65637207, -2.39745855331, 0.0) },
            { LVecBase3(5.82831144333, 17.5230751038, 4.52401590347), LVecBase3(809.65637207, -2.39745855331, 0.0) },
            { LVecBase3(4.09594917297, 26.7909412384, 4.44915866852), LVecBase3(809.65637207, -2.39745855331, 0.0) },
            { LVecBase3(4.39108037949, 37.2143096924, 5.16932630539), LVecBase3(809.65637207, -2.39745855331, 0.0) },
        };
        controller_->play_motion_path(mopath, 2.3);
    }

    void remove_transparency_attrib(NodePath model)
    {
        NodePathCollection gn_npc = model.find_all_matches("**/+GeomNode");
        for (int k = 0, k_end = gn_npc.get_num_paths(); k < k_end; ++k)
        {
            rpcore::RPGeomNode gn(gn_npc.get_path(k));
            const int geom_count = gn.get_num_geoms();
            for (int i = 0; i < geom_count; ++i)
            {
                auto shading_model = gn.get_material(i).get_shading_model();
                if (shading_model == rpcore::RPMaterial::ShadingModel::TRANSPARENT_MODEL)
                    gn->set_geom_state(i, gn->get_geom_state(i)->remove_attrib(TransparencyAttrib::get_class_slot()));
            }
        }
    }

private:
    std::unique_ptr<rpcore::RenderPipeline> render_pipeline_;
    std::unique_ptr<rpcore::MovementController> controller_;
};

int main()
{
    MainApp().run();

    return 0;
}
