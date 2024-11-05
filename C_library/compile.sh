#!/bin/bash

export PICO_SDK_PATH="/usr/share/pico-sdk"

mkdir build/

cd build/
cmake .. -Wno-dev
make all
