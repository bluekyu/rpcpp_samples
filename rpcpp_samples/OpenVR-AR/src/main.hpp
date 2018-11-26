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

#include <nodePath.h>
#include <texture.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/rpobject.hpp>

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
    MainApp();
    ~MainApp();

    void toggle_streaming_action();

private:
    void setup_event();
    void load_scenes();
    void setup_ar_camera();

    // this is not optimized.
    AsyncTask::DoneStatus upload_texture(rppanda::FunctionalTask* task);

    std::unique_ptr<rpcore::RenderPipeline> render_pipeline_;
    std::unique_ptr<rpcore::MovementController> controller_;
    rpplugins::OpenVRPlugin* openvr_plugin_;

    rpplugins::OpenVRCameraInterface* openvr_camera_;
    NodePath ar_camera_np_;

    bool is_streamed_ = false;
    std::vector<uint8_t> framebuffer_;
    PT(Texture) ar_camera_texture_;

    double last_task_time_ = 0.0f;
};
