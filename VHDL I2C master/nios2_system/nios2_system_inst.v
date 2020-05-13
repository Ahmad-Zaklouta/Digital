	nios2_system u0 (
		.clk_clk                    (<connected-to-clk_clk>),                    //                 clk.clk
		.i2c_wrapper_scl_ext_export (<connected-to-i2c_wrapper_scl_ext_export>), // i2c_wrapper_scl_ext.export
		.i2c_wrapper_sda_ext_export (<connected-to-i2c_wrapper_sda_ext_export>), // i2c_wrapper_sda_ext.export
		.interrupt_pio_ext_export   (<connected-to-interrupt_pio_ext_export>),   //   interrupt_pio_ext.export
		.led_pio_ext_export         (<connected-to-led_pio_ext_export>),         //         led_pio_ext.export
		.reset_reset_n              (<connected-to-reset_reset_n>),              //               reset.reset_n
		.sw_pio_ext_export          (<connected-to-sw_pio_ext_export>)           //          sw_pio_ext.export
	);

