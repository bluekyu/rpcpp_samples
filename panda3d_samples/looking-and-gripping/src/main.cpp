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

/**
 * Author: Shao Zhang and Phil Saltzman
 * Models and Textures by: Shaun Budhram, Will Houng, and David Tucker
 * Last Updated: 2017-07-23
 *
 * This tutorial will cover exposing joints and manipulating them.Specifically,
 * we will take control of the neck joint of a humanoid character and rotate that
 * joint to always face the mouse cursor.This will in turn make the head of the
 * character "look" at the mouse cursor.We will also expose the hand joint and
 * use it as a point to "attach" objects that the character can hold.By
 * parenting an object to a hand joint, the object will stay in the character's
 * hand even if the hand is moving through an animation.
 */

#include <asyncTaskManager.h>
#include <mouseWatcher.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/gui/onscreen_text.hpp>
#include <render_pipeline/rppanda/actor/actor.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>

// A simple function to make sure a value is in a given range, -1 to 1 by
// default
inline float clamp(float i, float mn=-1, float mx=1)
{
    return (std::min)((std::max)(i, mn), mx);
}

// Macro-like function used to reduce the amount to code needed to create the
// on screen instructions
inline rppanda::OnscreenText gen_label_text(const std::string& text, int i)
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

        eve_ = new rppanda::Actor(
            rppanda::Actor::ModelsType{"/$$rp/resources/looking-and-gripping/models/eve"},
            rppanda::Actor::AnimsType{{"walk", "/$$rp/resources/looking-and-gripping/models/eve_walk"}});
        eve_->reparent_to(get_render());

        // Now we use controlJoint to get a NodePath that's in control of her neck
        // This must be done before any animations are played
        eve_neck_ = eve_->control_joint(NodePath(), "modelRoot", "Neck");



        // Now we add a task that will take care of turning the head
        add_task(turn_head, this, "trun_head");
    }

    static AsyncTask::DoneStatus turn_head(GenericAsyncTask* task, void* user_data)
    {
        // Check to make sure the mouse is readable
        LookingGrippingDemo* self = reinterpret_cast<LookingGrippingDemo*>(user_data);
        if (self->get_mouse_watcher_node()->has_mouse())
        {
            // get the mouse position as a LVector2.The values for each axis are from -1 to
            // 1. The top-left is(-1, -1), the bottom right is(1, 1)
            const auto& mpos = self->get_mouse_watcher_node()->get_mouse();
            // Here we multiply the values to get the amount of degrees to turn
            // Restrain is used to make sure the values returned by getMouse are in the
            // valid range.If this particular model were to turn more than this,
            // significant tearing would be visable
            self->eve_neck_.set_p(clamp(mpos.get_x()) * 50);
            self->eve_neck_.set_h(clamp(mpos.get_y()) * 20);
        }

        return AsyncTask::DoneStatus::DS_cont;
    }

private:
    rppanda::OnscreenText title_;
    rppanda::OnscreenText esckey_text_;
    rppanda::OnscreenText onekey_text_;
    rppanda::OnscreenText twokey_text_;
    rppanda::OnscreenText threekey_text_;
    rppanda::OnscreenText fourkey_text_;

    PT(rppanda::Actor) eve_;
    NodePath eve_neck_;
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
