let color = parseInt('0xFFFFFF');
color = 0xff0000; // azul
color = 0x241206;
// color = 0xa3a3cf;
// if(process.argv.length > 4) {
//     filename = process.argv[2];
//     filename_out_h = process.argv[3];
//     filename_out_c = process.argv[4];
// }
// else {
//     console.log(`node import_tmx.js <filename_tmx> <filename_output_header> <filename_output_c>`);
//     process.exit(1);
// }

const a = (color & 0xF0000000) >> (24 + 4);
const r = (color & 0x00F00000) >> (16 + 4);
const g = (color & 0x0000F000) >> (8 + 4);
const b = (color & 0x000000F0) >> (0 + 4);

const mask = 0xEEE;

let genesis_color = (((a << 12) | (b << 8) | (g << 4) | (r << 0)) & mask);

console.log(`genesis_color: ${genesis_color} r ${r} g ${g} b ${b}`);

// let color9bits = 3584;
// genesis_color = 3242;

const a_9b = 0;
const r_9b = (genesis_color & 0x00e) >> (1);
const g_9b = (genesis_color & 0x0e0) >> (1 + 4);
const b_9b = (genesis_color & 0xe00) >> (1 + 4 + 4);

// console.log(`real_color: r ${r_9b} g ${g_9b} b ${b_9b}`);

const real_r = parseInt(r_9b * 255 / 7);
const real_g = parseInt(g_9b * 255 / 7);
const real_b = parseInt(b_9b * 255 / 7);

console.log(`real_color: r ${real_r} g ${real_g} b ${real_b}`);
