{ BAYER.PAS : Bayer matrix generator }
{ Compatible: Turbo/TMT/Free Pascal  }

{$i include\common.inc}		{ common definitions }
{$i include\bayer.inc}		{ Bayer matrix API }
{$i include\bitmap.inc}		{ bitmap handlers }

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
        write  (f, #9);
        for i := 0 to m^.size-2 do
        begin
                write(f, m^.data^[i]:4, ',');
                if (i+1) mod m^.width = 0 then
                begin
                	writeln(f);
                        write(f, #9);
                end;
        end;
       	writeln(f, m^.data^[m^.size-1]:4, ');');
        writeln(f);

        writeln(f, #9+cname+
		   'F	: array[0..'+prefix+'SIZE-1] of double = (');
        write  (f, #9);
        for i := 0 to m^.size-2 do
        begin
        	write(f, 1.0 * (m^.data^[i]+1)/m^.size:0:4, ',');
                if (i+1) mod m^.width = 0 then
                begin
                	writeln(f);
                        write(f, #9);
                end;
        end;
        writeln(f, 1.0 * (m^.data^[m^.size-1]+1)/m^.size:0:4, ');');
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

var	m	: BAYER_MATRIX;
	f	: TEXT;
        fname	: string;
        level	: uint;
	resp	: char;

begin
        writeln('Bayer matrix generator v1.0a (', COMPILER, ')');
        writeln('Coded by Trinh D.D. Nguyen');
        writeln;

        if paramcount <> 2 then	error('USAGE', 'bayer <filename> <level>');

	fname := paramstr(1);
        level := uint(atoi(paramstr(2)));

        if (level < 1) or (level > 4) then
		error('ERROR', 'level must be within the range [1..4]');

        if fexist(fname) then
        begin
        	write(fname, ' is already exist. Overwrite (y/n)? ');
                readln(resp);
                if upcase(resp) = 'N' then exit;
        end;

        write('. Creating file      : ');
	if header_open(f, fname) then
        begin
        	writeln('created [', fname, ']');

                write('. Constructing matrix: ');
		m := bayerCreate(level);
                if m <> nil then
                begin
                	writeln('ok [', m^.width, 'x', m^.width, ']');

                        write('. Writing header     : ');
        	        header_write(f, m);
                        writeln('ok');

                        write('. Cleaning up        : ');
	        	bayerFree(m);
		        header_close(f);
                        writeln('ok');
                end
                else	writeln('failed.');
	end
        else	writeln('failed');
end.
