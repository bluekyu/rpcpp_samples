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
#include <load_prc_file.h>
#include <nodePathCollection.h>

#include <NvFlex.h>

#include <render_pipeline/rppanda/showbase/showbase.h>
#include <render_pipeline/rpcore/render_pipeline.h>
#include <render_pipeline/rpcore/mount_manager.h>
#include <render_pipeline/rpcore/pluginbase/day_manager.h>
#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/util/movement_controller.h>
#include <render_pipeline/rpcore/util/instancing_node.hpp>
#include <render_pipeline/rpcore/util/primitives.hpp>
#include <render_pipeline/rpcore/pluginbase/manager.h>

#include <render_pipeline/rpplugins/flex/include/flex_plugin.hpp>
#include <render_pipeline/rpplugins/flex/include/flex_instance_interface.hpp>
#include <render_pipeline/rpplugins/flex/include/flex_buffer.hpp>

class ParticlesInstance: public FlexInstanceInterface
{
public:
    ParticlesInstance(const std::shared_ptr<rpcore::InstancingNode>& instanced_node): instanced_node_(instanced_node) {}

    void initialize(FlexBuffer& buffer) final;
    void sync_flex(FlexBuffer& buffer) final;

    std::shared_ptr<rpcore::InstancingNode> instanced_node_;
};

void ParticlesInstance::initialize(FlexBuffer& buffer)
{
    const auto& transforms = instanced_node_->get_transforms();

    auto phase = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide);
    for (size_t k = 0, k_end = transforms.size(); k < k_end; ++k)
    {
        buffer.positions_.push_back(LVecBase4(transforms[k].get_row3(3), 1));
        buffer.phases_.push_back(phase);
    }
}

void ParticlesInstance::sync_flex(FlexBuffer& buffer)
{
    // read back
    auto& transforms = instanced_node_->modify_transforms();

    auto& t = transforms.begin();

    for (int k = 0, k_end = transforms.size(); k < k_end; ++k)
    {
        *t = (LMatrix4f::translate_mat(buffer.positions_[k].get_xyz()));
        ++t;
    }

    instanced_node_->upload_transforms();
}

int main(int argc, char* argv[])
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "window-title Render Pipeline - Instancing Example");

    PandaFramework framework;
    framework.open_framework(argc, argv);
    WindowFramework* window = framework.open_window();

    // configure panda3d in program.
    rpcore::RenderPipeline* render_pipeline = new rpcore::RenderPipeline;
    render_pipeline->get_mount_mgr()->set_base_path("../share/render_pipeline");
    render_pipeline->get_mount_mgr()->set_config_dir("../etc/render_pipeline");
    render_pipeline->create(&framework, window);

    // Set time of day
    render_pipeline->get_daytime_mgr()->set_time("19:17");

    if (!render_pipeline->get_plugin_mgr()->is_plugin_enabled("flex"))
    {
        rpcore::RPObject::global_error("Main", "Flex plugin is NOT enabled.");
        return 1;
    }

    auto& flex_plugin = std::dynamic_pointer_cast<FlexPlugin>(render_pipeline->get_plugin_mgr()->get_instance("flex"));

    NodePath particle = rpcore::create_sphere("particle", 6, 6);
    particle.set_scale(0.1);

    NodePath instanced_node = rpcore::Globals::render.attach_new_node("instanced_node");
    particle.reparent_to(instanced_node);

    auto instancing = std::make_shared<rpcore::InstancingNode>(instanced_node);

    std::vector<LMatrix4f> matrices;
    for (int z = 0; z < 30; ++z)
        for (int y = 0; y < 30; ++y)
            for (int x = 0; x < 30; ++x)
            {
                matrices.push_back(LMatrix4f::translate_mat((x+(rand()%10)*0.1)/3.0f, (y+(rand()%10)*0.1)/3.0f, 50+z+(rand()%10)*0.1));
            }

    std::cout << "Loaded " << matrices.size() << " instances!" << std::endl;

    instancing->set_transforms(matrices);
    instancing->upload_transforms();

    flex_plugin->add_instance(std::make_shared<ParticlesInstance>(instancing));

    // Initialize movement controller, this is a convenience class
    // to provide an improved camera control compared to Panda3Ds default
    // mouse controller.
    std::shared_ptr<rpcore::MovementController> controller = std::make_shared<rpcore::MovementController>(rpcore::Globals::base);
    controller->set_initial_position_hpr(
        LVecBase3f(-23.2, -32.5, 5.3),
        LVecBase3f(-33.8, -8.3, 0.0));
    controller->setup();

    framework.main_loop();
    framework.close_framework();

    return 0;
}
