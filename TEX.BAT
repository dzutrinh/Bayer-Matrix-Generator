@echo off

::
:: Level 1 to 4 Bayer patterns generator
:: All generated textures are stored in the TEXTURES folder
::

if not exist gentex.exe goto error
if not exist textures\nul mkdir textures
gentex textures\bayer1.pgm 1 
gentex textures\bayer2.pgm 2 
gentex textures\bayer3.pgm 3 
gentex textures\bayer4.pgm 4
goto done
:error
echo ERROR: gentex.exe is not found
:done
