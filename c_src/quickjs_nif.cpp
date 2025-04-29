#include <fine.hpp>
#include <iostream>

extern "C" {
  #include "quickjs.h"
}

int64_t add1(ErlNifEnv* env, int64_t a) {
  JSRuntime *rt = JS_NewRuntime();
  JSContext *ctx = JS_NewContext(rt);

  JSValue global = JS_GetGlobalObject(ctx);
  JSValue js_a = JS_NewInt32(ctx, a);
  JS_SetPropertyStr(ctx, global, "a", js_a);
  const char *script = "var b = a + 1;";
  JS_Eval(ctx, script, strlen(script), "<input>", 0);
  JSValue js_b = JS_GetPropertyStr(ctx, global, "b");
  int32_t b;
  JS_ToInt32(ctx, &b, js_b);

  JS_FreeValue(ctx, js_b);
  JS_FreeValue(ctx, global);
  JS_FreeContext(ctx);
  JS_FreeRuntime(rt);

  return b;
}
FINE_NIF(add1, 0);

std::string eval(ErlNifEnv* env, std::string script) {
  JSRuntime *rt = JS_NewRuntime();
  JSContext *ctx = JS_NewContext(rt);

  JSValue result = JS_Eval(ctx, script.c_str(), script.size(), "<eval>", JS_EVAL_TYPE_GLOBAL);
  JSValue json = JS_JSONStringify(ctx, result, JS_UNDEFINED, JS_UNDEFINED);

  std::string out;
  if (!JS_IsException(json)) {
    const char *cstr = JS_ToCString(ctx, json);
    out = cstr ? cstr : "<null>";
    JS_FreeCString(ctx, cstr);
  } else {
    out = "__quickjs_eval_failed";
  }

  JS_FreeValue(ctx, json);
  JS_FreeValue(ctx, result);
  JS_FreeContext(ctx);
  JS_FreeRuntime(rt);

  return out;
}
FINE_NIF(eval, 0);

// fine::Term eval(ErlNifEnv* env, std::string script) {
//   JSRuntime* rt = JS_NewRuntime();
//   JSContext* ctx = JS_NewContext(rt);

//   JSValue result = JS_Eval(ctx, script.c_str(), script.size(), "<eval>", JS_EVAL_TYPE_GLOBAL);

//   if (JS_IsUndefined(result)) {
//     JS_FreeValue(ctx, result);
//     JS_FreeContext(ctx);
//     JS_FreeRuntime(rt);
//     return fine::encode(env, fine::Atom("undefined"));
//   }

//   if (JS_IsNull(result)) {
//     JS_FreeValue(ctx, result);
//     JS_FreeContext(ctx);
//     JS_FreeRuntime(rt);
//     return fine::encode(env, fine::Atom("nil"));
//   }

//   if (JS_IsString(result)) {
//     const char* str = JS_ToCString(ctx, result);
//     std::string out = str ? str : "__quickjs_string_error";
//     JS_FreeCString(ctx, str);
//     JS_FreeValue(ctx, result);
//     JS_FreeContext(ctx);
//     JS_FreeRuntime(rt);
//     return fine::encode(env, out);
//   }

//   if (JS_IsNumber(result)) {
//     double num;
//     if (JS_ToFloat64(ctx, &num, result) == 0) {
//       JS_FreeValue(ctx, result);
//       JS_FreeContext(ctx);
//       JS_FreeRuntime(rt);
//       return fine::encode(env, num);
//     }
//   }

//   JS_FreeValue(ctx, result);
//   JS_FreeContext(ctx);
//   JS_FreeRuntime(rt);

//   return fine::encode(env, fine::Atom("badarg"));
// }

std::string eval2(ErlNifEnv* env, std::string script, std::map<std::string, uint64_t> bindings) {
  JSRuntime *rt = JS_NewRuntime();
  JSContext *ctx = JS_NewContext(rt);

  JSValue global = JS_GetGlobalObject(ctx);
  for (const auto& [key, value] : bindings) {
    JS_SetPropertyStr(ctx, global, key.c_str(), JS_NewInt64(ctx, static_cast<int64_t>(value)));
  }

  JSValue result = JS_Eval(ctx, script.c_str(), script.size(), "<eval>", JS_EVAL_TYPE_GLOBAL);
  JSValue json = JS_JSONStringify(ctx, result, JS_UNDEFINED, JS_UNDEFINED);

  std::string out;
  if (!JS_IsException(json)) {
    const char *cstr = JS_ToCString(ctx, json);
    out = cstr ? cstr : "<null>";
    JS_FreeCString(ctx, cstr);
  } else {
    out = "__quickjs_eval_failed";
  }

  JS_FreeValue(ctx, json);
  JS_FreeValue(ctx, result);
  JS_FreeValue(ctx, global);
  JS_FreeContext(ctx);
  JS_FreeRuntime(rt);

  return out;
}
FINE_NIF(eval2, 0);

FINE_INIT("Elixir.QuickJS");
