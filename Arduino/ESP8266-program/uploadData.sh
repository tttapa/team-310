#!/bin/bash
cd data
dir=*
for file in $dir
do
    echo "$file"
    curl -F "file=@$file" "hal-9310.local/edit.html"
done
