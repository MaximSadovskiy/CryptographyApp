@echo off

echo Generating Visual Studio 2022 profile...

IF EXIST .\vendors\premake\premake5.exe (
  IF EXIST premake5.lua (
    .\vendors\premake\premake5.exe --file=premake5.lua vs2022
	goto END
  ) ELSE (
    ECHO Cannot find premake config "premake5.lua"
	PAUSE
    goto END
  )
) ELSE (
  ECHO premake5.exe cannot be found
  PAUSE
  goto END
)

:END
ECHO Exiting...