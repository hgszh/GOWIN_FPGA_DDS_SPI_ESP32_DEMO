module gw_gao(
    \data_recv[39] ,
    \data_recv[38] ,
    \data_recv[37] ,
    \data_recv[36] ,
    \data_recv[35] ,
    \data_recv[34] ,
    \data_recv[33] ,
    \data_recv[32] ,
    \data_recv[31] ,
    \data_recv[30] ,
    \data_recv[29] ,
    \data_recv[28] ,
    \data_recv[27] ,
    \data_recv[26] ,
    \data_recv[25] ,
    \data_recv[24] ,
    \data_recv[23] ,
    \data_recv[22] ,
    \data_recv[21] ,
    \data_recv[20] ,
    \data_recv[19] ,
    \data_recv[18] ,
    \data_recv[17] ,
    \data_recv[16] ,
    \data_recv[15] ,
    \data_recv[14] ,
    \data_recv[13] ,
    \data_recv[12] ,
    \data_recv[11] ,
    \data_recv[10] ,
    \data_recv[9] ,
    \data_recv[8] ,
    \data_recv[7] ,
    \data_recv[6] ,
    \data_recv[5] ,
    \data_recv[4] ,
    \data_recv[3] ,
    \data_recv[2] ,
    \data_recv[1] ,
    \data_recv[0] ,
    spi_cs,
    \data_send[39] ,
    tms_pad_i,
    tck_pad_i,
    tdi_pad_i,
    tdo_pad_o
);

input \data_recv[39] ;
input \data_recv[38] ;
input \data_recv[37] ;
input \data_recv[36] ;
input \data_recv[35] ;
input \data_recv[34] ;
input \data_recv[33] ;
input \data_recv[32] ;
input \data_recv[31] ;
input \data_recv[30] ;
input \data_recv[29] ;
input \data_recv[28] ;
input \data_recv[27] ;
input \data_recv[26] ;
input \data_recv[25] ;
input \data_recv[24] ;
input \data_recv[23] ;
input \data_recv[22] ;
input \data_recv[21] ;
input \data_recv[20] ;
input \data_recv[19] ;
input \data_recv[18] ;
input \data_recv[17] ;
input \data_recv[16] ;
input \data_recv[15] ;
input \data_recv[14] ;
input \data_recv[13] ;
input \data_recv[12] ;
input \data_recv[11] ;
input \data_recv[10] ;
input \data_recv[9] ;
input \data_recv[8] ;
input \data_recv[7] ;
input \data_recv[6] ;
input \data_recv[5] ;
input \data_recv[4] ;
input \data_recv[3] ;
input \data_recv[2] ;
input \data_recv[1] ;
input \data_recv[0] ;
input spi_cs;
input \data_send[39] ;
input tms_pad_i;
input tck_pad_i;
input tdi_pad_i;
output tdo_pad_o;

wire \data_recv[39] ;
wire \data_recv[38] ;
wire \data_recv[37] ;
wire \data_recv[36] ;
wire \data_recv[35] ;
wire \data_recv[34] ;
wire \data_recv[33] ;
wire \data_recv[32] ;
wire \data_recv[31] ;
wire \data_recv[30] ;
wire \data_recv[29] ;
wire \data_recv[28] ;
wire \data_recv[27] ;
wire \data_recv[26] ;
wire \data_recv[25] ;
wire \data_recv[24] ;
wire \data_recv[23] ;
wire \data_recv[22] ;
wire \data_recv[21] ;
wire \data_recv[20] ;
wire \data_recv[19] ;
wire \data_recv[18] ;
wire \data_recv[17] ;
wire \data_recv[16] ;
wire \data_recv[15] ;
wire \data_recv[14] ;
wire \data_recv[13] ;
wire \data_recv[12] ;
wire \data_recv[11] ;
wire \data_recv[10] ;
wire \data_recv[9] ;
wire \data_recv[8] ;
wire \data_recv[7] ;
wire \data_recv[6] ;
wire \data_recv[5] ;
wire \data_recv[4] ;
wire \data_recv[3] ;
wire \data_recv[2] ;
wire \data_recv[1] ;
wire \data_recv[0] ;
wire spi_cs;
wire \data_send[39] ;
wire tms_pad_i;
wire tck_pad_i;
wire tdi_pad_i;
wire tdo_pad_o;
wire tms_i_c;
wire tck_i_c;
wire tdi_i_c;
wire tdo_o_c;
wire [9:0] control0;
wire gao_jtag_tck;
wire gao_jtag_reset;
wire run_test_idle_er1;
wire run_test_idle_er2;
wire shift_dr_capture_dr;
wire update_dr;
wire pause_dr;
wire enable_er1;
wire enable_er2;
wire gao_jtag_tdi;
wire tdo_er1;

IBUF tms_ibuf (
    .I(tms_pad_i),
    .O(tms_i_c)
);

IBUF tck_ibuf (
    .I(tck_pad_i),
    .O(tck_i_c)
);

IBUF tdi_ibuf (
    .I(tdi_pad_i),
    .O(tdi_i_c)
);

OBUF tdo_obuf (
    .I(tdo_o_c),
    .O(tdo_pad_o)
);

GW_JTAG  u_gw_jtag(
    .tms_pad_i(tms_i_c),
    .tck_pad_i(tck_i_c),
    .tdi_pad_i(tdi_i_c),
    .tdo_pad_o(tdo_o_c),
    .tck_o(gao_jtag_tck),
    .test_logic_reset_o(gao_jtag_reset),
    .run_test_idle_er1_o(run_test_idle_er1),
    .run_test_idle_er2_o(run_test_idle_er2),
    .shift_dr_capture_dr_o(shift_dr_capture_dr),
    .update_dr_o(update_dr),
    .pause_dr_o(pause_dr),
    .enable_er1_o(enable_er1),
    .enable_er2_o(enable_er2),
    .tdi_o(gao_jtag_tdi),
    .tdo_er1_i(tdo_er1),
    .tdo_er2_i(1'b0)
);

gw_con_top  u_icon_top(
    .tck_i(gao_jtag_tck),
    .tdi_i(gao_jtag_tdi),
    .tdo_o(tdo_er1),
    .rst_i(gao_jtag_reset),
    .control0(control0[9:0]),
    .enable_i(enable_er1),
    .shift_dr_capture_dr_i(shift_dr_capture_dr),
    .update_dr_i(update_dr)
);

ao_top_0  u_la0_top(
    .control(control0[9:0]),
    .trig0_i(spi_cs),
    .data_i({\data_recv[39] ,\data_recv[38] ,\data_recv[37] ,\data_recv[36] ,\data_recv[35] ,\data_recv[34] ,\data_recv[33] ,\data_recv[32] ,\data_recv[31] ,\data_recv[30] ,\data_recv[29] ,\data_recv[28] ,\data_recv[27] ,\data_recv[26] ,\data_recv[25] ,\data_recv[24] ,\data_recv[23] ,\data_recv[22] ,\data_recv[21] ,\data_recv[20] ,\data_recv[19] ,\data_recv[18] ,\data_recv[17] ,\data_recv[16] ,\data_recv[15] ,\data_recv[14] ,\data_recv[13] ,\data_recv[12] ,\data_recv[11] ,\data_recv[10] ,\data_recv[9] ,\data_recv[8] ,\data_recv[7] ,\data_recv[6] ,\data_recv[5] ,\data_recv[4] ,\data_recv[3] ,\data_recv[2] ,\data_recv[1] ,\data_recv[0] }),
    .clk_i(\data_send[39] )
);

endmodule
