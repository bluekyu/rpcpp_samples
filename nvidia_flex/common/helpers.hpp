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

#include <rpflex/flex_buffer.hpp>

#include "maths.hpp"

void CreateParticleGrid(rpflex::FlexBuffer& buffer, const LVecBase3f& lower, int dimx, int dimy, int dimz, float radius,
    const LVecBase3f& velocity, float invMass, bool rigid, float rigidStiffness, int phase, float jitter=0.005f)
{
    if (rigid && buffer.rigid_indices_.empty())
        buffer.rigid_offsets_.push_back(0);

    for (int x = 0; x < dimx; ++x)
    {
        for (int y = 0; y < dimy; ++y)
        {
            for (int z=0; z < dimz; ++z)
            {
                if (rigid)
                    buffer.rigid_indices_.push_back(int(buffer.positions_.size()));

                LVecBase3f position = lower + LVecBase3f(float(x), float(y), float(z))*radius + RandomUnitVector()*jitter;

                buffer.positions_.push_back(LVecBase4f(position, invMass));
                buffer.velocities_.push_back(velocity);
                buffer.phases_.push_back(phase);
            }
        }
    }

    if (rigid)
    {
        buffer.rigid_coefficients_.push_back(rigidStiffness);
        buffer.rigid_offsets_.push_back(int(buffer.rigid_indices_.size()));
    }
}
