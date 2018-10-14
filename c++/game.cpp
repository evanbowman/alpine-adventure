#include "game.hpp"
#include "videoContext.hpp"
#include <thread>
#include "context.hpp"
#include <iostream>


namespace Game {

    void ContextDeleter::operator()(Context *p) {
        delete p;
        gContext = nullptr;
    }


    ContextPtr initialize() {
        if (gContext) {
            throw std::runtime_error("cannot initialize with bound context");
        }
        if (not sf::Shader::isAvailable()) {
            throw std::runtime_error("failed to set up shaders");
        }
        ContextPtr ctx(new Context);
        gContext = ctx.get();
        return ctx;
    }


    void update() {
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
        const auto elapsed = gContext->deltaClock_.restart().asMicroseconds();
        gContext->camera_.update(elapsed);
        auto stop = high_resolution_clock::now();
        auto logicUpdateDelta =
            std::chrono::duration_cast<nanoseconds>(stop - start);
        static const microseconds logicUpdateLimit(2000);
        std::this_thread::sleep_for(logicUpdateLimit -
                                    logicUpdateDelta);
    }


    ObjectPtr makeObject() {
        auto obj = std::make_shared<Object>();
        gContext->objects_.enter([&](ObjectList& list) {
            list.push_front(obj);
        });
        return obj;
    }


    ObjectPtr makeWidjet() {
        auto widjet = std::make_shared<Object>();
        gContext->widjets_.enter([&](ObjectList& list) {
            list.push_front(widjet);
        });
        return widjet;
    }


    static void windowEventLoop(VideoContext& vidCtx) {
        sf::Event event;
        while (vidCtx.window_.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                vidCtx.window_.close();
                gContext->running_ = false;
                break;

            case sf::Event::KeyPressed:
                gContext->keyStates_.pressed(event.key.code);
                break;

            case sf::Event::KeyReleased:
                gContext->keyStates_.released(event.key.code);
                break;

            case sf::Event::TextEntered:
                if (gContext->textChannelActive_) {
                    gContext->textChannel_.enter([&](TextChannel& t) {
                        t.push_back(event.text.unicode);
                    });
                }
                break;

            default: // FIXME
                break;
            }
        }
    }


    void drawObjects(VideoContext& vidCtx) {
        vidCtx.shadowMap_.clear();
        vidCtx.shadowMap_.setView(gContext->camera_.getOverworldView());
        vidCtx.world_.clear({220, 220, 220});
        vidCtx.world_.setView(gContext->camera_.getOverworldView());
        gContext->objects_.enter([&](ObjectList& list) {
            for (auto& object : list) {
                if (auto face = object->getFace()) {
                    face->setPosition(object->getPosition());
                    face->display(vidCtx.world_);
                }
                if (auto shadow = object->getShadow()) {
                    shadow->setPosition(object->getPosition());
                    shadow->display(vidCtx.shadowMap_);
                }
            }

        });
        vidCtx.world_.display();
        vidCtx.shadowMap_.display();

        vidCtx.window_.clear();
        sf::Sprite shadowMapSprite(vidCtx.shadowMap_.getTexture());
        vidCtx.lightingShader.setUniform("shadowMap",
                                         sf::Shader::CurrentTexture);
        vidCtx.lightingShader.setUniform("world",
                                         vidCtx.world_.getTexture());
        vidCtx.window_.draw(shadowMapSprite, &vidCtx.lightingShader);
    }


    void drawWidjets(VideoContext& vidCtx) {
        const sf::View windowView = gContext->camera_.getWindowView();
        const sf::Vector2f screenSize = {
            windowView.getSize().x / 2.f, windowView.getSize().y / 2.f
        };
        sf::View widjetView{
            {screenSize.x / 2, screenSize.y / 2}, screenSize
        };
        vidCtx.window_.setView(widjetView);
        gContext->widjets_.enter([&](ObjectList& list) {
            for (auto& widjet : list) {
                if (widjet->isVisible()) {
                    if (auto spr = widjet->getFace()) {
                        spr->display(vidCtx.window_);
                    }
                }
            }
        });
    }


    void display(VideoContext& vidCtx) {
        gContext->videoRequests_.enter([&vidCtx](VideoRequestVector& reqs) {
            for (auto& req : reqs) {
                req(vidCtx);
            }
            reqs.clear();
        });
        windowEventLoop(vidCtx);
        vidCtx.window_.setView(gContext->camera_.getWindowView());
        drawObjects(vidCtx);
        drawWidjets(vidCtx);
        vidCtx.window_.display();
    }


    void runVideoLoop() {
        VideoContext vidCtx;
        while (vidCtx.window_.isOpen()) {
            display(vidCtx);
        }
    }


    bool isRunning() {
        return gContext->running_;
    }


}
