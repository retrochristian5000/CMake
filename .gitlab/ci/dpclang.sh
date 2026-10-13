#!/bin/sh

set -e

baseurl="https://github.com/intel/llvm/releases/download/nightly-2026-10-13/$tarball"

case "$(uname -s)-$(uname -m)" in
    Linux-x86_64)
        tarball="sycl_linux.tar.gz"
        shatool="sha256sum"
        sha256sum="e7d12ce82f6cb1eeb79d30f12a6d0a06323fcb820f37f7d018fe202b440ce8c3"
        ;;
    *)
        echo "Unrecognized platform $(uname -s)-$(uname -m)"
        exit 1
        ;;
esac
readonly shatool
readonly sha256sum

cd .gitlab

echo "$sha256sum  $tarball" > dpclang.sha256sum
curl -OL "$baseurl/$tarball"
$shatool --check dpclang.sha256sum
mkdir dpclang
tar -C dpclang -xzf "$tarball"
rm "$tarball" dpclang.sha256sum
