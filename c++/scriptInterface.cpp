#include "context.hpp"
#include "game.hpp"
#include "util.hpp"
#include "script.hpp"
#include "videoContext.hpp"
#include <chibi/eval.h>

#include <iostream>


namespace Game {

#ifndef NDEBUG // NOTE: In release mode, no error checking for params.
#define TYPE_EXCEPTION(__TYPE_, __ARG_)                         \
    return sexp_type_exception(ctx, self, __TYPE_, __ARG_);
#define EXPECT_EXACT(__EXACT_VAR)                       \
    if (!sexp_exact_integerp(__EXACT_VAR))              \
        TYPE_EXCEPTION(SEXP_FIXNUM, __EXACT_VAR);
#define EXPECT_FLOAT(__FLOAT_VAR)                                       \
    if (!sexp_flonump(__FLOAT_VAR)) TYPE_EXCEPTION(SEXP_FLONUM, __FLOAT_VAR);
#define EXPECT_PAIR(__PAIR_VAR)                                         \
    if (!sexp_pairp(__PAIR_VAR)) TYPE_EXCEPTION(SEXP_PAIR, __PAIR_VAR)
#define EXPECT_STRING(__STRING_VAR)                                     \
    if (!sexp_stringp(__STRING_VAR)) TYPE_EXCEPTION(SEXP_STRING, __STRING_VAR);
#define EXPECT_BOOL(__BOOL_VAR)                                         \
    if (!sexp_booleanp(__BOOL_VAR)) TYPE_EXCEPTION(SEXP_BOOLEAN, __BOOL_VAR);
#define EXPECT_POINTER(__POINTER_VAR)                   \
    if (!sexp_cpointerp(__POINTER_VAR))                 \
        TYPE_EXCEPTION(SEXP_CPOINTER, __POINTER_VAR);
#else
#define EXPECT_EXACT(__EXACT_VAR)
#define EXPECT_FLOAT(__FLOAT_VAR)
#define EXPECT_PAIR(__PAIR_VAR)
#define EXPECT_STRING(__STRING_VAR)
#define EXPECT_BOOL(__BOOL_VAR)
#endif // NDEBUG


    void update();


    static sexp Game_update(sexp ctx, sexp self, sexp_sint_t n) {
        Game::update();
        return SEXP_NULL;
    }

    static sexp Game_isRunning(sexp ctx, sexp self, sexp_sint_t n) {
        return sexp_make_boolean(Game::isRunning());
    }

    static sexp Game_makeObject(sexp ctx, sexp self, sexp_sint_t n) {
        return sexp_make_cpointer(ctx, SEXP_CPOINTER, Game::makeObject().get(),
                                  nullptr, false);
    }

    // TODO: If an object has a solid collider attached, move needs to check for
    // nearby objects, and prevent overlap with solids.
    static sexp Object_move(sexp ctx, sexp self, sexp_sint_t n,
                            sexp obj, sexp x, sexp y) {
        EXPECT_FLOAT(x); EXPECT_FLOAT(y);
        Object* object = (Object*)sexp_cpointer_value(obj);
        object->setPosition({(float)sexp_flonum_value(x),
                             (float)sexp_flonum_value(y)});
        return obj;
    }

    static sexp Object_setFace(sexp ctx, sexp self, sexp_sint_t n,
                               sexp obj, sexp txtr) {
        auto spr = make_unique<Sprite>(*(sf::Texture*)sexp_cpointer_value(txtr));
        ((Object*)sexp_cpointer_value(obj))->setFace(std::move(spr));
        return obj;
    }

    static sexp Object_setShadow(sexp ctx, sexp self, sexp_sint_t n,
                                 sexp obj, sexp txtr) {
        auto spr = make_unique<Sprite>(*(sf::Texture*)sexp_cpointer_value(txtr));
        ((Object*)sexp_cpointer_value(obj))->setShadow(std::move(spr));
        return obj;
    }

