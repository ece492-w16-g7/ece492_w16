library ieee;

use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

entity LED_detector is
	generic (
		IW	: INTEGER := 7
	);

    port (
        -- inputs
        clk                         : in STD_LOGIC;
        reset_n                     : in STD_LOGIC;

        -- From 2015G4 rgb_to_..._threshold.vhd
        stream_in_data              : in STD_LOGIC_VECTOR(IW downto 0);
        stream_in_startofpacket     : in STD_LOGIC;
        stream_in_endofpacket       : in STD_LOGIC;
        stream_in_valid             : in STD_LOGIC;
		  
		  stream_out_ready            : in STD_LOGIC;

        -- outputs        
        stream_out_data             : buffer STD_LOGIC_VECTOR(IW downto 0);
        stream_out_startofpacket    : buffer STD_LOGIC;
        stream_out_endofpacket      : buffer STD_LOGIC;
        stream_out_valid            : buffer STD_LOGIC;
		  
		  stream_in_ready             : buffer STD_LOGIC;

        position_ready_irq          : out STD_LOGIC;
        position                    : out STD_LOGIC_VECTOR(15 downto 0));

end entity;

architecture detector of LED_detector is
    type FRAME_STATE is (PROCESSING, END_FRAME, START_FRAME);

    signal current_state : FRAME_STATE := START_FRAME;

    signal channel_pixel : STD_LOGIC_VECTOR(7 downto 0);

    variable counter : UNSIGNED := x"0";

    variable consecutive_pixels : INTEGER := 0;
    variable biggest_blob_size : INTEGER := 0;
	 
	 variable threshold_passed : STD_LOGIC := '0';

    signal biggest_blob : STD_LOGIC_VECTOR(15 downto 0);

    constant consecutive_pixels_thresh : INTEGER := 5;
    constant amplification_multiplier : INTEGER := 3;
    constant binary_threshold_value : INTEGER := 250;

    constant frame_size_row : INTEGER := 480;
    constant frame_size_col : INTEGER := 640;
	 constant frame_pixel_size : INTEGER := 307200;

begin

    process (clk)
    begin
		if rising_edge(clk) then
        case current_state is
        
            when START_FRAME => 
                position_ready_irq <= '0';
                counter := x"0";
                current_state <= PROCESSING;

            when PROCESSING =>
				
					if (stream_in_valid = '1') and (stream_in_ready = '1') then 
                channel_pixel <= stream_in_data(15 downto 8);

                if (UNSIGNED(channel_pixel) > binary_threshold_value) then
                    consecutive_pixels := consecutive_pixels + 1;
						  threshold_passed := '1';
                elsif (consecutive_pixels > consecutive_pixels_thresh) then
                    if (consecutive_pixels > biggest_blob_size) then
                        biggest_blob_size := consecutive_pixels;
                        -- Counter will count pixel number.
                        biggest_blob <= STD_LOGIC_VECTOR(counter / frame_size_row) & STD_LOGIC_VECTOR(counter mod frame_size_col);
                    end if;
                else
                    consecutive_pixels := 0;
                end if;
                
                -- TODO: End frame if necessary.
                counter := counter + 1;
					 
					 if (threshold_passed = '1') then 
						stream_out_data <= x"11";
					 else 
						stream_out_data <= x"00";
					 end if;
					 
					 stream_out_valid <= stream_in_valid;
					 stream_in_ready <= stream_out_ready;
					 stream_out_startofpacket <= stream_in_startofpacket;
					 stream_out_endofpacket <= stream_in_endofpacket;
					 
					 threshold_passed := '0';
					 
					 if (counter = frame_pixel_size) then
						current_state <= END_FRAME;
					 end if;
					 
					end if;
					 
            when END_FRAME => 
                position <= biggest_blob;
                position_ready_irq <= '1';
                current_state <= START_FRAME;
					
			end case;
		  end if;

    end process;

end architecture;
