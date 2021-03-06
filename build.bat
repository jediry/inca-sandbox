@ECHO OFF

:: Uncomment this SETLOCAL if you want built.bat to "forget" the discovered Python/SCons after executing SCons.
::
:: When this line is commented out, a small number of environment variables will be set in the calling shell's
:: environment, allowing the "find" scripts to remember where Python/SCons were previously found, making subsequent
:: runs faster. However, it does modify the calling shell's environment, and some people may not like this. Also,
:: if they "remember" the results of the previous run, the "find" scripts will not detect any newly installed Python
:: or SCons until the command shell is closed and re-launched, so uncommenting this can be helpful when testing the
:: "find" scripts.
::SETLOCAL


:: Ensure that most environment variables set here, stay here...except for those that are explicitly propagated back
:: to the caller at the end of this script
SETLOCAL


:: Uncomment these (or set them in your environment) to make bootstrap-scons print out information about where it's
:: looking for things
::SET FIND_PYTHON_DEBUG=1
::SET FIND_SCONS_DEBUG=1

:: Where bootstrap-scons is located, relative to this script (%~dp0 expands to the directory containing this script)
SET BOOTSTRAP_SCONS=%~dp0external\bootstrap-scons

:: Where to put Python and/or SCons if we have to download them. This is used to initialise FIND_PYTHON_LOCAL_ROOT and
:: FIND_SCONS_LOCAL_ROOT immediately below.
SET FIND_STUFF_LOCAL_ROOT=%~dp0external

:: Set default versions of Python & SCons, and where to install them, but let the user
:: override these by setting them in the environment, if desired
IF "%FIND_PYTHON_VERSION%"    == "" (SET FIND_PYTHON_VERSION=3.7.0)
IF "%FIND_PYTHON_LOCAL_ROOT%" == "" (SET FIND_PYTHON_LOCAL_ROOT=%FIND_STUFF_LOCAL_ROOT%)
IF "%FIND_SCONS_VERSION%"     == "" (SET FIND_SCONS_VERSION=3.0.5)
IF "%FIND_SCONS_LOCAL_ROOT%"  == "" (SET FIND_SCONS_LOCAL_ROOT=%FIND_STUFF_LOCAL_ROOT%)


:: Do it! Locate/download Python and/or SCons, and then invoke SCons
CALL %BOOTSTRAP_SCONS%\scons.bat %*


:: Configure the environment in the calling shell. Since everyting up through here is inside of SETLOCAL, we need to
:: ENDLOCAL here so that we can modify the outer environment. Environment variables within ( )s are expanded
:: immediately, before executing anything inside, which allows us to pass values out of the SETLOCAL/ENDLOCAL scope.
::
:: However, if the first call to SETLOCAL (around line 11) is uncommented, that will defeat this and prevent the calling
:: environment from being altered, by still keeping everything in this script inside of a SETLOCAL scope.
(
    ENDLOCAL
    SET "FOUND_SCONS_AT=%FOUND_SCONS_AT%"
    SET "FOUND_PYTHON_AT=%FOUND_PYTHON_AT%"
    SET "PYTHONPATH=%PYTHONPATH%"
    SET "PATH=%PATH%"
)