    static sexp Game_setCameraTarget(sexp ctx, sexp self, sexp_sint_t n,
                                     sexp obj) {
        // TODO: This could be bad when there're a lot of objects, but
        // on the other hand, changing the camera target almost never
        // happens... but I can imagine other cases where we would
        // want the shared pointer instead of the raw object
        // pointer... maybe shared_from_this would be ok.
        const auto object = (Object*)sexp_cpointer_value(obj);
        Game::gContext->gameObjects_.enter([&](Context::GameObjectList& list) {
            for (auto& obj : list) {
                if (obj.get() == object) {
                    Game::gContext->camera_.setTarget(obj);
                    return;
                }
            }
        });
        return SEXP_NULL;
    }

    static sexp Game_keyPressed(sexp ctx, sexp self, sexp_sint_t n, sexp key) {
        EXPECT_EXACT(key);
        const auto sel = (sf::Keyboard::Key)sexp_uint_value(key);
        const bool pressed = Game::gContext->keyStates_.isPressed(sel);
        return sexp_make_boolean(pressed);
    }

    static sexp Game_createTexture(sexp ctx, sexp self, sexp_sint_t n, sexp fname) {
        EXPECT_STRING(fname);
        std::string name(sexp_string_data(fname));
        auto fut = Game::gContext->videoRequest([&name](VideoContext& vidCtx) {
                return gContext->textureDB_.load(name, vidCtx);
            });
        return sexp_make_cpointer(ctx, SEXP_CPOINTER, fut.get(), nullptr, false);
    }

    static sexp Game_log(sexp ctx, sexp self, sexp_sint_t n, sexp msg) {
        EXPECT_STRING(msg);
        std::cout << sexp_string_data(msg) << std::endl;
        return SEXP_NULL;
    }

    static sexp Game_setFramerateLimit(sexp ctx, sexp self, sexp_sint_t n, sexp limit) {
        EXPECT_EXACT(limit);
        const auto lim = sexp_uint_value(limit);
        auto fut = Game::gContext->videoRequest([lim](VideoContext& vidCtx) {
                vidCtx.window_.setFramerateLimit(lim);
                return nullptr;
            });
        fut.wait();
        return SEXP_NULL;
    }

    static sexp Game_setVSyncEnabled(sexp ctx, sexp self, sexp_sint_t n, sexp enabled) {
        EXPECT_BOOL(enabled);
        const bool val = sexp_unbox_boolean(enabled);
        auto fut = Game::gContext->videoRequest([val](VideoContext& vidCtx) {
                vidCtx.window_.setVerticalSyncEnabled(val);
                return nullptr;
            });
        fut.wait();
        return SEXP_NULL;
    }

    static sexp Game_sleep(sexp ctx, sexp self, sexp_sint_t n, sexp duration) {
        EXPECT_EXACT(duration);
        const auto val = sexp_uint_value(duration);
        std::this_thread::sleep_for(std::chrono::microseconds(val));
        return SEXP_NULL;
    }

    static sexp Game_setTextChannelActive(sexp ctx, sexp self, sexp_sint_t n,
                                          sexp enabled) {
        EXPECT_BOOL(enabled);
        gContext->textChannelActive_ = sexp_unbox_boolean(enabled);
        return SEXP_NULL;
    }


    static sexp Game_pollTextChannel(sexp ctx, sexp self, sexp_sint_t n) {
        unsigned result;
        bool resultSet = false;
        gContext->textChannel_.enter([&](Context::TextChannel& t) {
            if (not t.empty()) {
                result = t.front();
                t.pop_front();
                resultSet = true;
            }
        });
        if (resultSet) {
            return sexp_make_unsigned_integer(ctx, result);
        } else {
            return SEXP_FALSE;
        }
    }


