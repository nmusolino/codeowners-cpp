#!/bin/bash

set -x 

case "$TRAVIS_OS_NAME" in
    osx)
	export HOMEBREW_NO_INSTALL_CLEANUP=1
	brew install cmake || brew upgrade cmake
	# Pin the dependents of boost, to avoid a cascade of upgrades
	# when we upgrade boost.
	brew pin cgal sfcgal postgis
	brew install boost || brew upgrade boost
	;;
    linux)
	which python
	python --version	
	which pip
	pip --version
	sudo apt purge cmake
	pip install cmake
	;;
    *)
	echo "install-deps.sh: Unknown TRAVIS_OS_NAME: $TRAVIS_OS_NAME"
	exit 1
esac

which cmake
cmake --version
