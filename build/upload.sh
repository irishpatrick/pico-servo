#!/bin/bash

sudo openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -c "program examples/$1.elf verify reset exit"
