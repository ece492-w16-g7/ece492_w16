library ieee;

use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

entity LED_detector is
    type FRAME_TYPE is (CALIBRATION, PROCESSING, END_FRAME);

    port (
        -- inputs
        clk : in STD_LOGIC;
        reset_n : in STD_LOGIC;

        -- From 2015G4 rgb_to_..._threshold.vhd
        stream_in_data : in STD_LOGIC_VECTOR(23 downto 0);

        -- outputs
        position_output_ready : out STD_LOGIC;
        position : out STD_LOGIC_VECTOR(15 downto 0));

end entity;

architecture detector of LED_detector is
    signal current_state : FRAME_TYPE := CALIBRATION;

    signal channel_pixel : STD_LOGIC_VECTOR(7 downto 0);
    signal baseline_pixel : STD_LOGIC_VECTOR(7 downto 0);

    variable counter : INTEGER := 0;

    -- Only need 10 calibration frames.
    constant n_calibration_frames : INTEGER := 10;

    variable consecutive_pixels : INTEGER := 0;
    variable biggest_blob_size : INTEGER := 0;

    signal biggest_blob : STD_LOGIC_VECTOR(15 downto 0);

    constant consecutive_pixels_thresh : INTEGER := 5;
    constant amplification_multiplier : INTEGER := 3;
    constant binary_threshold_value : INTEGER := 250;

    constant frame_size_row : INTEGER := 480;
    constant frame_size_col : INTEGER := 640;

begin

    process (clk)
    begin
        case current_state is

            when CALIBRATION =>
                if (frame_counter = n_calibration_frames) then
                    current_state <= PROCESSING;
                else
                    -- TODO: Do the below for every pixel in current frame.
                    -- TODO: Load the current baseline pixel to signal.
                    channel_pixel <= stream_in_data(15 downto 8);
                    baseline_pixel <= STD_LOGIC_VECTOR((UNSIGNED(baseline_pixel) + UNSIGNED(channel_pixel))) / 2)
                    -- TODO: Save new pixel value to memory.
                    -- TODO: End frame if necessary.
                end if;

            when PROCESSING =>
                -- TODO: Do the following for all pixels in current frame.
                -- TODO: Load the baseline pixel to signal.
                channel_pixel <= stream_in_data(15 downto 8);
                channel_pixel <= STD_LOGIC_VECTOR(UNSIGNED(channel_pixel) - UNSIGNED(baseline_pixel));
                channel_pixel <= STD_LOGIC_VECTOR(UNSIGNED(channel_pixel) * amplification_multiplier);

                if (UNSIGNED(channel_pixel) > binary_threshold_value) then
                    consecutive_pixels = consecutive_pixels + 1;
                else if (consecutive_pixels > consecutive_pixels_thresh) then
                    if (consecutive_pixels > biggest_blob_size) then
                        biggest_blob_size <= consecutive_pixels;
                        -- Counter will count pixel number.
                        biggest_blob <= STD_LOGIC_VECTOR(counter / frame_size_row) & STD_LOGIC_VECTOR(counter mod frame_size_col);
                    end if
                else
                    consecutive_pixels <= 0;
                end if

                -- TODO: End frame if necessary.

            when END_FRAME => ;

        end case;

    end process;

end architecture;
