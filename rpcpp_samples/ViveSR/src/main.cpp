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

#include "main.hpp"

#include <load_prc_file.h>

#include <thread>
#include <chrono>

#include <fmt/ostream.h>

#include <imgui.h>

#include <render_pipeline/rppanda/showbase/messenger.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/manager.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/util/primitives.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>

#include <openvr_plugin.hpp>
#include <rpplugins/vivesr/plugin.hpp>
#include <rpplugins/vivesr/see_through_module.hpp>
#include <rpplugins/ar_render/plugin.hpp>
#include <rpplugins/ar_render/ar_composite_stage.hpp>

MainApp::MainApp(int argc, char* argv[]) : ShowBase()
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "sync-video false\n"
        "window-title Render Pipeline - OpenVR Tracked Camera Demo");

    // configure panda3d in program.
    render_pipeline_ = std::make_unique<rpcore::RenderPipeline>();

    render_pipeline_->get_mount_mgr()->set_config_dir("../etc/rpsamples/vivesr");

    render_pipeline_->create(argc, argv, this);

    auto plugin_mgr = render_pipeline_->get_plugin_mgr();
    if (!plugin_mgr->is_plugin_enabled("openvr"))
    {
        render_pipeline_->error("openvr plugin is not enabled!");
        render_pipeline_->error("Enable openvr in plugins.yaml");
    
        std::exit(1);
    }

    if (!plugin_mgr->is_plugin_enabled("vivesr"))
    {
        render_pipeline_->error("openvr plugin is not enabled!");
        render_pipeline_->error("Enable openvr in plugins.yaml");

        std::exit(1);
    }

    if (plugin_mgr->is_plugin_enabled("imgui"))
    {
        rppanda::Messenger::get_global_instance()->send(
            "imgui-setup-context",
            EventParameter(new rppanda::FunctionalTask([this](rppanda::FunctionalTask* task) {
                ImGui::SetCurrentContext(std::static_pointer_cast<ImGuiContext>(task->get_user_data()).get());
                accept("imgui-new-frame", [this](auto) { on_imgui_new_frame(); });
                return AsyncTask::DS_done;
            }, "MainApp::setup-imgui"))
        );
    }
    else
    {
        rpcore::RPObject::global_error("World", "imgui plugin is not enabled.");
    }

    // Set time of day
    render_pipeline_->get_daytime_mgr()->set_time(0.769f);

    openvr_plugin_ = static_cast<rpplugins::OpenVRPlugin*>(plugin_mgr->get_instance("openvr")->downcast());
    vivesr_plugin_ = static_cast<rpplugins::ViveSRPlugin*>(plugin_mgr->get_instance("vivesr")->downcast());

    left_plane_ = rpcore::create_plane("left");
    left_plane_.reparent_to(rpcore::Globals::render);

    // set rough materail
    rpcore::RPGeomNode plane_geom(left_plane_);
    auto mat = plane_geom.get_material(0);
    mat.set_base_color(1.0f);
    mat.set_roughness(1.0f);
    mat.set_specular_ior(1.0f);
    mat.set_metallic(0.0f);
    plane_geom.set_material(0, mat);

    left_plane_.set_hpr(0, 90, 0);

    {
        right_plane_ = rpcore::create_plane("right");
        right_plane_.reparent_to(rpcore::Globals::render);

        // set rough materail
        rpcore::RPGeomNode plane_geom(right_plane_);
        auto mat = plane_geom.get_material(0);
        mat.set_base_color(1.0f);
        mat.set_roughness(1.0f);
        mat.set_specular_ior(1.0f);
        mat.set_metallic(0.0f);
        plane_geom.set_material(0, mat);

        right_plane_.set_hpr(0, 90, 0);
        right_plane_.set_pos(15, 0, 0);
    }

    setup_event();
}

MainApp::~MainApp()
{
}

void MainApp::toggle_streaming_action(int frame_type)
{
    switch (frame_type)
    {
    case 0:
        toggle_streaming_seethrough(false);
        break;

    case 1:
        toggle_streaming_seethrough(true);
        break;

    default:
        break;
    }
}

