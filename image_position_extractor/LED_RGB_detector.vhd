--/**
-- * ECE492 - Group 7 - Winter 2016
-- *
-- * Description: This is the LED detector that thresholds the incoming
-- *                frame so that the LED's position can be found.       
-- * Author: Patrick Kuczera
-- * Date: Apr 5, 2016
-- *
-- */

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_misc.all;

ENTITY LED_detector IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	IDW	:INTEGER									:= 23; -- Incoming frame's data width
	ODW	:INTEGER									:= 23 -- Outcoming frame's data width
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (

	-- Inputs
	clk                          :IN    STD_LOGIC;
	reset                        :IN		STD_LOGIC;

	stream_in_data					  :IN		STD_LOGIC_VECTOR(IDW DOWNTO  0);	
	stream_in_startofpacket		  :IN		STD_LOGIC;
	stream_in_endofpacket		  :IN		STD_LOGIC;	
	stream_in_valid				  :IN		STD_LOGIC;
	stream_out_ready				  :IN		STD_LOGIC;

	threshold_active             :IN   STD_LOGIC;
	threshold_range              :IN   STD_LOGIC_VECTOR(9 DOWNTO 0);

	-- Bidirectional

	-- Outputs
	stream_in_ready				     :BUFFER	STD_LOGIC;
	stream_out_data				     :BUFFER	STD_LOGIC_VECTOR(ODW DOWNTO  0);	
	stream_out_startofpacket	     :BUFFER	STD_LOGIC;
	stream_out_endofpacket	        :BUFFER	STD_LOGIC;	
	stream_out_valid				     :BUFFER	STD_LOGIC;

	-- MM Slave
	avs_readdata_centroid           :OUT   STD_LOGIC_VECTOR(31 DOWNTO 0);
	avs_read                        :IN    STD_LOGIC;
	avs_address                     :IN    STD_LOGIC_VECTOR(2 DOWNTO 0);
	avs_chipselect                  :IN    STD_LOGIC;
	irq                             :OUT   STD_LOGIC

);

END LED_detector;

ARCHITECTURE Behaviour OF LED_detector IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
	SIGNAL	transfer_data				    :STD_LOGIC;
	SIGNAL	converted_data				    :STD_LOGIC_VECTOR(ODW DOWNTO  0);	
	SIGNAL	converted_startofpacket	    :STD_LOGIC;
	SIGNAL	converted_endofpacket	    :STD_LOGIC;	
	SIGNAL	converted_valid			    :STD_LOGIC;
	
	-- Internal Registers
	SIGNAL	data							    :STD_LOGIC_VECTOR(IDW DOWNTO  0);	
	SIGNAL	startofpacket				    :STD_LOGIC;
	SIGNAL	endofpacket					    :STD_LOGIC;	
	SIGNAL	valid							    :STD_LOGIC;
	SIGNAL   threshold                   :STD_LOGIC;
	
	SIGNAL	binary_data   					 :STD_LOGIC_VECTOR(IDW DOWNTO  0);
	
	SIGNAL   m_reg                       :STD_LOGIC_VECTOR(9 DOWNTO 0);
	SIGNAL   n_reg                       :STD_LOGIC_VECTOR(9 DOWNTO 0);
	SIGNAL   m_counter                   :STD_LOGIC_VECTOR(9 DOWNTO 0);
	SIGNAL   n_counter                   :STD_LOGIC_VECTOR(9 DOWNTO 0);
	SIGNAL   length_current              :STD_LOGIC_VECTOR(9 DOWNTO 0);
	SIGNAL   length_max                  :STD_LOGIC_VECTOR(9 DOWNTO 0);
	-- State Machine Registers
	
	-- Integers

BEGIN

-- *****************************************************************************
-- *                         Finite State Machine(s)                           *
-- *****************************************************************************


