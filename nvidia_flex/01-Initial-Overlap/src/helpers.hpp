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

#include <flex_buffer.hpp>

void add_box(FlexBuffer& buffer, const LVecBase3f& half_edge=LVecBase3f(2.0f), const LVecBase3f& center=LVecBase3f(0.0f), const LQuaternionf& quat=LQuaternionf(), bool dynamic=false)
{
    // physics
    buffer.shape_positions_.push_back(LVecBase4f(center, 0.0f));
    buffer.shape_rotations_.push_back(quat);

    buffer.shape_prev_positions_.push_back(buffer.shape_positions_.back());
    buffer.shape_prev_rotations_.push_back(buffer.shape_rotations_.back());

    NvFlexCollisionGeometry geo;
    geo.box.halfExtents[0] = half_edge[0];
    geo.box.halfExtents[1] = half_edge[1];
    geo.box.halfExtents[2] = half_edge[2];

    buffer.shape_geometry_.push_back(geo);
    buffer.shape_flags_.push_back(NvFlexMakeShapeFlags(eNvFlexShapeBox, dynamic));
}
