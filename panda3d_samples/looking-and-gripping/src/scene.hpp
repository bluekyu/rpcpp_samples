#pragma once

#include <memory>

class Scene
{
public:
    Scene(void);
    ~Scene(void);

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
