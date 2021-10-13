$erroractionpreference = "stop"

if ("$env:CMAKE_CONFIGURATION".Contains("borland5.5")) {
    $filename = "bcc5.5-1"
    $sha256sum = "895B76F8F1AD8030F31ACE239EBC623DC7379C121A540F55F611B93F3CB9AF52"
} elseif ("$env:CMAKE_CONFIGURATION".Contains("borland5.8")) {
    $filename = "bcc5.8-1"
    $sha256sum = "C30981BFD540C933E76D82D873DEE05E7482F34F68E309065DE0D181C95F77E3"
} else {
    throw ('unknown CMAKE_CONFIGURATION: ' + "$env:CMAKE_CONFIGURATION")
}
$tarball = "$filename.zip"

$outdir = $pwd.Path
$outdir = "$outdir\.gitlab"
$ProgressPreference = 'SilentlyContinue'
Invoke-WebRequest -Uri "https://cmake.org/files/dependencies/internal/$tarball" -OutFile "$outdir\$tarball"
$hash = Get-FileHash "$outdir\$tarball" -Algorithm SHA256
if ($hash.Hash -ne $sha256sum) {
    exit 1
}

Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::ExtractToDirectory("$outdir\$tarball", "$outdir")
Move-Item -Path "$outdir\$filename" -Destination "$outdir\bcc"

$tools = "bcc32", "ilink32"
foreach ($tool in $tools) {
    $cfg = Get-Content -path "$outdir\bcc\bin\$tool.cfg.in" -Raw
    $cfg = $cfg -replace "@BCC_ROOT@","$outdir\bcc"
    $cfg | Set-Content -path "$outdir\bcc\bin\$tool.cfg"
}
