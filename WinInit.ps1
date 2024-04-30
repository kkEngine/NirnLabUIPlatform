param($Work)

# restart PowerShell with -noexit, the same script, and 1
if (!$Work) {
    powershell -noexit -file $MyInvocation.MyCommand.Path 1
    return
}

$buildDir = mkdir -Force build
Set-Location $buildDir

cmake .. -DVCPKG_ROOT="F:\vcpkg"
