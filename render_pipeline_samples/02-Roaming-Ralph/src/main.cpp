/**
 * MIT License
 *
 * Copyright (c) 2014-2016 tobspr <tobias.springer1@gmail.com>
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
 *
 * Roaming Ralph Sample (modified)
 * 
 * This is the default roaming ralph sample, with the render pipeline.
 * Using the render pipeline is only the matter of a few lines, which have
 * been explicitely marked.
 *
 * NOTICE: Since this is a straight copy of the standard roaming ralph sample,
 *         this attempts to keep as close to the original code to make it easier
 *         to see where to load the render pipeline.
 * 
 *         If you find a bug/suggestion in this code, then you should report
 *         that to the sample included in Panda3D, and not this code.
 *
 *         (and yeah, this code could surely be written in a much nicer way)
 */

#include <load_prc_file.h>
#include <virtualFileSystem.h>
#include <nodePathCollection.h>
#include <graphicsWindow.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/showbase/loader.hpp>
#include <render_pipeline/rppanda/showbase/messenger.hpp>
#include <render_pipeline/rppanda/actor/actor.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>
#include <render_pipeline/rpcore/pluginbase/day_manager.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rpcore/util/movement_controller.hpp>
#include <render_pipeline/rpcore/loader.hpp>

class World : public rppanda::ShowBase
{
public:
    static constexpr float SPEED = 0.5f;

public:
    World() : ShowBase(true)
    {
        // Setup window size, title and so on
        load_prc_file_data("",
            "window-title Render Pipeline - Shading Models Demo");

        // configure panda3d in program.
        render_pipeline_ = std::make_unique<rpcore::RenderPipeline>();

        render_pipeline_->get_mount_mgr()->set_config_dir("../etc/rpsamples/default");
        VirtualFileSystem::get_global_ptr()->mount("../share/rpcpp_samples/02-Roaming-Ralph", "/$$app", 0);

        render_pipeline_->create(this);

        // Set time of day
        render_pipeline_->get_daytime_mgr()->set_time("7:40");

        // Use a special effect for rendering the scene, this is because the
        // roaming ralph model has no normals or valid materials
        render_pipeline_->set_effect(get_render(), "/$$app/scene-effect.yaml", {}, 250);

        key_map_ = { {"left", 0}, {"right", 0}, {"forward", 0}, {"backward", 0}, {"cam-left", 0}, {"cam-right", 0} };
        speed_ = 1.0f;
        get_win()->set_clear_color(LColor(0, 0, 0, 1));

        // Set up the environment
        // 
        // This environment model contains collision meshes.  If you look
        // in the egg file, you will see the following:
        // 
        //    <Collide> { Polyset keep descend }
        // 
        // This tag causes the following mesh to be converted to a collision
        // mesh -- a mesh which is optimized for collision, not rendering.
        // It also keeps the original mesh, so there are now two copies ---
        // one optimized for rendering, one for collisions.

        // Load the scene
        environ_ = get_loader()->load_model("/$$app/resources/world");
        environ_.reparent_to(get_render());
        environ_.set_pos(0);

        // Remove wall nodes
        environ_.find("**/wall").remove_node();

        // Create the main character, Ralph
        ralph_ = new rppanda::Actor(
            rppanda::Actor::ModelsType{ "/$$app/resources/ralph" },
            rppanda::Actor::AnimsType{
                { "run", "/$$app/resources/ralph-run" },
                { "walk", "/$$app/resources/ralph-walk" }
            });
        ralph_->reparent_to(get_render());
        ralph_->set_scale(0.2f);
        ralph_->set_pos(-110.9f, 29.4f, 1.8f);

        // Create a floater object.We use the "floater" as a temporary
        // variable in a variety of calculations.

        floater_ = NodePath(new PandaNode("floater"));
        floater_.reparent_to(get_render());

        // Accept the control keys for movement and rotation

        accept("escape", [this](const Event*) { user_exit(); });
        accept("a", [this](const Event*) { set_key("left", 1); });
        accept("d", [this](const Event*) { set_key("right", 1); });
        accept("w", [this](const Event*) { set_key("forward", 1); });
        accept("s", [this](const Event*) { set_key("backward", 1); });
        accept("arrow_left", [this](const Event*) { set_key("cam-left", 1); });
        accept("arrow_right", [this](const Event*) { set_key("cam-right", 1); });
        accept("a-up", [this](const Event*) { set_key("left", 0); });
        accept("d-up", [this](const Event*) { set_key("right", 0); });
        accept("w-up", [this](const Event*) { set_key("forward", 0); });
        accept("s-up", [this](const Event*) { set_key("backward", 0); });
        accept("arrow_left-up", [this](const Event*) { set_key("cam-left", 0); });
        accept("arrow_right-up", [this](const Event*) { set_key("cam-right", 0); });
        accept("=", [this](const Event*) { adjust_speed(0.25f); });
        accept("+", [this](const Event*) { adjust_speed(0.25f); });
        accept("-", [this](const Event*) { adjust_speed(-0.25f); });

        add_task([this](rppanda::FunctionalTask* task) { return move(task); }, "move_task");

        // Game state variables
        is_moving_ = false;

        // Set up the camera
        disable_mouse();
        get_camera().set_pos(ralph_->get_x() + 10, ralph_->get_y() + 10, 2);
        get_cam_lens()->set_fov(80);

        // We will detect the height of the terrain by creating a collision
        // ray and casting it downward toward the terrain.  One ray will
        // start above ralph's head, and the other will start above the camera.
        // A ray may hit the terrain, or it may hit a rock or a tree.  If it
        // hits the terrain, we can detect the height.  If it hits anything
        // else, we rule that the move is illegal.
    }

