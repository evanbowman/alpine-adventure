#include "game.hpp"
#include "script.hpp"
#include "keyStates.hpp"
#include <sstream>
#include <fstream>
#include <thread>
#include <future>


template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


namespace Game {

    static Context* context;


    class TextureDB {
    public:
        std::future<sf::Texture*> loadTexture(const std::string& name) {
            std::promise<sf::Texture*> promise;
            auto future = promise.get_future();
            {
                std::lock_guard<std::mutex> guard(requestsMutex_);
                requests_.push_back({name, std::move(promise)});
            }
            return future;
        }

        void update() {
            std::lock_guard<std::mutex> guard(requestsMutex_);
            for (auto& req : requests_) {
                auto inserted = textures_.insert({req.first, sf::Texture{}});
                if (inserted.second) {
                    if (not inserted.first->second.loadFromFile(req.first)) {
                        // ...
                    }
                    req.second.set_value(&inserted.first->second);
                } else {
                    // ...
                }
            }
            requests_.clear();
        }

    private:
        std::mutex requestsMutex_;
        std::vector<std::pair<std::string,
                              std::promise<sf::Texture*>>> requests_;
        std::map<std::string, sf::Texture> textures_;
    };


    struct Context {
        sf::VideoMode videoMode_;
        sf::RenderWindow window_;
        sf::RenderTexture shadowMap_;
        sf::RenderTexture world_;
        Camera camera_;
        KeyStates keyStates_;
        std::list<ObjectPtr> gameObjects_;
        TextureDB textureDB_;
        sf::Shader lightingShader;
        sf::Texture testFace_, testShadow_;
        sf::Texture vignetteTexture_;
        sf::Sprite vignette_;
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
        ctx->window_.setVerticalSyncEnabled(true);
        ctx->window_.setFramerateLimit(60);
        ctx->window_.setMouseCursorVisible(false);
        if (not sf::Shader::isAvailable()) {
            throw std::runtime_error("unable to run without shaders");
        }
        ctx->lightingShader.loadFromFile("lighting.vert", "lighting.frag");
        ctx->vignetteTexture_.loadFromFile("vignette.png");
        ctx->vignette_.setTexture(context->vignetteTexture_);
        const auto windowSize = context->window_.getSize();
        ctx->vignette_.setScale(windowSize.x / 450.f, windowSize.y / 450.f);
        ctx->vignette_.setOrigin(225, 225);
        ctx->vignette_.setColor({255, 255, 255, 181});
        return ctx;
    }


    void update() {
        using namespace std::chrono;
        auto start = high_resolution_clock::now();
        const auto elapsed = context->deltaClock_.restart().asMicroseconds();
        context->camera_.update(elapsed);
        auto stop = high_resolution_clock::now();
        auto logicUpdateDelta =
            std::chrono::duration_cast<nanoseconds>(stop - start);
        static const microseconds logicUpdateLimit(2000);
        std::this_thread::sleep_for(logicUpdateLimit -
                                    logicUpdateDelta);
    }


    ObjectPtr makeObject() {
        auto obj = std::make_shared<Object>();
        context->gameObjects_.push_back(obj);
        return obj;
    }


    void bind(Context& ctx) {
        context = &ctx;
    }


    static void windowEventLoop() {
        sf::Event event;
        while (context->window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                context->window_.close();
            } else if (event.type == sf::Event::KeyPressed) {
                context->keyStates_.pressed(event.key.code);
            } else if (event.type == sf::Event::KeyReleased) {
                context->keyStates_.released(event.key.code);
            }
        }
    }


    void display() {
        windowEventLoop();
        context->textureDB_.update();
        context->shadowMap_.clear();
        context->shadowMap_.setView(context->camera_.getOverworldView());
        context->world_.clear({220, 220, 220});
        context->world_.setView(context->camera_.getOverworldView());
        for (auto& object : context->gameObjects_) {
            if (auto face = object->getFace()) {
                face->setPosition(object->getPosition());
                face->display(context->world_);
            }
            if (auto shadow = object->getShadow()) {
                shadow->setPosition(object->getPosition());
                shadow->display(context->shadowMap_);
            }
        }
        context->world_.display();
        context->shadowMap_.display();

        context->window_.clear();
        context->window_.setView(context->camera_.getWindowView());
        sf::Sprite shadowMapSprite(context->shadowMap_.getTexture());
        context->lightingShader.setUniform("shadowMap",
                                           sf::Shader::CurrentTexture);
        context->lightingShader.setUniform("world",
                                           context->world_.getTexture());
        context->window_.draw(shadowMapSprite, &context->lightingShader);
        context->vignette_
            .setPosition(context->camera_.getWindowView().getCenter());
        context->window_.draw(context->vignette_);
        context->window_.display();
    }


    bool isRunning() {
        return context->window_.isOpen();
    }
}


#include <chibi/eval.h>
#ifndef NDEBUG // NOTE: In release mode, no error checking for params.
#define TYPE_EXCEPTION(__TYPE_, __ARG_) \
    return sexp_type_exception(ctx, self, __TYPE_, __ARG_);
