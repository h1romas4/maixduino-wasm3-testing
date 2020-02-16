import * as m5stack from "./arduino";

var width: i32, height: i32;

/** Gets an input pixel in the range [0, s]. */
@inline
function pget(x: u32, y: u32): u16 {
    return load<u16>((y * width + x) * 2);
}

/** Sets an output pixel in the range [s, 2*s]. */
@inline
function pset(x: u32, y: u32, v: u16): void {
    store<u16>((y * width + x) * 2, v);
}

export function draw(w: i32, h: i32): void {
    width = w;
    height = h;

    for (let r = 0; r < (height / 2); r++) {
        circle(w / 2, h / 2, r, <u16>m5stack.random(65536));
    }
}

function circle(x: i32, y: i32, r: i32, color: u16): void {
    let xx = r;
    let yy = 0;
    let err = 0;

    while(xx >= yy) {
        pset(x + xx, y + yy, color);
        pset(x + yy, y + xx, color);
        pset(x - yy, y + xx, color);
        pset(x - xx, y + yy, color);
        pset(x - xx, y - yy, color);
        pset(x - yy, y - xx, color);
        pset(x + yy, y - xx, color);
        pset(x + xx, y - yy, color);
        if(err <= 0) {
            yy++;
            err += 2 * yy + 1;
        } else {
            xx--;
            err -= 2 * xx + 1;
        }
    }
}
