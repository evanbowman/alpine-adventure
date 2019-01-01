#include "context.hpp"
#include "game.hpp"
#include "script.hpp"
#include "util.hpp"
#include "videoContext.hpp"

// FIXME!!!
#include "../../lisp/lib/lisp.hpp"
#include "../../lisp/lib/listBuilder.hpp"

#include <iostream>

namespace Game {


void update();


struct BuiltinFunctionInfo {
    const char* name;
    const char* docstring;
    size_t requiredArgs;
    lisp::CFunction impl;
};

static const BuiltinFunctionInfo functionExports[] = {
        {"update", "[] -> run the update step for engine builtins", 0,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             Game::update();
             return env.getNull();
         }},
        {"is-running?", "[] -> true if the engine\'s window is open", 0,
         [](lisp::Environment& env, const lisp::Arguments& args) ->
         lisp::ObjectPtr {
             return env.getBool(Game::isRunning());
         }},
        {"make-object", "[] -> newly created engine entity", 0,
         [](lisp::Environment& env, const lisp::Arguments& args) ->
         lisp::ObjectPtr {
             return env.create<lisp::RawPointer>(Game::makeObject().get());
         }},
        {"make-widjet", "[] -> newly created lightweight ui object", 0,
         [](lisp::Environment& env, const lisp::Arguments& args) -> lisp::ObjectPtr {
             return env.create<lisp::RawPointer>(Game::makeWidjet().get());
         }},
        {"set-zorder", "[obj z] -> obj, with z-order updated to z", 2,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             ((Object*)lisp::checkedCast<lisp::RawPointer>(args[0])->value())
                 ->setZOrder(lisp::checkedCast<lisp::Integer>(args[1])->value());
             return args[0];
         }},
        {"set-position", "[obj x-pos y-pos] -> obj, with new position", 3,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             Object* object =
                 (Object*)lisp::checkedCast<lisp::RawPointer>(args[0])->value();
             object->setPosition({(float)lisp::checkedCast<lisp::Float>(args[1])->value(),
                                  (float)lisp::checkedCast<lisp::Float>(args[2])->value()});
             return args[0];
         }},
        {"set-face", "[obj spr] -> obj, with visual representation set to spr", 2,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             auto tptr = (sf::Texture*)lisp::checkedCast<lisp::RawPointer>(args[1])->value();
             auto spr = make_unique<Sprite>(*tptr);
             ((Object*)lisp::checkedCast<lisp::RawPointer>(args[0])
              ->value())->setFace(std::move(spr));
             return args[0];
         }},
        {"set-shadow", "[obj spr] -> obj, with visual shadow set to spr", 2,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             auto tptr = (sf::Texture*)lisp::checkedCast<lisp::RawPointer>(args[1])->value();
             auto spr = make_unique<Sprite>(*tptr);
             ((Object*)lisp::checkedCast<lisp::RawPointer>(args[0])
              ->value())->setShadow(std::move(spr));
             return args[0];
         }},
        {"set-face-color",
         "[obj c] -> obj, with face color set to rgba values in list c", 2,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             if (auto spr = ((Object*)lisp::checkedCast<lisp::RawPointer>(args[0])->value())->getFace()) {
                 auto head = lisp::checkedCast<lisp::Pair>(args[1]);
                 spr->setColor({
                         (uint8_t)lisp::checkedCast<lisp::Integer>(lisp::listRef(head, 0))->value(),
                         (uint8_t)lisp::checkedCast<lisp::Integer>(lisp::listRef(head, 1))->value(),
                         (uint8_t)lisp::checkedCast<lisp::Integer>(lisp::listRef(head, 2))->value(),
                         (uint8_t)lisp::checkedCast<lisp::Integer>(lisp::listRef(head, 3))->value()
                     });
             } else {
                 std::cout << "object has no face" << std::endl;
             }
             return args[0];
         }},
        {"set-face-scale", "[obj x-scl y-scl] -> obj, with rescaled face", 3,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             if (auto spr = ((Object*)lisp::checkedCast<lisp::RawPointer>(args[0])
                             ->value())->getFace()) {
                 spr->setScale({(float)lisp::checkedCast<lisp::Float>(args[1])->value(),
                                (float)lisp::checkedCast<lisp::Float>(args[2])->value()});
             } else {
                 std::cout << "object has no face" << std::endl;
             }
             return args[0];
         }},
        // {"Object_setFaceSubrect", 2,
        //  [](lisp::Environment& env, const lisp::Arguments& args) {
        //      EXPECT_CUSTOM(obj, typeids::object);
        //      EXPECT_VECTOR(rect);
        //      if (auto spr = ((Object*)sexp_cpointer_value(obj))->getFace()) {
        //          spr->setSubRect({
        //                  (int)sexp_uint_value(sexp_vector_ref(rect, SEXP_ZERO)),
        //                  (int)sexp_uint_value(sexp_vector_ref(rect, SEXP_ONE)),
        //                  (int)sexp_uint_value(sexp_vector_ref(rect, SEXP_TWO)),
        //                  (int)sexp_uint_value(sexp_vector_ref(rect, SEXP_THREE))
        //              });
        //      } else {
        //          std::cout << "object has no face" << std::endl;
        //      }
        //      return obj;
        //  }},
        // {"Object_setFaceKeyframe", 2,
        //  [](lisp::Environment& env, const lisp::Arguments& args) {
        //      EXPECT_CUSTOM(obj, typeids::object); EXPECT_EXACT(frame);
        //      if (auto spr = ((Object*)sexp_cpointer_value(obj))->getFace()) {
        //          spr->setKeyframe(sexp_uint_value(frame));
        //      } else {
        //          std::cout << "object has no face" << std::endl;
        //      }
        //      return obj;
        //  }},
        // {"Object_setVisible", 2,
        //  [](lisp::Environment& env, const lisp::Arguments& args) {
        //      EXPECT_CUSTOM(obj, typeids::object); EXPECT_BOOL(visible);
        //      ((Object*)sexp_cpointer_value(obj))
        //          ->setVisible(sexp_unbox_boolean(visible));
        //      return obj;
        //  }},
        {"describe-window", "[] -> list of (x-center y-center width height)", 0,
         [](lisp::Environment& env, const lisp::Arguments&) {
             const auto& view = gContext->camera_.getOverworldView();
             auto center = view.getCenter();
             auto size = view.getSize();
             lisp::ListBuilder builder(env, env.create<lisp::Float>(center.x));
             builder.pushBack(env.create<lisp::Float>(center.y));
             builder.pushBack(env.create<lisp::Float>(size.x));
             builder.pushBack(env.create<lisp::Float>(size.y));
             return builder.result();
         }},
        {"set-camera-target", "[obj] -> update camera to track obj", 1,
         [](lisp::Environment& env, const lisp::Arguments& args) -> lisp::ObjectPtr {
             auto object = lisp::checkedCast<lisp::RawPointer>(args[0])->value();
             Game::gContext->objects_.enter([&](ObjectList& list) {
                 for (auto& obj : list) {
                     if (obj.get() == object) {
                         Game::gContext->camera_.setTarget(obj);
                         return;
                     }
                 }
             });
             return env.getNull();
         }},
        {"key-pressed?", "[key] -> true if key is pressed", 1,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             const auto sel =
                 (sf::Keyboard::Key)lisp::checkedCast<lisp::Integer>(args[0])->value();
             const bool pressed = Game::gContext->keyStates_.isPressed(sel);
             return env.getBool(pressed);
         }},
        {"create-texture", "[path] -> new texture, by loading the image from path", 1,
         [](lisp::Environment& env, const lisp::Arguments& args) -> lisp::ObjectPtr {
             std::string name(lisp::checkedCast<lisp::String>(args[0])->toAscii());
             auto fut = Game::gContext->videoRequest([&name](VideoContext& vidCtx) {
                 return gContext->textureDB_.load(name, vidCtx);
             });
             return env.create<lisp::RawPointer>(fut.get());
         }},
        {"log", "[str] -> write str to engine log", 1,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             gContext->logfile_ <<
                 lisp::checkedCast<lisp::String>(args[0])->toAscii() << std::endl;
             return env.getNull();
         }},
        {"set-framerate-limit", "[num] -> update engine framerate to num", 1,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             const auto lim = lisp::checkedCast<lisp::Integer>(args[0])->value();
             auto fut = Game::gContext->videoRequest([lim](VideoContext& vidCtx) {
                 vidCtx.window_.setFramerateLimit(lim);
                 return nullptr;
             });
             fut.wait();
             return env.getNull();
         }},
        {"set-vsync-enabled", "[bool] -> update vsync to bool", 1,
         [](lisp::Environment& env, const lisp::Arguments& args) {
             const bool val = lisp::checkedCast<lisp::Boolean>(args[0])->value();
             auto fut = Game::gContext->videoRequest([val](VideoContext& vidCtx) {
                 vidCtx.window_.setVerticalSyncEnabled(val);
                 return nullptr;
             });
             fut.wait();
             return env.getNull();
         }},
        // {"Game_setTextChannelActive", 1,
        //  [](lisp::Environment& env, const lisp::Arguments& args) {
        //      EXPECT_BOOL(enabled);
        //      gContext->textChannelActive_ = sexp_unbox_boolean(enabled);
        //      return SEXP_NULL;
        //  }},
        // {"Game_pollTextChannel", 0,
        //  [](lisp::Environment& env, const lisp::Arguments& args) {
        //      unsigned result;
        //      bool resultSet = false;
        //      gContext->textChannel_.enter([&](TextChannel& t) {
        //          if (not t.empty()) {
        //              result = t.front();
        //              t.pop_front();
        //              resultSet = true;
        //          }
        //      });
        //      if (resultSet) {
        //          return sexp_make_unsigned_integer(ctx, result);
        //      } else {
        //          return SEXP_FALSE;
        //      }
        //  }},
        // {"Game_sleep", 1,
        //  [](lisp::Environment& env, const lisp::Arguments& args) {
        //      EXPECT_EXACT(duration);
        //      const auto val = sexp_uint_value(duration);
        //      std::this_thread::sleep_for(std::chrono::microseconds(val));
        //      return SEXP_NULL;
        //  }}
    };

