/**
 * MIT License
 *
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

#include "world.hpp"

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/util/primitives.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rpcore/util/rptextnode.hpp>
#include <render_pipeline/rppanda/showbase/loader.hpp>
#include <render_pipeline/rpcore/pluginbase/manager.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>

#include <openvr_plugin.hpp>

#include <fmt/ostream.h>

World::World(rpcore::RenderPipeline& pipeline): pipeline_(pipeline)
{
    // Set time of day
    pipeline_.get_daytime_mgr()->set_time(0.569f);

    openvr_plugin_ = static_cast<rpplugins::OpenVRPlugin*>(pipeline_.get_plugin_mgr()->get_instance("openvr")->downcast());

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

    auto area = openvr_plugin_->get_play_area_size();
    if (area)
    {
        auto floor = rpcore::create_plane("floor");
        floor.set_scale(LVecBase3(area.value(), 1));
        floor.reparent_to(openvr_devices);
    }

    setup_event();
}

World::~World() = default;

void World::setup_event()
{
    // Init movement controller
    controller_ = new rpcore::MovementController(rpcore::Globals::base);
    controller_->set_initial_position_hpr(
        LVecBase3f(0.0f),
        LVecBase3f(0.0f, 0.0f, 0.0f));
    controller_->setup();

    // OpenVR screenshot
    accept("p", [this](const Event*) {
        openvr_plugin_->take_stereo_screenshots("screenshot_preview", "screenshot_stereo");
    });
}

void World::start()
{
    add_task([this](const rppanda::FunctionalTask*) {
        return update();
    }, "World::update");
}

AsyncTask::DoneStatus World::update()
{
    auto vr_system = openvr_plugin_->get_vr_system();

    // process events
    vr::VREvent_t ev;
    while (vr_system->PollNextEvent(&ev, sizeof(ev)))
    {
        std::cout << "Event : " << vr_system->GetEventTypeNameFromEnum(vr::EVREventType(ev.eventType)) << std::endl;
    }

    // process controller state
    for (uint i = 0, i_end = vr::k_unMaxTrackedDeviceCount; i < i_end; ++i)
    {
        if (openvr_plugin_->get_tracked_device_class(i) != vr::ETrackedDeviceClass::TrackedDeviceClass_Controller)
            continue;

        vr::VRControllerState_t state;
        if (vr_system->GetControllerState(i, &state, sizeof(vr::VRControllerState_t)))
        {
            if (state.ulButtonPressed != 0)
                std::cout << "Controller (" << i << ") Pressed" << std::endl;

            if (state.ulButtonTouched != 0)
                std::cout << "Controller (" << i << ") Touched" << std::endl;
        }
    }

    return AsyncTask::DS_cont;
}
