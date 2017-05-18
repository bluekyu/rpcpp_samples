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

#include <flex_plugin.hpp>
#include <flex_instance_interface.hpp>
#include <flex_buffer.hpp>

#include "helpers.hpp"
#include "mesh.hpp"

class Scene: public FlexInstanceInterface
{
public:
    Scene(const std::shared_ptr<FlexPlugin>& flex_plugin): flex_plugin_(flex_plugin)
    {
        auto& params = flex_plugin_->modify_flex_params();
        params.radius = 0.1f;
        params.numIterations = 2;

        // test max acceleration clamping is working, test at 5x gravity
        params.maxAcceleration = 50.0f;
    }

    void initialize(FlexBuffer& buffer) final
    {
        // plinth
        add_box(buffer, 1.0f, LVecBase3f(0.0f, 0.0f, 0.0f));

        buffer.positions_.push_back(LVecBase4f(0.0f, 0.5f, 0.0f, 1.0f));
        buffer.velocities_.push_back(LVecBase3f(0.0f));
        buffer.phases_.push_back(0);

        shapes_ = setup_shapes(buffer);
        update_shapes(buffer, shapes_);
    }

    void sync_flex(FlexBuffer& buffer) final
    {
        update_shapes(buffer, shapes_);
    }

private:
    std::shared_ptr<FlexPlugin> flex_plugin_;
    std::vector<NodePath> shapes_;
};
