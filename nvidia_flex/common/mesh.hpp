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

#include <render_pipeline/rpcore/util/primitives.hpp>
#include <render_pipeline/rpcore/util/circular_points_node.hpp>

#include <rpflex/plugin.hpp>
#include <rpflex/flex_buffer.hpp>
#include <rpflex/utils/shape.hpp>

class FlexEntity
{
public:
    virtual void update(rpflex::Plugin& rpflex_plugin) = 0;
};

class FlexShapeEntity: public FlexEntity
{
public:
    FlexShapeEntity(rpflex::Plugin& rpflex_plugin, const std::shared_ptr<rpflex::RPFlexShape>& flex_shape): flex_shape_(flex_shape)
    {
        const auto& buffer = rpflex_plugin.get_flex_buffer();

        const int flags = buffer.shape_flags[flex_shape_->get_shape_buffer_index()];

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
            nodepath_ = rpcore::create_cube("box");
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
            std::cout << "Unknown shape type." << std::endl;
        }

        nodepath_.reparent_to(rpcore::Globals::render);
    }

    void update(rpflex::Plugin& rpflex_plugin)
    {
        const auto& buffer = rpflex_plugin.get_flex_buffer();

        int index = flex_shape_->get_shape_buffer_index();

        const int flags = buffer.shape_flags[index];

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
        const LQuaternionf& rotation = buffer.shape_prev_rotations[index];
        const LVecBase3f& position = buffer.shape_prev_positions[index].get_xyz();

        NvFlexCollisionGeometry geo = buffer.shape_geometry[index];

        if (type == eNvFlexShapeSphere)
        {
        }
        else if (type == eNvFlexShapeCapsule)
        {
        }
        else if (type == eNvFlexShapeBox)
        {
            nodepath_.set_pos_quat_scale(position, rotation, LVecBase3f(geo.box.halfExtents[0], geo.box.halfExtents[1], geo.box.halfExtents[2]) * 2.0f);
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

private:
    NodePath nodepath_;
    std::shared_ptr<rpflex::RPFlexShape> flex_shape_;
};

class FlexParticlesEntity: public FlexEntity
{
public:
    FlexParticlesEntity(rpflex::Plugin& rpflex_plugin)
    {
        const auto& buffer = rpflex_plugin.get_flex_buffer();
        const auto& flex_parmas = rpflex_plugin.get_flex_params();

        std::vector<LPoint3f> positions;
        positions.reserve(buffer.positions.size());
        for (int k=0, k_end=buffer.positions.size(); k < k_end; ++k)
            positions.push_back(buffer.positions[k].get_xyz());

        particles_node_ = std::make_shared<rpcore::CircularPointsNode>("particles", positions, flex_parmas.radius, "", GeomEnums::UH_dynamic);
        particles_node_->get_nodepath().reparent_to(rpcore::Globals::render);
    }

    void update(rpflex::Plugin& rpflex_plugin)
    {
        const auto& buffer = rpflex_plugin.get_flex_buffer();

        std::vector<LPoint3f> positions;
        positions.reserve(particles_node_->get_point_count());
        for (int k=0, k_end=particles_node_->get_point_count(); k < k_end; ++k)
            positions.push_back(buffer.positions[k].get_xyz());

        particles_node_->set_positions(positions);
        particles_node_->upload_positions();
    }

    const std::shared_ptr<rpcore::CircularPointsNode>& get_particles_node(void) const
    {
        return particles_node_;
    }

private:
    std::shared_ptr<rpcore::CircularPointsNode> particles_node_;
};
