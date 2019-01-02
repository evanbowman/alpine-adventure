#pragma once

#include <SFML/Graphics.hpp>
#include <list>

#include "camera.hpp"
#include "object.hpp"
#include "types.hpp"

namespace Game {

struct Context;
struct ContextDeleter {
    void operator()(Context * p);
};
using ContextPtr = std::unique_ptr<Context, ContextDeleter>;

ContextPtr initialize();

void runUpdateLoop();

void runVideoLoop();

bool isRunning();

ObjectPtr makeObject();

ObjectPtr makeWidjet();
} // namespace Game
