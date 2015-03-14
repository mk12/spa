#!/bin/bash

# This script compiles the project, producing a 'spa' executable. Use the -t
# option (or --test) to compile the test binary instead, and use -d (or --debug)
# to compile for debugging.

name=$(basename "$0")
usage="usage: $name [-h] [-t] [-d]"

# Compiler and common options.
CC=clang++
options='-std=c++11 -Weverything -pedantic -Wno-padded -Wno-c++98-compat'
dist_opts='-DNDEBUG -Oz'
debug_opts='-g'

# Directions and file names.
src_dir='src'
tst_dir='tests'
bin_dir='dist'
target='spa'
tst_target='test'
main_file="$src_dir/spa.cpp"

# Search for source files.
src_files=$(find $src_dir -type f -name *.cpp -not -name spa.cpp)
tst_files=$(find $tst_dir -type f -name *.cpp)

compile() {
	mkdir -p $bin_dir
	$CC $options $1 -lreadline -o $bin_dir/$target $main_file $src_files
}

compile_tests() {
	mkdir -p $bin_dir
	$CC $options -o $bin_dir/$tst_target $src_files $tst_files
}

error_msg() {
	echo "$usage"
	exit 1
}

if (( $# == 0 )); then
	compile $dist_opts
elif (( $# == 1 )); then
	if [[ $1 == '-h' || $1 == '--help' ]]; then
		echo "$usage"
	elif [[ $1 == '-t' || $1 == '--test' ]]; then
		compile_tests
	elif [[ $1 == '-d' || $1 == '--debug' ]]; then
		compile $debug_opts
	else
		error_msg
	fi
else
	error_msg
fi
