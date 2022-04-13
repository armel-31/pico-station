#!/bin/bash
# Pico Tool for compilation and target deploy
#
# A. Surzur - Copyright (2022)

current_dir="$(pwd)"
export PICO_SDK_PATH=~/Coding/RaspberryPico/pico-sdk

clean_func() {
    echo "## Clean-up 'build' directory ##"
    cd $current_dir/build
    rm -rf *
    cd $current_dir
}

cmake_func() {
    cd $current_dir/build
    if [ "$type" = "release" ];then
        echo "## Create CMAKE configuration for 'RELEASE' ##"
        cmake ..
    elif [ "$type" = "debug" ];then
        echo "## Create CMAKE configuration for 'DEBUG' ##"
        cmake -DCMAKE_BUILD_TYPE=Debug ..
    else
        echo "## Create CMAKE configuration for 'RELEASE' ##"
        cmake ..
    fi
    cd $current_dir
}

build_func() {
    cmake_func
    cd $current_dir/build
    make
    cd $current_dir
}

load_func() {
    cd $current_dir
    # Run OpenOCD to load compiled ELF program on Pico target (SWD link)
    sudo openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program ./build/pico-sim7020e.elf verify reset exit"
}

reset_func() {
    cd $current_dir
    # Run OpenOCD to load compiled ELF program on Pico target (SWD link)
    sudo openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program ./build/pico-sim7020e.elf verify reset exit"
}


# Set default values
action="build"
type="release"

# Parse arguments sent to the tool
while [[ $# -gt 0 ]]; do
  key="$1"

  case $key in
    -h|--help)
      echo "-h --help   : print this help"
      echo "-a --action : select action to perform (clean | cmake | build | build_load | rebuild | load | all)"
      echo "-t --type   : select type of compilation (release | debug)"
      exit 0

      ;;
    -a|--action)
      action="$2"
      type="release"
      echo "# Selected action to perform : $action"
      shift
      shift
      ;;
    -t|--type)
      type="$2"
      echo "# Selected type of compilation : $type"
      shift
      shift
      ;;
    *)    # unknown option
      POSITIONAL+=("$1") # save it in an array for later
      shift # past argument
      ;;
  esac
done

if [ -z $action ];then
    echo "No argument parameter entered, selecting default parameters"
    action="build"
    type="release"
fi

if ! [[ -d $current_dir/build ]];then
    echo "No 'build' folder, I create it"
    mkdir build
fi

# Configuration / Clean-up
if [ "$action" = "clean" ];then
    clean_func
    echo "Project cleaned... exiting"
    exit 0
elif [ "$action" = "cmake" ];then
    cmake_func
    echo "CMAKE created for project"
    exit 0
elif [ "$action" = "build" ];then
    build_func
elif [ "$action" = "build_load" ];then
    build_func
    load_func
elif [ "$action" = "rebuild" ];then
    clean_func
    build_func
elif [ "$action" = "load" ];then
    load_func
elif [ "$action" = "all" ];then
    clean_func
    build_func
    load_func
fi
