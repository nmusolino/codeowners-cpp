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
	# Remove/install cmake.  Pip-installed version will be accessible
	# as /usr/local/bin/cmake
	sudo apt-get purge cmake
	sudo apt-get remove -y cmake
	sudo pip install --upgrade cmake
	PATH=/usr/local/bin:$PATH which cmake
	PATH=/usr/local/bin:$PATH cmake --version

	# Create symlinks so that new versions of gcc and g++ will take
	# priority in PATH.
	sudo ln -s /usr/bin/gcc-7 /usr/local/bin/gcc
	sudo ln -s /usr/bin/g++-7 /usr/local/bin/g++
	;;
    *)
	echo "install-deps.sh: Unknown TRAVIS_OS_NAME: $TRAVIS_OS_NAME"
	exit 1
esac

for program in make cmake $CC $CXX; do
    which $program
    $program --version
done

echo "Done installing dependencies."
exit 0
