	component nios2_system is
		port (
			clk_clk                    : in    std_logic                    := 'X';             -- clk
			i2c_wrapper_scl_ext_export : inout std_logic                    := 'X';             -- export
			i2c_wrapper_sda_ext_export : inout std_logic                    := 'X';             -- export
			interrupt_pio_ext_export   : in    std_logic_vector(3 downto 0) := (others => 'X'); -- export
			led_pio_ext_export         : out   std_logic_vector(9 downto 0);                    -- export
			reset_reset_n              : in    std_logic                    := 'X';             -- reset_n
			sw_pio_ext_export          : in    std_logic_vector(9 downto 0) := (others => 'X')  -- export
		);
	end component nios2_system;

	u0 : component nios2_system
		port map (
			clk_clk                    => CONNECTED_TO_clk_clk,                    --                 clk.clk
			i2c_wrapper_scl_ext_export => CONNECTED_TO_i2c_wrapper_scl_ext_export, -- i2c_wrapper_scl_ext.export
			i2c_wrapper_sda_ext_export => CONNECTED_TO_i2c_wrapper_sda_ext_export, -- i2c_wrapper_sda_ext.export
			interrupt_pio_ext_export   => CONNECTED_TO_interrupt_pio_ext_export,   --   interrupt_pio_ext.export
			led_pio_ext_export         => CONNECTED_TO_led_pio_ext_export,         --         led_pio_ext.export
			reset_reset_n              => CONNECTED_TO_reset_reset_n,              --               reset.reset_n
			sw_pio_ext_export          => CONNECTED_TO_sw_pio_ext_export           --          sw_pio_ext.export
		);

