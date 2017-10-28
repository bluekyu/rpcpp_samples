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

/**
 * Author: Shao Zhang, Phil Saltzman, and Elan Ruskin
 * Last Updated: 2015-03-13
 *
 * This tutorial shows how to load, play, and manipulate sounds
 * and sound intervals in a panda project.
 */

#include <pointLight.h>
#include <ambientLight.h>
#include <pgSliderBar.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/showbase/loader.hpp>
#include <render_pipeline/rppanda/interval/sound_interval.hpp>
#include <render_pipeline/rppanda/gui/onscreen_text.hpp>
#include <render_pipeline/rppanda/gui/direct_slider.hpp>
#include <render_pipeline/rppanda/interval/meta_interval.hpp>
#include <render_pipeline/rppanda/interval/lerp_interval.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>

class MusicBox : public rppanda::DirectObject
{
public:
    MusicBox(rppanda::ShowBase* base): base_(base)
    {
        // Our standard title and instructions text
        rppanda::OnscreenText::Parameters title_params;
        title_params.text = "Panda3D: Tutorial - Music Box";
        title_params.parent = base_->get_aspect_2d().find("a2d_bottom_center");
        title_params.pos = LVecBase2(0, 0.08f);
        title_params.scale = 0.08f;
        title_params.fg = LColor(1, 1, 1, 1);
        title_params.shadow = LColor(0, 0, 0, 0.5f);
        title_ = rppanda::OnscreenText(title_params);

        rppanda::OnscreenText::Parameters escape_text_params;
        escape_text_params.text = "ESC: Quit";
        escape_text_params.parent = base_->get_aspect_2d().find("a2d_top_left");
        escape_text_params.fg = LColor(1, 1, 1, 1);
        escape_text_params.pos = LVecBase2(0.06, -0.1f);
        escape_text_params.align = TextProperties::A_left;
        escape_text_params.scale = 0.05f;
        escape_text_ = rppanda::OnscreenText(escape_text_params);

        // Set up the key input
        accept("escape", [](const Event*) { std::exit(0); });

        // Fix the camera position
        base_->disable_mouse();

        // Loading sounds is done in a similar way to loading other things
        // Loading the main music box song
        music_box_sound_ = base_->get_loader()->load_music("/$$rp/resources/music-box/music/musicbox.ogg");
        music_box_sound_->set_volume(0.5f);  // Volume is a percentage from 0 to 1
                                            // 0 means loop forever, 1 (default) means
                                            // play once. 2 or higher means play that many times
        music_box_sound_->set_loop_count(0);

        // Set up a simple light.
        plight_ = new PointLight("light");
        plight_->set_color(LColor(0.7, 0.7, 0.5, 1));
        NodePath light_path = base_->get_render().attach_new_node(plight_);
        light_path.set_pos(0, 0, 20);
        base_->get_render().set_light(light_path);

        alight_ = new AmbientLight("ambient");
        alight_->set_color(LColor(0.3, 0.3, 0.4, 1));
        base_->get_render().set_light(base_->get_render().attach_new_node(alight_));

        // Enable per-pixel lighting
        base_->get_render().set_shader_auto();

        // Sound objects do not have a pause function, just play and stop.So we will
        // Use this variable to keep track of where the sound is at when it was stoped
        // to impliment pausing
        music_time_ = 0.0f;

        // Loading the open/close effect
        // loadSFX and loadMusic are identical.They are often used for organization
        // (loadMusic is used for background music, loadSfx is used for other effects)
        lid_sfx_ = base_->get_loader()->load_sfx("/$$rp/resources/music-box/music/openclose.ogg");
        // The open/close file has both effects in it.Fortunatly we can use intervals
        // to easily define parts of a sound file to play

        lid_open_sfx_ = new rppanda::SoundInterval(lid_sfx_, false, 2.0f, {}, 1.0f, 0.0f);
        lid_close_sfx_ = new rppanda::SoundInterval(lid_sfx_, false, 0.0f, {}, 1.0f, 5.0f);

        // For this tutorial, it seemed appropriate to have on screen controls.
        // The following code creates them.
        // This is a label for a slider
        rppanda::OnscreenText::Parameters slider_tex_params;
        slider_tex_params.text = "Volume";
        slider_tex_params.pos = LVecBase2(-0.1f, 0.87f);
        slider_tex_params.scale = 0.07f;
        slider_tex_params.fg = LColor(1, 1, 1, 1);
        slider_tex_params.shadow = LColor(0, 0, 0, 1);
        slider_tex_ = rppanda::OnscreenText(slider_tex_params);

        // The slider itself.It calls self.setMusicBoxVolume when changed
        auto slider_options = std::make_shared<rppanda::DirectSlider::Options>();
        slider_options->pos = LVecBase3(-0.1f, 0.0f, 0.75f);
        slider_options->scale = 0.8f;
        slider_options->value = 0.5f;
        slider_options->command = std::bind(&MusicBox::set_music_box_volume, this, std::placeholders::_1);
        slider_ = new rppanda::DirectSlider(NodePath(), slider_options);

        // A button that calls self.toggleMusicBox when pressed
        auto button_options = std::make_shared<rppanda::DirectButton::Options>();
        button_options->pos = LVecBase3(0.9f, 0.0f, 0.75f);
        button_options->text = { "Open" };
        button_options->scale = 0.1f;
        button_options->pad = LVecBase2(0.2f, 0.2f);
        button_options->command = std::bind(&MusicBox::toggle_music_box, this, std::placeholders::_1);
        button_ = new rppanda::DirectButton(NodePath(), button_options);

        // A variable to represent the state of the simulation.It starts closed
        box_open_ = false;

        // Here we load and set up the music box.It was modeled in a complex way, so
        // setting it up will be complicated
        music_box_ = base_->get_loader()->load_model("/$$rp/resources/music-box/models/MusicBox");
        music_box_.set_pos(0, 60, -9);
        music_box_.reparent_to(base_->get_render());
        // Just like the scene graph contains hierarchies of nodes, so can
        // models.You can get the NodePath for the node using the find
        // function, and then you can animate the model by moving its parts
        // To see the hierarchy of a model, use, the ls function
        // self.musicBox.ls() prints out the entire hierarchy of the model

        // Finding pieces of the model
        lid_ = music_box_.find("**/lid");
        panda_ = music_box_.find("**/turningthing");

        // This model was made with the hinge in the wrong place
        // this is here so we have something to turn
        hinge_node_ = music_box_.find("**/box").attach_new_node("nHingeNode");
        hinge_node_.set_pos(.8659, 6.5, 5.4);

        // WRT - ie with respect to.Reparents the object without changing
        // its position, size, or orientation
        lid_.wrt_reparent_to(hinge_node_);
        hinge_node_.set_hpr(0, 90, 0);

        // This sets up an interval to play the close sound and actually close the box
        // at the same time.
        lid_close_ = new rppanda::Parallel({lid_close_sfx_, new rppanda::LerpHprInterval(
            hinge_node_, 2.0f, LVecBase3(0, 90, 0), {}, {}, {}, CLerpInterval::BlendType::BT_ease_in_out)});

        // Same thing for opening the box
        lid_open_ = new rppanda::Parallel({ lid_open_sfx_, new rppanda::LerpHprInterval(
            hinge_node_, 2.0f, LVecBase3(0, 0, 0), {}, {}, {}, CLerpInterval::BlendType::BT_ease_in_out)});

        // The interval for turning the panda
        //panda_turn_ = panda_. hprInterval(7, (360, 0, 0))
    }

