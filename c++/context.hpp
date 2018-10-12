#pragma once

#include "keyStates.hpp"
#include "camera.hpp"
#include "textureDB.hpp"
#include "object.hpp"
#include <forward_list>
#include <deque>
#include "util.hpp"
#include <fstream>


namespace Game {

    using GameObjectList = std::forward_list<ObjectPtr>;
    using TextChannel = std::deque<unsigned>;


    struct Context {
        Camera camera_;
        KeyStates keyStates_;
        TextureDB textureDB_;
        sf::Clock deltaClock_;
        std::ofstream logfile_;
        bool running_;

        Synchronized<GameObjectList> gameObjects_;

        // Graphics updates *need* to happen on the main thread for some
        // operating systems. The easiest way to enforce this, is to require all
        // operations that deal with graphics state to accept a VideoContext as
        // a parameter. The graphics loop is the sole owner of the video
        // context, and anyone who wants to modify it must submit an asyncronous
        // request.
        std::mutex videoRequestsMutex_;
        std::vector<std::packaged_task<void*(VideoContext&)>> videoRequests_;

        template <typename F>
        std::future<void*> videoRequest(F&& task) {
            std::packaged_task<void*(VideoContext&)> t(std::forward<F>(task));
            auto fut = t.get_future();
            std::lock_guard<std::mutex> guard(videoRequestsMutex_);
            videoRequests_.push_back(std::move(t));
            return fut;
        }

        Synchronized<TextChannel> textChannel_;
        bool textChannelActive_;

        Context();
    };


    extern Context* gContext;

}
