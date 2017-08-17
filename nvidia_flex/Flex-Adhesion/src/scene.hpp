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
 * NVIDIA FleX Demo from "demo/scenes/adhesion.h"
 */

#pragma once

#include <rpflex/plugin.hpp>
#include <rpflex/instance_interface.hpp>
#include <rpflex/utils/shape_box.hpp>

#include "mesh.hpp"
#include "helpers.hpp"

class Scene: public rpflex::InstanceInterface
{
public:
    void initialize(rpflex::Plugin& rpflex_plugin) final
    {
        auto& buffer = rpflex_plugin.get_flex_buffer();
        auto& flex_params = rpflex_plugin.get_flex_params();
        auto& params = rpflex_plugin.get_plugin_params();

        float radius = 0.1f;

        flex_params.radius = radius;

        flex_params.fluid = true;
        flex_params.numIterations = 3;
        flex_params.vorticityConfinement = 0.0f;
        flex_params.fluidRestDistance = flex_params.radius*0.55f;
        flex_params.anisotropyScale = 3.0f / radius;
        flex_params.smoothing = 0.5f;
        flex_params.relaxationFactor = 1.f;
        flex_params.restitution = 0.0f;
        flex_params.collisionDistance = 0.01f;

        //g_fluidColor = Vec4(0.0f, 0.8f, 0.2f, 1.0f);
        //g_fluidColor = Vec4(0.7f, 0.6f, 0.6f, 0.2f);

        flex_params.dynamicFriction = 0.5f;
        flex_params.viscosity = 50.0f;
        flex_params.adhesion = 0.5f;
        flex_params.cohesion = 0.08f;
        flex_params.surfaceTension = 0.0f;

        params.num_extra_particles = 64 * 1024;

        entities_.push_back(std::make_shared<FlexShapeEntity>(rpflex_plugin, std::make_shared<rpflex::RPFlexShapeBox>(
            rpflex_plugin, LVecBase3f(1.0f, 0.1f, 1.5f), LVecBase3f(0.0f, 0.0f, 1.5f))));
        entities_.push_back(std::make_shared<FlexShapeEntity>(rpflex_plugin, std::make_shared<rpflex::RPFlexShapeBox>(
            rpflex_plugin, LVecBase3f(1.0f, 6.0f, 0.1f), LVecBase3f(-1.0f, 0.0f, 3.0f))));

        emitter.mEnabled = true;
        emitter.mSpeed = (flex_params.fluidRestDistance*2.f / (1/60.0f));

        //flex_params.numPlanes = 3;
    }

    void post_initialize(rpflex::Plugin& rpflex_plugin) final
    {
        // create particle node.
        particles_entity_ = std::make_shared<FlexParticlesEntity>(rpflex_plugin);
        entities_.push_back(particles_entity_);

        particles_entity_->get_particles_node()->set_active_point_count(rpflex_plugin.get_flex_buffer().active_indices.size());

        // emitter
        rpcore::Globals::base->accept("e", [](const Event* ev, void* data) {
            reinterpret_cast<Scene*>(data)->toggle_emit();
        }, this);
    }

    void sync_flex(rpflex::Plugin& rpflex_plugin) final
    {
        auto& buffer = rpflex_plugin.get_flex_buffer();

        if (is_emit)
        {
            UpdateEmitters(rpflex_plugin, emitter);
            particles_entity_->get_particles_node()->set_active_point_count(buffer.active_indices.size());
        }

        for (auto& entity: entities_)
            entity->update(rpflex_plugin);
    }

    void toggle_emit()
    {
        is_emit = !is_emit;
    }

private:
    std::vector<std::shared_ptr<FlexEntity>> entities_;
    std::shared_ptr<FlexParticlesEntity> particles_entity_;
    Emitter emitter;
    bool is_emit = false;
};
