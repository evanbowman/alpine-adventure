#include "context.hpp"


static const auto videoMode = sf::VideoMode::getDesktopMode();


namespace Game {

    Context::Context() : camera_({videoMode.width / 2.f, videoMode.height / 2.f},
                                 {videoMode.width, videoMode.height}),
                         logfile_("logfile.txt"),
                         running_(true),
                         textChannelActive_(false) {}

    Context* gContext;


}
