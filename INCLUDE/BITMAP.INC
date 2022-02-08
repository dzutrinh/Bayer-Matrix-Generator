{$ifndef __BAYER_BITMAP_INC__}
{$define __BAYER_BITMAP_INC__}

type    bitmap_format = (bm_mono, bm_gray);
	bitmap = ^tbitmap;
	tbitmap = record
		data	: pbarr;
                fmt 	: bitmap_format;
                w, h	: uint;
                stride	: uint;
                size	: uint;
	end;
        bitmap_signature = string[2];

const   SIG_P4	: bitmap_signature = 'P4';
	SIG_P5	: bitmap_signature = 'P5';

        BITMASK	: array[0..7] of byte = ($80, $40, $20, $10, $08, $04, $02, $01);

{$ifdef	VER70}
	BMP_MAX = 65528;		{ 16-bit: limit to a segment }
{$else}
        BMP_MAX	= 32 * 1024 * 1024;	{ 32-bit: raise the limit to 32MB }
{$endif}

function        bitmap_create(fmt: bitmap_format; w, h: uint): bitmap;
var	bmp	: bitmap;
	stride,
	size	: uint;
begin
	bitmap_create := nil;

	if fmt = bm_mono then
		stride := ((w + 7) div 8)
        else	stride := w;
        size := h * stride;

	if (memavail < size) then
        begin
        	debug('Out of memory');
		exit;
        end;

	new(bmp);
        bmp^.fmt    := fmt;
	bmp^.w      := w;
	bmp^.h      := h;
	bmp^.size   := size;
        bmp^.stride := stride;
	getmem(bmp^.data, size);
        if bmp^.fmt = bm_mono then
		fillchar(bmp^.data^, size, $FF)
        else	fillchar(bmp^.data^, size, $00);
	bitmap_create := bmp;
end;

procedure	bitmap_destroy(var bmp: bitmap);
begin
	if bmp = nil then exit;
	if bmp^.data <> nil then freemem(bmp^.data, bmp^.size);
	dispose(bmp);
	bmp := nil;
end;

procedure	bitmap_setpix1(const bmp: bitmap; x, y: uint; f: boolean);
var     ibyte, o,
	ibit	: uint;
begin
	ibyte := x shr 3;
        ibit  := x and 7;
        o     := y * bmp^.stride + ibyte;
        if f then
        	bmp^.data^[o] := bmp^.data^[o] and not BITMASK[ibit]
        else	bmp^.data^[o] := bmp^.data^[o] or BITMASK[ibit]
end;

function	bitmap_getpix1(const bmp: bitmap; x, y: uint): boolean;
var     ibyte, o,
	ibit	: uint;
begin
	ibyte := x shr 3;
        ibit  := x and 7;
        o     := y * bmp^.stride + ibyte;
        bitmap_getpix1 := bmp^.data^[o] and BITMASK[ibit] = 0;
end;

function	bitmap_getpix8(const bmp: bitmap; x, y: uint): byte;
begin
        bitmap_getpix8 := bmp^.data^[y * bmp^.w + x];
end;

procedure	bitmap_setpix8(const bmp: bitmap; x, y, v: uint);
begin
        bmp^.data^[y * bmp^.w + x] := v;
end;

procedure	bitmap_setpix(const bmp: bitmap; x, y, v: uint);
begin
	if bmp = nil then exit;
        if (x > bmp^.w-1) or (y > bmp^.h-1) then exit;
	if bmp^.fmt = bm_mono then
        	bitmap_setpix1(bmp, x, y, v <> 0)
        else	bitmap_setpix8(bmp, x, y, v);
end;

function	bitmap_getpix(const bmp: bitmap; x, y: uint): byte;
begin
        bitmap_getpix := 0;
	if bmp = nil then exit;
        if (x > bmp^.w-1) or (y > bmp^.h-1) then exit;
        if bmp^.fmt = bm_mono then
       		bitmap_getpix := byte(bitmap_getpix1(bmp, x, y))
        else	bitmap_getpix := bitmap_getpix8(bmp, x, y);
