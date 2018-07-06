############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2016 Xilinx, Inc. All Rights Reserved.
############################################################
open_project trabalhofinal
set_top doImgProc
add_files trabalhofinal/core.h
add_files trabalhofinal/core.cpp
add_files -tb trabalhofinal/test_core.cpp
open_solution "solution1"
set_part {xc7a35tcpg236-1} -tool vivado
create_clock -period 10 -name default
#source "./trabalhofinal/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -rtl verilog -format ip_catalog
