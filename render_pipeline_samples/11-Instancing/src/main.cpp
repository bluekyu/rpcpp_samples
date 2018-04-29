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
#include <nodePathCollection.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/util/instancing_node.hpp>
#include <render_pipeline/rpcore/loader.hpp>

int main(int argc, char* argv[])
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "window-title Render Pipeline - Instancing Example");

    // configure panda3d in program.
    auto render_pipeline = std::make_unique<rpcore::RenderPipeline>(argc, argv);

    {
        render_pipeline->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
        render_pipeline->create();

        // Set time of day
        render_pipeline->get_daytime_mgr()->set_time("19:17");

        // Load the scene
        NodePath model = rpcore::RPLoader::load_model("/$$rp/models/11-Instancing/Scene.bam");
        model.reparent_to(rpcore::Globals::render);

        // Find the prefab object, we are going to in instance this object
        // multiple times
        NodePath prefab = model.find("**/InstancedObjectPrefab");

        // get matrix of parent node to get local transform.
        LMatrix4f parent_mat = prefab.get_mat(rpcore::Globals::render);
        parent_mat.invert_in_place();

        // Collect all instances
        std::vector<LMatrix4f> matrices;
        auto models = model.find_all_matches("**/PREFAB*");
        for (int k = 0, k_end = models.get_num_paths(); k < k_end; ++k)
        {
            matrices.push_back(models.get_path(k).get_mat(rpcore::Globals::render) * parent_mat);
            models.get_path(k).remove_node();
        }

        std::cout << "Loaded " << matrices.size() << " instances!" << std::endl;

        rpcore::InstancingNode instancing(prefab);
        instancing.set_transforms(matrices);
        instancing.upload_transforms();

        // Initialize movement controller, this is a convenience class
        // to provide an improved camera control compared to Panda3Ds default
        // mouse controller.
        auto controller = std::make_unique<rpcore::MovementController>(rpcore::Globals::base);
        controller->set_initial_position_hpr(
            LVecBase3f(-23.2, -32.5, 5.3),
            LVecBase3f(-33.8, -8.3, 0.0));
        controller->setup();

        render_pipeline->run();

        // release resources out of scope
    }

    // release explicitly
    render_pipeline.reset();

    return 0;
}
