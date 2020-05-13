library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity system_top is
  generic(
	GC_SYSTEM_CLK   	: integer := 50000000;
	GC_I2C_CLK 			: integer  := 2_000_000); 

port(
	clk       					:	in		STD_LOGIC;   	   					-- system clock
	rst_n						:	in 		std_logic;							-- synchronous active low reset(key0)
	sw							:	in 		std_logic_vector(9 downto 0);		-- DE1-SoC toggle switches (intput data)
	keys 						:	in		std_logic_vector(2 downto 0);		-- DE1-SoC push buttons (key3,key2,key1)
	led							:	out 	std_logic_vector(9 downto 0);		-- DE1-SoC LEDs (output data)
	alert						:	in		std_logic;
	sda							:	inout 	std_logic;							-- bidirectional serial I2C data
	scl	 						:	inout 	std_logic);							-- bidirectional serial I2C clock

end system_top;

architecture system_top_arc of system_top is

signal synced_keys, keys_tmp	:	std_logic_vector(2 downto 0);
signal synced_alert, alert_tmp	:	std_logic;
signal synced_sw, sw_tmp		:	std_logic_vector(9 downto 0);

component nios2_system is
	port(
			clk_clk                 		 : in  std_logic                    	:= 'X';             -- clk
			reset_reset_n            		 : in  std_logic                   		:= 'X';             -- reset_n
			led_pio_ext_export       		 : out std_logic_vector(9 downto 0);                    -- export
			sw_pio_ext_export        		 : in  std_logic_vector(9 downto 0) 	:= (others => 'X'); -- export
			interrupt_pio_ext_export 		 : in  std_logic_vector(3 downto 0) 	:= (others => 'X');  -- export
			i2c_wrapper_sda_ext_export 		 : inout std_logic                    	:= 'X';             -- export
			i2c_wrapper_scl_ext_export 		 : inout std_logic                    	:= 'X'              -- export                                  -- readra
		);
end component nios2_system;


begin

cpu : component nios2_system
		port map (
			clk_clk                  => clk,            	  			--               clk.clk
			reset_reset_n            => rst_n,            				--             reset.reset_n
			led_pio_ext_export       => led,       						--      		 led_pio_ext.export
			sw_pio_ext_export        => synced_sw,        						--        sw_pio_ext.export
			interrupt_pio_ext_export => synced_alert & synced_keys,  	-- interrupt_pio_ext.export
			i2c_wrapper_sda_ext_export => sda, 							-- i2c_wrapper_sda_ext.export
			i2c_wrapper_scl_ext_export => scl  						    -- i2c_wrapper_scl_ext.export          
		);	

		
key_alert_sync: process(clk)

begin
if rising_edge(clk) then
	keys_tmp <= keys;
	synced_keys <= keys_tmp;
	alert_tmp <= alert;
	synced_alert <= alert_tmp;
	sw_tmp <= sw;
	synced_sw <= sw_tmp;
end if;

end process key_alert_sync;
end system_top_arc;