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

#include <fmt/ostream.h>

#include <render_pipeline/rppanda/showbase/messenger.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/manager.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/util/primitives.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>

#include <rpplugins/openvr/plugin.hpp>
#include <rpplugins/openvr/camera_interface.hpp>

MainApp::MainApp() : ShowBase(true), RPObject("MainApp")
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "sync-video false\n"
        "window-title Render Pipeline - OpenVR Tracked Camera Demo");

    // configure panda3d in program.
    render_pipeline_ = std::make_unique<rpcore::RenderPipeline>();

    render_pipeline_->get_mount_mgr()->set_config_dir("../etc/rpsamples/vr");

    render_pipeline_->create(this);

    if (!render_pipeline_->get_plugin_mgr()->is_plugin_enabled("openvr"))
    {
        render_pipeline_->error("openvr plugin is not enabled!");
        render_pipeline_->error("Enable openvr in plugins.yaml");

        std::exit(1);
    }

    // Set time of day
    render_pipeline_->get_daytime_mgr()->set_time(0.769f);

    openvr_plugin_ = static_cast<rpplugins::OpenVRPlugin*>(render_pipeline_->get_plugin_mgr()->get_instance("openvr")->downcast());
    if (!openvr_plugin_->has_tracked_camera())
    {
        rpcore::RPObject::global_error("Application", "No tracked camera!");
        std::exit(1);
    }

    openvr_camera_ = openvr_plugin_->get_tracked_camera();

    info(fmt::format("Number of Camera ({}), Frame Layout ({})", openvr_camera_->get_num_cameras(), openvr_camera_->get_frame_layout()));

    preview_plane_ = rpcore::create_plane("preview");
    preview_plane_.reparent_to(rpcore::Globals::render);

    // set rough materail
    rpcore::RPGeomNode plane_geom(preview_plane_);
    auto mat = plane_geom.get_material(0);
    mat.set_base_color(1.0f);
    mat.set_roughness(1.0f);
    mat.set_specular_ior(1.0f);
    mat.set_metallic(0.0f);
    plane_geom.set_material(0, mat);

    uint32_t width;
    uint32_t height;
    uint32_t buffer_size;
    openvr_camera_->get_frame_size(width, height, buffer_size, frame_type_);
    framebuffer_.resize(buffer_size);
    preview_plane_.set_hpr(0, 90, 0);
    preview_plane_.set_scale(10, 10 * height / float(width), 1);

    info(fmt::format("Width ({}), Height ({}), Buffer Size ({})", width, height, buffer_size));

    PT(Texture) tex = Texture::make_texture();
    // color is sRGBA
    tex->setup_2d_texture(width, height, Texture::ComponentType::T_unsigned_byte, Texture::Format::F_srgb_alpha);
    PTA_uchar ram_image = tex->make_ram_image();
    memset(ram_image.p(), 0xFF, width * height * 4);
    preview_plane_.set_texture(tex);

    // Y flip
    preview_plane_.set_tex_scale(TextureStage::get_default(), 1.0f, -1.0f);

    setup_event();
}

MainApp::~MainApp() = default;

void MainApp::setup_event()
{
    // Init movement controller
    controller_ = std::make_unique<rpcore::MovementController>(rpcore::Globals::base);
    controller_->set_initial_position_hpr(
        LVecBase3(0.0f),
        LVecBase3(0.0f, 0.0f, 0.0f));
    controller_->setup();

    accept("l", [this](auto) {
        toggle_streaming_action();
    });
}

void MainApp::toggle_streaming_action()
{
    static const std::string task_name = "MainApp::upload_texture";

    if (is_streamed_)
    {
        rpcore::Globals::base->remove_task(task_name);
        openvr_camera_->release_video_streaming_service();
        is_streamed_ = false;
    }
    else
    {
        is_streamed_ = openvr_camera_->acquire_video_streaming_service();
        last_task_time_ = 0;
        rpcore::Globals::base->add_task(std::bind(&MainApp::upload_texture, this, std::placeholders::_1), task_name);
    }
}

AsyncTask::DoneStatus MainApp::upload_texture(rppanda::FunctionalTask* task)
{
    auto elapsed_time = task->get_elapsed_time();
    if ((elapsed_time - last_task_time_) < (16.0 / 1000.0))
        return AsyncTask::DS_cont;

    last_task_time_ = elapsed_time;

    vr::CameraVideoStreamFrameHeader_t header;
    openvr_camera_->get_frame_header(header, frame_type_);

    static uint32_t last_frame_sequence = 0;

    // frame hasn't changed yet, nothing to do
    if (header.nFrameSequence == last_frame_sequence)
        return AsyncTask::DS_cont;

    // Frame has changed, do the more expensive frame buffer copy
    auto err = openvr_camera_->get_framebuffer(header, framebuffer_, frame_type_);
    if (err != vr::VRTrackedCameraError_None)
        return AsyncTask::DS_cont;

    auto tex = preview_plane_.get_texture();
    PTA_uchar ram_image = tex->modify_ram_image();

    auto dest = ram_image.p();
    for (size_t k = 0, k_end = framebuffer_.size(); k < k_end; k+=4)
    {
        // bgra = rgba
        dest[k + 2] = framebuffer_[k + 0];
        dest[k + 1] = framebuffer_[k + 1];
        dest[k + 0] = framebuffer_[k + 2];
        dest[k + 3] = framebuffer_[k + 3];
    }

    last_frame_sequence = header.nFrameSequence;

    return AsyncTask::DS_cont;
}

void MainApp::setup_gl_texture()
{
}

// ************************************************************************************************

int main()
{
    MainApp().run();

    return 0;
}