    ALLOC_DELETED_CHAIN(MusicBox);

    void set_music_box_volume(const std::shared_ptr<void>&)
    {
        float new_vol = slider_->get_gui_item()->get_value();
        music_box_sound_->set_volume(new_vol);
    }

    void toggle_music_box(const std::shared_ptr<void>&)
    {
        if (box_open_)
        {
            lid_open_->pause();

            lid_close_->start();    // Start the close box interval

            // Save the current time of the music
            music_time_ = music_box_sound_->get_time();
            music_box_sound_->stop();   // Stop the music
            button_->set_text("Open");
        }
        else
        {
            lid_close_->pause();

            lid_open_->start();    // Start the close box interval

            // Reset the time of the music so it starts where it left off
            music_box_sound_->set_time(music_time_);
            music_box_sound_->play();   // Play the music
            button_->set_text("Close");
        }

        // Set our state to opposite what it was
        box_open_ = !box_open_;
        //(closed to open or open to closed)
    }

private:
    rppanda::ShowBase* base_;
    rppanda::OnscreenText title_;
    rppanda::OnscreenText escape_text_;
    PT(AudioSound) music_box_sound_;
    PT(PointLight) plight_;
    PT(AmbientLight) alight_;
    float music_time_;
    PT(AudioSound) lid_sfx_;
    PT(rppanda::SoundInterval) lid_open_sfx_;
    PT(rppanda::SoundInterval) lid_close_sfx_;
    rppanda::OnscreenText slider_tex_;
    PT(rppanda::DirectSlider) slider_;
    PT(rppanda::DirectButton) button_;
    bool box_open_;
    NodePath music_box_;
    NodePath lid_;
    NodePath panda_;
    NodePath hinge_node_;
    PT(rppanda::Parallel) lid_close_;
    PT(rppanda::Parallel) lid_open_;
};

int main(int argc, char* argv[])
{
    // setup directory of Render Pipeline. you can use mounted path.
    auto mount_manager = std::make_unique<rpcore::MountManager>();
    mount_manager->mount();

    PT(rppanda::ShowBase) base = new rppanda::ShowBase(argc, argv);

    {
        MusicBox mb(base);
        base->run();
    }

    // release explicitly
    base.clear();
    mount_manager.reset();

    return 0;
}
