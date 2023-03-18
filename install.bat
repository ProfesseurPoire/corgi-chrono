@setlocal enableextensions
@cd /d "%~dp0"

set project_name=corgi-chrono

rmdir /s /q "C:\Program Files\%project_name%"

for %%i in (x64) do (

    setlocal

    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" %%i
    
    for %%j in (Debug Release) do (
        setlocal
        mkdir build\build%%i\%%j
        cd build\build%%i\%%j
        cmake -DCMAKE_BUILD_TYPE=%%j -DCMAKE_INSTALL_PREFIX="C:\\Program Files\\%project_name%" -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -G "Visual Studio 16 2019" -A x64 ..\..\..
        msbuild INSTALL.vcxproj /property:Configuration=%%j    
        endlocal
    )
    endlocal
)