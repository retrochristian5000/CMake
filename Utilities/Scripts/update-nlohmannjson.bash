#!/usr/bin/env bash

set -e
set -x
shopt -s dotglob

readonly name="nlohmann_json"
readonly ownership="nlohmann_json Upstream <mail@nlohmann.me>"
readonly subtree="Utilities/cmnlohmann_json"
readonly repo="https://github.com/nlohmann/json.git"
readonly tag="v3.11.2"
readonly shortlog=false
readonly paths="
  include
  LICENSE.MIT
"

extract_source () {
    git_archive

    pushd "${extractdir}/${name}-reduced"
    echo "* -whitespace" >> .gitattributes
    # Convert headers to use relative #include lines.
    sed -i 's/^#include *<nlohmann\/\([^>]*\)>$/#include "\1"/'                 -- include/nlohmann/*.hpp
    sed -i 's/^#include *<nlohmann\/detail\/\([^>]*\)>$/#include "\1"/'         -- include/nlohmann/detail/*.hpp
    sed -i 's/^#include *<nlohmann\/detail\/\([^>]*\)>$/#include "..\/\1"/'     -- include/nlohmann/detail/*/*.hpp
    sed -i 's/^#include *<nlohmann\/detail\/\([^>]*\)>$/#include "..\/..\/\1"/' -- include/nlohmann/detail/*/*/*.hpp
    sed -i 's/^#include *<nlohmann\/\([^>]*\)>$/#include "..\/\1"/'             -- include/nlohmann/*/*.hpp
    sed -i 's/^#include *<nlohmann\/\([^>]*\)>$/#include "..\/..\/\1"/'         -- include/nlohmann/*/*/*.hpp
    sed -i 's/^#include *<nlohmann\/\([^>]*\)>$/#include "..\/..\/..\/\1"/'     -- include/nlohmann/*/*/*/*.hpp
    popd
}

. "${BASH_SOURCE%/*}/update-third-party.bash"
