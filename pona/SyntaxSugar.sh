#!/bin/bash

MAX_ARGS=32

let n=2
while [ $n -le $MAX_ARGS ]; do
	let m=$n-1
	let p=$n-2
	printf "inline static NODE CHOICE("
	let i=0
	while [ $i -le $p ]; do
		printf "NODE choice$i, "
		let i=i+1
	done
	printf "NODE choice$m) {\n"
	printf "\treturn"
	let i=0
	while [ $i -le $p ]; do
		printf " OR(choice$i,"
		let i=i+1
	done
	printf " choice$m"
	let i=0
	while [ $i -le $p ]; do
		printf ")"
		let i=i+1
	done
	printf ";\n"
	printf "}\n"
	let n=n+1
done

let n=2
while [ $n -le $MAX_ARGS ]; do
	let m=$n-1
	let p=$n-2
	printf "inline static NODE GLUE("
	let i=0
	while [ $i -le $p ]; do
		printf "NODE node$i, "
		let i=i+1
	done
	printf "NODE node$m) {\n"
	let i=0
	while [ $i -le $p ]; do
		let j=i+1
		printf "	node$i->next_ = node$j;\n"
		let i=j
	done
	printf "	return node0;\n"
	printf "}\n"
	let n=n+1
done
