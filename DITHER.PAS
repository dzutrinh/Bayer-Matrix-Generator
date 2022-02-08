{ DITHER.PAS: Test drive for BAYER.PAS }
{ Compatible: Turbo/TMT/Free Pascal    }

{ enable the symbol to below display debugging message }
(* {$define __DEBUG__} *)

{$i include\common.inc}		{ common definitions }
{$i include\matrix.inc}		{ generated using BAYER.EXE }
{$i include\bitmap.inc}         { bitmap handlers }

const   DTH_GAUGE	: array[0..9] of string[10] =
		('>         ',
		 '=>        ',
		 '==>       ',
		 '===>      ',
		 '====>     ',
		 '=====>    ',
		 '======>   ',
		 '=======>  ',
		 '========> ',
		 '=========>');

function	gauge_text(current, total: uint): string;
begin
	gauge_text := DTH_GAUGE[10 * current div total];
end;

function	dither_bayer(const bmp: bitmap): bitmap;
var	out	: bitmap;
	x, y, c	: uint;
        pixel	: double;
begin
        dither_bayer := nil;

	if bmp = nil then exit;
        out := bitmap_create(bm_mono, bmp^.w, bmp^.h);
        if out = nil then exit;

        for y := 0 to bmp^.h-1 do
        begin
                write('. Dithering image: [', gauge_text(y, bmp^.h), ']'#13);
        	for x := 0 to bmp^.w-1 do
                begin
                        pixel := bitmap_getpix(bmp, x, y) / 255.0;
                        if pixel > bayerf(x, y) then
                        	c := 255
                        else	c := 0;
			bitmap_setpix(out, x, y, c);
                end;
        end;
	writeln;
        dither_bayer := out;
end;

var	bmp, out: bitmap;
	infile,
	outfile	: string;

begin
	writeln('Bayer dithering demo v1.0a (', COMPILER, ')');
        writeln('Coded by Trinh D.D. Nguyen');
	writeln;

	if paramcount = 0 then error('USAGE', 'dither <graymap> [outfile]');

        infile := paramstr(1);
        outfile := paramstr(2);

        if pos('.', infile) = 0 then infile := infile + '.pgm';
        if outfile = '' then outfile := replace_ext(infile, '.pbm');

        write('. Loading graymap: ');
	bmp := bitmap_load(infile);
        if bmp <> nil then
        begin
        	writeln('ok [', bmp^.w, 'x', bmp^.h, '], ',
			bmp^.size, ' bytes (', bmp^.size div 1024, ' KB)');

		out := dither_bayer(bmp);
                if out <> nil then
                begin
                	writeln('. Dithered bitmap: ok [', out^.w, 'x', out^.h, '], ',
				out^.size, ' bytes (', out^.size div 1024, ' KB)');

                        write  ('. Saving bitmap  : ');
	        	if bitmap_save(outfile, out) then
                        	writeln('ok [', outfile, ']')
                        else	writeln('failed');

        		bitmap_destroy(out);
                end
                else	writeln('failed');

		bitmap_destroy(bmp);
        end
        else	writeln('failed');
end.
