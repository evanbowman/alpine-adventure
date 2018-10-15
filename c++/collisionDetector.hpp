#pragma once

#include "collider.hpp"
#include "object.hpp"

namespace Game {

class CollisionDetector {
public:
    void update();


    void track(ObjectPtr object);


private:
    struct Region {};


    std::map<std::pair<int, int>, Region> regions_;
};


}
