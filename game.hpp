#pragma once

#include <list>
#include <SFML/Graphics.hpp>

#include "types.hpp"
#include "camera.hpp"
#include "object.hpp"

namespace Game {

    struct Context;
    struct ContextDeleter { void operator()(Context *p); };
    using ContextPtr = std::unique_ptr<Context, ContextDeleter>;


    ContextPtr initialize();


    void bind(Context& context);


    void runUpdateLoop();


    void update();


    void display();


    bool isRunning();


    ObjectPtr makeObject();
}
