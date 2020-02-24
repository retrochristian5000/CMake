#!/usr/bin/env bash

set -e
set -x
shopt -s dotglob

readonly name="fmt"
readonly ownership="fmt upstream <kwrobot@kitware.com>"
readonly subtree="Utilities/cmfmt"
readonly repo="https://github.com/fmtlib/fmt.git"
readonly tag="6.1.2"
readonly shortlog=false
readonly paths="
  LICENSE.rst
  README.rst
  include/fmt/*.h
  src/*.cc
"

extract_source () {
    git_archive
    pushd "${extractdir}/${name}-reduced"
    echo "* -whitespace" > .gitattributes
    popd
}

. "${BASH_SOURCE%/*}/update-third-party.bash"
