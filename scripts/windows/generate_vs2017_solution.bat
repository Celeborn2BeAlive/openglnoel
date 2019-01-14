@set SOURCE_DIR=%~dp0\..\..
@set BUILD_DIR=%SOURCE_DIR%\..\openglnoel-build
@if not exist %BUILD_DIR% @mkdir %BUILD_DIR%
@pushd %BUILD_DIR%
cmake -G "Visual Studio 15 2017 Win64" -DASSIMP_BUILD_TESTS=0 %SOURCE_DIR%

pause