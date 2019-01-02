#pragma once

#include "types.hpp"

class ScriptEngine {
public:
    ScriptEngine();
    ScriptEngine(const ScriptEngine &) = delete;
    ScriptEngine(ScriptEngine &&) = delete;
    ScriptEngine & operator=(const ScriptEngine &) = delete;
    ScriptEngine & operator=(ScriptEngine &&) = delete;
    ~ScriptEngine();

    void run(const String & script);

    void exportFunction(const String & sym, const String & nameSpace,
                        const String & docstring, int argc, void * (*proc)());

    void setGlobal(const String & sym, const String & nameSpace,
                   unsigned value);

private:
    struct State;

    State * state_;
};
