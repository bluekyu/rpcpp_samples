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
// Copyright (c) 2013-2016 NVIDIA Corporation. All rights reserved.

#pragma once

#include <cmath>
#include <float.h>
#include <cassert>
#include <string.h>

const float kPi = 3.141592653589f;
const float k2Pi = 2.0f*kPi;
const float kInvPi = 1.0f/kPi;
const float kInv2Pi = 0.5f/kPi;
const float kDegToRad = kPi/180.0f;
const float kRadToDeg = 180.0f/kPi;

inline float Sin(float theta)
{
    return sinf(theta);
}

inline float Cos(float theta)
{
    return cosf(theta);
}

// random number generator
inline uint32_t Rand(void)
{
    static uint32_t seed1 = 315645664;
    static uint32_t seed2 = seed1 ^ 0x13ab45fe;

    seed1 = (seed2 ^ ((seed1 << 5) | (seed1 >> 27))) ^ (seed1*seed2);
    seed2 = seed1 ^ ((seed2 << 12) | (seed2 >> 20));

    return seed1;
}

// returns a random number in the range [min, max)
inline uint32_t Rand(uint32_t min, uint32_t max)
{
    return min + Rand()%(max-min);
}

// returns random number between 0-1
inline float Randf()
{
    uint32_t value = Rand();
    uint32_t limit = 0xffffffff;

    return (float)value*(1.0f/(float)limit);
}

// returns random number between min and max
inline float Randf(float min, float max)
{
    //	return Lerp(min, max, ParticleRandf());
    float t = Randf();
    return (1.0f-t)*min + t*(max);
}

// returns random number between 0-max
inline float Randf(float max)
{
    return Randf()*max;
}

// returns a random unit vector (also can add an offset to generate around an off axis vector)
inline LVecBase3f RandomUnitVector()
{
    float phi = Randf(kPi*2.0f);
    float theta = Randf(kPi*2.0f);

    float cosTheta = Cos(theta);
    float sinTheta = Sin(theta);

    float cosPhi = Cos(phi);
    float sinPhi = Sin(phi);

    return LVecBase3f(cosTheta*sinPhi, cosPhi, sinTheta*sinPhi);
}

inline LVecBase3f RandVec3() { return LVecBase3f(Randf(-1.0f, 1.0f), Randf(-1.0f, 1.0f), Randf(-1.0f, 1.0f)); }
