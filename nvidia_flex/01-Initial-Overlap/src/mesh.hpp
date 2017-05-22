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

#include <render_pipeline/rpcore/util/primitives.hpp>

#include <flex_buffer.hpp>

std::vector<NodePath> setup_shapes(const FlexBuffer& buffer)
{
    std::vector<NodePath> shapes;

    for (int i=0, i_end=buffer.shape_flags_.size(); i < i_end; ++i)
    {
        const int flags = buffer.shape_flags_[i];

        // unpack flags
        int type = int(flags & eNvFlexShapeFlagTypeMask);
        //bool dynamic = int(flags&eNvFlexShapeFlagDynamic) > 0;

        if (type == eNvFlexShapeSphere)
        {
        }
        else if (type == eNvFlexShapeCapsule)
        {
        }
        else if (type == eNvFlexShapeBox)
        {
            NodePath np = rpcore::create_cube("box");
            np.reparent_to(rpcore::Globals::render);
            shapes.push_back(np);

            NodePath p = rpcore::create_points("point", 1);
            p.set_pos(5, 5, 5);
            p.reparent_to(rpcore::Globals::render);
        }
        else if (type == eNvFlexShapeConvexMesh)
        {
        }
        else if (type == eNvFlexShapeTriangleMesh)
        {
        }
        else if (type == eNvFlexShapeSDF)
        {
        }
        else
        {
            shapes.push_back(NodePath());
        }
    }

    return shapes;
}

void update_shapes(const FlexBuffer& buffer, std::vector<NodePath>& shapes)
{
    for (int i=0, i_end=buffer.shape_flags_.size(); i < i_end; ++i)
    {
        const int flags = buffer.shape_flags_[i];

        // unpack flags
        int type = int(flags & eNvFlexShapeFlagTypeMask);
        //bool dynamic = int(flags&eNvFlexShapeFlagDynamic) > 0;

        LVecBase3f color = LVecBase3f(0.9f);

        if (flags & eNvFlexShapeFlagTrigger)
        {
            color = LVecBase3f(0.6f, 1.0, 0.6f);
        }

        // render with prev positions to match particle update order
        // can also think of this as current/next
        const LQuaternionf& rotation = buffer.shape_prev_rotations_[i];
        const LVecBase3f& position = buffer.shape_prev_positions_[i].get_xyz();

        NvFlexCollisionGeometry geo = buffer.shape_geometry_[i];

        if (type == eNvFlexShapeSphere)
        {
        }
        else if (type == eNvFlexShapeCapsule)
        {
        }
        else if (type == eNvFlexShapeBox)
        {
            shapes[i].set_pos_quat_scale(position, rotation, LVecBase3f(geo.box.halfExtents[0], geo.box.halfExtents[1], geo.box.halfExtents[2]) * 2.0f);
        }
        else if (type == eNvFlexShapeConvexMesh)
        {
        }
        else if (type == eNvFlexShapeTriangleMesh)
        {
        }
        else if (type == eNvFlexShapeSDF)
        {
        }
    }
}
