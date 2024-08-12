param($Work)

# restart PowerShell with -noexit, the same script, and 1
if (!$Work) {
    powershell -noexit -file $MyInvocation.MyCommand.Path 1
    return
}

$buildDir = mkdir -Force build
Set-Location $buildDir

cmake .. -DVCPKG_ROOT="F:\vcpkg" -DOUTPUT_PATH="F:/Steam/steamapps/common/Skyrim Special Edition_NirnLabUIPlatform"
cmake --build . --config Release --parallel 4
