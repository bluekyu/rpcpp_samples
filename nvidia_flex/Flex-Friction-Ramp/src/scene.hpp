/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2016, Center of human-centered interaction for coexistence.
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

#include <NvFlex.h>

#include <rpflex/plugin.hpp>
#include <rpflex/instance_interface.hpp>
#include <rpflex/flex_buffer.hpp>
#include <rpflex/utils/shape_box.hpp>

#include "mesh.hpp"

class Scene: public rpflex::InstanceInterface
{
public:
    Scene(const std::shared_ptr<rpflex::Plugin>& flex_plugin): flex_plugin_(flex_plugin)
    {
        auto& params = flex_plugin_->modify_flex_params();
        params.radius = 0.1f;
        params.dynamicFriction = 0.35f;
        params.dissipation = 0.0f;
        params.numIterations = 8;
        params.viscosity = 0.0f;
        params.drag = 0.0f;
        params.lift = 0.0f;
        params.collisionDistance = params.radius * 0.5f;
    }

    void initialize(rpflex::FlexBuffer& buffer) final
    {
        for (int i = 0; i < 3; ++i)
        {
            LQuaternionf quat;
            quat.set_from_axis_angle(-11.25f*(i + 1), LVecBase3f(0.0f, -1.0f, 0.0f));

            // ramp
            entities_.push_back(std::make_shared<FlexShapeEntity>(buffer, std::make_shared<rpflex::RPFlexShapeBox>(buffer,
                LVecBase3f(5.0f, 1.0f, 0.25f), LVecBase3f(3.0f, i*2.0f, 1.0f), quat)));
        }

        //buffer.positions_.push_back(LVecBase4f(0.0f, 0.0f, 3.5f, 1.0f));
        //buffer.velocities_.push_back(LVecBase3f(0.0f));
        //buffer.phases_.push_back(0);

        //entities_.push_back(std::make_shared<FlexParticlesEntity>(buffer, flex_plugin_->get_flex_params()));

        for (auto& entity: entities_)
            entity->update(buffer);
    }

    void sync_flex(rpflex::FlexBuffer& buffer) final
    {
        for (auto& entity: entities_)
            entity->update(buffer);
    }

private:
    std::shared_ptr<rpflex::Plugin> flex_plugin_;
    std::vector<std::shared_ptr<FlexEntity>> entities_;
};
