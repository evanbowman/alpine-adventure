#pragma once

#include "types.hpp"


class ScriptEngine {
public:
    ScriptEngine();
    ScriptEngine(const ScriptEngine&) = delete;
    ScriptEngine(ScriptEngine&&) = delete;
    ScriptEngine& operator=(const ScriptEngine&) = delete;
    ScriptEngine& operator=(ScriptEngine&&) = delete;
    ~ScriptEngine();

    void run(const String& script);

    void exportFunction(const String& sym,
                        int argc,
                        void*(*proc)());

    void setGlobal(const String& sym, unsigned value);

private:
    struct State;

    State* state_;
};
