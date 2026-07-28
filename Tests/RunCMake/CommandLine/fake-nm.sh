#!/bin/sh

set -eu

if test "$#" -eq 4 &&
   test "$1" = "--no-weak" &&
   test "$2" = "--defined-only" &&
   test "$3" = "--format=posix"; then
  print_file_name=0
  input=$4
elif test "$#" -eq 5 &&
     test "$1" = "--no-weak" &&
     test "$2" = "--defined-only" &&
     test "$3" = "--format=posix" &&
     test "$4" = "--print-file-name"; then
  print_file_name=1
  input=$5
else
  echo "unexpected nm arguments: $*" >&2
  exit 1
fi

pending_symbol=0

finish_symbol()
{
  if test "${pending_symbol}" -eq 1; then
    printf ' 0 0\r\n'
    pending_symbol=0
  fi
}

start_symbol()
{
  finish_symbol
  if test "${print_file_name}" -eq 1; then
    printf '%s: %s T' "$1" "$2"
  else
    printf '%s T' "$2"
  fi
  pending_symbol=1
}

process_object()
{
  object=$1
  case "${object}" in
    \"*\")
      object=${object#\"}
      object=${object%\"}
      ;;
  esac

  case "${object}" in
    *single.obj)
      start_symbol "${object}" single
      ;;
    *first.obj)
      start_symbol "${object}" first
      ;;
    *no-symbols.obj)
      sleep 0.1
      printf '%s: no symbols\n' "${object}" >&2
      sleep 0.1
      finish_symbol
      ;;
    *second.obj)
      start_symbol "${object}" second
      ;;
    *spaced.obj)
      start_symbol "${object}" spaced
      ;;
    *mixed.obj)
      start_symbol "${object}" mixed
      ;;
    *)
      echo "unexpected nm input: ${object}" >&2
      exit 1
      ;;
  esac
}

case "${input}" in
  @*)
    response=${input#@}
    while IFS= read -r object || test -n "${object}"; do
      process_object "${object}"
    done < "${response}"
    ;;
  *)
    process_object "${input}"
    ;;
esac
finish_symbol
