//Copyright (C)2014-2023 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.9 (64-bit)
//Part Number: GW1NR-LV9QN88PC6/I5
//Device: GW1NR-9
//Device Version: C
//Created Time: Fri Jan  3 06:01:36 2025

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

    sys_clk your_instance_name(
        .clkout(clkout_o), //output clkout
        .clkoutd(clkoutd_o), //output clkoutd
        .clkoutd3(clkoutd3_o), //output clkoutd3
        .reset(reset_i), //input reset
        .reset_p(reset_p_i), //input reset_p
        .clkin(clkin_i) //input clkin
    );

//--------Copy end-------------------
