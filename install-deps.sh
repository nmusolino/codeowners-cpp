#!/bin/bash

set -x 

case "$TRAVIS_OS_NAME" in
    osx)
	brew install cmake || brew upgrade cmake
	# Pin the dependents of boost, to avoid a cascade of upgrades
	# when we upgrade boost.
	brew pin cgal sfcgal postgis
	brew install boost || brew upgrade boost
	;;
    linux)
	echo "install-deps.sh: dependency installation not implemented for Linux" 
	exit 1
	;;
    *)
	echo "install-deps.sh: Unknown TRAVIS_OS_NAME: $TRAVIS_OS_NAME"
	exit 1
esac
