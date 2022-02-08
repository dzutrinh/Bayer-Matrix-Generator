{$ifndef __BAYER_MATRIX_API__}
{$define __BAYER_MATRIX_API__}

type    BAYER_MATRIX	= ^BAYER_MATRIX_T;
	BAYER_MATRIX_T	= record
		data	: pbarr;        { actual matrix data            }
                level	: uint;		{ level of the matrix in [1..4] }
                width	: uint;		{ width of the matrix           }
                size	: uint;		{ size of the matrix = level^2  }
        end;

function	bayerOffset(const m: BAYER_MATRIX; x, y: uint): uint;
begin
	bayerOffset := (y mod m^.width) * m^.width + (x mod m^.width);
end;

function        bayerAt(const m: BAYER_MATRIX; x, y: uint): uint;
begin
	bayerAt := m^.data^[(y mod m^.width) * m^.width + (x mod m^.width)];
end;

procedure	bayerGenerate(var m: BAYER_MATRIX; x, y, size, value, step: uint);
var	half	: uint;
begin
	if size = 1 then
        	m^.data^[bayerOffset(m, x, y)] := value
        else
        begin
        	half := size shr 1;
	        bayerGenerate(m, x     , y     , half, value+step*0, step*4);
        	bayerGenerate(m, x+half, y+half, half, value+step*1, step*4);
	        bayerGenerate(m, x+half, y     , half, value+step*2, step*4);
        	bayerGenerate(m, x     , y+half, half, value+step*3, step*4);
        end;
end;

procedure	bayerConstruct(var m: BAYER_MATRIX);
begin
        bayerGenerate(m, 0, 0, m^.width, 0, 1);
end;

function	bayerCreate(level: byte): BAYER_MATRIX;
var	p	: BAYER_MATRIX;
	size	: uint;
begin
	bayerCreate := nil;
        if level > 4 then exit;

	size := (1 shl level);
	size := size * size;

        if memavail < size then exit;

        new(p);
	p^.level := level;
        p^.size := size;
	p^.width := (1 shl level);

        getmem(p^.data, p^.size);
        bayerConstruct(p);

        bayerCreate := p;
end;

procedure	bayerFree(var m: BAYER_MATRIX);
begin
	freemem(m^.data, m^.size);
        dispose(m);
        m := nil;
end;

procedure	bayerPrint(const m: BAYER_MATRIX);
var	x, y	: byte;
begin
	for y := 0 to m^.width-1 do
        begin
        	for x := 0 to m^.width-1 do
                	write((1.0 * (m^.data^[bayerOffset(m, x, y)]+1)/m^.size):6:4, ' ');
                writeln;
        end;
end;

{$endif}
