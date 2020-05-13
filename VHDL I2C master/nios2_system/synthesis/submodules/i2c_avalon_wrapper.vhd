library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity i2c_avalon_wrapper is
  port (clock      : in    std_logic;
        resetn     : in    std_logic;
        read       : in    std_logic;
        write      : in    std_logic;
        chipselect : in    std_logic;
        address    : in    std_logic_vector(1 downto 0);
        writedata  : in    std_logic_vector(31 downto 0);
		readdata   : out   std_logic_vector(31 downto 0);
        sda        : inout std_logic;
        scl        : inout std_logic);
end i2c_avalon_wrapper;
architecture Structure of i2c_avalon_wrapper is


  signal ctrl_reg : std_logic_vector(31 downto 0);
  signal addr_reg : std_logic_vector(31 downto 0);
  signal data_reg : std_logic_vector(31 downto 0);

  signal write_reg : std_logic_vector(15 downto 0);
  signal read_reg  : std_logic_vector(15 downto 0);

  alias cmd           : std_logic is ctrl_reg(0);
  alias rnw           : std_logic is ctrl_reg(1);
  alias nobytes       : std_logic_vector is ctrl_reg(3 downto 2);
  alias ack_error     : std_logic is ctrl_reg(4);
  alias busy          : std_logic is ctrl_reg(5);
  alias wrapper_busy  : std_logic is ctrl_reg(6);
  alias wrapper_error : std_logic is ctrl_reg(7);
  alias wrapper_rst   : std_logic is ctrl_reg(8);

  signal wrapper_busy_state : std_logic;  --intermediate signaling of busy
                                          --for state machine. registered to wrapper_busy(ctrl_reg)
  signal cmd_rst            : std_logic;  --Clear cmd bit in ctrl_reg when command has been
  --accepted.

  alias i2c_pointer     : std_logic_vector is addr_reg(15 downto 8);
  alias i2c_device_addr : std_logic_vector is addr_reg(6 downto 0);
  --signal addr_reg    : std_logic_vector(7 downto 0);


  --i2c master signals
  signal i2c_valid     : std_logic;
  signal i2c_addr      : std_logic_vector(6 downto 0);
  signal i2c_rnw       : std_logic;
  signal i2c_data_wr   : std_logic_vector(7 downto 0);
  signal i2c_busy      : std_logic;
  signal i2c_data_rd   : std_logic_vector(7 downto 0);
  signal i2c_ack_error : std_logic;



  constant GC_SYSTEM_CLK : integer := 50_000_000;
  constant GC_I2C_CLK    : integer := 200_000;




--wrapper state machine
  type statetype is (sIDLE, sADDR, sWRITE_DATA, sWAIT_DATA, sWAIT_STOP);
  signal state : statetype;

  component i2c_master is
    generic (
      GC_SYSTEM_CLK : integer;
      GC_I2C_CLK    : integer);
    port (
      clk       : in    std_logic;
      rst_n  : in    std_logic;
      valid     : in    std_logic;
      addr      : in    std_logic_vector(6 downto 0);
      rnw       : in    std_logic;
      data_wr   : in    std_logic_vector(7 downto 0);
      busy      : out   std_logic;
      data_rd   : out   std_logic_vector(7 downto 0);
      ack_error : out   std_logic;
      sda       : inout std_logic;
      scl       : inout std_logic);
  end component i2c_master;


