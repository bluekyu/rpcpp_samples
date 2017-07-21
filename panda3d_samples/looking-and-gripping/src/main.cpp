/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017, Younguk Kim (bluekyu)
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

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/gui/onscreen_text.hpp>
#include <render_pipeline/rppanda/actor/actor.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>

// Macro-like function used to reduce the amount to code needed to create the
// on screen instructions
rppanda::OnscreenText gen_label_text(const std::string& text, int i)
{
    rppanda::OnscreenText::Parameters params;
    params.text = text;
    params.fg = LColor(1, 1, 1, 1);
    params.parent = rppanda::ShowBase::get_global_ptr()->get_aspect_2d().find("a2d_top_left");
    params.align = TextProperties::A_left;
    params.pos = LVecBase2(0.06f, -0.08f * i);
    params.shadow = LColor(0, 0, 0, 0.5f);
    params.scale = 0.06f;
    return rppanda::OnscreenText(params);
}

class LookingGrippingDemo: public rppanda::ShowBase
{
public:
    LookingGrippingDemo(int& argc, char**& argv): ShowBase(argc, argv)
    {
        // This code puts the standard title and instruction text on screen
        rppanda::OnscreenText::Parameters params;
        params.text = "Panda3D: Tutorial - Joint Manipulation";
        params.fg = LColor(1, 1, 1, 1);
        params.parent = rppanda::ShowBase::get_global_ptr()->get_aspect_2d().find("a2d_bottom_right");
        params.align = TextProperties::A_right;
        params.pos = LVecBase2(-0.1f, 0.1f);
        params.shadow = LColor(0, 0, 0, 0.5f);
        params.scale = 0.08f;
        title_ = rppanda::OnscreenText(params);

        // Set up key input
        esckey_text_ = gen_label_text("ESC: Quit", 1);
        onekey_text_ = gen_label_text("[1]: Teapot", 2);
        twokey_text_ = gen_label_text("[2]: Candy cane", 3);
        threekey_text_ = gen_label_text("[3]: Banana", 4);
        fourkey_text_ = gen_label_text("[4]: Sword", 5);

        disable_mouse();
        get_camera().set_pos(0, -15, 2);    // Position the camera

        eve_ = std::make_shared<rppanda::Actor>(
            rppanda::Actor::ModelsType{"/$$rp/resources/looking-and-gripping/models/eve"},
            rppanda::Actor::AnimsType{{"walk", "/$$rp/resources/looking-and-gripping/models/eve_walk"}});
        eve_->reparent_to(get_render());

        rppanda::Actor t;
        
        t = std::move(*eve_);
    }

private:
    rppanda::OnscreenText title_;
    rppanda::OnscreenText esckey_text_;
    rppanda::OnscreenText onekey_text_;
    rppanda::OnscreenText twokey_text_;
    rppanda::OnscreenText threekey_text_;
    rppanda::OnscreenText fourkey_text_;

    std::shared_ptr<rppanda::Actor> eve_;
};

int main(int argc, char* argv[])
{
    // setup directory of Render Pipeline. you can use mounted path.
    rpcore::MountManager mount_manager;
    mount_manager.set_base_path("../share/render_pipeline");
    mount_manager.mount();

    LookingGrippingDemo demo(argc, argv);
    demo.run();

    return 0;
}
