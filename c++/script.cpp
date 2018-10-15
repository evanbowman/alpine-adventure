#include "script.hpp"
#include <chibi/eval.h>

struct ScriptEngine::State {
    sexp ctx, env;
};

ScriptEngine::ScriptEngine() : state_(new State) {
    state_->ctx = sexp_make_eval_context(nullptr, nullptr, nullptr, 0, 0);
    state_->env = sexp_load_standard_env(state_->ctx, nullptr, SEXP_SEVEN);
    sexp_load_standard_ports(state_->ctx, nullptr, stdin, stdout, stderr, 0);
}

ScriptEngine::TypeId ScriptEngine::registerType(const std::string & name) {
    sexp_gc_var2(namestr, type);
    namestr = sexp_c_string(state_->ctx, name.c_str(), name.length());
    type = sexp_register_c_type(state_->ctx, namestr, nullptr);
    if (sexp_exceptionp(type)) {
        throw std::runtime_error("failed to register type: " + name);
    }
    return sexp_type_tag(type);
}

void ScriptEngine::run(const String & script) {
    sexp_gc_var2(result, mainfile);
    mainfile = sexp_c_string(state_->ctx, script.c_str(), -1);
    result = sexp_load(state_->ctx, mainfile, nullptr);
    if (sexp_exceptionp(result)) {
        printf("\n[[ Exception ]]");
        printf("\n     source: ");
        sexp_write(state_->ctx, sexp_exception_source(result),
                   sexp_current_output_port(state_->ctx));
        printf("\n    message: ");
        sexp_write(state_->ctx, sexp_exception_message(result),
                   sexp_current_output_port(state_->ctx));
        printf("\n  irritants: ");
        sexp_write(state_->ctx, sexp_exception_irritants(result),
                   sexp_current_output_port(state_->ctx));
        printf("\n  procedure: ");
        sexp_write(state_->ctx, sexp_exception_procedure(result),
                   sexp_current_output_port(state_->ctx));
        printf("\n\n");
        exit(1); // FIXME
    }
}

ScriptEngine::~ScriptEngine() {
    sexp_destroy_context(state_->ctx);
    delete state_;
}

void ScriptEngine::exportFunction(const String & sym, int argc,
                                  void * (*proc)()) {
    sexp_define_foreign(state_->ctx, state_->env, sym.c_str(), argc, proc);
}

void ScriptEngine::setGlobal(const String & sym, unsigned value) {
    sexp_env_define(state_->ctx, state_->env,
                    sexp_intern(state_->ctx, sym.c_str(), sym.length()),
                    sexp_make_integer(state_->ctx, value));
}
