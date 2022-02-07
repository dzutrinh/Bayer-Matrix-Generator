{ BAYER.PAS : Bayer matrix generator }
{ Compatible: Turbo/TMT/Free Pascal  }

uses	dos;

{$i common.inc}		{ common definitions }
{$i bayer.inc}		{ Bayer matrix API }

var	m	: BAYER_MATRIX;
	f	: TEXT;
        fname	: string;
        level	: integer;

begin
        writeln('Bayer matrix generator * v1.0 (', COMPILER, ')');
        writeln('Coded by Trinh D.D. Nguyen');
        writeln;

        if paramcount <> 2 then	error('USAGE', 'bayer <filename> <level>');

	fname := paramstr(1);
        level := atoi(paramstr(2));

        if (level < 1) or (level > 4) then
		error('ERROR', 'level must be within the range [1..4]');

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
