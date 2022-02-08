{$ifndef __BAYER_COMMON_INC__}
{$define __BAYER_COMMON_INC__}

{$ifdef VER70}
{$N+}			{ enable numeric processor, BP7 only }
{$endif}

uses	dos;

const
{$ifdef	__TMT__}
	COMPILER	= 'TMT';
{$else}
{$ifdef	VER70}
	COMPILER	= 'BPC';
{$else}
{$ifdef	FPC}
	COMPILER	= 'FPC';
{$else}
	COMPILER        = 'N/A';
{$endif}
{$endif}
{$endif}

{$R-}
type	tbarr = array[0..0] of byte;	{ pointer to array type }
        pbarr = ^tbarr;

{$ifdef	VER70}
        uint	= word;			{ 16-bit word type }
{$else}
	uint	= dword;		{ 32-bit dword type }
{$endif}

const	yesno	: array[boolean] of string[3] = ('no ', 'yes');

function	atoi(s: string): longint;
var	e	: integer;
	v	: longint;
begin
	val(s, v, e);
        atoi := v;
end;

function	itoa(v: longint): string;
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

{ these functions require DOS unit }
function	replace_ext(path, newext: string): string;
var	d	: dirstr;
	n	: namestr;
        e	: extstr;
begin
        fsplit(path, d, n, e);
        replace_ext := n + newext;		{ we discard the path portion }
end;

function	fexist(path: string): boolean;
begin
	fexist := fsearch(path, '') <> '';	{ simply search for it }
end;

{$endif}
