#!/bin/bash

case "$TRAVIS_OS_NAME" in
    osx)
        export HOMEBREW_NO_INSTALL_CLEANUP=1
        brew install cmake || brew upgrade cmake
        # Pin the dependents of boost, to avoid a cascade of upgrades
        # when we upgrade boost.
        brew pin cgal sfcgal postgis
        brew install boost || brew upgrade boost
        case "$TRAVIS_COMPILER" in
            gcc)
                brew install gcc-7
                export CC=gcc-7 CXX=g++-7
            ;;
            *)
                echo "No compiler installation needed for: $TRAVIS_COMPILER"
        esac
        ;;
    linux)
        # Remove/install cmake.  Pip-installed version will be accessible
        # as /usr/local/bin/cmake
        sudo apt-get purge cmake
        sudo apt-get remove -y cmake
        sudo pip install --upgrade cmake
        PATH=/usr/local/bin:$PATH which cmake
        PATH=/usr/local/bin:$PATH cmake --version
        ;;
    *)
        echo "install-deps.sh: Unknown TRAVIS_OS_NAME: $TRAVIS_OS_NAME"
        exit 1
esac

for program in make cmake $CC $CXX; do
    echo "$program:" $(which $program)
    $program --version
done

echo "Done installing dependencies."
