// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2013-2017 NVIDIA Corporation. All rights reserved.

#pragma once

#include <rpflex/plugin.hpp>
#include <rpflex/flex_buffer.hpp>

#include "maths.hpp"

struct Emitter
{
    Emitter() : mSpeed(0.0f), mEnabled(false), mLeftOver(0.0f), mWidth(8)   {}

    LVecBase3f mPos;
    LVecBase3f mDir;
    LVecBase3f mRight;
    float mSpeed;
    bool mEnabled;
    float mLeftOver;
    int mWidth;
};

void CreateParticleGrid(rpflex::FlexBuffer& buffer, const LVecBase3f& lower, int dimx, int dimy, int dimz, float radius,
    const LVecBase3f& velocity, float invMass, bool rigid, float rigidStiffness, int phase, float jitter=0.005f)
{
    if (rigid && buffer.rigid_indices.empty())
        buffer.rigid_offsets.push_back(0);

    for (int x = 0; x < dimx; ++x)
    {
        for (int y = 0; y < dimy; ++y)
        {
            for (int z=0; z < dimz; ++z)
            {
                if (rigid)
                    buffer.rigid_indices.push_back(int(buffer.positions.size()));

                LVecBase3f position = lower + LVecBase3f(float(x), float(y), float(z))*radius + RandomUnitVector()*jitter;

                buffer.positions.push_back(LVecBase4f(position, invMass));
                buffer.velocities.push_back(velocity);
                buffer.phases.push_back(phase);
            }
        }
    }

    if (rigid)
    {
        buffer.rigid_coefficients.push_back(rigidStiffness);
        buffer.rigid_offsets.push_back(int(buffer.rigid_indices.size()));
    }
}

void UpdateEmitters(rpflex::Plugin& rpflex_plugin, Emitter& emitter)
{
    const auto& flex_params = rpflex_plugin.get_flex_params();
    auto& buffer = rpflex_plugin.get_flex_buffer();

    NodePath cam = rpcore::Globals::base->get_cam();

    LVecBase3f spinned_cam = cam.get_hpr(rpcore::Globals::render) + LVecBase3f(15, 0, 0);

    LQuaternionf rot;
    rot.set_hpr(spinned_cam);
    LMatrix4f mat;
    rot.extract_to_matrix(mat);

    const LVecBase3f forward((LVecBase4f(0, 1, 0, 0) * mat).get_xyz());
    const LVecBase3f right(forward.cross(LVecBase3f(0.0f, 0.0f, 1.0f)).normalized());

    emitter.mDir = (forward + LVecBase3f(0.0, 0.0f, 0.4f)).normalized();
    emitter.mRight = right;
    emitter.mPos = cam.get_pos(rpcore::Globals::render) + forward*1.f + LVecBase3f(0.0f, 0.0f, 0.2f) + right*0.65f;

    // process emitters
    int activeCount = NvFlexGetActiveCount(rpflex_plugin.get_flex_solver());

    float r;
    int phase;

    if (flex_params.fluid)
    {
        r = flex_params.fluidRestDistance;
        phase = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid);
    }
    else
    {
        r = flex_params.solidRestDistance;
        phase = NvFlexMakePhase(0, eNvFlexPhaseSelfCollide);
    }

    float numParticles = (emitter.mSpeed / r) * (1/60.0f);

    // whole number to emit
    int n = int(numParticles + emitter.mLeftOver);

    if (n)
        emitter.mLeftOver = (numParticles + emitter.mLeftOver) - n;
    else
        emitter.mLeftOver += numParticles;

    // create a grid of particles (n particles thick)
    for (int k = 0; k < n; ++k)
    {
        int emitterWidth = emitter.mWidth;
        int numParticles = emitterWidth*emitterWidth;
        for (int i = 0; i < numParticles; ++i)
        {
            float x = float(i%emitterWidth) - float(emitterWidth/2);
            float y = float((i / emitterWidth) % emitterWidth) - float(emitterWidth/2);

            if ((x*x + y*y) <= (emitterWidth / 2)*(emitterWidth / 2))
            {
                LVecBase3f up = emitter.mDir.cross(emitter.mRight).normalized();
                LVecBase3f offset = r*(emitter.mRight*x + up*y) + float(k)*emitter.mDir*r;

                if (activeCount < buffer.positions.size())
                {
                    buffer.positions[activeCount] = LVecBase4f(emitter.mPos + offset, 1.0f);
                    buffer.velocities[activeCount] = emitter.mDir*emitter.mSpeed;
                    buffer.phases[activeCount] = phase;

                    buffer.active_indices.push_back(activeCount);

                    activeCount++;
                }
            }
        }
    }
}

