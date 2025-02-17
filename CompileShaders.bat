@echo off
setlocal enabledelayedexpansion

REM Set the path to the glslc compiler
set GLSLC_PATH=C:\VulkanSDK\1.4.304.0\Bin\glslc.exe

REM Set the relative directory containing the shader files
set SHADER_DIR=Shaders
set OUTPUT_DIR=CompiledShaders

REM Ensure the output directory exists and clear its contents
if exist "%OUTPUT_DIR%" (
    rmdir /s /q "%OUTPUT_DIR%"
)
mkdir "%OUTPUT_DIR%"

REM Extensions to compile
set EXTENSIONS=glsl frag vert geom comp tese tesc mesh task rgen rint rmiss rahit rchit rcall

REM Iterate over all directories in the shader directory and compile them
for /r "%SHADER_DIR%" %%d in (.) do (
    REM Get the relative path from SHADER_DIR to the current directory
    set "FULL_PATH=%%d"
    set "REL_PATH=!FULL_PATH:%~dp0%SHADER_DIR%\=!"

    REM Ensure the corresponding output directory exists
    if not exist "%OUTPUT_DIR%\!REL_PATH!" (
        mkdir "%OUTPUT_DIR%\!REL_PATH!"
    )

    REM Check for duplicate output names
    for %%e in (%EXTENSIONS%) do (
        for %%f in (%%d\*.%%e) do (
            set "OUTPUT_NAME=%OUTPUT_DIR%\!REL_PATH!\%%~nf.spv"
            if exist "!OUTPUT_NAME!" (
                echo Error: Duplicate output name detected for %%f resulting in !OUTPUT_NAME!.
                pause
                endlocal
                exit /b 1
            )
            echo Compiling %%f
            set "OUTPUT_NAME=%OUTPUT_DIR%\!REL_PATH!\%%~nf.spv"
            %GLSLC_PATH% "%%f" -o "!OUTPUT_NAME!"
        )
    )
)   

echo All shaders compiled.
pause
endlocal
