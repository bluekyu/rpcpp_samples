#pragma once

#include <render_pipeline/rppanda/showbase/showbase.hpp>

namespace rpcore {
class RenderPipeline;
class MovementController;
}

class World : public rppanda::ShowBase
{
public:
    World(int argc, char* argv[]);

    virtual ~World();

    ALLOC_DELETED_CHAIN(World);

private:
    void on_imgui_new_frame();

    std::unique_ptr<rpcore::RenderPipeline> render_pipeline_;
    PT(rpcore::MovementController) controller_;
};
