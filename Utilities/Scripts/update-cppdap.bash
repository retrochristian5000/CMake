#!/usr/bin/env bash

set -e
set -x
shopt -s dotglob

readonly name="cppdap"
readonly ownership="cppdap Upstream <kwrobot@kitware.com>"
readonly subtree="Utilities/cmcppdap"
readonly repo="https://github.com/bradking/cppdap.git"
readonly tag="6bccb27e58a215be8be869c6d3bd6a9d09954354" # 2023-05-23 + xlclang fix
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
