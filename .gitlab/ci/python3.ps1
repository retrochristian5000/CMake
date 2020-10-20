$erroractionpreference = "stop"

$version = "3.8.6"
$sha256sum = "8168D0CE0B81D95939CAFB9C14CE8D181951AC63A84E8927C6A9157BB10B3368"
$filename = "python-$version-embed-amd64"
$tarball = "$filename.zip"

$outdir = $pwd.Path
$outdir = "$outdir\.gitlab"
Invoke-WebRequest -Uri "https://www.python.org/ftp/python/$version/$tarball" -OutFile "$outdir\$tarball"
$hash = Get-FileHash "$outdir\$tarball" -Algorithm SHA256
if ($hash.Hash -ne $sha256sum) {
    exit 1
}

Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::ExtractToDirectory("$outdir\$tarball", "$outdir\python3")
Remove-Item -Path "$outdir\python3\python38._pth"
Remove-Item -Path "$outdir\$tarball"
