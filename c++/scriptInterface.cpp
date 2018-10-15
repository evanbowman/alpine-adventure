#include "context.hpp"
#include "game.hpp"
#include "script.hpp"
#include "util.hpp"
#include "videoContext.hpp"
#include <chibi/eval.h>

#include <iostream>

namespace Game {

#ifndef NDEBUG // NOTE: In release mode, no error checking for params.
#define TYPE_EXCEPTION(__TYPE_, __ARG_)                                        \
    return sexp_type_exception(ctx, self, __TYPE_, __ARG_);
#define EXPECT_EXACT(__EXACT_VAR)                                              \
    if (!sexp_exact_integerp(__EXACT_VAR))                                     \
        TYPE_EXCEPTION(SEXP_FIXNUM, __EXACT_VAR);
#define EXPECT_FLOAT(__FLOAT_VAR)                                              \
    if (!sexp_flonump(__FLOAT_VAR))                                            \
        TYPE_EXCEPTION(SEXP_FLONUM, __FLOAT_VAR);
#define EXPECT_PAIR(__PAIR_VAR)                                                \
    if (!sexp_pairp(__PAIR_VAR))                                               \
    TYPE_EXCEPTION(SEXP_PAIR, __PAIR_VAR)
#define EXPECT_STRING(__STRING_VAR)                                            \
    if (!sexp_stringp(__STRING_VAR))                                           \
        TYPE_EXCEPTION(SEXP_STRING, __STRING_VAR);
#define EXPECT_BOOL(__BOOL_VAR)                                                \
    if (!sexp_booleanp(__BOOL_VAR))                                            \
        TYPE_EXCEPTION(SEXP_BOOLEAN, __BOOL_VAR);
#define EXPECT_POINTER(__POINTER_VAR)                                          \
    if (!sexp_cpointerp(__POINTER_VAR))                                        \
        TYPE_EXCEPTION(SEXP_CPOINTER, __POINTER_VAR);
#define EXPECT_VECTOR(__VECTOR_VAR)                                            \
    if (!sexp_vectorp(__VECTOR_VAR))                                           \
        TYPE_EXCEPTION(SEXP_VECTOR, __VECTOR_VAR);
#define EXPECT_CUSTOM(__VAR, __TYPEID)                                         \
    if (!sexp_check_tag(__VAR, __TYPEID))                                      \
        TYPE_EXCEPTION(__TYPEID, __VAR);
#else
#define EXPECT_EXACT(__EXACT_VAR)
#define EXPECT_FLOAT(__FLOAT_VAR)
#define EXPECT_PAIR(__PAIR_VAR)
#define EXPECT_STRING(__STRING_VAR)
#define EXPECT_BOOL(__BOOL_VAR)
#endif // NDEBUG

void update();

namespace typeids {
sexp_uint_t texture, object;
}

struct FunctionExport {
    using Fn = void * (*)();
    const char * name_;
    int argc_;
    Fn fn_;

