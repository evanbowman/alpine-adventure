#pragma once

#include <list>
#include <SFML/Graphics.hpp>

#include "types.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "widjet.hpp"

namespace Game {

    struct Context;
    struct ContextDeleter { void operator()(Context *p); };
    using ContextPtr = std::unique_ptr<Context, ContextDeleter>;


    ContextPtr initialize();


    void runUpdateLoop();


    void runVideoLoop();


    bool isRunning();


    ObjectPtr makeObject();


    WidjetPtr makeWidjet();
}