struct {
    const char * name_;
    unsigned value_;
} globals[] = {
    {"key-up", sf::Keyboard::Key::Up},
    {"key-down", sf::Keyboard::Key::Down},
    {"key-left", sf::Keyboard::Key::Left},
    {"key-right", sf::Keyboard::Key::Right},
    {"key-esc", sf::Keyboard::Key::Escape},
    {"key-a", sf::Keyboard::Key::A},
    {"key-b", sf::Keyboard::Key::B},
    {"key-c", sf::Keyboard::Key::C},
    {"key-d", sf::Keyboard::Key::D},
    {"key-e", sf::Keyboard::Key::E},
    {"key-f", sf::Keyboard::Key::F},
    {"key-g", sf::Keyboard::Key::G},
    {"key-h", sf::Keyboard::Key::H},
    {"key-i", sf::Keyboard::Key::I},
    {"key-j", sf::Keyboard::Key::J},
    {"key-k", sf::Keyboard::Key::K},
    {"key-l", sf::Keyboard::Key::L},
    {"key-m", sf::Keyboard::Key::M},
    {"key-n", sf::Keyboard::Key::N},
    {"key-o", sf::Keyboard::Key::O},
    {"key-p", sf::Keyboard::Key::P},
    {"key-q", sf::Keyboard::Key::Q},
    {"key-r", sf::Keyboard::Key::R},
    {"key-s", sf::Keyboard::Key::S},
    {"key-t", sf::Keyboard::Key::T},
    {"key-u", sf::Keyboard::Key::U},
    {"key-v", sf::Keyboard::Key::V},
    {"key-w", sf::Keyboard::Key::W},
    {"key-x", sf::Keyboard::Key::X},
    {"key-y", sf::Keyboard::Key::Y},
    {"key-z", sf::Keyboard::Key::Z},
    {"key-backspace", sf::Keyboard::Key::BackSpace},
    {"key-return", sf::Keyboard::Key::Return},
    {"key-count", sf::Keyboard::Key::KeyCount},
    {"key-lshift", sf::Keyboard::Key::LShift},
    {"key-rshift", sf::Keyboard::Key::RShift},
};

void runUpdateLoop() {
    ScriptEngine engine;
    std::for_each(
        std::begin(functionExports), std::end(functionExports),
        [&](const BuiltinFunctionInfo& info) {
            engine.exportFunction(info.name, "engine",
                                  info.docstring,
                                  info.requiredArgs,
                                  (void * (*)())info.impl);
        });
    for (const auto & global : globals) {
        engine.setGlobal(global.name_, "engine", global.value_);
    }
    engine.run("ebl/main.ebl");
}
}
