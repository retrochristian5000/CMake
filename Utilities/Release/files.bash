#!/usr/bin/env bash

set -e

usage='usage: hash.bash [<options>] [--]

Options:

    --version <ver>            CMake <major>.<minor> version number to push.
                               Defaults to version of source tree.
'

die() {
    echo "$@" 1>&2; exit 1
}

readonly cmake_source_dir="${BASH_SOURCE%/*}/../.."

cmake_version_component()
{
  sed -n "
/^set(CMake_VERSION_${1}/ {s/set(CMake_VERSION_${1} *\([0-9]*\))/\1/;p;}
" "${cmake_source_dir}/Source/CMakeVersion.cmake"
}


version=''
while test "$#" != 0; do
    case "$1" in
    --version) shift; version="$1" ;;
    --) shift ; break ;;
    -*) die "$usage" ;;
    *) break ;;
    esac
    shift
done
test "$#" = 0 || die "$usage"

if test -z "$version"; then
    cmake_version_major="$(cmake_version_component MAJOR)"
    cmake_version_minor="$(cmake_version_component MINOR)"
    cmake_version_patch="$(cmake_version_component PATCH)"
    cmake_version_rc="$(cmake_version_component RC)"
    version="${cmake_version_major}.${cmake_version_minor}.${cmake_version_patch}"
    if test -n "$cmake_version_rc"; then
      version="$version-rc$cmake_version_rc"
    fi
fi
readonly version

readonly files_v1_in="${BASH_SOURCE%/*}/files-v1.json.in"
sed "s/@version@/$version/g" "$files_v1_in" \
  | jq . \
  > "cmake-$version-files-v1.json"

readonly algos='
  256
'
for algo in $algos; do
  shasum -a $algo \
    "cmake-$version-files-v1.json" \
    $(jq -r '.files[].name' "cmake-$version-files-v1.json") \
  | LC_ALL=C sort -k 2 \
  > "cmake-$version-SHA-$algo.txt"
done
