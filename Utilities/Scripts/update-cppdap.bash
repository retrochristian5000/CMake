#!/usr/bin/env bash

set -e
set -x
shopt -s dotglob

readonly name="cppdap"
readonly ownership="cppdap Upstream <kwrobot@kitware.com>"
readonly subtree="Utilities/cmcppdap"
readonly repo="https://github.com/google/cppdap.git"
readonly tag="a4bc9773434f045d28a17c27396f469b63fb4f6a"
readonly shortlog=false
readonly paths="
  LICENSE
  include
  src
"

extract_source () {
    git_archive

    pushd "${extractdir}/${name}-reduced"
    echo "* -whitespace" > .gitattributes
    fromdos LICENSE include/dap/* src/*
    echo "" >> LICENSE
    echo "" >> src/nlohmann_json_serializer.h
    popd
}

. "${BASH_SOURCE%/*}/update-third-party.bash"
