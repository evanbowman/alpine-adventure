#include "game.hpp"
#include "script.hpp"
#include "keyStates.hpp"
#include <sstream>
#include <fstream>
#include <thread>


template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}



namespace Game {

    static Context* context;


    struct Context {
        sf::VideoMode videoMode_;
        sf::RenderWindow window_;
        sf::RenderTexture shadowMap_;
        sf::RenderTexture world_;
        Camera camera_;
        KeyStates keyStates_;
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
        auto gfx = make_unique<GraphicsComponent>(context->testFace_,
                                                  context->testShadow_);
        obj->setGraphicsComponent(std::move(gfx));
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
        context->shadowMap_.clear();
        context->shadowMap_.setView(context->camera_.getOverworldView());
        context->world_.clear({220, 220, 220});
        context->world_.setView(context->camera_.getOverworldView());
        for (auto& object : context->gameObjects_) {
            if (auto gfx = object->getGraphicsComponent()) {
                gfx->setPosition(object->getPosition());
                gfx->drawFace(context->world_);
                gfx->mapShadow(context->shadowMap_);
            }
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

sexp Game_keyPressed(sexp ctx, sexp self, sexp_sint_t n, sexp key) {
    EXPECT_EXACT(key);
    const auto sel = (sf::Keyboard::Key)sexp_uint_value(key);
    const bool pressed = Game::context->keyStates_.isPressed(sel);
    return sexp_make_boolean(pressed);
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
        engine.exportFunction("Object_move", 3, (Fn)Object_setPosition);
        engine.setGlobal("Key_up", sf::Keyboard::Key::Up);
        engine.setGlobal("Key_down", sf::Keyboard::Key::Down);
        engine.setGlobal("Key_left", sf::Keyboard::Key::Left);
        engine.setGlobal("Key_right", sf::Keyboard::Key::Right);
        engine.setGlobal("Key_esc", sf::Keyboard::Key::Escape);
        engine.run("main.scm");
    }


}
