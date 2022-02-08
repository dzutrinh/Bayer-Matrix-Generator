@echo off

::
:: For testing sample images included in the repo,
:: which are stored within the TEST folder.
::

if !%1==! goto param
if not exist dither.exe goto error
if not exist test\nul goto missing
dither test\%1
goto done
:param
echo USAGE: dth image
goto done
:missing
echo ERROR: missing test folder
goto done
:error
echo ERROR: dither.exe is not found
:done

