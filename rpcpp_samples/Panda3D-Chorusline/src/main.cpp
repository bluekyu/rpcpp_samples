/**
 * MIT License
 *
 * Copyright (c) 2018 Center of Human-centered Interaction for Coexistence
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
#include <render_pipeline/rppanda/actor/actor.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>

class World : public rppanda::DirectObject
{
public:
    World(rpcore::RenderPipeline* pipeline) : pipeline_(pipeline)
    {
        base_ = pipeline_->get_showbase();

        PT(rppanda::Actor) panda = new rppanda::Actor(
            rppanda::Actor::ModelsType{ "models/panda-model" });
        panda->set_pos(0, 0, 0);
        panda->set_scale(0.007f);

        panda->set_material(rpcore::RPMaterial().get_material());
        auto tex_collection = panda->find_all_textures();
        for (int k = 0, k_end = tex_collection.get_num_textures(); k < k_end; ++k)
            tex_collection.get_texture(k)->set_format(Texture::Format::F_srgb);

        auto chorusline = NodePath("chorusline");

        for (int i = 0; i < 50; ++i)
        {
            auto placeholder = chorusline.attach_new_node("Dancer-Placeholder");
            placeholder.set_pos(i * 5, 0, 0);
            panda->instance_to(placeholder);
        }

        for (int i = 0; i < 3; ++i)
        {
            auto placeholder = rpcore::Globals::render.attach_new_node("Line-Placeholder");
            placeholder.set_pos(0, i * 10, 0);
            chorusline.instance_to(placeholder);
        }
    }

private:
    rpcore::RenderPipeline* pipeline_;
    rppanda::ShowBase* base_;
};

int main(int argc, char* argv[])
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "window-title Render Pipeline - Panda3D-Chorusline");

    // configure panda3d in program.
    auto render_pipeline = std::make_unique<rpcore::RenderPipeline>();

    {
        render_pipeline->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
        render_pipeline->create(argc, argv);

        // Set time of day
        render_pipeline->get_daytime_mgr()->set_time(0.769f);

        auto world = std::make_unique<World>(render_pipeline.get());

        // Init movement controller
        auto controller = std::make_unique<rpcore::MovementController>(rpcore::Globals::base);
        controller->set_initial_position_hpr(
            LVecBase3f(-50, -23, 26),
            LVecBase3f(-67, -15, 0));
        controller->setup();

        render_pipeline->run();

        // release resources out of scope
    }

    // release explicitly
    render_pipeline.reset();

    return 0;
}
