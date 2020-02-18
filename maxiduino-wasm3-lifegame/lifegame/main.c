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

// link arduino library
m3ApiRawFunction(math_randome) {
    m3ApiReturnType (float_t)
    m3ApiReturn     ((float_t)rand() / RAND_MAX);
}

M3Result LinkFunction(IM3Runtime runtime) {
    IM3Module module = runtime->modules;
    const char* math = "Math";

    m3_LinkRawFunction(module, math, "random", "i()",  &math_randome);

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

    // link arduino library
    result = LinkFunction(runtime);
    if (result) FATAL("LinkArduino %s", result);

    // function lookup
    result = m3_FindFunction(&init, runtime, "init");
    if (result) FATAL("m3_FindFunction: %s", result);

    result = m3_FindFunction(&step, runtime, "step");
    if (result) FATAL("m3_FindFunction: %s", result);
}

void main(int argc, char* argv[])
{
    M3Result result = m3Err_none;

    // lifegame
    const uint32_t width = 320;
    const uint32_t height = 240;
    const uint32_t size = width * height;
    const char* i_argv[3] = { "320", "240", NULL };

    // make stable
    msleep(1000);

    // init maixduino
    k210_begin();
    lcd_begin();

    // init wasm
    init_wasm();

    // vram malloc
    uint16_t* vram = malloc(sizeof(uint16_t) * width * height);
    memset(vram, 0x0, sizeof(uint16_t) * size);

    // Initialize the module with the universe's width and height
    result = m3_CallWithArgs (init, 2, i_argv);
    if (result) FATAL("m3_CallWithArgs: %s", result);

    printf("init end!\n");

    // get wasm memory
    uint32_t* mem = (uint32_t*)(m3_GetMemory(runtime, 0, 0));
    // printf("dist: %lx, src: %lx\n", &mem[0], &mem[size]);

    while(1) {
        // bitblt (32bit to 16bit color)
        for(uint32_t i = 0; i < size; i++) {
            uint32_t color32 = mem[size + i];
            uint16_t r = ((color32 & 0x00ff0000) >> 16) >> 3;
            uint16_t g = ((color32 & 0x0000ff00) >> 8) >> 2;
            uint16_t b = ((color32 & 0x000000ff)) >> 3;
            vram[i] = 0x000000 | (r << 11) | (g << 5) | b;
        }
        lcd_set_area(0, 0, width - 1, height - 1);
        tft_write_half(vram, width * height);
        // copy output to input
        memcpy(&mem[0], &mem[size], size * sizeof(uint32_t));
        // call wasm
        result = m3_CallWithArgs(step, 0, NULL);
        if (result) FATAL("m3_CallWithArgs: %s", result);
    }
}
