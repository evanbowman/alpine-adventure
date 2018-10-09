#include "game.hpp"
#include "script.hpp"
#include <sstream>
#include <fstream>
#include <thread>


#include <chibi/eval.h>
extern "C" {
sexp Game_update(sexp ctx, sexp self, sexp_sint_t n) {
    Game::update();
    return SEXP_NULL;
}

sexp Game_isRunning(sexp ctx, sexp self, sexp_sint_t n) {
    return sexp_make_boolean(Game::isRunning());
}

sexp Game_makeObject(sexp ctx, sexp self, sexp_sint_t n) {
    return sexp_make_cpointer(ctx, 0, Game::makeObject().get(),
                              nullptr, false);
}

sexp Object_setPosition(sexp ctx, sexp self, sexp_sint_t n,
                        sexp obj, sexp x, sexp y) {
    Object* object = (Object*)sexp_cpointer_value(obj);
    object->setPosition({(float)sexp_flonum_value(x),
                         (float)sexp_flonum_value(y)});
    return obj;
}
}


namespace Game {

    static Context* context;


    struct Context {
        sf::VideoMode videoMode_;
        sf::RenderWindow window_;
        sf::RenderTexture shadowMap_;
        sf::RenderTexture world_;
        Camera camera_;
        std::list<ObjectPtr> gameObjects_;
        sf::Shader lightingShader;
        sf::Texture testFace_, testShadow_;
        sf::Texture vignette_;
        sf::Clock deltaClock_;

        Context() :
            videoMode_(sf::VideoMode::getDesktopMode()),
            window_(videoMode_, "Test", sf::Style::Fullscreen),
            camera_({videoMode_.width / 2.f, videoMode_.height / 2.f},
                    {videoMode_.width, videoMode_.height}) {}
    };


    void ContextDeleter::operator()(Context *p) {
        delete p;
    }


    ContextPtr initialize() {
        ContextPtr ctx(new Context);
        bind(*ctx);
        ctx->shadowMap_.create(ctx->videoMode_.width, ctx->videoMode_.height);
        ctx->world_.create(ctx->videoMode_.width, ctx->videoMode_.height);
        ctx->testFace_.loadFromFile("Sprite-0001.png");
        ctx->testShadow_.loadFromFile("Sprite-0002.png");
        auto obj = makeObject();
        obj->setPosition({150, 150});
        ctx->camera_.setTarget(obj);
        ctx->window_.setVerticalSyncEnabled(true);
        ctx->window_.setFramerateLimit(60);
        ctx->window_.setMouseCursorVisible(false);
        if (not sf::Shader::isAvailable()) {
            throw std::runtime_error("unable to run without shaders");
        }
        ctx->lightingShader.loadFromFile("lighting.vert", "lighting.frag");
        ctx->vignette_.loadFromFile("vignette.png");
        return ctx;
    }


    void runUpdateLoop() {
        using Fn = void*(*)();
        ScriptEngine engine;
        engine.exportFunction("Game_update", 0, (Fn)Game_update);
        engine.exportFunction("Game_isRunning", 0, (Fn)Game_isRunning);
        engine.exportFunction("Game_makeObject", 0, (Fn)Game_makeObject);
        engine.exportFunction("Object_setPosition", 3, (Fn)Object_setPosition);
        engine.run("main.scm");
    }


    void update() {
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
        const auto elapsed = context->deltaClock_.restart().asMicroseconds();
        auto& guy = context->gameObjects_.front();
        auto pos = guy->getPosition();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            guy->setPosition({pos.x, pos.y - 1});
        }
        pos = guy->getPosition();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            guy->setPosition({pos.x, pos.y + 1});
        }
        pos = guy->getPosition();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            guy->setPosition({pos.x - 1, pos.y});
        }
        pos = guy->getPosition();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            guy->setPosition({pos.x + 1, pos.y});
        }
        context->camera_.update(elapsed);
        auto stop = high_resolution_clock::now();
        auto logicUpdateDelta =
            std::chrono::duration_cast<nanoseconds>(stop - start);
        static const microseconds logicUpdateLimit(2000);
        std::this_thread::sleep_for(logicUpdateLimit -
                                    logicUpdateDelta);
    }


    ObjectPtr makeObject() {
        auto obj = std::make_shared<Object>(context->testFace_,
                                            context->testShadow_);
        context->gameObjects_.push_back(obj);
        return obj;
    }


    void bind(Context& ctx) {
        context = &ctx;
    }


    void display() {
        sf::Event event;
        while (context->window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                context->window_.close();
            }
        }
        context->shadowMap_.clear();
        context->shadowMap_.setView(context->camera_.getOverworldView());
        context->world_.clear({220, 220, 220});
        context->world_.setView(context->camera_.getOverworldView());
        for (auto& object : context->gameObjects_) {
            object->drawFace(context->world_);
            object->mapShadow(context->shadowMap_);
        }
        context->world_.display();
        context->shadowMap_.display();

        context->window_.clear();
        context->window_.setView(context->camera_.getWindowView());
        sf::Sprite vignette(context->vignette_);
        const auto windowSize = context->window_.getSize();
        vignette.setScale(windowSize.x / 450.f, windowSize.y / 450.f);
        vignette.setOrigin(225, 225);
        vignette.setPosition(context->camera_.getWindowView().getCenter());
        vignette.setColor({255, 255, 255, 181});
        sf::Sprite shadowMapSprite(context->shadowMap_.getTexture());
        context->lightingShader.setUniform("shadowMap",
                                           sf::Shader::CurrentTexture);
        context->lightingShader.setUniform("world",
                                           context->world_.getTexture());
        context->window_.draw(shadowMapSprite, &context->lightingShader);
        context->window_.draw(vignette);
        context->window_.display();
    }


    bool isRunning() {
        return context->window_.isOpen();
    }
}
