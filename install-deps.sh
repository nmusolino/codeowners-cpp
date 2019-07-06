#!/bin/bash

set -x 

case "$TRAVIS_OS_NAME" in
    osx)
	export HOMEBREW_NO_AUTO_UPDATE=1
	export HOMEBREW_NO_INSTALL_CLEANUP=1
	brew install cmake || brew upgrade cmake
	# Pin the dependents of boost, to avoid a cascade of upgrades
	# when we upgrade boost.
	brew pin cgal sfcgal postgis
	brew install boost || brew upgrade boost
	;;
    linux)
	echo "install-deps.sh: no-op for Linux" 
	;;
    *)
	echo "install-deps.sh: Unknown TRAVIS_OS_NAME: $TRAVIS_OS_NAME"
	exit 1
esac