end;

{*--------------------------------------------------------------------------*}
{*                  quick and dirty PBM/PGM loader/writer                   *}
{*--------------------------------------------------------------------------*}

function	bitmap_load(name: string): bitmap;
var	bmp	: bitmap;
	fmt	: bitmap_format;
	f	: file;
        w, h, nr: uint;
	s	: longint;
	buf	: string;

	function	fgetc(var f: file): integer;
	var	v	: byte;
	begin
		if not eof(f) then
		begin
			blockread(f, v, 1);
                        fgetc := v;
		end	else fgetc := -1;
	end;

        function        getsig(var f: file): bitmap_signature;
        var	sig	: bitmap_signature;
        begin
        	sig := '';
                sig := sig + char(fgetc(f));
                sig := sig + char(fgetc(f));
                fgetc(f);
		getsig := sig;
        end;

        function        getstr(var f: file): string;
        var	s	: string;
        	c, k	: integer;
        begin
                s := '';
                k := 1;
                while (k < 256) do
                begin
                	c := fgetc(f);
                        if c = $0A then break;
                        s[k] := char(byte(c));
                        inc(k);
                end;
                s[0] := char(k-1);
                getstr := s;
        end;

begin
	bitmap_load := nil;
        assign(f, name);
	{$i-}
        reset(f, 1);
        {$i+}
        if ioresult <> 0 then exit;

        buf := getsig(f);
        if (buf[1] <> 'P') or
	   ((buf[2] <> '4') and (buf[2] <> '5')) then
	begin
        	debug('Unsupport bitmap format');
		close(f);
		exit;
	end;

        if buf = SIG_P4 then fmt := bm_mono else fmt := bm_gray;

        w := atoi(getstr(f));
        h := atoi(getstr(f));
        s := longint(h) * w;

        if s > BMP_MAX then
        begin
        	debug('File is too large. Use TMT/Free Pascal to build this program.');
        	close(f);
                exit;
        end;

	if fmt = bm_gray then getstr(f);	{ bypass pixel size }

        bmp := bitmap_create(fmt, w, h);
        blockread(f, bmp^.data^, s, nr);
        if nr <> s then
        begin
        	debug('Corrupted bitmap');
                close(f);
                bitmap_destroy(bmp);
                exit;
        end;

        close(f);
        bitmap_load := bmp;
end;

function	bitmap_save(name: string; const bmp: bitmap): boolean;
var	f	: file;
        ret	: boolean;
        nw	: uint;
        sig	: bitmap_signature;
label	done, fail;

	function	fputc(var f: file; c: byte): boolean;
        begin
        	fputc := false;
        	{$i-}
		blockwrite(f, c, 1);
                {$i+}
        	if ioresult <> 0 then exit;
                fputc := true;
        end;

        function	putstr(var f: file; st: string): boolean;
        var	i	: byte;
        begin
                putstr := false;
                if st = '' then exit;
        	for i := 1 to length(st) do
			if not fputc(f, byte(st[i])) then exit;
                if not fputc(f, $0A) then exit;
                putstr := true;
        end;

begin
	bitmap_save := false;
        ret := true;
        if (bmp = nil) or (name = '') then exit;

        assign(f, name);
        {$i-}
        rewrite(f, 1);
        {$i+}
        if ioresult <> 0 then
        begin
        	debug('File creation error');
                exit;
        end;

        if bmp^.fmt = bm_mono then sig := SIG_P4 else sig := SIG_P5;
        if not putstr(f, sig) then goto fail;
        if not putstr(f, itoa(bmp^.w)) then goto fail;
	if not putstr(f, itoa(bmp^.h)) then goto fail;
	if bmp^.fmt <> bm_mono then if not putstr(f, '255') then goto fail;

        blockwrite(f, bmp^.data^, bmp^.size, nw);
        if bmp^.size <> nw then goto fail;

        goto done;

fail:
	debug('File write error');
	ret := false;

done:
        close(f);
        bitmap_save := ret;
end;

{$endif}
