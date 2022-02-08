{ GENTEX.PAS: Bayer texture generator  }
{ Compatible: Turbo/TMT/Free Pascal    }

{$i include\common.inc}		{ common definitions }
{$i include\bayer.inc}		{ Bayer matrix API   }
{$i include\bitmap.inc}         { bitmap handlers    }

const	TEX_SIZE	= 128;

function        bayerTexGen(const m: BAYER_MATRIX; tile: boolean): bitmap;
var	x, y,
	z, t	: uint;
	tex	: bitmap;
begin
	bayerTexGen := nil;
        if m = nil then exit;

	z := TEX_SIZE div m^.width;
	tex := bitmap_create(bm_gray, TEX_SIZE, TEX_SIZE);
        if tex = nil then exit;

        for y := 0 to TEX_SIZE-1 do
        begin
        	for x := 0 to TEX_SIZE-1 do
                begin
                	if tile then
                        	t := bayerAt(m, x div 4, y div 4)
                        else	t := bayerAt(m, x div z, y div z);
                        t := round((t/m^.size) * $FF);
                        bitmap_setpix(tex, x, y, t);
                end;
        end;

        bayerTexGen := tex;
end;

var	filename: string;
	level	: uint;
        tile	: boolean;
        m	: BAYER_MATRIX;
        tex	: bitmap;

begin
        writeln('Bayer texture generator v1.0 (', COMPILER, ')');
	writeln('Coded by Trinh D.D. Nguyen');
        writeln;

	if paramcount < 2 then
        	error('USAGE', 'gentex <filename> <level> [tile]');

	filename := paramstr(1);
        level	 := atoi(paramstr(2));
        tile	 := paramstr(3) = 'tile';

        if (level < 1) or (level > 4) then
        	error('ERROR', 'level must be in the range [1..4]');

        writeln('. Output file       : ', filename);
        writeln('. Matrix level      : ', level);
        writeln('. Tile pattern      : ', yesno[tile]);

        write  ('. Creating matrix   : ');
        m := bayerCreate(level);
        if m <> nil then
        begin
        	writeln('ok [', m^.width, 'x', m^.width, '], ', m^.size, ' bytes');

                write('. Generating texture: ');
		tex := bayerTexGen(m, tile);
                if tex <> nil then
                begin
                	writeln('ok [', tex^.w, 'x', tex^.h, '], ', tex^.size, ' bytes');

                        write('. Saving texture    : ');
		        if bitmap_save(filename, tex) then
                        	writeln('ok')
                        else	writeln('failed');
	        	bitmap_destroy(tex);
                end
                else	writeln('failed');
		bayerFree(m);
	end
        else	writeln('failed')
end.

