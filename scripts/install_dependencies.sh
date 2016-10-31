#!/bin/sh

################################################################
#
#  Awesome Robot Visualizer Dependency Installer
# -----------------------------------------------
#
#   This script installs the dependencies necessary to
# build and run ARVisualizer. It should work without issues
# on Ubuntu-based platforms.
#
# Usage:
#   install_dependencies [all]
#
#   all: If specified, will install all dependencies for all
#        components of the project (including samples, etc.)
#
#        If this script is run without arguments, it will
#        install only the dependencies necessary to build
#        the library library.
#
################################################################

MIN_DEPS='libglm-dev python unzip cmake'
ALL_DEPS='libglm-dev python unzip cmake python-pip libopencv-dev libclang-dev'

GRN='\033[0;32m'
RED='\033[0;31m'
BLU='\033[1;36m'
YEL='\033[1;33m'
END='\033[0m'

fail () {
    echo ${RED}'\t'FAILED!
    echo '\t'$1
    echo ${END}
    exit 1
}

if [ $# -eq 1 ] && [ $1 = "all" ]; then
        SETUP_ALL=1
        echo ${YEL}Installing ALL dependencies!${END}'\n'
else
    SETUP_ALL=0
    echo ${YEL}Installing only minimum required dependencies${END}'\n'
fi

echo ${BLU}Installing packaged dependencies...
echo       ===================================${END}
if [ ${SETUP_ALL} -eq 1 ]; then
    sudo apt-get install ${ALL_DEPS}
    sudo pip install cldoc
else
    sudo apt-get install ${MIN_DEPS}
fi

if [ $? -eq 0 ]; then
    echo ${GRN}'\t'OK'\n'${END}
else
    fail "Failed to get necessary libraries"
fi

cur_dir=$(pwd)
temp_dir=`mktemp -d` && cd $temp_dir

echo ${BLU}Downloading GLFW source...
echo       ==========================${END}
wget https://github.com/glfw/glfw/releases/download/3.1.2/glfw-3.1.2.zip

if [ $? -eq 0 ]; then
    echo ${GRN}'\t'OK'\n'${END}
else
    fail "Failed to download GLFW"
fi

echo ${BLU}Unzipping...
echo       ============${END}
unzip glfw-3.1.2.zip

if [ $? -eq 0 ]; then
    echo ${GRN}'\t'OK'\n'${END}
else
    fail "Failed to unzip GLFW"
fi

echo ${BLU}Building...
echo       ===========${END}
mkdir glfw.bin
cd glfw.bin

cmake ../glfw-3.1.2

if [ $? -eq 0 ]; then
    echo ${GRN}'\t'OK'\n'${END}
else
    fail "CMAKE Failed!"
fi

make

if [ $? -eq 0 ]; then
    echo ${GRN}'\t'OK'\n'${END}
else
    fail "Failed to compile GLFW!"
fi

echo ${BLU}Installing GLFW...
echo       ==================${END}
sudo make install

if [ $? -eq 0 ]; then
    echo ${GRN}'\t'OK'\n'${END}
else
    fail "Failed to install GLFW!"
fi

cd ${cur_dir}
rm -rf ${temp_dir}

echo ${GRN}All Done :D${END}