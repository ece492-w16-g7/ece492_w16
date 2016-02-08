-- niosII_system_Scaler_1_tb.vhd


library IEEE;
library altera;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use altera.alt_cusp121_package.all;

entity niosII_system_Scaler_1_tb is
end entity niosII_system_Scaler_1_tb;

architecture rtl of niosII_system_Scaler_1_tb is
	component alt_cusp121_clock_reset is
		port (
			clock : out std_logic;
			reset : out std_logic
		);
	end component alt_cusp121_clock_reset;

	component niosII_system_Scaler_1 is
		generic (
			CONTROL_DEPTH            : integer := 29;
			PARAMETERISATION         : string  := "<scalerParams><SCL_NAME>scaler</SCL_NAME><SCL_RUNTIME_CONTROL>0</SCL_RUNTIME_CONTROL><SCL_IN_WIDTH>1024</SCL_IN_WIDTH><SCL_IN_HEIGHT>768</SCL_IN_HEIGHT><SCL_OUT_WIDTH>640</SCL_OUT_WIDTH><SCL_OUT_HEIGHT>480</SCL_OUT_HEIGHT><SCL_BPS>8</SCL_BPS><SCL_CHANNELS_IN_SEQ>3</SCL_CHANNELS_IN_SEQ><SCL_CHANNELS_IN_PAR>1</SCL_CHANNELS_IN_PAR><SCL_ALGORITHM><NAME>POLYPHASE</NAME><V><TAPS>4</TAPS><PHASES>16</PHASES></V><H><TAPS>4</TAPS><PHASES>16</PHASES></H></SCL_ALGORITHM><SCL_PRECISION><V><SIGNED>true</SIGNED><INTEGER_BITS>1</INTEGER_BITS><FRACTION_BITS>7</FRACTION_BITS></V><H><SIGNED>true</SIGNED><INTEGER_BITS>1</INTEGER_BITS><FRACTION_BITS>7</FRACTION_BITS><KERNEL_BITS>9</KERNEL_BITS></H></SCL_PRECISION><SCL_COEFFICIENTS><LOAD_AT_RUNTIME>false</LOAD_AT_RUNTIME><ARE_IDENTICAL>0</ARE_IDENTICAL><V><BANKS>2</BANKS><FUNCTION>LANCZOS_2</FUNCTION><SYMMETRIC>0</SYMMETRIC></V><H><BANKS>2</BANKS><FUNCTION>LANCZOS_2</FUNCTION><SYMMETRIC>0</SYMMETRIC></H></SCL_COEFFICIENTS></scalerParams>";
			AUTO_DEVICE_FAMILY       : string  := "";
			AUTO_CONTROL_CLOCKS_SAME : string  := "0"
		);
		port (
			clock              : in  std_logic                    := 'X';
			din_data           : in  std_logic_vector(7 downto 0) := (others => 'X');
			din_endofpacket    : in  std_logic                    := 'X';
			din_ready          : out std_logic;
			din_startofpacket  : in  std_logic                    := 'X';
			din_valid          : in  std_logic                    := 'X';
			dout_data          : out std_logic_vector(7 downto 0);
			dout_endofpacket   : out std_logic;
			dout_ready         : in  std_logic                    := 'X';
			dout_startofpacket : out std_logic;
			dout_valid         : out std_logic;
			reset              : in  std_logic                    := 'X'
		);
	end component niosII_system_Scaler_1;

	signal dut_din_ready     : std_logic;                    -- dut:din_ready -> din_tester:data
	signal din_tester_q      : std_logic_vector(0 downto 0); -- din_tester:q -> dut:din_valid
	signal builtin_1_w1_q    : std_logic_vector(0 downto 0); -- ["1", builtin_1_w1:q, "1"] -> [din_tester:ena, dut:dout_ready]
	signal clocksource_clock : std_logic;                    -- clocksource:clock -> [dut:clock, din_tester:clock]
	signal clocksource_reset : std_logic;                    -- clocksource:reset -> din_tester:reset

begin

	builtin_1_w1_q <= "1";

	clocksource : component alt_cusp121_clock_reset
		port map (
			clock => clocksource_clock, -- clock.clk
			reset => clocksource_reset  --      .reset
		);

	dut : component niosII_system_Scaler_1
		generic map (
			CONTROL_DEPTH            => 29,
			PARAMETERISATION         => "<scalerParams><SCL_NAME>scaler</SCL_NAME><SCL_RUNTIME_CONTROL>0</SCL_RUNTIME_CONTROL><SCL_IN_WIDTH>320</SCL_IN_WIDTH><SCL_IN_HEIGHT>240</SCL_IN_HEIGHT><SCL_OUT_WIDTH>640</SCL_OUT_WIDTH><SCL_OUT_HEIGHT>480</SCL_OUT_HEIGHT><SCL_BPS>8</SCL_BPS><SCL_CHANNELS_IN_SEQ>3</SCL_CHANNELS_IN_SEQ><SCL_CHANNELS_IN_PAR>1</SCL_CHANNELS_IN_PAR><SCL_ALGORITHM><NAME>POLYPHASE</NAME><V><TAPS>4</TAPS><PHASES>16</PHASES></V><H><TAPS>4</TAPS><PHASES>16</PHASES></H></SCL_ALGORITHM><SCL_PRECISION><V><SIGNED>true</SIGNED><INTEGER_BITS>1</INTEGER_BITS><FRACTION_BITS>7</FRACTION_BITS></V><H><SIGNED>true</SIGNED><INTEGER_BITS>1</INTEGER_BITS><FRACTION_BITS>7</FRACTION_BITS><KERNEL_BITS>9</KERNEL_BITS></H></SCL_PRECISION><SCL_COEFFICIENTS><LOAD_AT_RUNTIME>false</LOAD_AT_RUNTIME><ARE_IDENTICAL>0</ARE_IDENTICAL><V><BANKS>2</BANKS><FUNCTION>LANCZOS_2</FUNCTION><SYMMETRIC>0</SYMMETRIC><FILENAME /></V><H><BANKS>2</BANKS><FUNCTION>LANCZOS_2</FUNCTION><SYMMETRIC>0</SYMMETRIC><FILENAME /></H></SCL_COEFFICIENTS></scalerParams>",
			AUTO_DEVICE_FAMILY       => "Cyclone II",
			AUTO_CONTROL_CLOCKS_SAME => "0"
		)
		port map (
			clock              => clocksource_clock, -- clock.clk
			reset              => open,              -- reset.reset
			din_ready          => dut_din_ready,     --   din.ready
			din_valid          => din_tester_q(0),   --      .valid
			din_data           => open,              --      .data
			din_startofpacket  => open,              --      .startofpacket
			din_endofpacket    => open,              --      .endofpacket
			dout_ready         => '1',               --  dout.ready
			dout_valid         => open,              --      .valid
			dout_data          => open,              --      .data
			dout_startofpacket => open,              --      .startofpacket
			dout_endofpacket   => open               --      .endofpacket
		);

	din_tester : process (clocksource_clock, clocksource_reset)
	begin
		if clocksource_reset = '1' then
			din_tester_q(0) <= '0';
		elsif clocksource_clock'EVENT and clocksource_clock = '1' and builtin_1_w1_q(0) = '1' then
			din_tester_q(0) <= dut_din_ready;
		end if;
	end process;

end architecture rtl; -- of niosII_system_Scaler_1_tb
