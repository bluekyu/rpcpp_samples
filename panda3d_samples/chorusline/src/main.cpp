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

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rppanda/actor/actor.hpp>
#include <render_pipeline/rpcore/mount_manager.hpp>

class ChorusLineDemo : public rppanda::ShowBase
{
public:
    ChorusLineDemo(int& argc, char**& argv): ShowBase(argc, argv)
    {
        PT(rppanda::Actor) panda = new rppanda::Actor(
            rppanda::Actor::ModelsType{ "models/panda-model" });
        panda->set_pos(0, 0, 0);
        panda->set_scale(0.007f);

        auto chorusline = NodePath("chorusline");

        for (int i = 0; i < 50; ++i)
        {
            auto placeholder = chorusline.attach_new_node("Panda-Placeholder");
            placeholder.set_pos(i * 5, 0, 0);
            panda->instance_to(placeholder);
        }

        for (int i = 0; i < 3; ++i)
        {
            auto placeholder = get_render().attach_new_node("Line-Placeholder");
            placeholder.set_pos(0, i * 10, 0);
            chorusline.instance_to(placeholder);
        }
    }

    ALLOC_DELETED_CHAIN(ChorusLineDemo);
};

int main(int argc, char* argv[])
{
    // setup directory of Render Pipeline. you can use mounted path.
    auto mount_manager = std::make_unique<rpcore::MountManager>();
    mount_manager->mount();

    {
        ChorusLineDemo demo(argc, argv);
        demo.run();
    }

    // release explicitly
    mount_manager.reset();

    return 0;
}
