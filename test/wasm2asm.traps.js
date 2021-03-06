function asmFunc(global, env, buffer) {
 "use asm";
 var HEAP8 = new global.Int8Array(buffer);
 var HEAP16 = new global.Int16Array(buffer);
 var HEAP32 = new global.Int32Array(buffer);
 var HEAPU8 = new global.Uint8Array(buffer);
 var HEAPU16 = new global.Uint16Array(buffer);
 var HEAPU32 = new global.Uint32Array(buffer);
 var HEAPF32 = new global.Float32Array(buffer);
 var HEAPF64 = new global.Float64Array(buffer);
 var Math_imul = global.Math.imul;
 var Math_fround = global.Math.fround;
 var Math_abs = global.Math.abs;
 var Math_clz32 = global.Math.clz32;
 function $0() {
  
 }
 
 function $1(x, y) {
  x = x | 0;
  y = y | 0;
  var $2 = 0, $3 = 0, $4 = 0;
  return x + y | 0 | 0;
 }
 
 function $2(x, y) {
  x = x | 0;
  y = y | 0;
  var $2 = 0, $3 = 0, $4 = 0;
  return (x | 0) / (y | 0) | 0 | 0;
 }
 
 function __wasm_ctz_i32(x) {
  x = x | 0;
  var $1 = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0;
  if ((x | 0) == (0 | 0)) $9 = 32; else $9 = 31 - Math_clz32(x ^ (x - 1 | 0) | 0) | 0;
  return $9 | 0;
 }
 
 function __wasm_popcnt_i32(x) {
  x = x | 0;
  var count = 0, $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0;
  count = 0;
  b : {
   l : do {
    $5 = count;
    if ((x | 0) == (0 | 0)) break b;
    x = x & (x - 1 | 0) | 0;
    count = count + 1 | 0;
    continue l;
    break l;
   } while (1);
  };
  return $5 | 0;
 }
 
 function __wasm_rotl_i32(x, k) {
  x = x | 0;
  k = k | 0;
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0;
  return ((4294967295 >>> (k & 31 | 0) | 0) & x | 0) << (k & 31 | 0) | 0 | (((4294967295 << (32 - (k & 31 | 0) | 0) | 0) & x | 0) >>> (32 - (k & 31 | 0) | 0) | 0) | 0 | 0;
 }
 
 function __wasm_rotr_i32(x, k) {
  x = x | 0;
  k = k | 0;
  var $2 = 0, $3 = 0, $4 = 0, $5 = 0, $6 = 0, $7 = 0, $8 = 0, $9 = 0, $10 = 0, $11 = 0, $12 = 0, $13 = 0, $14 = 0, $15 = 0, $16 = 0, $17 = 0, $18 = 0, $19 = 0, $20 = 0;
  return ((4294967295 << (k & 31 | 0) | 0) & x | 0) >>> (k & 31 | 0) | 0 | (((4294967295 >>> (32 - (k & 31 | 0) | 0) | 0) & x | 0) << (32 - (k & 31 | 0) | 0) | 0) | 0 | 0;
 }
 
 return {
  empty: $0, 
  add: $1, 
  div_s: $2
 };
}

var asmModule = asmFunc({
 Math: Math, 
 Int8Array: Int8Array, 
 Int16Array: Int16Array, 
 Int32Array: Int32Array, 
 Uint8Array: Uint8Array, 
 Uint16Array: Uint16Array, 
 Uint32Array: Uint32Array, 
 Float32Array: Float32Array, 
 Float64Array: Float64Array
}, {
 
}, new ArrayBuffer(65536));
function check1() {
 var wasm2asm_i32$0 = 0;
 asmModule.empty();
 wasm2asm_i32$0 = 1;
 return wasm2asm_i32$0 | 0;
}

if (!check1()) fail1();
function check2() {
 return (asmModule.add(1 | 0, 1 | 0) | 0 | 0) == (2 | 0) | 0;
}

if (!check2()) fail2();
function check3() {
 function f() {
  div_s(0 | 0, 0 | 0);
 }
 
 try {
  f();
 } catch (e) {
  return e.message.includes("integer divide by zero");
 };
 return 0;
}

if (!check3()) fail3();
function check4() {
 function f() {
  div_s(2147483648 | 0, 4294967295 | 0);
 }
 
 try {
  f();
 } catch (e) {
  return e.message.includes("integer overflow");
 };
 return 0;
}

if (!check4()) fail4();
