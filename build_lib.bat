SET "MOONLIB=%cd:\=/%"
SET "MOONLIB_WIN=%cd%"
SET PATH=%MOONLIB_WIN%\bin;%PATH%
SET "GDK=D:\code\megadrive\sgdk200"

make -f %MOONLIB_WIN%\makelib.gen cleanrelease
make -f %MOONLIB_WIN%\makelib.gen release
make -f %MOONLIB_WIN%\makelib.gen cleandebug
make -f %MOONLIB_WIN%\makelib.gen debug

@ECHO.
@ECHO.
@ECHO -------------------------------------------
@ECHO MOONLIB is now ready!...
@echo off
rem @PAUSE >nul
