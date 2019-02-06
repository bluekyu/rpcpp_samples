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
#include <cardMaker.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/showbase/loader.hpp>
#include <render_pipeline/rppanda/interval/actor_interval.hpp>
#include <render_pipeline/rppanda/interval/lerp_interval.hpp>
#include <render_pipeline/rppanda/actor/actor.hpp>
#include <render_pipeline/rppanda/gui/onscreen_text.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/native/tag_state_manager.h>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>

// Function to put instructions on the screen.
rppanda::OnscreenText add_instructions(float pos, const std::string& msg)
{
    return rppanda::OnscreenText(msg, rppanda::OnscreenText::Style::plain,
        LVecBase2(0.08f, -pos - 0.54f), 0.0f, LVecBase2(0.05f), LColor{1, 1, 1, 1}, {},
        LColor{0, 0, 0, 1}, rppanda::OnscreenText::Default::shadow_offset,
        {}, TextNode::Alignment::A_left, {}, {}, false, nullptr, rpcore::Globals::base->get_aspect_2d().find("a2d_top_left"));
}

class World : public rppanda::DirectObject
{
public:
    World(rpcore::RenderPipeline* pipeline) : pipeline_(pipeline)
    {
        base_ = pipeline_->get_showbase();

        inst_p_ = add_instructions(0.06f, "P : stop/start the Panda Rotation");
        inst_o_ = add_instructions(0.12f, "O : stop/start the Walk Cycle");
        inst_t_ = add_instructions(0.18f, "T : stop/start the Teapot");
        inst_x_ = add_instructions(0.24f, "K / L: change daytime");
        inst_n_ = add_instructions(0.30f, "N: toggle shadow of the Panda");
        inst_m_ = add_instructions(0.36f, "M: toggle shadow of the Teapot");

        // Load the scene.

        rpcore::RPMaterial basic_mat;
        basic_mat.set_base_color(LColor(1));
        basic_mat.set_roughness(1);
        basic_mat.set_specular_ior(1);

        auto floor_tex = base_->get_loader()->load_texture("maps/envir-ground.jpg");
        floor_tex->set_format(Texture::Format::F_srgb);
        auto cm = CardMaker("");
        cm.set_frame(-2, 2, -2, 2);
        auto floor = rpcore::Globals::render.attach_new_node(new PandaNode("floor"));

        for (int y = 0; y < 12; ++y)
        {
            for (int x = 0; x < 12; ++x)
            {
                auto nn = floor.attach_new_node(cm.generate());
                nn.set_p(-90);
                nn.set_pos((x - 6) * 4, (y - 6) * 4, 0);
                floor.set_texture(floor_tex);
                floor.flatten_strong();

                nn.set_material(basic_mat.get_material());
            }
        }

        panda_axis_ = rpcore::Globals::render.attach_new_node("panda axis");
        panda_model_ = new rppanda::Actor(
            rppanda::Actor::ModelsType{ "panda-model" },
            rppanda::Actor::AnimsType{ { "walk", "panda-walk4" } });
        panda_model_->set_material(basic_mat.get_material());
        auto tex_collection = panda_model_->find_all_textures();
        for (int k = 0, k_end = tex_collection.get_num_textures(); k < k_end; ++k)
            tex_collection.get_texture(k)->set_format(Texture::Format::F_srgb);

        panda_model_->reparent_to(panda_axis_);
        panda_model_->set_pos(9, 0, 0);
        panda_model_->set_scale(0.01f);
        panda_walk_ = panda_model_->actor_interval({ "walk" }, false, false, {}, {}, {}, {}, {}, 1.8f);
        panda_walk_->loop();
        panda_movement_ = new rppanda::LerpHprInterval(panda_axis_, 20.0, LPoint3(-360, 0, 0), LPoint3(0, 0, 0));
        panda_movement_->loop();

        teapot_ = base_->get_loader()->load_model("teapot");
        teapot_.set_material(basic_mat.get_material());
        teapot_.reparent_to(rpcore::Globals::render);
        teapot_.set_pos(0, -20, 10);
        teapot_movement_ = new rppanda::LerpHprInterval(teapot_, 50, LPoint3(0, 360, 360));
        teapot_movement_->loop();

        accept("escape", [this](auto) { base_->user_exit(); });

        accept("k", [this](auto) { change_daytime(false); });
        accept("l", [this](auto) { change_daytime(true); });
        accept("p", [this](auto) { toggle_interval(panda_movement_); });
        accept("t", [this](auto) { toggle_interval(teapot_movement_); });
        accept("o", [this](auto) { toggle_interval(panda_walk_); });
        accept("n", [this](auto) { toggle_shadow(panda_axis_); });
        accept("m", [this](auto) { toggle_shadow(teapot_); });
    }

    void toggle_interval(CInterval* ival) const
    {
        if (ival->is_playing())
            ival->pause();
        else
            ival->resume();
    }

    void change_daytime(bool increase) const
    {
        auto daytime_mgr = pipeline_->get_daytime_mgr();
        daytime_mgr->set_time(daytime_mgr->get_time() + (increase ? 0.005f : -0.005f));
    }

    void toggle_shadow(NodePath np) const
    {
        const auto shadow_mask = pipeline_->get_tag_mgr()->get_mask("shadow");
        if (np.is_hidden(shadow_mask))
            np.show(shadow_mask);
        else
            np.hide(shadow_mask);
    }

private:
    rpcore::RenderPipeline* pipeline_;
    rppanda::ShowBase* base_;

    rppanda::OnscreenText inst_p_;
    rppanda::OnscreenText inst_o_;
    rppanda::OnscreenText inst_t_;
    rppanda::OnscreenText inst_x_;
    rppanda::OnscreenText inst_n_;
    rppanda::OnscreenText inst_m_;

    NodePath panda_axis_;
    PT(rppanda::Actor) panda_model_;
    PT(rppanda::ActorInterval) panda_walk_;
    PT(rppanda::LerpHprInterval) panda_movement_;
    NodePath teapot_;
    PT(rppanda::LerpHprInterval) teapot_movement_;
};

int main()
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "window-title Render Pipeline - Panda3D-Shadows");

    // configure panda3d in program.
    auto render_pipeline = std::make_unique<rpcore::RenderPipeline>();

    {
        render_pipeline->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
        render_pipeline->create();

        // Set time of day
        render_pipeline->get_daytime_mgr()->set_time(0.769f);

        auto world = std::make_unique<World>(render_pipeline.get());

        // Init movement controller
        auto controller = std::make_unique<rpcore::MovementController>(rpcore::Globals::base);
        controller->set_initial_position(
            LVecBase3(30, -45, 26),
            LVecBase3(0, 0, 0));
        controller->setup();

        render_pipeline->run();

        // release resources out of scope
    }

    // release explicitly
    render_pipeline.reset();

    return 0;
}
