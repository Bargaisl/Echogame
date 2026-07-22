$env:Path = "$env:TEMP\w64devkit\bin;$env:Path"
$gcc = "$env:TEMP\w64devkit\bin\gcc.exe"
$inc = "$env:TEMP\raylib\raylib-6.0_win64_mingw-w64\include"
$lib = "$env:TEMP\raylib\raylib-6.0_win64_mingw-w64\lib"
$upx = "$env:TEMP\upx\upx-4.2.4-win64\upx.exe"

Stop-Process -Name "echo" -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 1

& $gcc -std=c99 -Os -s -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-ident -fmerge-all-constants -ffunction-sections -fdata-sections main.c game.c world.c particles.c audio.c -o echo.exe "-I$inc" "-L$lib" -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lshell32 "-Wl,--gc-sections" "-Wl,--strip-all" "-Wl,--as-needed"

if (Test-Path $upx) {
    & $upx --best -f echo.exe
}

$file = Get-Item echo.exe
Write-Host "Build complete! File size: $($file.Length) bytes ($([math]::Round($file.Length/1KB, 2)) KB)"
