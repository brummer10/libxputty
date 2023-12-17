#!/bin/bash

# setup a sub-shell, source the selected PawPaw environment and run make in it

# check if we found the PawPawBuilds directory
PawPawBuilds=$(locate -l 1 PawPawBuilds)
if [ -z "$PawPawBuilds" ]
then
    tput setaf 1
    echo  'Could find PawPaw build directory!'
    tput setaf 3
    echo 'please clone PawPaw from'
    tput setaf 4
    echo '    https://github.com/DISTRHO/PawPaw.git'
    tput setaf 3
    echo 'and run ./bootstrap-plugins.sh win64
and ./bootstrap-plugins.sh linux
on it in order to use this script'
    tput sgr0
    exit 1
fi

# Check if we found local.env in the PawPaw directory
PawPaw=$(locate -l 1 PawPaw/local.env)
if [ -z "$PawPaw" ]
then
    echo -e "Could find PawPaw!\nplease clone it from https://github.com/DISTRHO/PawPaw.git \nand run ./bootstrap-plugins.sh win64 \nand ./bootstrap-plugins.sh linux \non it in order to use this script"
    exit 1
fi

# save current working directory
CURDIR=$PWD

# grab out the PawPaw directory
PAWPAW=$(dirname $PawPaw)

# build for MSWindows
function windows() {
    export EXTRAQUIET=1
    make clean && cd $PAWPAW && tput setaf 4 && source local.env win64 && \
    tput sgr0 && unset EXTRAQUIET && cd $CURDIR && make
}

# build for Linux
function linux() {
    export EXTRAQUIET=1
    # skip LTO support on Linux (produce a bunch of warnings for cairo static lib)
    export PAWPAW_SKIP_LTO=1
    make clean && cd $PAWPAW && tput setaf 4 && source local.env Linux && \
    tput sgr0 && unset EXTRAQUIET && cd $CURDIR && make
}

# check for given command-line option
case "$1" in
	linux)
		linux 
		;;
	windows)
		windows 
		;;
	*)
		printf "Usage:\n\t $0 {linux|windows} \n"
		exit 1
esac

exit 0
