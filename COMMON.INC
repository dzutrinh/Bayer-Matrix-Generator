{$ifndef __BAYER_COMMON_INC__}
{$define __BAYER_COMMON_INC__}

const
{$ifdef	__TMT__}
	COMPILER	= 'TMT Pascal';
{$else}
{$ifdef	VER70}
	COMPILER	= 'Borland Pascal';
{$else}
{$ifdef	FPC}
	COMPILER	= 'Free Pascal';
{$else}
	COMPILER        = 'Unknown';
{$endif}
{$endif}
{$endif}

type	pbyte = ^byte;
{$ifdef	VER70}
        uint	= word;
{$else}
	uint	= dword;
{$endif}

function	atoi(s: string): longint;
var	e	: integer;
	v	: longint;
begin
	val(s, v, e);
        atoi := v;
end;

function	itoa(v: word): string;
var	s	: string;
begin
	s := '';
        str(v, s);
        itoa := s;
end;

procedure	error(caption, msg: string);
begin
        writeln(caption, ': ', msg);
        halt(1);
end;

procedure	debug(msg: string);
begin
{$ifdef	__DEBUG__}
	writeln;
	writeln('DEBUG: ', msg);
        writeln;
{$endif}
end;

function	fexist(name: string): boolean;
begin
	fexist := fsearch(name, '') <> '';
end;

{$endif}
