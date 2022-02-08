{$ifndef __BAYER_MATRIX_INC__}
{$define __BAYER_MATRIX_INC__}

const	BAYER_MATRIX_WIDTH   = 8;
	BAYER_MATRIX_SIZE    = 64;
	BAYER_MATRIX_MASK    = 7;
	BAYER_MATRIX_SHIFT   = 3;

	BAYER_MATRIX_8I	: array[0..BAYER_MATRIX_SIZE-1] of byte = (
	   0,  32,   8,  40,   2,  34,  10,  42,
	  48,  16,  56,  24,  50,  18,  58,  26,
	  12,  44,   4,  36,  14,  46,   6,  38,
	  60,  28,  52,  20,  62,  30,  54,  22,
	   3,  35,  11,  43,   1,  33,   9,  41,
	  51,  19,  59,  27,  49,  17,  57,  25,
	  15,  47,   7,  39,  13,  45,   5,  37,
	  63,  31,  55,  23,  61,  29,  53,  21);

	BAYER_MATRIX_8F	: array[0..BAYER_MATRIX_SIZE-1] of double = (
	0.0156,0.5156,0.1406,0.6406,0.0469,0.5469,0.1719,0.6719,
	0.7656,0.2656,0.8906,0.3906,0.7969,0.2969,0.9219,0.4219,
	0.2031,0.7031,0.0781,0.5781,0.2344,0.7344,0.1094,0.6094,
	0.9531,0.4531,0.8281,0.3281,0.9844,0.4844,0.8594,0.3594,
	0.0625,0.5625,0.1875,0.6875,0.0312,0.5312,0.1562,0.6562,
	0.8125,0.3125,0.9375,0.4375,0.7812,0.2812,0.9063,0.4062,
	0.2500,0.7500,0.1250,0.6250,0.2187,0.7187,0.0937,0.5937,
	1.0000,0.5000,0.8750,0.3750,0.9688,0.4687,0.8438,0.3437);

function        bayeri(x, y: longint): longint;
begin
	bayeri := BAYER_MATRIX_8I[(y and BAYER_MATRIX_MASK) shl BAYER_MATRIX_SHIFT
	                         +(x and BAYER_MATRIX_MASK)];
end;

function        bayerf(x, y: longint): double;
begin
	bayerf := BAYER_MATRIX_8F[(y and BAYER_MATRIX_MASK) shl BAYER_MATRIX_SHIFT
	                         +(x and BAYER_MATRIX_MASK)];
end;

{$endif}
