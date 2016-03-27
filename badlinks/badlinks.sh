#!/usr/bin/env bash
files=$(find -L $1 -type l -mtime 7)
for i in $files; do
echo $i
done
