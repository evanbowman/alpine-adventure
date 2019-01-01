#include "script.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

// FIXME!!!
#include "../../lisp/lib/lisp.hpp"
#include "../../lisp/lib/persistent.hpp"

struct ScriptEngine::State {
    lisp::Context context_;
};

ScriptEngine::ScriptEngine() : state_(new State) {
    auto& env = state_->context_.topLevel();
    env.openDLL("libfs");
    env.openDLL("libsys");
}

void ScriptEngine::run(const String & script) {
    auto& env = state_->context_.topLevel();
    try {
        std::ifstream t(script);
        std::stringstream buffer;
        buffer << t.rdbuf();
        env.exec(buffer.str());
    } catch (const std::exception& ex) {
        std::cout << "Error:\n" << ex.what() << std::endl;
    }
}

ScriptEngine::~ScriptEngine() {
    delete state_;
}

void ScriptEngine::exportFunction(const String & sym, const String & nameSpace,
                                  const String & docstring, int argc,
                                  void * (*proc)()) {
    auto& env = state_->context_.topLevel();
    lisp::Persistent<lisp::String> doc(env, env.create<lisp::String>(docstring.c_str(),
                                                                     docstring.length()));
    const auto param = reinterpret_cast<lisp::ObjectPtr (*)(lisp::Environment&,
                                                            const lisp::Arguments&)>(proc);
    auto fn = env.create<lisp::Function>(doc.get(), size_t(0), param);
    env.setGlobal(sym, nameSpace, fn);
}

void ScriptEngine::setGlobal(const String & sym, const String & nameSpace, unsigned value) {
    auto& env = state_->context_.topLevel();
    auto num = env.create<lisp::Integer>((lisp::Integer::Rep)value);
    env.setGlobal(sym, nameSpace, num);
}
