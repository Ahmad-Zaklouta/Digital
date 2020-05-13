
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_lab1_2 is
end;

architecture testbench of tb_lab1_2 is
	signal LED      : std_logic_vector(9 downto 0);
	signal SW        : std_logic_vector(9 downto 0);
	signal HEX0	     : std_logic_vector(6 downto 0);
	signal clk50 	 : std_logic;
	signal reset_n 	 : std_logic;
	signal ext_ena_n : std_logic;
	signal clk_ena 	 : boolean;
	constant clk_period : time := 20 ns; -- 50 MHz


	component lab1_2 is
		port (
			LED      : out std_logic_vector(9 downto 0);
			SW        : in std_logic_vector(9 downto 0);
			HEX0      : out std_logic_vector(6 downto 0);
			clk50     : in std_logic;
			reset_n   : in  std_logic;
			ext_ena_n : in  std_logic
			);
	end component lab1_2;

begin
  UUT : lab1_2
	port map(
	  LED      => LED,
      SW        => SW,
      HEX0      => HEX0,
      clk50     => clk50,
      reset_n   => reset_n,
      ext_ena_n => ext_ena_n
      );

  -- create a 50 MHz clock
  clk50 <= not clk50 after clk_period/2 when clk_ena else '0';

  stimuli_process : process
  begin
	--set default values
	clk_ena <= false;
	reset_n <= '1';
	ext_ena_n <= '1';
	SW        <= (others => '0');

	--enable clk and wait for 3 clk periods
	clk_ena <= true;
	wait for 3*clk_period;

	--assert reset_n for 3 clk periods
	reset_n <= '0';
	wait for 3*clk_period;

	--deassert reset_n for 3 clk periods
	reset_n <= '1';
	wait for 3*clk_period;

	--enable counter and wait for 20 clk_periods
	ext_ena_n <= '0';
	wait for 20*clk_period;

	--assert reset_n for 3 clk periods
	reset_n <= '0';
	wait for 3*clk_period;

	--deassert reset_n for 10 clk periods
	reset_n <= '1';
	wait for 3*clk_period;

	--disable clk
	clk_ena <= false;

	--end of simulation
	wait;
  end process stimuli_process;
end architecture testbench;