#define EXPECT_EXACT(__EXACT_VAR) \
    if (!sexp_exact_integerp(__EXACT_VAR)) \
        TYPE_EXCEPTION(SEXP_FIXNUM, __EXACT_VAR);
#define EXPECT_FLOAT(__FLOAT_VAR) \
    if (!sexp_flonump(__FLOAT_VAR)) TYPE_EXCEPTION(SEXP_FLONUM, __FLOAT_VAR);
#define EXPECT_PAIR(__PAIR_VAR) \
    if (!sexp_pairp(__PAIR_VAR)) TYPE_EXCEPTION(SEXP_PAIR, __PAIR_VAR)
#define EXPECT_STRING(__STRING_VAR) \
    if (!sexp_stringp(__STRING_VAR)) TYPE_EXCEPTION(SEXP_STRING, __STRING_VAR);
#define EXPECT_BOOL(__BOOL_VAR) \
    if (!sexp_booleanp(__BOOL_VAR)) TYPE_EXCEPTION(SEXP_BOOLEAN, __BOOL_VAR);
#define EXPECT_POINTER(__POINTER_VAR) \
    if (!sexp_cpointerp(__POINTER_VAR)) \
        TYPE_EXCEPTION(SEXP_CPOINTER, __POINTER_VAR);
#else
#define EXPECT_EXACT(__EXACT_VAR)
#define EXPECT_FLOAT(__FLOAT_VAR)
#define EXPECT_PAIR(__PAIR_VAR)
#define EXPECT_STRING(__STRING_VAR)
#define EXPECT_BOOL(__BOOL_VAR)
#endif // NDEBUG

#include <iostream>
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
    EXPECT_FLOAT(x); EXPECT_FLOAT(y);
    Object* object = (Object*)sexp_cpointer_value(obj);
    object->setPosition({(float)sexp_flonum_value(x),
                         (float)sexp_flonum_value(y)});
    return obj;
}

sexp Object_setFace(sexp ctx, sexp self, sexp_sint_t n,
                    sexp obj) {
    auto face = make_unique<Sprite>(Game::context->testFace_);
    ((Object*)sexp_cpointer_value(obj))->setFace(std::move(face));
    return obj;
}

sexp Object_setShadow(sexp ctx, sexp self, sexp_sint_t n,
                      sexp obj) {
    auto shadow = make_unique<Sprite>(Game::context->testShadow_);
    ((Object*)sexp_cpointer_value(obj))->setShadow(std::move(shadow));
    return obj;
}

sexp Game_setCameraTarget(sexp ctx, sexp self, sexp_sint_t n,
                          sexp obj) {
    const auto object = (Object*)sexp_cpointer_value(obj);
    for (auto& obj : Game::context->gameObjects_) {
        if (obj.get() == object) {
            Game::context->camera_.setTarget(obj);
            return SEXP_NULL;
        }
    }
    return SEXP_NULL;
}

sexp Game_keyPressed(sexp ctx, sexp self, sexp_sint_t n, sexp key) {
    EXPECT_EXACT(key);
    const auto sel = (sf::Keyboard::Key)sexp_uint_value(key);
    const bool pressed = Game::context->keyStates_.isPressed(sel);
    return sexp_make_boolean(pressed);
}

sexp Game_createTexture(sexp ctx, sexp self, sexp_sint_t n, sexp fname) {
    EXPECT_STRING(fname);
    auto fut = Game::context->textureDB_.loadTexture(sexp_string_data(fname));
    fut.wait();
    std::cout << "created texture" << std::endl;
    return sexp_make_cpointer(ctx, 0, fut.get(), nullptr, false);
}

}


namespace Game {

    void runUpdateLoop() {
        using Fn = void*(*)();
        ScriptEngine engine;
        engine.exportFunction("Game_update", 0, (Fn)Game_update);
        engine.exportFunction("Game_isRunning", 0, (Fn)Game_isRunning);
        engine.exportFunction("Game_makeObject", 0, (Fn)Game_makeObject);
        engine.exportFunction("Game_keyPressed", 1, (Fn)(Game_keyPressed));
        engine.exportFunction("Game_createTexture", 1, (Fn)(Game_createTexture));
        engine.exportFunction("Object_move", 3, (Fn)Object_setPosition);
        engine.exportFunction("Object_setFace", 1, (Fn)Object_setFace);
        engine.exportFunction("Object_setShadow", 1, (Fn)Object_setShadow);
        engine.setGlobal("Key_up", sf::Keyboard::Key::Up);
        engine.setGlobal("Key_down", sf::Keyboard::Key::Down);
        engine.setGlobal("Key_left", sf::Keyboard::Key::Left);
        engine.setGlobal("Key_right", sf::Keyboard::Key::Right);
        engine.setGlobal("Key_esc", sf::Keyboard::Key::Escape);
        engine.exportFunction("Game_setCameraTarget", 1,
                              (Fn)Game_setCameraTarget);

        engine.run("main.scm");
    }


}