    virtual ~World() = default;

    // Records the state of the arrow keys
    void set_key(const std::string& key, int value)
    {
        key_map_[key] = value;
    }

    // Adjust movement speed
    void adjust_speed(float delta)
    {
        float new_speed = speed_ + delta;
        if (0 <= new_speed && new_speed <= 3)
            speed_ = new_speed;
    }

    // Accepts arrow keys to move either the player or the menu cursor,
    // Also deals with grid checking and collision detection
    AsyncTask::DoneStatus move(rppanda::FunctionalTask* task)
    {
        // If the camera - left key is pressed, move camera left.
        // If the camera - right key is pressed, move camera right.

        auto cam = get_camera();
        const auto dt = rpcore::Globals::clock->get_dt();

        cam.look_at(*ralph_);
        if (key_map_["cam-left"])
            cam.set_x(cam, +20 * dt);
        if (key_map_["cam-right"])
            cam.set_x(cam, -20 * dt);

        // save ralph's initial position so that we can restore it,
        // in case he falls off the map or runs into something.

        auto startpos = ralph_->get_pos();

        // If a move - key is pressed, move ralph in the specified direction.

        if (key_map_["left"])
            ralph_->set_h(ralph_->get_h() + 300 * dt);
        if (key_map_["right"])
            ralph_->set_h(ralph_->get_h() - 300 * dt);
        if (key_map_["forward"])
            ralph_->set_y(*ralph_, -25 * speed_ * dt);
        if (key_map_["backward"])
            ralph_->set_y(*ralph_, 25 * speed_ * dt);

        // If ralph is moving, loop the run animation.
        // If he is standing still, stop the animation.

        if (key_map_["forward"] || key_map_["backward"] || key_map_["left"] || key_map_["right"])
        {
            if (!is_moving_)
            {
                ralph_->loop({"run"});
                is_moving_ = true;
            }
        }
        else
        {
            if (is_moving_)
            {
                ralph_->stop_all();
                ralph_->pose({"walk"}, 5);
                is_moving_ = false;
            }
        }

        // If the camera is too far from ralph, move it closer.
        // If the camera is too close to ralph, move it farther.


    }

private:
    std::unique_ptr<rpcore::RenderPipeline> render_pipeline_;
    std::unique_ptr<rpcore::MovementController> controller_;

    std::unordered_map<std::string, int> key_map_;
    float speed_;
    NodePath environ_;
    PT(rppanda::Actor) ralph_;
    NodePath floater_;
    bool is_moving_;
};

int main()
{
    World().run();

    return 0;
}