-- *****************************************************************************
-- *                             Sequential Logic                              *
-- *****************************************************************************

	-- Output Registers
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				stream_out_data				    <= (OTHERS => '0');
				stream_out_startofpacket	    <= '0';
				stream_out_endofpacket		    <= '0';
				stream_out_valid				    <= '0';
			ELSIF (transfer_data = '1') THEN
				stream_out_data			    	<= converted_data;
				stream_out_startofpacket	   <= converted_startofpacket;
				stream_out_endofpacket		   <= converted_endofpacket;
				stream_out_valid				   <= converted_valid;
			END IF;
		END IF;
	END PROCESS;


	-- Internal Registers
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				data							  	<= (OTHERS => '0');
				startofpacket					<= '0';
				endofpacket						<= '0';
				valid								<= '0';
			ELSIF (stream_in_ready = '1') THEN
				data							   <= stream_in_data;
				startofpacket					<= stream_in_startofpacket;
				endofpacket						<= stream_in_endofpacket;
				valid								<= stream_in_valid;
			ELSIF (transfer_data = '1') THEN
				data							   <= (OTHERS => '0');
				startofpacket					<= '0';
				endofpacket						<= '0';
				valid							   <= '0';
			END IF;
		END IF;
	END PROCESS;
	
	
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN 
			IF ((reset = '1') OR ((stream_in_ready = '1') AND (stream_in_startofpacket = '1'))) THEN
				m_reg                      <= (OTHERS => '0');
				n_reg                      <= (OTHERS => '0');
				m_counter                  <= (OTHERS => '0');
				n_counter                  <= (OTHERS => '0');
				length_current             <= (OTHERS => '0');
				length_max                 <= (OTHERS => '0');
			
			ELSIF ((stream_in_ready = '1') AND (stream_in_endofpacket = '1')) THEN
				avs_readdata_centroid(31 DOWNTO 26) <= (OTHERS => '0');
				avs_readdata_centroid(25 DOWNTO 16)   <= m_reg;
				avs_readdata_centroid(15 DOWNTO 10) <= (OTHERS => '0');
				avs_readdata_centroid(9 DOWNTO 0)   <= (n_reg - ('0' & length_max(9 DOWNTO 1)));
			ELSIF ((transfer_data = '1') AND (valid = '1')) THEN
				IF (n_counter = B"1001111111") THEN
					n_counter <= (OTHERS => '0');
					m_counter <= m_counter + 1;
				ELSE 
					n_counter <= n_counter + 1;
				END IF;
				
				IF ((threshold = '1') AND (n_counter < B"1001111110")) THEN
					length_current <= length_current + 1;
				ELSIF (length_current > length_max) THEN
					length_max <= length_current;
					m_reg <= m_counter;
					n_reg <= n_counter;
				ELSE
					length_current <= (OTHERS => '0');
				END IF;
			ELSE 
			--DO NOTHING
			END IF;
		END IF;
	END PROCESS;
	
	
	PROCESS(clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				irq <= '0';
			ELSE
				IF ((stream_in_ready = '1') AND (stream_in_endofpacket = '1')) THEN
					irq <= '1';
				ELSIF ((avs_read = '1') AND (avs_chipselect = '1')) THEN 
					irq <= '0';
				END IF;
			END IF;
		END IF;
	END PROCESS;

	
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				binary_data						<= (others => '0');
			ELSIF ((stream_in_ready = '1') AND (stream_in_startofpacket = '1')) THEN
				binary_data						<= (others => '0');
			ELSIF ((transfer_data = '1') AND (valid = '1')) THEN
				IF (threshold = '1') THEN
					binary_data					<= (others => '1');
				ELSE
					binary_data					<= (others => '0');
				END IF;                                 
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- Output Assignments
	stream_in_ready <= stream_in_valid AND (NOT valid OR transfer_data);

	-- Internal Assignments
	transfer_data <= 
			NOT stream_out_valid OR (stream_out_ready AND stream_out_valid);

	threshold <= '1' WHEN ((("00" & data(23 DOWNTO 16)) + ("00" & data(15 DOWNTO 8)) + ("00" & data(7 DOWNTO 0))) > threshold_range(9 DOWNTO 0))
                    ELSE '0';

	converted_data(ODW DOWNTO 0) <= data(ODW DOWNTO 0) WHEN (threshold_active = '0')
                                                      ELSE binary_data(ODW DOWNTO 0);
	converted_startofpacket      <= startofpacket;
	converted_endofpacket 	     <= endofpacket;
	converted_valid 			     <= valid;

END Behaviour;

