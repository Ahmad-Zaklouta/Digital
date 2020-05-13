
module nios2_system (
	clk_clk,
	i2c_wrapper_scl_ext_export,
	i2c_wrapper_sda_ext_export,
	interrupt_pio_ext_export,
	led_pio_ext_export,
	reset_reset_n,
	sw_pio_ext_export);	

	input		clk_clk;
	inout		i2c_wrapper_scl_ext_export;
	inout		i2c_wrapper_sda_ext_export;
	input	[3:0]	interrupt_pio_ext_export;
	output	[9:0]	led_pio_ext_export;
	input		reset_reset_n;
	input	[9:0]	sw_pio_ext_export;
endmodule
