#include <bsp.h>
#include <sysctl.h>
#include <stdlib.h>
#include "maixduino.h"
#include "lcd/lcd.h"

#include "wasm3.h"
#include "m3_env.h"
#include "m3_api_defs.h"

#include "app.wasm.h"

#define FATAL(msg, ...) { printf("Fatal: " msg "\n", ##__VA_ARGS__); return; }

// link library
// m3ApiRawFunction(env_abort) {
//     m3ApiReturnType (void)
//     m3ApiGetArg(int32_t, message)
//     m3ApiGetArg(int32_t, fileName)
//     m3ApiGetArg(int32_t, lineNumber)
//     m3ApiGetArg(int32_t, columnNumber);
// }

M3Result LinkFunction(IM3Runtime runtime) {
    IM3Module module = runtime->modules;
    const char* env = "env";

    // TODO: Fatal: m3_CallWithArgs: [trap] stack overflow
    // @see ../wasm/package.json
    //  --runtime full
    //  --runtime full --use abort=
    // m3_LinkRawFunction(module, env, "abort", "v(iiii)",  &env_abort);

    return m3Err_none;
}

IM3Environment env;
IM3Runtime runtime;
IM3Module module;

IM3Function init;
IM3Function step;

static void init_wasm(void)
{
    M3Result result = m3Err_none;

    uint8_t* wasm = (uint8_t*)build_app_wasm;
    uint32_t fsize = build_app_wasm_len - 1;

    env = m3_NewEnvironment();
    if (!env) FATAL("m3_NewEnvironment failed");

    runtime = m3_NewRuntime(env, 1024, NULL);
    if (!runtime) FATAL("m3_NewRuntime failed");

    result = m3_ParseModule(env, &module, wasm, fsize);
    if (result) FATAL("m3_ParseModule: %s", result);

    result = m3_LoadModule(runtime, module);
    if (result) FATAL("m3_LoadModule: %s", result);

    // link function
    result = LinkFunction(runtime);
    if (result) FATAL("LinkArduino %s", result);

    // function lookup
    result = m3_FindFunction(&init, runtime, "init");
    if (result) FATAL("m3_FindFunction: %s", result);
}

void main(int argc, char* argv[])
{
    M3Result result = m3Err_none;

    // make stable
    msleep(1000);

    // init maixduino
    k210_begin();
    lcd_begin();

    // init wasm
    init_wasm();

    // call function width stdlib
    result = m3_CallWithArgs(init, 0, NULL);
    if (result) FATAL("m3_CallWithArgs: %s", result);
    uint32_t value = *(uint32_t*)(runtime->stack);

    printf("@Result: %d\n", value);
}
