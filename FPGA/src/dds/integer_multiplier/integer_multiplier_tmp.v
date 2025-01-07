//Copyright (C)2014-2023 Gowin Semiconductor Corporation.
//All rights reserved.
//File Title: Template file for instantiation
//Tool Version: V1.9.9
//Part Number: GW1NR-LV9QN88PC6/I5
//Device: GW1NR-9
//Device Version: C
//Created Time: Thu Oct 10 03:04:56 2024

//Change the instance name and port connections to the signal names
//--------Copy here to design--------

	DDS_Ampl_Multiplier your_instance_name(
		.mul_a(mul_a_i), //input [15:0] mul_a
		.mul_b(mul_b_i), //input [11:0] mul_b
		.product(product_o) //output [27:0] product
	);

//--------Copy end-------------------
