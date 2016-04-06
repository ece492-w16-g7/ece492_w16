LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_misc.all;
use IEEE.NUMERIC_STD.all;


ENTITY LED_detector IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	IW		:INTEGER									:= 23;
	OW		:INTEGER									:= 23;
	
	EIW	:INTEGER									:= 1;
	EOW	:INTEGER									:= 1
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (

	-- Inputs
	clk								:IN		STD_LOGIC;
	reset								:IN		STD_LOGIC;

	stream_in_data					:IN		STD_LOGIC_VECTOR(IW DOWNTO  0);	
	stream_in_startofpacket		:IN		STD_LOGIC;
	stream_in_endofpacket		:IN		STD_LOGIC;
	stream_in_empty				:IN		STD_LOGIC_VECTOR(EIW DOWNTO  0);	
	stream_in_valid				:IN		STD_LOGIC;

	stream_out_ready				:IN		STD_LOGIC;
	
	--threshold_sw               :IN      STD_LOGIC_VECTOR(7 DOWNTO 0);

	-- Bidirectional

	-- Outputs
	stream_in_ready				:BUFFER	STD_LOGIC;

	stream_out_data				:BUFFER	STD_LOGIC_VECTOR(OW DOWNTO  0);	
	stream_out_startofpacket	:BUFFER	STD_LOGIC;
	stream_out_endofpacket		:BUFFER	STD_LOGIC;
	stream_out_empty				:BUFFER	STD_LOGIC_VECTOR(EOW DOWNTO  0);	
	stream_out_valid				:BUFFER	STD_LOGIC;
	
	position_irq               :OUT     STD_LOGIC;
	m_position                 :OUT     STD_LOGIC_VECTOR(9 DOWNTO 0);
	n_position                 :OUT     STD_LOGIC_VECTOR(9 DOWNTO 0)
);

END LED_detector;

ARCHITECTURE detector OF LED_detector IS

	TYPE FRAME_STATE IS (PROCESSING, IDLE);

	SIGNAL current_state : FRAME_STATE := IDLE;
	SIGNAL y_component   : STD_LOGIC_VECTOR(7 DOWNTO 0);
	
	VARIABLE m_counter          : INTEGER := 0;
	VARIABLE n_counter          : INTEGER := 0;
	VARIABLE consecutive_pixels : INTEGER := 0;
	VARIABLE max_consecutive    : INTEGER := 0;
	VARIABLE led_detected       : STD_LOGIC := '0';
	
	SIGNAL m_stored : STD_LOGIC_VECTOR(9 DOWNTO 0);
	SIGNAL n_stored : STD_LOGIC_VECTOR(9 DOWNTO 0);
	
	CONSTANT frame_size_row     : INTEGER := 640;
	--CONSTANT frame_size_column  : INTEGER := 240;
	CONSTANT threshold_value    : INTEGER := 250;
	CONSTANT consecutive_thresh : INTEGER := 5;

BEGIN
	
	y_component <= stream_in_data(7 DOWNTO 0);
	
	stream_out_data <= stream_in_data;
	stream_out_startofpacket <= stream_in_startofpacket;
	stream_out_endofpacket <= stream_in_endofpacket;
	stream_out_empty <= stream_in_empty;
	stream_out_valid <= stream_in_valid;
	
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			CASE current_state IS	
				WHEN PROCESSING =>
					IF (UNSIGNED(y_component) > threshold_value) THEN
						consecutive_pixels := consecutive_pixels + 1;
					ELSIF (consecutive_pixels > consecutive_thresh) THEN
						led_detected := '1';
						IF (consecutive_pixels > max_consecutive) THEN
							max_consecutive := consecutive_pixels;
							m_stored <= m_counter;
							n_stored <= n_counter - STD_LOGIC_VECTOR(consecutive_pixels/ 2);
						END IF;
					ELSE
						consecutive_pixels := 0;
					END IF;
					
					n_counter := n_counter +1;
					IF (n_counter = frame_size_row) THEN
						n_counter := 0;
						m_counter := m_counter +1;
					END IF;
					
				WHEN IDLE =>
					NULL;
			END CASE;
		END IF;
	END PROCESS;
	
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (stream_in_startofpacket = '1') THEN
				position_irq <= '0';
				m_counter := 0;
				n_counter := 0;
				consecutive_pixels := 0;
				max_consecutive := 0;
				led_detected <= '0';
				current_state <= PROCESSING;
			ELSIF (stream_in_endofpacket = '1') AND (led_detected = '1') THEN
				position_irq <= '1';
				m_position <= m_stored;
				n_position <= n_stored;
				current_state <= IDLE;
			ELSIF (stream_in_valid = '1') THEN 
				current_state <= PROCESSING;
			ELSE
				current_state <= IDLE;
			END IF;
		END IF;
	END PROCESS;
END detector;