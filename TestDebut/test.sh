#!/bin/sh

size=$1
for ((i = 1; i < size; i++))
do
#	open -a "Terminal"
	echo "cochonnerie "$i | nc   localhost 9034
done

