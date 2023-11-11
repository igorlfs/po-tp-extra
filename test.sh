#!/bin/bash

gcc main.c -o main

test -f "all-actual" && rm all-actual

for file in ./input/*; do
	if [[ -f "$file" ]]; then
		./main "$file" >>all-actual
	fi
done
