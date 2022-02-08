@echo off

::
:: Auto generate a Bayer matrix of a specified level, write it
:: to a source file and place it into the INCLUDE folder.
::

if not exist bayer.exe goto error
if !%1==! goto param
bayer include\matrix.inc %1
goto done
:param
echo USAGE: mat level
goto done
:error
echo ERROR: bayer.exe is not found
:done
