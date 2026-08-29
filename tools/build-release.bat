@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1
set "PATH=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"
set "CMAKE=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
where cl
where ninja
"%CMAKE%" --preset windows-release -DCMAKE_MAKE_PROGRAM=ninja
if errorlevel 1 exit /b 1
"%CMAKE%" --build --preset windows-release
exit /b %ERRORLEVEL%
