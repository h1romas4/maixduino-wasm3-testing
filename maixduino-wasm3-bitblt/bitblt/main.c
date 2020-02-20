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

uint32_t arandom(uint32_t max) {
    return rand();
}

// link arduino library
m3ApiRawFunction(m3_arduino_random) {
    m3ApiReturnType (uint32_t)
    m3ApiGetArg     (uint32_t, max)
    m3ApiReturn     (arandom(max));
}

M3Result LinkArduino(IM3Runtime runtime) {
    IM3Module module = runtime->modules;
    const char* arduino = "arduino";

    m3_LinkRawFunction (module, arduino, "random", "i(i)",  &m3_arduino_random);

    return m3Err_none;
}

IM3Environment env;
IM3Runtime runtime;
IM3Module module;
IM3Function draw;

const char* i_argv[3] = { "320", "240", NULL };

static void run_wasm(void)
{
    M3Result result = m3Err_none;

    uint8_t* wasm = (uint8_t*)build_app_wasm;
    uint32_t fsize = build_app_wasm_len - 1;

    env = m3_NewEnvironment ();
    if (!env) FATAL("m3_NewEnvironment failed");

    runtime = m3_NewRuntime (env, 1024, NULL);
    if (!runtime) FATAL("m3_NewRuntime failed");

    result = m3_ParseModule (env, &module, wasm, fsize);
    if (result) FATAL("m3_ParseModule: %s", result);

    result = m3_LoadModule (runtime, module);
    if (result) FATAL("m3_LoadModule: %s", result);

    // link arduino library
    result = LinkArduino (runtime);
    if (result) FATAL("LinkArduino %s", result);

    // function lookup
    result = m3_FindFunction (&draw, runtime, "draw");
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
    run_wasm();

    while(1) {
        // call wasm
        result = m3_CallWithArgs (draw, 2, i_argv);
        if (result) FATAL("m3_CallWithArgs: %s", result);
        // bitblt
        uint16_t* vram = (uint16_t*)(m3_GetMemory(runtime, 0, 0));
        lcd_set_area(0, 0, 320 - 1, 240 - 1);
        tft_write_half(vram, 320 * 240);
    }
}