    template <typename F>
    FunctionExport(const char * name, int argc, F fn)
        : name_(name), argc_(argc),
          /* NOTE: + to convert lambda to function ptr*/
          fn_(reinterpret_cast<Fn>(+fn)) {}

} functionExports[] = {
        {"Game_update", 0,
         [](sexp ctx, sexp self, sexp_sint_t n) {
             Game::update();
             return SEXP_NULL;
         }},
        {"Game_isRunning", 0,
         [](sexp ctx, sexp self, sexp_sint_t n) {
             return sexp_make_boolean(Game::isRunning());
         }},
        {"Game_makeObject", 0,
         [](sexp ctx, sexp self, sexp_sint_t n) {
             return sexp_make_cpointer(ctx, typeids::object,
                                       Game::makeObject().get(),
                                       nullptr, false);
         }},
        {"Game_makeWidjet", 0,
         [](sexp ctx, sexp self, sexp_sint_t n) {
             return sexp_make_cpointer(ctx, typeids::object,
                                       Game::makeWidjet().get(),
                                       nullptr, false);
         }},
        {"Object_setZOrder", 2,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp obj, sexp z) {
             EXPECT_CUSTOM(obj, typeids::object); EXPECT_EXACT(z);
             ((Object*)sexp_cpointer_value(obj))
                 ->setZOrder(sexp_uint_value(z));
             return obj;
         }},
        {"Object_setPosition", 3,
         [](sexp ctx, sexp self, sexp_sint_t n,
            sexp obj, sexp x, sexp y) {
             EXPECT_FLOAT(x); EXPECT_FLOAT(y);
             Object* object = (Object*)sexp_cpointer_value(obj);
             object->setPosition({(float)sexp_flonum_value(x),
                                  (float)sexp_flonum_value(y)});
             return obj;
         }},
        {"Object_setFace", 2,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp obj, sexp txtr) {
             EXPECT_CUSTOM(obj, typeids::object);
             EXPECT_CUSTOM(txtr, typeids::texture);
             auto tptr = (sf::Texture*)sexp_cpointer_value(txtr);
             auto spr = make_unique<Sprite>(*tptr);
             ((Object*)sexp_cpointer_value(obj))->setFace(std::move(spr));
             return obj;
         }},
        {"Object_setShadow", 2,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp obj, sexp txtr) {
             EXPECT_CUSTOM(obj, typeids::object);
             EXPECT_CUSTOM(txtr, typeids::texture);
             auto tptr = (sf::Texture*)sexp_cpointer_value(txtr);
             auto spr = make_unique<Sprite>(*tptr);
             ((Object*)sexp_cpointer_value(obj))->setShadow(std::move(spr));
             return obj;
         }},
        {"Object_setFaceColor", 2,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp obj, sexp colorVector) {
             EXPECT_CUSTOM(obj, typeids::object); EXPECT_VECTOR(colorVector);
             if (auto spr = ((Object*)sexp_cpointer_value(obj))->getFace()) {
                 spr->setColor({
                         (uint8_t)sexp_uint_value(sexp_vector_ref(colorVector, SEXP_ZERO)),
                         (uint8_t)sexp_uint_value(sexp_vector_ref(colorVector, SEXP_ONE)),
                         (uint8_t)sexp_uint_value(sexp_vector_ref(colorVector, SEXP_TWO)),
                         (uint8_t)sexp_uint_value(sexp_vector_ref(colorVector, SEXP_THREE)),
                     });
             } else {
                 std::cout << "object has no face" << std::endl;
             }
             return obj;
         }},
        {"Object_setFaceScale", 3,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp obj, sexp xs, sexp ys) {
             EXPECT_CUSTOM(obj, typeids::object);
             EXPECT_FLOAT(xs); EXPECT_FLOAT(ys);
             if (auto spr = ((Object*)sexp_cpointer_value(obj))->getFace()) {
                 spr->setScale({(float)sexp_flonum_value(xs),
                                (float)sexp_flonum_value(ys)});
             } else {
                 std::cout << "object has no face" << std::endl;
             }
             return obj;
         }},
        {"Object_setFaceSubrect", 2,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp obj, sexp rect) {
             EXPECT_CUSTOM(obj, typeids::object);
             EXPECT_VECTOR(rect);
             if (auto spr = ((Object*)sexp_cpointer_value(obj))->getFace()) {
                 spr->setSubRect({
                         (int)sexp_uint_value(sexp_vector_ref(rect, SEXP_ZERO)),
                         (int)sexp_uint_value(sexp_vector_ref(rect, SEXP_ONE)),
                         (int)sexp_uint_value(sexp_vector_ref(rect, SEXP_TWO)),
                         (int)sexp_uint_value(sexp_vector_ref(rect, SEXP_THREE))
                     });
             } else {
                 std::cout << "object has no face" << std::endl;
             }
             return obj;
         }},
        {"Object_setFaceKeyframe", 2,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp obj, sexp frame) {
             EXPECT_CUSTOM(obj, typeids::object); EXPECT_EXACT(frame);
             if (auto spr = ((Object*)sexp_cpointer_value(obj))->getFace()) {
                 spr->setKeyframe(sexp_uint_value(frame));
             } else {
                 std::cout << "object has no face" << std::endl;
             }
             return obj;
         }},
        {"Object_setVisible", 2,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp obj, sexp visible) {
             EXPECT_CUSTOM(obj, typeids::object); EXPECT_BOOL(visible);
             ((Object*)sexp_cpointer_value(obj))
                 ->setVisible(sexp_unbox_boolean(visible));
             return obj;
         }},
        {"Game_describeWindow", 0,
         [](sexp ctx, sexp self, sexp_sint_t n) {
             const auto& view = gContext->camera_.getOverworldView();
             auto center = view.getCenter();
             auto size = view.getSize();
             auto tofloat = [&](float val) { return sexp_make_flonum(ctx, val); };
             sexp result = sexp_make_vector(ctx, SEXP_FOUR, SEXP_NULL);
             sexp_vector_set(result, SEXP_ZERO, tofloat(center.x));
             sexp_vector_set(result, SEXP_ONE, tofloat(center.y));
             sexp_vector_set(result, SEXP_TWO, tofloat(size.x));
             sexp_vector_set(result, SEXP_THREE, tofloat(size.y));
             return result;
         }},
        {"Game_setCameraTarget", 1,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp obj) {
             EXPECT_CUSTOM(obj, typeids::object);
             // TODO: This could be bad when there're a lot of objects, but
             // on the other hand, changing the camera target almost never
             // happens... but I can imagine other cases where we would
             // want the shared pointer instead of the raw object
             // pointer... maybe shared_from_this would be ok.
             const auto object = (Object*)sexp_cpointer_value(obj);
             Game::gContext->objects_.enter([&](ObjectList& list) {
                 for (auto& obj : list) {
                     if (obj.get() == object) {
                         Game::gContext->camera_.setTarget(obj);
                         return;
                     }
                 }
             });
             return SEXP_NULL;
         }},
        {"Game_keyPressed", 1,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp key) {
             EXPECT_EXACT(key);
             const auto sel = (sf::Keyboard::Key)sexp_uint_value(key);
             const bool pressed = Game::gContext->keyStates_.isPressed(sel);
             return sexp_make_boolean(pressed);
         }},
        {"Game_createTexture", 1,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp fname) {
             EXPECT_STRING(fname);
             std::string name(sexp_string_data(fname));
             auto fut = Game::gContext->videoRequest([&name](VideoContext& vidCtx) {
                 return gContext->textureDB_.load(name, vidCtx);
             });
             return sexp_make_cpointer(ctx, typeids::texture,
                                       fut.get(), nullptr, false);
         }},
        {"Game_log", 1,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp msg) {
             EXPECT_STRING(msg);
             gContext->logfile_ << sexp_string_data(msg) << std::endl;
             return SEXP_NULL;
         }},
        {"Game_setFramerateLimit", 1,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp limit) {
             EXPECT_EXACT(limit);
             const auto lim = sexp_uint_value(limit);
             auto fut = Game::gContext->videoRequest([lim](VideoContext& vidCtx) {
                 vidCtx.window_.setFramerateLimit(lim);
                 return nullptr;
             });
             fut.wait();
             return SEXP_NULL;
         }},
        {"Game_setVSyncEnabled", 1,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp enabled) {
             EXPECT_BOOL(enabled);
             const bool val = sexp_unbox_boolean(enabled);
             auto fut = Game::gContext->videoRequest([val](VideoContext& vidCtx) {
                 vidCtx.window_.setVerticalSyncEnabled(val);
                 return nullptr;
             });
             fut.wait();
             return SEXP_NULL;
         }},
        {"Game_setTextChannelActive", 1,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp enabled) {
             EXPECT_BOOL(enabled);
             gContext->textChannelActive_ = sexp_unbox_boolean(enabled);
             return SEXP_NULL;
         }},
        {"Game_pollTextChannel", 0,
         [](sexp ctx, sexp self, sexp_sint_t n) {
             unsigned result;
             bool resultSet = false;
             gContext->textChannel_.enter([&](TextChannel& t) {
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
         }},
        {"Game_sleep", 1,
         [](sexp ctx, sexp self, sexp_sint_t n, sexp duration) {
             EXPECT_EXACT(duration);
             const auto val = sexp_uint_value(duration);
             std::this_thread::sleep_for(std::chrono::microseconds(val));
             return SEXP_NULL;
         }}
    };

struct {
    const char * name_;
    unsigned value_;
} globals[] = {
    {"Key_up", sf::Keyboard::Key::Up},
    {"Key_down", sf::Keyboard::Key::Down},
    {"Key_left", sf::Keyboard::Key::Left},
    {"Key_right", sf::Keyboard::Key::Right},
    {"Key_esc", sf::Keyboard::Key::Escape},
    {"Key_a", sf::Keyboard::Key::A},
    {"Key_b", sf::Keyboard::Key::B},
    {"Key_c", sf::Keyboard::Key::C},
    {"Key_d", sf::Keyboard::Key::D},
    {"Key_e", sf::Keyboard::Key::E},
    {"Key_f", sf::Keyboard::Key::F},
    {"Key_g", sf::Keyboard::Key::G},
    {"Key_h", sf::Keyboard::Key::H},
    {"Key_i", sf::Keyboard::Key::I},
    {"Key_j", sf::Keyboard::Key::J},
    {"Key_k", sf::Keyboard::Key::K},
    {"Key_l", sf::Keyboard::Key::L},
    {"Key_m", sf::Keyboard::Key::M},
    {"Key_n", sf::Keyboard::Key::N},
    {"Key_o", sf::Keyboard::Key::O},
    {"Key_p", sf::Keyboard::Key::P},
    {"Key_q", sf::Keyboard::Key::Q},
    {"Key_r", sf::Keyboard::Key::R},
    {"Key_s", sf::Keyboard::Key::S},
    {"Key_t", sf::Keyboard::Key::T},
    {"Key_u", sf::Keyboard::Key::U},
    {"Key_v", sf::Keyboard::Key::V},
    {"Key_w", sf::Keyboard::Key::W},
    {"Key_x", sf::Keyboard::Key::X},
    {"Key_y", sf::Keyboard::Key::Y},
    {"Key_z", sf::Keyboard::Key::Z},
    {"Key_backspace", sf::Keyboard::Key::BackSpace},
    {"Key_return", sf::Keyboard::Key::Return},
    {"Key_count", sf::Keyboard::Key::KeyCount},
    {"Key_lshift", sf::Keyboard::Key::LShift},
    {"Key_rshift", sf::Keyboard::Key::RShift},
};

void runUpdateLoop() {
    ScriptEngine engine;
    // FIXME: type registration was causing segfaults upon finalization
    // typeids::texture = engine.registerType("Game_Texture");
    // typeids::object = engine.registerType("Game_Object");
    typeids::texture = SEXP_CPOINTER;
    typeids::object = SEXP_CPOINTER;
    for (const auto & fnExport : functionExports) {
        engine.exportFunction(fnExport.name_, fnExport.argc_, fnExport.fn_);
    }
    for (const auto & global : globals) {
        engine.setGlobal(global.name_, global.value_);
    }
    engine.run("./scheme/main.scm");
}
}
