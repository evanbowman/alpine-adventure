#pragma once

#include "keyStates.hpp"
#include "camera.hpp"
#include "textureDB.hpp"
#include "object.hpp"
#include "widjet.hpp"
#include <forward_list>
#include <deque>
#include "util.hpp"
#include <fstream>


namespace Game {

    using ObjectList = std::forward_list<ObjectPtr>;
    using WidjetList = std::forward_list<WidjetPtr>;
    using TextChannel = std::deque<unsigned>;
    using VideoRequest = std::packaged_task<void*(VideoContext&)>;
    using VideoRequestVector = std::vector<VideoRequest>;


    struct Context {
        Camera camera_;
        KeyStates keyStates_;
        TextureDB textureDB_;
        sf::Clock deltaClock_;
        std::ofstream logfile_;
        bool running_;

        Synchronized<ObjectList> objects_;
        Synchronized<WidjetList> widjets_;

        // Graphics updates *need* to happen on the main thread for some
        // operating systems. The easiest way to enforce this, is to require all
        // operations that deal with graphics state to accept a VideoContext as
        // a parameter. The graphics loop is the sole owner of the video
        // context, and anyone who wants to modify it must submit an asyncronous
        // request.
        Synchronized<VideoRequestVector> videoRequests_;

        template <typename F>
        std::future<void*> videoRequest(F&& task) {
            std::packaged_task<void*(VideoContext&)> t(std::forward<F>(task));
            auto fut = t.get_future();
            videoRequests_.enter([&](VideoRequestVector& reqs) {
                reqs.push_back(std::move(t));
            });
            return fut;
        }

        Synchronized<TextChannel> textChannel_;
        bool textChannelActive_;

        Context();
    };


    extern Context* gContext;

}
