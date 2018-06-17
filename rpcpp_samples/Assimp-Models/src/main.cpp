/**
 * MIT License
 *
 * Copyright (c) 2018 Younguk Kim (bluekyu)
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

#include "main.hpp"

#include <load_prc_file.h>
#include <virtualFileSystem.h>
#include <load_dso.h>
#include <materialCollection.h>

#include <fmt/ostream.h>

#include <render_pipeline/rppanda/showbase/messenger.hpp>
#include <render_pipeline/rppanda/showbase/loader.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>
#include <render_pipeline/rppanda/util/filesystem.hpp>

MainApp::MainApp(int argc, char* argv[]) : ShowBase(), RPObject("MainApp")
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "sync-video false\n"
        "window-title Render Pipeline - Assimp-Models");

    // load librpassimp.so explicitly
    void* tmp = load_dso(
        rppanda::convert_path(rppanda::get_library_location().parent_path()),
        Filename::dso_filename("librpassimp.so"));
    if (tmp == nullptr)
    {
        std::cerr << "Failed to load librpassimp.so" << std::endl;
        exit(1);
    }

    // configure panda3d in program.
    render_pipeline_ = std::make_unique<rpcore::RenderPipeline>();

    render_pipeline_->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
    VirtualFileSystem::get_global_ptr()->mount("../share/rpcpp_samples/Assimp-Models", "/$$app", 0);

    render_pipeline_->create(argc, argv, this);

    // Set time of day
    render_pipeline_->get_daytime_mgr()->set_time(0.769f);

    load_models();

    setup_event();
}

MainApp::~MainApp() = default;

void MainApp::setup_event()
{
    // Init movement controller
    controller_ = std::make_unique<rpcore::MovementController>(rpcore::Globals::base);
    controller_->set_initial_position_hpr(
        LVecBase3f(0.0f),
        LVecBase3f(0.0f, 0.0f, 0.0f));
    controller_->setup();
}

void MainApp::load_models()
{
    auto loader = get_loader();

    auto hamburger_obj = loader->load_model("/$$app/Hamburger/Hamburger.obj", {}, true);
    if (hamburger_obj)
    {
        hamburger_obj.reparent_to(rpcore::Globals::render);

        auto all_mat = hamburger_obj.find_all_materials();
        for (int k = 0, k_end = all_mat.get_num_materials(); k < k_end; ++k)
        {
            auto mat = all_mat.get_material(k);

            if (!mat->has_roughness())
            {
                rpcore::RPMaterial rpmat(mat);

                rpmat.set_shading_model(rpcore::RPMaterial::ShadingModel::DEFAULT_MODEL);

                if (rpmat->has_diffuse())
                    rpmat.set_base_color(rpmat->get_diffuse());
                else
                    rpmat.set_base_color(LColor(1));
                rpmat.set_normal_factor(0.0f);
                rpmat.set_roughness(1.0f);
                rpmat.set_specular_ior(1.0f);
                rpmat.set_metallic(0.0f);
                rpmat.set_arbitrary0(0.0f);
            }
        }
    }

    auto hamburger_gltf = loader->load_model("/$$app/Hamburger/Hamburger.gltf", {}, true);
    if (hamburger_gltf)
        hamburger_gltf.reparent_to(rpcore::Globals::render);
}

// ************************************************************************************************

int main(int argc, char* argv[])
{
    PT(MainApp) app = new MainApp(argc, argv);
    app->run();

    return 0;
}
