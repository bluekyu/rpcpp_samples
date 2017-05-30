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

#include <rpflex/plugin.hpp>
#include <rpflex/instance_interface.hpp>

#include "mesh.hpp"
#include "helpers.hpp"

class Scene: public rpflex::InstanceInterface
{
public:
    Scene(float radius): radius_(radius)
    {
    }

    void initialize(rpflex::Plugin& rpflex_plugin) final
    {
        auto& buffer = rpflex_plugin.get_flex_buffer();
        auto& flex_params = rpflex_plugin.get_flex_params();
        auto& params = rpflex_plugin.get_plugin_params();

        const float rest_distance = radius_ * 0.65f;

        int dx = int(std::ceil(1.0f / rest_distance));
        int dy = int(std::ceil(1.0f / rest_distance));
        int dz = int(std::ceil(2.0f / rest_distance));

        CreateParticleGrid(buffer, LVecBase3f(0.0f, 0.0f, rest_distance), dx, dy, dz, rest_distance, LVecBase3f(0.0f), 1.0f, false,
            0.0f, NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid), rest_distance*0.01f);

        params.scene_lower = LVecBase3f(0.0f, 0.5f, 0.0f);
        params.scene_upper = LVecBase3f(3.0f, 0.5f, 0.0f);

        params.substeps_count = 2;

        flex_params.fluid = true;
        flex_params.radius = radius_;
        flex_params.fluidRestDistance = rest_distance;
        flex_params.dynamicFriction = 0.f;
        flex_params.restitution = 0.001f;

        flex_params.numIterations = 3;
        flex_params.relaxationFactor = 1.0f;

        flex_params.smoothing = 0.4f;
        flex_params.anisotropyScale = 3.0f / radius_;

        flex_params.viscosity = 0.001f;
        flex_params.cohesion = 0.1f;
        flex_params.vorticityConfinement = 80.0f;
        flex_params.surfaceTension = 0.0f;

        flex_params.numPlanes = 5;

        // limit velocity to CFL condition
        flex_params.maxSpeed = 0.5f * radius_ * params.substeps_count / (1.0f / 30.0f);

        params.max_diffuse_particles = 0;

        params.wave_floor_tilt = 0.0f;

        // create particle node.
        entities_.push_back(std::make_shared<FlexParticlesEntity>(rpflex_plugin));
        for (auto& entity: entities_)
            entity->update(rpflex_plugin);
    }

    void sync_flex(rpflex::Plugin& rpflex_plugin) final
    {
        auto& buffer = rpflex_plugin.get_flex_buffer();
        for (auto& entity: entities_)
            entity->update(rpflex_plugin);
    }

private:
    std::vector<std::shared_ptr<FlexEntity>> entities_;

    float radius_;
};
