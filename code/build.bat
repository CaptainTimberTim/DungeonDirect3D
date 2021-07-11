@echo off

IF NOT EXIST "../build" mkdir "../build"
pushd "../build"

set buildFlags= -DDEBUG_TD=1 -DCONSOLE_APP_
set optimize= -O2

	REM Now using MD instead of -MTd for external symbol resolving of linker
set compilerFlags= -MD -Gm- -nologo -GR- -EHa- -Od -Oi -FC -Z7 
set compilerWarningLevel= -WX -W4 -wd4201 -wd4100 -wd4189 -wd4456 -wd4505 -wd4005 -wd4239 -wd4706 -wd4127 -wd4390

	REM subsystem for x86 need 5.1 || 'console' for main(), 'windows' for WinMain
set linkerFlags= /link -incremental:no -opt:ref 
   REM -subsystem:windows,5.2 
set libIncludes= /NODEFAULTLIB:LIBCMT User32.lib Gdi32.lib winmm.lib d3d11.lib d3dcompiler.lib


cl -EHsc %compilerFlags% %buildFlags% %compilerWarningLevel% "../code/Main.cpp" %linkerFlags% %libIncludes%

REM cl -EHsc %optimize% -nologo %buildFlags% %addIncludeDir% %compilerWarningLevel% "../code/Main.cpp" %linkerFlags% %addLinkerLibs%  %libIncludes% 

popd