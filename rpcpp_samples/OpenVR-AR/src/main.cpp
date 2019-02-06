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
#include <virtualFileSystem.h>

#include <fmt/ostream.h>

#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/manager.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/util/primitives.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rpcore/util/rptextnode.hpp>
#include <render_pipeline/rpcore/stage_manager.hpp>

#include <rpplugins/openvr/plugin.hpp>
#include <rpplugins/openvr/camera_interface.hpp>

#include <rpplugins/ar_render/plugin.hpp>
#include <rpplugins/ar_render/ar_composite_stage.hpp>

#define MR_MODE 0

MainApp::MainApp() : ShowBase(true), RPObject("MainApp")
{
    // Setup window size, title and so on
    load_prc_file_data("",
        "sync-video false\n"
        "window-title Render Pipeline - OpenVR-AR Demo");

    // configure panda3d in program.
    render_pipeline_ = std::make_unique<rpcore::RenderPipeline>();

    render_pipeline_->get_mount_mgr()->set_config_dir("../etc/rpsamples/ar");
    VirtualFileSystem::get_global_ptr()->mount("../share/rpcpp_samples", "/$$app", 0);

    render_pipeline_->create(this);

    auto plugin_mgr = render_pipeline_->get_plugin_mgr();
    if (!plugin_mgr->is_plugin_enabled("openvr"))
    {
        render_pipeline_->error("openvr plugin is not enabled!");
        render_pipeline_->error("Enable openvr in plugins.yaml");

        std::exit(1);
    }

    if (!plugin_mgr->is_plugin_enabled("ar_render"))
    {
        render_pipeline_->error("ar_render plugin is not enabled!");
        render_pipeline_->error("Enable ar_render in plugins.yaml");

        std::exit(1);
    }

    // Set time of day
    render_pipeline_->get_daytime_mgr()->set_time(0.569f);

    openvr_plugin_ = static_cast<rpplugins::OpenVRPlugin*>(plugin_mgr->get_instance("openvr")->downcast());

    setup_event();
    load_scenes();
    setup_ar_camera();
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

    // OpenVR screenshot
    accept("p", [this](const Event*) {
        openvr_plugin_->take_stereo_screenshots("screenshot_preview", "screenshot_stereo");
    });

    accept("l", [this](auto) {
        toggle_streaming_action();
    });
}

void MainApp::load_scenes()
{
    // axis on origin
    NodePath axis_model = rpcore::RPLoader::load_model("/$$app/models/zup-axis.bam");
    axis_model.reparent_to(rpcore::Globals::render);

    auto openvr_devices = rpcore::Globals::render.find("openvr_devices");
    const auto child_count = openvr_devices.get_num_children();
    for (int k = 1; k < child_count; ++k)
    {
        auto device = openvr_devices.get_child(k);

        auto axis = axis_model.copy_to(device);
        axis.set_scale(0.01f);

        // add label to devcies
        rpcore::RPTextNode label("device_label", device);
        label.set_text(device.get_name() + " : " + device.get_tag("serial_number"));
        label.set_text_color(LColor(1, 0, 0, 1));
        label.set_pixel_size(10.0f);
        label.get_np().set_two_sided(true);
        label.get_np().set_pos(0.0f, 0.0f, 0.1f);
    }
}

void MainApp::setup_ar_camera()
{
    if (!openvr_plugin_->has_tracked_camera())
    {
        rpcore::RPObject::global_error("Application", "No tracked camera!");
        return;
    }

    openvr_camera_ = openvr_plugin_->get_tracked_camera();

    PT(Camera) ar_camera_ = openvr_camera_->create_camera_node(0);
    const auto real_object_mask = DrawMask::bit(20);
    ar_camera_->set_camera_mask(real_object_mask);

    rpcore::Globals::render.hide(real_object_mask);

    auto openvr_node_group = openvr_plugin_->get_device_node_group();
    ar_camera_np_ = openvr_node_group.attach_new_node(ar_camera_);

    uint32_t width;
    uint32_t height;
    uint32_t buffer_size;
    openvr_camera_->get_frame_size(width, height, buffer_size);
    framebuffer_.resize(buffer_size);

    ar_camera_texture_ = Texture::make_texture();
    // color is sRGBA
    ar_camera_texture_->setup_2d_texture(width, height, Texture::ComponentType::T_unsigned_byte, Texture::Format::F_srgb_alpha);
    ar_camera_texture_->set_wrap_u(SamplerState::WrapMode::WM_clamp);
    ar_camera_texture_->set_wrap_v(SamplerState::WrapMode::WM_clamp);
    ar_camera_texture_->set_magfilter(SamplerState::FilterType::FT_linear);
    ar_camera_texture_->set_minfilter(SamplerState::FilterType::FT_linear_mipmap_linear);

    PTA_uchar ram_image = ar_camera_texture_->make_ram_image();
    memset(ram_image.p(), 0xFF, width * height * 4);

    auto ar_plugin = static_cast<rpplugins::ARRenderPlugin*>(render_pipeline_->get_plugin_mgr()->get_instance("ar_render")->downcast());
    ar_plugin->set_ar_camera(ar_camera_np_);
    ar_plugin->set_ar_camera_color_texture(ar_camera_texture_, false, true);

#if MR_MODE
    openvr_node_group.show_through(real_object_mask);

    auto ar_composite_stage = static_cast<rpplugins::ARCompositeStage*>(render_pipeline_->get_stage_mgr()->get_stage("ARCompositeStage")->downcast());
    ar_composite_stage->remove_occlusion(true);
    //ar_composite_stage->render_only_valid_ar_depth(true);
#endif
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
    openvr_camera_->get_frame_header(header);

    static uint32_t last_frame_sequence = 0;

    // frame hasn't changed yet, nothing to do
    if (header.nFrameSequence == last_frame_sequence)
        return AsyncTask::DS_cont;

    // Frame has changed, do the more expensive frame buffer copy
    auto err = openvr_camera_->get_framebuffer(header, framebuffer_);
    if (err != vr::VRTrackedCameraError_None)
        return AsyncTask::DS_cont;

    PTA_uchar ram_image = ar_camera_texture_->modify_ram_image();
    std::memcpy(ram_image.p(), framebuffer_.data(), framebuffer_.size() * sizeof(decltype(framebuffer_)::value_type));

    const auto& pose = header.standingTrackedDevicePose;
    if (pose.bPoseIsValid)
    {
        ar_camera_np_.set_mat(LMatrix4::z_to_y_up_mat() *
            rpplugins::OpenVRPlugin::convert_matrix(pose.mDeviceToAbsoluteTracking) *
            LMatrix4::y_to_z_up_mat());
    }

    last_frame_sequence = header.nFrameSequence;

    return AsyncTask::DS_cont;
}

// ************************************************************************************************

int main()
{
    MainApp().run();

    return 0;
}
