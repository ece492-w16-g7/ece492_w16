library ieee;

use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

entity LED_detector is
	generic (
		-- Incoming width
		IW	: INTEGER := 7
	);

    port (
        -- inputs
        clk                         : in STD_LOGIC;
        reset_n                     : in STD_LOGIC;

        -- From 2015G4 rgb_to_..._threshold.vhd
        -- Also look at the streaming specs on eClass
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
	-- Because it's a stream of pixels, we need to track start,
	-- end, and middle frames
    type FRAME_STATE is (PROCESSING, END_FRAME, START_FRAME);

    signal current_state : FRAME_STATE := START_FRAME;

    -- Obtained pixel from incoming stream
    signal channel_pixel : STD_LOGIC_VECTOR(7 downto 0);

    -- Count incoming pixel #s 
    variable counter : UNSIGNED := x"0";

    -- Count consecutive white pixels to detect blobs 
    -- that may be the LED
    variable consecutive_pixels : INTEGER := 0;
    variable biggest_blob_size : INTEGER := 0;
	 
	-- Simple boolean-like variable 
	variable threshold_passed : STD_LOGIC := '0';

	-- Keep track of the (x,y) location of the biggest
	-- blob found so far
    signal biggest_blob : STD_LOGIC_VECTOR(15 downto 0);

    -- Constants
    constant consecutive_pixels_thresh : INTEGER := 5;
    constant amplification_multiplier : INTEGER := 3;
    constant binary_threshold_value : INTEGER := 250;

    constant frame_size_row : INTEGER := 320;
    constant frame_size_col : INTEGER := 240;
	constant frame_pixel_size : INTEGER := 76800;

begin

    process (clk)
    begin
		if rising_edge(clk) then
	        case current_state is
	        	
	        	-- Starting pixel of a frame means we need to
	        	-- deassert the irq and reset pixel counter
	            when START_FRAME => 
	                position_ready_irq <= '0';
	                counter := x"0";
	                current_state <= PROCESSING;

	            when PROCESSING =>
					
					-- A lot of this is similar to G4 from 2015
					if (stream_in_valid = '1') and (stream_in_ready = '1') then 
	                	-- Store in channel_pixel so we can modify
	                	channel_pixel <= stream_in_data;

	                	-- If it passes threshold, we can increase consecutive 
	                	-- pixels
		                if (UNSIGNED(channel_pixel) > binary_threshold_value) then
		                    consecutive_pixels := consecutive_pixels + 1;
							threshold_passed := '1';
						-- If the read pixel is now a black pixel, the chain of 
						-- white pixels has been broken and we can check to see if
						-- the found blob is bigger than what we have previously 
						-- found
		                elsif (consecutive_pixels > consecutive_pixels_thresh) then
		                	-- Blob found is biggest for this frame so we update
		                	-- the biggest blob
		                    if (consecutive_pixels > biggest_blob_size) then
		                        biggest_blob_size := consecutive_pixels;
		                        
		                        -- Counter will count pixel number.
		                        -- This is just a way to extract the (x,y) 
		                        -- coordinate information from the pixel counter
		                        biggest_blob <= STD_LOGIC_VECTOR(counter / frame_size_row) & STD_LOGIC_VECTOR(counter mod frame_size_col);
		                    end if;
		                else
		                	-- If the consecutive pixels found doesn't even 
		                	-- break the threshold, we can just reset the 
		                	-- consecutive_pixels and continue
		                    consecutive_pixels := 0;
		                end if;
		                
		                -- Increase pixel counter
		                counter := counter + 1;
							 
						-- Sending out pure black or white stream based on
						-- if the threshold passed
						if (threshold_passed = '1') then 
							stream_out_data <= x"11";
						else 
							stream_out_data <= x"00";
						end if;
						
						-- Stream_out signals set
						stream_out_valid <= stream_in_valid;
						stream_in_ready <= stream_out_ready;
						stream_out_startofpacket <= stream_in_startofpacket;
						stream_out_endofpacket <= stream_in_endofpacket;
						 
						-- Reset variable to 0
						threshold_passed := '0';
						 
						-- Check if frame should end 
						if (counter = frame_pixel_size) then
							current_state <= END_FRAME;
						end if;
						 
					end if;
						 
				-- Last pixel in the frame means we can now send out
				-- biggest_blob found as the position. We also set the
				-- irq so the software knows position is ready.
	            when END_FRAME => 
	                position <= biggest_blob;
	                position_ready_irq <= '1';
	                current_state <= START_FRAME;
						
			end case;
	  	end if;

    end process;

end architecture;
