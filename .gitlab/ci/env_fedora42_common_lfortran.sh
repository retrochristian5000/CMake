export FC=/usr/bin/lfortran

# FIXME(lfortran): lfortran races during compiler id
# See https://github.com/lfortran/lfortran/issues/8294
export CTEST_MAX_PARALLELISM=1
