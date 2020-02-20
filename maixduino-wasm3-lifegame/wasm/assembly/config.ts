// On the WASM side, 32-bit color values are modified in ABGR order (alpha, blue, green, red)
// because WASM is little endian. This results in RGBA in memory, which is exactly what the image
// buffer, composed of 8-bit components, expects on the JS side.
// export const BGR_ALIVE: u32 = 0xD392E6 | 1;
// export const BGR_DEAD: u32 = 0xA61B85 & ~1;
export const BGR_ALIVE: u32 = 0xD392E6 | 1;
export const BGR_DEAD: u32 = 0x000000 & ~1;
export const BIT_ROT: u32 = 2;
