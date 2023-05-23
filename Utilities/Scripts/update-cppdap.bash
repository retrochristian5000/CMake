#!/usr/bin/env bash

set -e
set -x
shopt -s dotglob

readonly name="cppdap"
readonly ownership="cppdap Upstream <kwrobot@kitware.com>"
readonly subtree="Utilities/cmcppdap"
readonly repo="https://github.com/google/cppdap.git"
readonly tag="1344b56dc5b6de2d18ad44ff764dba11f3bc59a0" # 2023-05-23
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