begin


  data_reg(15 downto 0)  <= write_reg;
  data_reg(31 downto 16) <= read_reg;

  i2c_master_inst : i2c_master
    generic map (
      GC_SYSTEM_CLK => GC_SYSTEM_CLK,
      GC_I2C_CLK    => GC_I2C_CLK)
    port map (
      clk       => clock,
      rst_n  => resetn,
      valid     => i2c_valid,
      addr      => i2c_addr,
      rnw       => i2c_rnw,
      data_wr   => i2c_data_wr,
      busy      => i2c_busy,
      data_rd   => i2c_data_rd,
      ack_error => i2c_ack_error,
      sda       => sda,
      scl       => scl
      );

  --general register interface
  process(clock)
  begin
    if rising_edge(clock) then
      busy         <= i2c_busy;
      ack_error    <= i2c_ack_error;
      wrapper_busy <= wrapper_busy_state;  --update from state machine

      if wrapper_rst = '1' then
        ctrl_reg  <= (others => '0');
        addr_reg  <= (others => '0');
        write_reg <= (others => '0');
        readdata  <= (others => '0');
      else
        if nobytes = "11" then
          wrapper_error <= '1';
        end if;
        if cmd_rst = '1' then
          ctrl_reg(0) <= '0';
        end if;
        if chipselect = '1' and write = '1' then
          case address is
            when "00" =>
              ctrl_reg(3 downto 0) <= writedata(3 downto 0);
            when "01" =>
              addr_reg <= writedata;
            when "10" =>
              write_reg <= writedata(15 downto 0);
            when others =>
              null;
          end case;
        elsif chipselect = '1' and read = '1' then
          case address is
            when "00" =>
              readdata <= ctrl_reg;
            when "01" =>
              readdata <= addr_reg;
            when "10" =>
              readdata <= data_reg;
            when others =>
              readdata <= data_reg;
          end case;
        end if;
      end if;

	  if resetn = '0' then
	    ctrl_reg      <= (others => '0');
        addr_reg      <= (others => '0');
        write_reg     <= (others => '0');
        readdata      <= (others => '0');
        ack_error     <= '0';
        wrapper_error <= '0';
	  end if;

    end if;
  end process;


--wrapper state machine

  process(clock)
    variable byte_count : integer range 0 to 3 := 2;
  begin
    if rising_edge(clock) then
      wrapper_busy_state <= '1';
      cmd_rst            <= '0';
      case state is
        when sIDLE =>
          byte_count         := 2;
          i2c_valid          <= '0';
          i2c_rnw            <= '0';
          i2c_data_wr        <= X"00";
          i2c_addr           <= "0000000";
          wrapper_busy_state <= '0';
          if cmd = '1' then
            state      <= sADDR;
            cmd_Rst    <= '1';
            byte_count := to_integer(unsigned(nobytes));
          end if;
-------------------------------------------------------------------------------
-- sADDR activate for writing address
-------------------------------------------------------------------------------
        when sADDR =>
          if wrapper_error = '1' then
            state <= sIDLE;
          else
            i2c_data_wr <= i2c_pointer;
            i2c_valid   <= '1';
            i2c_addr    <= i2c_device_addr;
            i2c_rnw     <= rnw;
            if busy = '0' and i2c_busy = '1' then  --wait for busy to be pulled
              state <= sWAIT_DATA;
            end if;
          end if;


-------------------------------------------------------------------------------
-- sWAIT_DATA
-------------------------------------------------------------------------------
        when sWAIT_DATA =>
          if busy = '1' and i2c_busy = '0' then  -- wait for busy to be
                                        -- pulled low sACK2 or sMACK
            if rnw = '0' then
              if byte_count = 0 then
                state <= sWAIT_STOP;
              else
                state <= sWRITE_DATA;
              end if;
            else
              read_reg((8*byte_count)-1 downto 8*(byte_count-1)) <= i2c_data_rd;  --sample first data
              byte_count                                         := byte_count - 1;
              if byte_count = 0 then
                state <= sWAIT_STOP;
              end if;
            end if;
          end if;


-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
        when sWRITE_DATA =>
          i2c_data_wr <= write_reg((8*byte_count)-1 downto 8*(byte_count-1));
          if busy = '0' and i2c_busy = '1' then  --busy just pulled high,cmd
            state      <= sWAIT_DATA;
            byte_count := byte_count - 1;
          end if;


-------------------------------------------------------------------------------
--
-------------------------------------------------------------------------------
        when sWAIT_STOP =>
          --end of transaction
          i2c_valid <= '0';
          --wait for stop state to complete.
          if (busy = '1' and i2c_busy = '0') then  --busy just pulled low, stop
                                                   --completed
            state <= sIDLE;
          end if;
        when others =>
          state <= sIDLE;

      end case;

	  if resetn = '0' then
		  state              <= sIDLE;
		  i2c_rnw            <= '0';
		  i2c_data_wr        <= X"00";
		  i2c_valid          <= '0';
		  i2c_addr           <= "0000000";
		  byte_count         := 2;
		  read_reg           <= (others => '0');
		  --wrapper ctrl signals
		  cmd_rst            <= '0';
		  wrapper_busy_state <= '0';
	  end if;
	end if;
  end process;

end Structure;