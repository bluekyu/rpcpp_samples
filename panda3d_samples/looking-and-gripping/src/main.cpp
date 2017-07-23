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
 * Last Updated: 2015-03-13
 *
 * This tutorial will cover exposing joints and manipulating them.Specifically,
 * we will take control of the neck joint of a humanoid character and rotate that
 * joint to always face the mouse cursor.This will in turn make the head of the
 * character "look" at the mouse cursor.We will also expose the hand joint and
 * use it as a point to "attach" objects that the character can hold.By
 * parenting an object to a hand joint, the object will stay in the character's
 * hand even if the hand is moving through an animation.
 */

#include <ambientLight.h>
#include <directionalLight.h>
#include <asyncTaskManager.h>
#include <mouseWatcher.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/gui/onscreen_text.hpp>
#include <render_pipeline/rppanda/actor/actor.hpp>
#include <render_pipeline/rppanda/showbase/loader.hpp>
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

        // Set up the key input
        accept("escape", [](const Event*) { std::exit(0); });
        accept("1", [](const Event*, void* data) { reinterpret_cast<LookingGrippingDemo*>(data)->switch_object(0); }, this);
        accept("2", [](const Event*, void* data) { reinterpret_cast<LookingGrippingDemo*>(data)->switch_object(1); }, this);
        accept("3", [](const Event*, void* data) { reinterpret_cast<LookingGrippingDemo*>(data)->switch_object(2); }, this);
        accept("4", [](const Event*, void* data) { reinterpret_cast<LookingGrippingDemo*>(data)->switch_object(3); }, this);

        disable_mouse();
        get_camera().set_pos(0, -15, 2);    // Position the camera

        eve_ = new rppanda::Actor(
            rppanda::Actor::ModelsType{"/$$rp/resources/looking-and-gripping/models/eve"},
            rppanda::Actor::AnimsType{{"walk", "/$$rp/resources/looking-and-gripping/models/eve_walk"}});
        eve_->reparent_to(get_render());

        // Now we use controlJoint to get a NodePath that's in control of her neck
        // This must be done before any animations are played
        eve_neck_ = eve_->control_joint(NodePath(), "modelRoot", "Neck");

        // We now play an animation.An animation must be played, or at least posed
        // for the nodepath we just got from controlJoint to actually effect the
        // model
        //self.eve.actorInterval("walk", playRate=2).loop()

        // Now we add a task that will take care of turning the head
        add_task(turn_head, this, "trun_head");

        // Now we will expose the joint the hand joint.ExposeJoint allows us to
        // get the position of a joint while it is animating.This is different than
        // controlJonit which stops that joint from animating but lets us move it.
        // This is particularly usefull for putting an object(like a weapon) in an
        // actor's hand
        right_hand_ = eve_->expose_joint(NodePath(), "modelRoot", "RightHand");

        // This is a table with models, positions, rotations, and scales of objects to
        // be attached to our exposed joint.These are stock models and so they needed
        // to be repositioned to look right.
        std::vector<std::tuple<std::string, LVecBase3f, LVecBase3f, float>> positions = {
            {"/$$rp/resources/panda3d/models/teapot", {0, -.66f, -.95f}, {90, 0, 90}, .4f},
            {"/$$rp/resources/looking-and-gripping/models/candycane", {.15f, -.99f, -.22f}, {90, 0, 90}, 1},
            {"/$$rp/resources/looking-and-gripping/models/banana", {.08f, -.1f, .09f}, {0, -90, 0}, 1.75f},
            {"/$$rp/resources/looking-and-gripping/models/sword", {.11f, .19f, .06f}, {0, 0, 90}, 1}
        };

        for (const auto& row: positions)
        {
            NodePath np = get_loader()->load_model(std::get<0>(row));
            np.set_pos(std::get<1>(row));
            np.set_hpr(std::get<2>(row));
            np.set_scale(std::get<3>(row));

            // Reparent the model to the exposed joint.That way when the joint moves,
            // the model we just loaded will move with it.
            np.reparent_to(right_hand_);
            models_.push_back(np);
        }

        switch_object(0);   // Make object 0 the first shown
        setup_lights();     // Put in some default lighting
    }

    /**
     * This is what we use to change which object it being held.It just hides all of
     * the objects and then unhides the one that was selected
     */
    void switch_object(int i)
    {
        for (auto& np: models_)
            np.hide();
        models_[i].show();
    }

    /**
     * This task gets the position of mouse each frame, and rotates the neck based
     * on it.
     */
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

    void setup_lights(void)
    {
        PT(AmbientLight) ambient_light = new AmbientLight("ambientLight");
        ambient_light->set_color(LColor(0.4f, 0.4f, 0.35f, 1.0f));
        PT(DirectionalLight) directional_light = new DirectionalLight("directionalLight");
        directional_light->set_direction(LVector3(0, 8, -2.5));
        directional_light->set_color(LColor(0.9, 0.8, 0.9, 1));
        get_render().set_light(get_render().attach_new_node(directional_light));
        get_render().set_light(get_render().attach_new_node(ambient_light));
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
    NodePath right_hand_;
    std::vector<NodePath> models_;
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