# Build libauto-play.so with the Android NDK via qpm/cmake.
$ErrorActionPreference = "Stop"
qpm restore
if (-not (Test-Path "./build")) { New-Item -ItemType Directory -Path "./build" | Out-Null }
& cmake -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -B build -S .
& cmake --build ./build
Write-Host "Built build/libauto-play.so"
