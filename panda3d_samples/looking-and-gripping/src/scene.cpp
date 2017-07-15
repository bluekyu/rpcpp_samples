#include "scene.hpp"

#include <render_pipeline/rppanda/gui/onscreen_text.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>

class Scene::Impl
{
public:
    rppanda::OnscreenText title_;
};

Scene::Scene(void): impl_(std::make_unique<Impl>())
{
    rppanda::OnscreenText::Parameters params;
    params.text = "Panda3D: Tutorial - Joint Manipulation";
    params.fg = LColor(1, 1, 1, 1);
    params.parent = rppanda::ShowBase::get_global_ptr()->get_aspect_2d().find("a2d_bottom_right");
    params.align = TextProperties::A_right;
    params.pos = LVecBase2(-0.1f, 0.1f);
    params.shadow = LColor(0, 0, 0, 0.5f);
    params.scale = 0.08f;
    impl_->title_ = rppanda::OnscreenText(params);
}

Scene::~Scene(void) = default;
