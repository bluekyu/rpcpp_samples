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

#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/primitives.hpp>
#include <render_pipeline/rpcore/util/rpmaterial.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rpcore/loader.hpp>

World::World(rpcore::RenderPipeline* pipeline): pipeline_(pipeline)
{
    create_copper_sphere();
    create_aluminum_sphere();
}

World::~World() = default;

void World::create_copper_sphere()
{
    // create sphere
    auto sphere = rpcore::create_sphere("copper-sphere", 180, 360);
    sphere.reparent_to(rpcore::Globals::render);
    sphere.set_pos({ -2, 0, 0 });

    rpcore::RPGeomNode gn(sphere);


    // set copper material to GeomNode
    rpcore::RPMaterial m;
    m.set_shading_model(rpcore::RPMaterial::ShadingModel::DEFAULT_MODEL);
    m.set_base_color({ 0.955f, 0.637f, 0.538f, 1.0f });
    m.set_normal_factor(0.874667f);
    m.set_roughness(0.526151f);
    m.set_specular_ior(1.5f);
    m.set_metallic(1);
    m.set_arbitrary0(0);
    gn.set_material(0, m);


    // load copper texture and set to GeomNode
    PT(Texture) t = rpcore::RPLoader::load_texture("/$$app/scene/tex/CopperBasecolor.png");
    t->set_format(Texture::Format::F_srgb);

    gn.set_basecolor_texture(0, t);
    gn.set_normal_texture(0, rpcore::RPLoader::load_texture("/$$app/scene/tex/CopperNormal.png"));
    gn.set_specular_texture(0, rpcore::RPLoader::load_texture("/$$app/scene/tex/CopperSpecular.png"));
    gn.set_roughness_texture(0, rpcore::RPLoader::load_texture("/$$app/scene/tex/empty_roughness.png"));
}

void World::create_aluminum_sphere()
{
    // create sphere
    auto sphere = rpcore::create_sphere("aluminum-sphere", 180, 360);
    sphere.reparent_to(rpcore::Globals::render);
    sphere.set_pos({ 2, 0, 0 });


    // set aluminum material to NodePath
    rpcore::RPMaterial m;
    m.set_shading_model(rpcore::RPMaterial::ShadingModel::DEFAULT_MODEL);
    m.set_base_color({ 0.913f, 0.921f, 0.925f, 1.0f });
    m.set_normal_factor(0.821333f);
    m.set_roughness(0.345455f);
    m.set_specular_ior(1.5f);
    m.set_metallic(1);
    m.set_arbitrary0(0);

    // NOTE: sphere geometry has already material.
    // so, we set high priority.
    sphere.set_material(m.get_material(), 1);


    // load aluminum texture and set to NodePath
    rpcore::RPRenderState state(sphere.get_state());

    PT(Texture) t = rpcore::RPLoader::load_texture("/$$app/scene/tex/MetalBasecolor.png");
    t->set_format(Texture::Format::F_srgb);

    state.set_basecolor_texture(t)
         .set_normal_texture(rpcore::RPLoader::load_texture("/$$app/scene/tex/MetalNormal.png"))
         .set_specular_texture(rpcore::RPLoader::load_texture("/$$app/scene/tex/MetalSpecular.png"))
         .set_roughness_texture(rpcore::RPLoader::load_texture("/$$app/scene/tex/empty_roughness.png"));

    sphere.set_state(state.get_state());
}