void MainApp::toggle_streaming_seethrough(bool undistortion)
{
    auto seethrough_module = vivesr_plugin_->get_seethrough_module();

    if (seethrough_module->is_callback_registered(undistortion))
    {
        seethrough_module->unregister_callback(undistortion);
        task_->remove();
        task_ = nullptr;
    }
    else
    {
        auto frame_size = seethrough_module->get_frame_size(undistortion);
        const int width = std::get<0>(frame_size);
        const int height = std::get<1>(frame_size);
        const int channel = std::get<2>(frame_size);

        PT(Texture) left_tex = Texture::make_texture();
        left_tex->setup_2d_texture(width, height, Texture::ComponentType::T_unsigned_byte,
            channel == 3 ? Texture::Format::F_srgb : Texture::Format::F_srgb_alpha);
        memset(left_tex->make_ram_image().p(), 0xFF, width * height * channel);

        PT(Texture) right_tex = Texture::make_texture();
        right_tex->setup_2d_texture(width, height, Texture::ComponentType::T_unsigned_byte,
            channel == 3 ? Texture::Format::F_srgb : Texture::Format::F_srgb_alpha);
        memset(right_tex->make_ram_image().p(), 0xFF, width * height * channel);

        left_plane_.set_texture(left_tex);
        right_plane_.set_texture(right_tex);

        // Y flip
        left_plane_.set_tex_scale(TextureStage::get_default(), 1.0f, -1.0f);
        right_plane_.set_tex_scale(TextureStage::get_default(), 1.0f, -1.0f);

        left_plane_.set_scale(10.0f, 10.0f * height / width, 1.0f);
        right_plane_.set_scale(10.0f, 10.0f * height / width, 1.0f);

        seethrough_module->register_callback(undistortion);

        task_ = add_task([this, undistortion](rppanda::FunctionalTask*) {
            upload_seethrough_frame(undistortion);
            return AsyncTask::DS_cont;
        }, "toggle_streaming_action");
    }
}

void MainApp::setup_event()
{
    // Init movement controller
    controller_ = std::make_unique<rpcore::MovementController>(rpcore::Globals::base);
    controller_->set_initial_position_hpr(
        LVecBase3f(0.0f, 0.0f, 0.0f),
        LVecBase3f(0.0f, 0.0f, 0.0f));
    controller_->setup();
}

void MainApp::on_imgui_new_frame()
{
    ImGui::Begin("ViveSR Config");

    static const char* frame_type[] = {
        "Distorted",
        "Undistorted",
    };

    auto last_frame_type = current_frame_type_;
    if (ImGui::Combo("combo", &current_frame_type_, frame_type, IM_ARRAYSIZE(frame_type)))
    {
        if (last_frame_type != -1)
            toggle_streaming_action(last_frame_type);
        toggle_streaming_action(current_frame_type_);
    }

    ImGui::End();
}

void MainApp::copy_buffer_to_texture(const unsigned char* buffer, Texture* tex)
{
    PTA_uchar ram_image = tex->modify_ram_image();
    auto dest = ram_image.p();

    switch (tex->get_format())
    {
        case Texture::Format::F_srgb:
        case Texture::Format::F_rgb:
        {
            for (size_t k = 0, k_end = ram_image.size(); k < k_end; k += 3)
            {
                // bgr = rgb
                dest[k + 2] = buffer[k + 0];
                dest[k + 1] = buffer[k + 1];
                dest[k + 0] = buffer[k + 2];
            }
            break;
        }

        case Texture::Format::F_srgb_alpha:
        case Texture::Format::F_rgba:
        {
            for (size_t k = 0, k_end = ram_image.size(); k < k_end; k += 4)
            {
                // bgra = rgba
                dest[k + 2] = buffer[k + 0];
                dest[k + 1] = buffer[k + 1];
                dest[k + 0] = buffer[k + 2];
                dest[k + 3] = buffer[k + 3];
            }
            break;
        }

        default:
            break;
    }
}

void MainApp::upload_seethrough_frame(bool undistortion)
{
    auto frame_data = vivesr_plugin_->get_seethrough_module()->get_frame_data(undistortion);
    copy_buffer_to_texture(frame_data->frame_left.data(), left_plane_.get_texture());
    copy_buffer_to_texture(frame_data->frame_right.data(), right_plane_.get_texture());
}

// ************************************************************************************************

int main(int argc, char* argv[])
{
    PT(MainApp) app = new MainApp(argc, argv);
    app->run();

    return 0;
}