    void runUpdateLoop() {
        using Fn = void*(*)();
        ScriptEngine engine;
        engine.exportFunction("Game_update", 0, (Fn)Game_update);
        engine.exportFunction("Game_isRunning", 0, (Fn)Game_isRunning);
        engine.exportFunction("Game_makeObject", 0, (Fn)Game_makeObject);
        engine.exportFunction("Game_keyPressed", 1, (Fn)(Game_keyPressed));
        engine.exportFunction("Game_createTexture", 1, (Fn)Game_createTexture);
        engine.exportFunction("Game_setTextChannelActive", 1,
                              (Fn)Game_setTextChannelActive);
        engine.exportFunction("Game_pollTextChannel", 0,
                              (Fn)Game_pollTextChannel);
        engine.exportFunction("Game_setFramerateLimit", 1,
                              (Fn)Game_setFramerateLimit);
        engine.exportFunction("Game_setVSyncEnabled", 1,
                              (Fn)Game_setVSyncEnabled);
        engine.exportFunction("Game_log", 1, (Fn)Game_log);
        engine.exportFunction("Game_sleep", 1, (Fn)Game_sleep);
        engine.exportFunction("Object_move", 3, (Fn)Object_move);
        engine.exportFunction("Object_setFace", 2, (Fn)Object_setFace);
        engine.exportFunction("Object_setShadow", 2, (Fn)Object_setShadow);
        engine.setGlobal("Key_up", sf::Keyboard::Key::Up);
        engine.setGlobal("Key_down", sf::Keyboard::Key::Down);
        engine.setGlobal("Key_left", sf::Keyboard::Key::Left);
        engine.setGlobal("Key_right", sf::Keyboard::Key::Right);
        engine.setGlobal("Key_esc", sf::Keyboard::Key::Escape);
        engine.setGlobal("Key_a", sf::Keyboard::Key::A);
        engine.setGlobal("Key_b", sf::Keyboard::Key::B);
        engine.setGlobal("Key_c", sf::Keyboard::Key::C);
        engine.setGlobal("Key_d", sf::Keyboard::Key::D);
        engine.setGlobal("Key_e", sf::Keyboard::Key::E);
        engine.setGlobal("Key_f", sf::Keyboard::Key::F);
        engine.setGlobal("Key_g", sf::Keyboard::Key::G);
        engine.setGlobal("Key_h", sf::Keyboard::Key::H);
        engine.setGlobal("Key_i", sf::Keyboard::Key::I);
        engine.setGlobal("Key_j", sf::Keyboard::Key::J);
        engine.setGlobal("Key_k", sf::Keyboard::Key::K);
        engine.setGlobal("Key_l", sf::Keyboard::Key::L);
        engine.setGlobal("Key_m", sf::Keyboard::Key::M);
        engine.setGlobal("Key_n", sf::Keyboard::Key::N);
        engine.setGlobal("Key_o", sf::Keyboard::Key::O);
        engine.setGlobal("Key_p", sf::Keyboard::Key::P);
        engine.setGlobal("Key_q", sf::Keyboard::Key::Q);
        engine.setGlobal("Key_r", sf::Keyboard::Key::R);
        engine.setGlobal("Key_s", sf::Keyboard::Key::S);
        engine.setGlobal("Key_t", sf::Keyboard::Key::T);
        engine.setGlobal("Key_u", sf::Keyboard::Key::U);
        engine.setGlobal("Key_v", sf::Keyboard::Key::V);
        engine.setGlobal("Key_w", sf::Keyboard::Key::W);
        engine.setGlobal("Key_x", sf::Keyboard::Key::X);
        engine.setGlobal("Key_y", sf::Keyboard::Key::Y);
        engine.setGlobal("Key_z", sf::Keyboard::Key::Z);
        engine.setGlobal("Key_backspace", sf::Keyboard::Key::BackSpace);
        engine.setGlobal("Key_return", sf::Keyboard::Key::Return);
        engine.setGlobal("Key_count", sf::Keyboard::Key::KeyCount);
        engine.setGlobal("Key_lshift", sf::Keyboard::Key::LShift);
        engine.setGlobal("Key_rshift", sf::Keyboard::Key::RShift);
        engine.exportFunction("Game_setCameraTarget", 1,
                              (Fn)Game_setCameraTarget);

        engine.run("./scheme/main.scm");
    }


}
