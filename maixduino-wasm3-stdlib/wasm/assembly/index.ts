export function init(): u32 {
    var map = new Map<i32,u32>();
    map.set(0, 10);
    return map.get(0);
}
