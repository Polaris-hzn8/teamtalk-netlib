@echo off
setlocal

rem 获取脚本所在目录
set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%"

set SRC_DIR=.
set GEM_DIR=.\gen
set DEST_DIR=..\src\pbgen
set PROTOC=..\third\protobuf\bin\protoc.exe

echo Current directory: %CD%
echo PROTOC path: %PROTOC%
echo Script directory: %SCRIPT_DIR%

if not exist "%PROTOC%" (
    echo ERROR: protoc.exe not found!
    echo Please check the path: %PROTOC%
    echo Full path: %CD%\%PROTOC%
    pause
    exit /b 1
)

echo Listing proto files:
dir /b *.proto

rem 清理旧文件
if exist "%GEM_DIR%" rmdir /s /q "%GEM_DIR%"
if exist "%DEST_DIR%" rmdir /s /q "%DEST_DIR%"

rem 生成 C++ 代码
mkdir "%GEM_DIR%\cpp"
for %%f in (*.proto) do (
    echo Generating C++ for %%f
    "%PROTOC%" -I="%SRC_DIR%" --cpp_out="%GEM_DIR%\cpp" "%%f"
)

rem 复制到目标目录
mkdir "%DEST_DIR%"
copy "%GEM_DIR%\cpp\*" "%DEST_DIR%"

echo.
echo Cleaning temporary files...
if exist "%GEM_DIR%" (
    echo Removing %GEM_DIR%
    rmdir /s /q "%GEM_DIR%"
)

echo.
echo Done! Generated files are in %DEST_DIR%
dir /b "%DEST_DIR%"
pause

