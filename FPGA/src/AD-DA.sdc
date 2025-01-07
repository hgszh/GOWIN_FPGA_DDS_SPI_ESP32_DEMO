//Copyright (C)2014-2025 GOWIN Semiconductor Corporation.
//All rights reserved.
//File Title: Timing Constraints file
//Tool Version: V1.9.11 (64-bit) 
//Created Time: 2025-01-03 17:27:36
create_clock -name xtal_clk -period 37.037 -waveform {0 18.518} [get_ports {xtal_27MHz}]
create_clock -name spi_sclk -period 20 -waveform {0 5} [get_nets {spi_sclk}]
create_clock -name clk_216MHz -period 4.63 -waveform {0 2.315} [get_nets {clk_216MHz}]
create_clock -name clk_2MHz -period 500 -waveform {0 166.667} [get_nets {clk_2MHz}]
create_clock -name clk_72MHz -period 13.889 -waveform {0 6.944} [get_nets {clk_72MHz}]
set_clock_groups -asynchronous -group [get_clocks {clk_216MHz clk_2MHz clk_72MHz}] -group [get_clocks {spi_sclk}]
set_clock_groups -asynchronous -group [get_clocks {clk_2MHz}] -group [get_clocks {clk_72MHz clk_216MHz}]
set_clock_groups -asynchronous -group [get_clocks {clk_72MHz}] -group [get_clocks {clk_2MHz clk_216MHz}]
set_clock_groups -asynchronous -group [get_clocks {clk_216MHz}] -group [get_clocks {clk_72MHz clk_2MHz}]
