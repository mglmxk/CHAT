#!/bin/bash
rm -rf build
cmake -B build -GNinja && ninja -C build
./build/CHATS