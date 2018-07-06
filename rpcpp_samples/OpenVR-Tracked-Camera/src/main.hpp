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

#pragma once

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/rpobject.hpp>

#include <openvr.h>

namespace rpcore {
class RenderPipeline;
class MovementController;
}

namespace rpplugins {
class OpenVRPlugin;
class OpenVRCameraInterface;
}

class MainApp : public rppanda::ShowBase, public rpcore::RPObject
{
public:
    MainApp(int argc, char* argv[]);
    virtual ~MainApp();

    ALLOC_DELETED_CHAIN(MainApp);

    void toggle_streaming_action();

private:
    void setup_event();

    std::unique_ptr<rpcore::RenderPipeline> render_pipeline_;
    PT(rpcore::MovementController) controller_;

    // this is not optimized.
    AsyncTask::DoneStatus upload_texture(rppanda::FunctionalTask* task);

    void setup_gl_texture();

    rpplugins::OpenVRPlugin* openvr_plugin_;
    rpplugins::OpenVRCameraInterface* openvr_camera_;

    bool is_streamed_ = false;
    NodePath preview_plane_;
    std::vector<uint8_t> framebuffer_;
    vr::EVRTrackedCameraFrameType frame_type_ = vr::VRTrackedCameraFrameType_Distorted;

    double last_task_time_ = 0.0f;
};
