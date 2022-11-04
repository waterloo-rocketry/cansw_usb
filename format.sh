#!/usr/bin/env bash

set -e

usage()
{
  echo "Usage: $0 [-h] [-a/--all]"
  echo
  echo "arguments:"
  echo " -a, --all     format all files in the repo"
  echo " -h, --help    show this help message and exit"
  exit 2
}

if ! type astyle &> /dev/null
then
    echo "Error: astyle is not installed or is not on the PATH."
    echo "Please install astyle to use this tool."
    exit 1
fi

WORKSPACE_DIR=$(git rev-parse --show-toplevel)
cd "$WORKSPACE_DIR"

if [ "$#" -eq 0 ] ; then
    ALL_FILES=$({ git ls-files -o --exclude-standard && git diff --diff-filter=d --name-only master; })
elif [ "$1" = "-a" ] || [ "$1" = "--all" ] ; then
    ALL_FILES=$({ git ls-files -cmo --exclude-standard; })
else
    usage
fi

FILES=$(echo "$ALL_FILES" | grep -v 'mcc_generated_files' | grep -E '\.c$|\.h$|\.cpp$|\.hpp$|\.cc$|\.hh$|\.ino$')

if [ ! -z "$FILES" ]
then
    astyle --project=.astylerc -n $FILES
fi
