@echo off
setlocal

set self=%~dp0
set vendor=%self%\vendor
set curl=%vendor%\curl

if "%1%"=="curl32" (
  cd "%curl%\builds"
  if %errorlevel% neq 0 exit /b %errorlevel%
  del * /Q /S
  cd "%curl%\winbuild"
  nmake /f Makefile.vc mode=static RTLIBCFG=static MACHINE=x86
  xcopy /y /s %curl%\builds\libcurl-vc-x86-release-static-ssl-static-ipv6-sspi %vendor\libcurl32\
  exit /b
)
if "%1%"=="curl64" (
  cd "%curl%\builds"
  if %errorlevel% neq 0 exit /b %errorlevel%
  del * /Q /S
  cd "%curl%\winbuild"
  nmake /f Makefile.vc mode=static RTLIBCFG=static MACHINE=x64
  xcopy /y /s %curl%\builds\libcurl-vc-x64-release-static-ssl-static-ipv6-sspi %vendor\libcurl64\
  exit /b
)

call %self%\wrapper i386 cmd /c %self%\build_deps curl32
call %self%\wrapper x86_64 cmd /c %self%\build_deps curl64
