{$ifndef __BAYER_MATRIX_INC__}
{$define __BAYER_MATRIX_INC__}

const	BAYER_MATRIX_WIDTH   = 2;
	BAYER_MATRIX_SIZE    = 4;
	BAYER_MATRIX_MASK    = 1;
	BAYER_MATRIX_SHIFT   = 1;

	BAYER_MATRIX_2I	: array[0..BAYER_MATRIX_SIZE-1] of byte = (
		   0,    2, 
		   3,    1);

	BAYER_MATRIX_2F	: array[0..BAYER_MATRIX_SIZE-1] of double = (
		0.250000, 0.750000, 
		1.000000, 0.500000);

function        bayeri(x, y: longint): longint;
begin
	bayeri := BAYER_MATRIX_2I[(y and BAYER_MATRIX_MASK) shl BAYER_MATRIX_SHIFT
	                         +(x and BAYER_MATRIX_MASK)];
end;

function        bayerf(x, y: longint): double;
begin
	bayerf := BAYER_MATRIX_2F[(y and BAYER_MATRIX_MASK) shl BAYER_MATRIX_SHIFT
	                         +(x and BAYER_MATRIX_MASK)];
end;

{$endif}
