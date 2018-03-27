/**
 * MIT License
 *
 * Copyright (c) 2018 Center of Human-centered Interaction for Coexistence
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

#include "world.hpp"

#include <lineSegs.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/render_pipeline.hpp>
#include <render_pipeline/rpcore/util/primitives.hpp>
#include <render_pipeline/rpcore/util/points_node.hpp>
#include <render_pipeline/rpcore/util/line_node.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>

World::World(rpcore::RenderPipeline* pipeline): pipeline_(pipeline)
{
    create_square_points();
    create_disk_points();
    create_sphere_points();

    create_line_primitive();
    create_vertex_colored_linesegs();
}

World::~World() = default;

void World::create_square_points()
{
    std::vector<LPoint3f> points = {
        { -2, 0, 2 },
        { -1, 0, 2 },
        { 0, 0, 2 },
        { 1, 0, 2 },
        { 2, 0, 2 },
    };

    rpcore::PointsNode pn("quad_points", points, 0.1f);
    pn.set_square_point_effect();
    pn.upload_positions();

    pn.get_nodepath().reparent_to(rpcore::Globals::render);
}

void World::create_disk_points()
{
    std::vector<LPoint3f> points = {
        { -2, 0, 1.5f },
        { -1, 0, 1.5f },
        { 0, 0, 1.5f },
        { 1, 0, 1.5f },
        { 2, 0, 1.5f },
    };

    rpcore::PointsNode pn("disk_points", points, 0.1f);
    pn.set_disk_point_effect();
    pn.upload_positions();

    pn.get_nodepath().reparent_to(rpcore::Globals::render);
}

void World::create_sphere_points()
{
    std::vector<LPoint3f> points = {
        { -2, 0, 1 },
        { -1, 0, 1 },
        { 0, 0, 1 },
        { 1, 0, 1 },
        { 2, 0, 1 },
    };

    rpcore::PointsNode pn("sphere_points", points, 0.1f);
    pn.set_sphere_point_effect();
    pn.upload_positions();

    pn.get_nodepath().reparent_to(rpcore::Globals::render);
}

void World::create_line_primitive()
{
    std::vector<LVecBase3> vertices = {
        { -2, 0, 0.5f },
        { -1, 0, 0.5f },
        { 0, 0, 0.5f },
        { 1, 0, 0.5f },
        { 2, 0, 0.5f },
    };

    auto line = rpcore::create_line("line", vertices, 10.0f);
    line.reparent_to(rpcore::Globals::render);

    rpcore::LineNode::set_line_effect(line);
}

void World::create_vertex_colored_linesegs()
{
    LineSegs line_segs;

    line_segs.set_color(1, 0, 0, 1);
    line_segs.move_to(-2, 0, 0);
    line_segs.draw_to(-0.5f, 0, 0);

    line_segs.set_color(0, 1, 0, 1);
    line_segs.draw_to(0.5f, 0, 0);

    line_segs.set_color(0, 0, 1, 1);
    line_segs.draw_to(2, 0, 0);

    line_segs.set_thickness(10.0f);

    auto line = rpcore::Globals::render.attach_new_node(line_segs.create());

    rpcore::RPMaterial mat;
    mat.set_roughness(1);
    mat.set_specular_ior(1);
    line.set_material(mat.get_material());

    rpcore::LineNode::set_vertex_color_line_effect(line);
}
