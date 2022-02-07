{ DITHER.PAS: Test drive for BAYER.PAS }
{ Compatible: Turbo/TMT/Free Pascal    }

uses	dos;

{ enable the symbol to below display debugging message }
(* {$define __DEBUG__} *)

{$ifdef VER70}
{$N+,E+}		{ enable numeric processor and math emulation }
{$endif}

{$i matrix.inc}		{ generated using BAYER.EXE }
{$i common.inc}		{ common definitions }
{$i bitmap.inc}         { bitmap handlers }

const   BMP_OUTPUT	: string = 'output.pgm';

function	dither_bayer(const bmp: bitmap): bitmap;
var	out	: bitmap;
	x, y    : uint;
        pixel	: double;
	p, q	: pbyte;
begin
        dither_bayer := nil;

	if bmp = nil then exit;
        out := bitmap_create(bmp^.w, bmp^.h);
        if out = nil then exit;

	p := bmp^.data;
	q := out^.data;
        for y := 0 to bmp^.h-1 do
        begin
        	for x := 0 to bmp^.w-1 do
                begin
                	pixel := p^/255.0;
			inc(p);
                        if pixel > bayerf(x, y) then
                        	q^ := 255
                        else	q^ := 0;
			inc(q);
                end;
        end;

        dither_bayer := out;
end;

var	bmp, out: bitmap;
	filename: string;

begin
	writeln('Bayer dithering demo (', COMPILER, ')');
	writeln;

	if paramcount = 0 then error('USAGE', 'dither <graymap>');

        filename := paramstr(1);
        if pos('.', filename) = 0 then filename := filename + '.pgm';

        write('. Loading graymap: ');
	bmp := bitmap_load(filename);
        if bmp <> nil then
        begin
        	writeln('ok [', bmp^.w, 'x', bmp^.h, '], ', bmp^.size, ' bytes');
                write('. Dithering      : ');
		out := dither_bayer(bmp);
                if out <> nil then
                begin
                	writeln('ok');
                        write('. Saving graymap : ');
	        	if bitmap_save(BMP_OUTPUT, out) then
                        	writeln('ok')
                        else	writeln('failed');
        		bitmap_destroy(out);
                end
                else	writeln('failed');
		bitmap_destroy(bmp);
        end
        else	writeln('failed');
end.
