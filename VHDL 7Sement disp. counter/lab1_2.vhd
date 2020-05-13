library IEEE;
use IEEE.std_logic_1164.all;
--use ieee.numeric_std.all;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity lab1_2 is
  port (
LED : out std_logic_vector(9 downto 0); -- Red LEDs
SW : in std_logic_vector(9 downto 0);    -- Switches end entity lab1;
hex0 : out std_logic_vector(6 downto 0);
clk50 : in std_logic;
reset_n : in std_logic;
ext_ena_n : in std_logic
);
end entity lab1_2;

architecture top_level of lab1_2 is

-- signal declarations
signal counter : std_logic_vector(3 downto 0);
signal state1 : std_logic;
signal state2 : std_logic;
signal buttonpress : std_logic;

begin

-- Insert statement to assign all switches to the LEDs end architecture top_level;



with counter select hex0 <=
	"1000000" when "0000", -- 0
	"1111001" when "0001", -- 1
	"0100100" when "0010", -- 2
	"0110000" when "0011", -- 3
	"0011001" when "0100", -- 4
	"0010010" when "0101", -- 5
	"0000010" when "0110", -- 6
	"1111000" when "0111", -- 7
	"0000000" when "1000", -- 8
	"0010000" when "1001", -- 9
	"0001000" when "1010", -- A
	"0000011" when "1011", -- b
	"1000110" when "1100", -- C
	"0100001" when "1101", -- d
	"0000110" when "1110", -- E
	"0001110" when "1111", -- F
	"1111111" when others; -- turn off all LEDs

edge_detect : process(clk50)
begin
if rising_edge(clk50) then
	state1 <= ext_ena_n;
	state2 <= state1;
end if;
end process edge_detect;

buttonpress <= not state1 and state2;

count : process(clk50)
begin
if rising_edge(clk50) then
	if reset_n = '0' then
		counter <= "0000";
	elsif buttonpress = '1' then
		counter <= counter + 1;
	end if;
end if;
end process count;
end architecture top_level;