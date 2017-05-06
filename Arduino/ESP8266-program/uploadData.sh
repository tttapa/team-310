#!/bin/bash
cd data
dir=*
for file in $dir
do
    echo "$file"
    curl -F "file=@$file" "3.1.0.1/edit.html"
done
