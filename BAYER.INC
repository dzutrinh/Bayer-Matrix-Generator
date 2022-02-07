{$ifndef __BAYER_MATRIX_API__}
{$define __BAYER_MATRIX_API__}

const
{$ifdef	VER70}
        BYTE_MAX_SIZE	= 16384;
{$else}
	BYTE_MAX_SIZE	= 65536;
{$endif}

type    BYTE_ARRAY	= array[0..BYTE_MAX_SIZE-1] of byte;
        BYTE_PTR	= ^BYTE_ARRAY;

	BAYER_MATRIX	= ^BAYER_MATRIX_T;
	BAYER_MATRIX_T	= record
		data	: BYTE_PTR;     { actual matrix data            }
                level	: uint;		{ level of the matrix in [1..4] }
                width	: uint;		{ width of the matrix           }
                size	: uint;		{ size of the matrix = level^2  }
        end;

function	bayerAt(const m: BAYER_MATRIX; x, y: uint): uint;
begin
	bayerAt := (y mod m^.width) * m^.width + (x mod m^.width);
end;

procedure	bayerGenerate(var m: BAYER_MATRIX; x, y, size, value, step: uint);
var	half	: uint;
begin
	if size = 1 then
        	m^.data^[bayerAt(m, x, y)] := value
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
                	write((1.0 * (m^.data^[bayerAt(m, x, y)]+1)/m^.size):6:4, ' ');
                writeln;
        end;
end;

function        header_open(var f: TEXT; name: string): boolean;
begin
	assign(f, name);
        {$I-}
        rewrite(f);
	{$I+}
        if ioresult <> 0 then
        begin
        	header_open := FALSE;
        	exit;
        end;
	writeln(f, '{$ifndef __BAYER_MATRIX_INC__}');
	writeln(f, '{$define __BAYER_MATRIX_INC__}');
        header_open := TRUE;
end;

procedure	header_close(var f: TEXT);
begin
	writeln(f, '{$endif}');
	close(f);
end;

procedure	header_write(var f: TEXT; const m: BAYER_MATRIX);
var	i, shift: uint;
        prefix,
	cname	: string;
begin
	case m^.level of
        1: shift := 1;
        2: shift := 2;
        3: shift := 3;
        4: shift := 4;
        end;

        prefix := 'BAYER_MATRIX_';
        cname := prefix+itoa(m^.width);
	writeln(f);
        write  (f, 'const');
        writeln(f, #9+prefix+'WIDTH   = '+itoa(m^.width)+';');
        writeln(f, #9+prefix+'SIZE    = '+itoa(m^.size)+';');
        writeln(f, #9+prefix+'MASK    = '+itoa(m^.width-1)+';');
        writeln(f, #9+prefix+'SHIFT   = '+itoa(shift)+';');
        writeln(f);
        writeln(f, #9+cname+
		   'I	: array[0..'+prefix+'SIZE-1] of byte = (');
        write  (f, #9#9);
        for i := 0 to m^.size-2 do
        begin
                write(f, m^.data^[i]:4, ', ');
                if (i+1) mod m^.width = 0 then
                begin
                	writeln(f);
                        write(f, #9#9);
                end;
        end;
       	writeln(f, m^.data^[m^.size-1]:4, ');');
        writeln(f);

        writeln(f, #9+cname+
		   'F	: array[0..'+prefix+'SIZE-1] of double = (');
        write  (f, #9#9);
        for i := 0 to m^.size-2 do
        begin
        	write(f, 1.0 * (m^.data^[i]+1)/m^.size:0:6, ', ');
                if (i+1) mod m^.width = 0 then
                begin
                	writeln(f);
                        write(f, #9#9);
                end;
        end;
        writeln(f, 1.0 * (m^.data^[m^.size-1]+1)/m^.size:0:6, ');');
        writeln(f);

	writeln(f, 'function        bayeri(x, y: longint): longint;');
	writeln(f, 'begin');
	writeln(f, #9+'bayeri := '+cname+'I'+
		   '[(y and '+prefix+'MASK) shl '+prefix+'SHIFT');
	writeln(f, #9'                         +(x and '+prefix+'MASK)];');
	writeln(f, 'end;');
        writeln(f);
	writeln(f, 'function        bayerf(x, y: longint): double;');
	writeln(f, 'begin');
	writeln(f, #9+'bayerf := '+cname+'F'+
		   '[(y and '+prefix+'MASK) shl '+prefix+'SHIFT');
	writeln(f, #9'                         +(x and '+prefix+'MASK)];');
	writeln(f, 'end;');
        writeln(f);
end;

{$endif}
