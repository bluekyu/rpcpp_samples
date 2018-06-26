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

namespace rpcore {
class RenderPipeline;
class MovementController;
}

namespace rpplugins {
class OpenVRPlugin;
class ViveSRPlugin;
}

class MainApp : public rppanda::ShowBase
{
public:
    MainApp(int argc, char* argv[]);
    virtual ~MainApp();

    ALLOC_DELETED_CHAIN(MainApp);

    void toggle_streaming_action(int frame_type);
    void toggle_streaming_seethrough(bool undistortion);

private:
    void setup_event();
    void on_imgui_new_frame();

    void copy_buffer_to_texture(const unsigned char* buffer, Texture* tex);

    void upload_seethrough_frame(bool undistortion);

    std::unique_ptr<rpcore::RenderPipeline> render_pipeline_;
    std::unique_ptr<rpcore::MovementController> controller_;

    rpplugins::OpenVRPlugin* openvr_plugin_;
    rpplugins::ViveSRPlugin* vivesr_plugin_;

    NodePath left_plane_;
    NodePath right_plane_;

    rppanda::FunctionalTask* task_ = nullptr;

    int current_frame_type_ = -1;
};
