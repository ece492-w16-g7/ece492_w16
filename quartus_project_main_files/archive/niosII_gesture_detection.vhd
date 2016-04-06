 -- Nancy Minderman
-- nancy.minderman@ualberta.ca
-- This file makes extensive use of Altera template structures.
-- This file is the top-level file for lab 1 winter 2014 for version 12.1sp1 on Windows 7


-- A library clause declares a name as a library.  It 
-- does not create the library; it simply forward declares 
-- it. 
library ieee;

-- Commonly imported packages:

	-- STD_LOGIC and STD_LOGIC_VECTOR types, and relevant functions
	use ieee.std_logic_1164.all;

	-- SIGNED and UNSIGNED types, and relevant functions
	use ieee.numeric_std.all;

	-- Basic sequential functions and concurrent procedures
	use ieee.VITAL_Primitives.all;
	
	use work.DE2_CONSTANTS.all;
	
entity niosII_gesture_detection is
	port
	(
		-- Input ports and 50 MHz Clock
		KEY		: in  std_logic_vector (0 downto 0);
		SW			: in 	std_logic_vector (17 downto 0);
		CLOCK_50	: in  std_logic;

		-- SRAM on board
		
		SRAM_ADDR	:	out	DE2_SRAM_ADDR_BUS;
		SRAM_DQ		:	inout DE2_SRAM_DATA_BUS;
		SRAM_WE_N	:	out	std_logic;
		SRAM_OE_N	:	out	std_logic;
		SRAM_UB_N	:	out 	std_logic;
		SRAM_LB_N	:	out	std_logic;
		SRAM_CE_N	:	out	std_logic;
		
		-- VGA Controller
		VGA_R       : OUT STD_LOGIC_VECTOR (9 downto 0);
		VGA_B       : OUT STD_LOGIC_VECTOR (9 downto 0);
		VGA_G       : OUT STD_LOGIC_VECTOR (9 downto 0);
		VGA_CLK     : OUT STD_LOGIC;
		VGA_BLANK   : OUT STD_LOGIC;
		VGA_HS      : OUT STD_LOGIC;
		VGA_VS      : OUT STD_LOGIC;
		VGA_SYNC    : OUT STD_LOGIC;
		
		I2C_SDAT    : inout std_logic                     := 'X';            
      I2C_SCLK    : out   std_logic;
		
		TD_CLK27    : in    std_logic                     := 'X';
      TD_DATA     : in    std_logic_vector(7 downto 0)  := (others => 'X');
      TD_HS       : in    std_logic                     := 'X';
      TD_VS       : in    std_logic                     := 'X';
      TD_RESET    : out   std_logic	
	);
end niosII_gesture_detection;

architecture structure of niosII_gesture_detection is
	component niosII_system is
	  port (
			clk_clk                                  : in    std_logic                     := 'X';             -- clk
			reset_reset_n                            : in    std_logic                     := 'X';             -- reset_n
			pixel_buffer_external_interface_DQ       : inout DE2_SRAM_DATA_BUS := (others => 'X');             -- DQ
			pixel_buffer_external_interface_ADDR     : out   DE2_SRAM_ADDR_BUS;                                -- ADDR
			pixel_buffer_external_interface_LB_N     : out   std_logic;                                        -- LB_N
			pixel_buffer_external_interface_UB_N     : out   std_logic;                                        -- UB_N
			pixel_buffer_external_interface_CE_N     : out   std_logic;                                        -- CE_N
			pixel_buffer_external_interface_OE_N     : out   std_logic;                                        -- OE_N
			pixel_buffer_external_interface_WE_N     : out   std_logic;                                        -- WE_N
			av_config_external_interface_SDAT        : inout std_logic                     := 'X';             -- SDAT
			av_config_external_interface_SCLK        : out   std_logic;                                        -- SCLK
			decoder_external_interface_TD_CLK27      : in    std_logic                     := 'X';             -- TD_CLK27
			decoder_external_interface_TD_DATA       : in    std_logic_vector(7 downto 0)  := (others => 'X'); -- TD_DATA
			decoder_external_interface_TD_HS         : in    std_logic                     := 'X';             -- TD_HS
			decoder_external_interface_TD_VS         : in    std_logic                     := 'X';             -- TD_VS
			decoder_external_interface_TD_RESET      : out   std_logic;                                        -- TD_RESET
			decoder_external_interface_overflow_flag : out   std_logic;                                        -- overflow_flag
			vga_controller_external_interface_CLK    : out   std_logic;                                        -- CLK
			vga_controller_external_interface_HS     : out   std_logic;                                        -- HS
			vga_controller_external_interface_VS     : out   std_logic;                                        -- VS
			vga_controller_external_interface_BLANK  : out   std_logic;                                        -- BLANK
			vga_controller_external_interface_SYNC   : out   std_logic;                                        -- SYNC
			vga_controller_external_interface_R      : out   std_logic_vector(9 downto 0);                     -- R
			vga_controller_external_interface_G      : out   std_logic_vector(9 downto 0);                     -- G
			vga_controller_external_interface_B      : out   std_logic_vector(9 downto 0)                      -- B
	  );
	end component niosII_system;

	--	These signals are for matching the provided IP core to
	-- The specific SDRAM chip in our system	 
	--signal BA	: std_logic_vector (1 downto 0);
	--signal DQM	:	std_logic_vector (1 downto 0);
	 
begin

	--DRAM_BA_1 <= BA(1);
	--DRAM_BA_0 <= BA(0);
	
	--DRAM_UDQM <= DQM(1);
	--DRAM_LDQM <= DQM(0);
	
	-- Component Instantiation Statement (optional)
	 
    u0 : component niosII_system
        port map (
            clk_clk                                  => CLOCK_50,   --   clk.clk
            reset_reset_n                            => KEY(0),     --   reset.reset_n
				
            pixel_buffer_external_interface_DQ       => SRAM_DQ,    --  .DQ
            pixel_buffer_external_interface_ADDR     => SRAM_ADDR,  --  .ADDR
            pixel_buffer_external_interface_LB_N     => SRAM_LB_N,  --  .LB_N
            pixel_buffer_external_interface_UB_N     => SRAM_UB_N,  --  .UB_N
            pixel_buffer_external_interface_CE_N     => SRAM_CE_N,  --  .CE_N
            pixel_buffer_external_interface_OE_N     => SRAM_OE_N,  --  .OE_N
            pixel_buffer_external_interface_WE_N     => SRAM_WE_N,  --  .WE_N
				
            av_config_external_interface_SDAT        => I2C_SDAT,   --  .SDAT
            av_config_external_interface_SCLK        => I2C_SCLK,   --  .SCLK
				
            decoder_external_interface_TD_CLK27      => TD_CLK27,   --  .TD_CLK27
            decoder_external_interface_TD_DATA       => TD_DATA,    --  .TD_DATA
            decoder_external_interface_TD_HS         => TD_HS,      --  .TD_HS
            decoder_external_interface_TD_VS         => TD_VS,      --  .TD_VS
            decoder_external_interface_TD_RESET      => TD_RESET,   --  .TD_RESET
            --decoder_external_interface_overflow_flag => <>, -- .overflow_flag
           
			   vga_controller_external_interface_CLK    => VGA_CLK,    --  .CLK
            vga_controller_external_interface_HS     => VGA_HS,     --  .HS
            vga_controller_external_interface_VS     => VGA_VS,     --  .VS
            vga_controller_external_interface_BLANK  => VGA_BLANK,  --  .BLANK
            vga_controller_external_interface_SYNC   => VGA_SYNC,   --  .SYNC
            vga_controller_external_interface_R      => VGA_R,      --  .R
            vga_controller_external_interface_G      => VGA_G,      --  .G
            vga_controller_external_interface_B      => VGA_B       --  .B
        );

end structure;


library ieee;

-- Commonly imported packages:

	-- STD_LOGIC and STD_LOGIC_VECTOR types, and relevant functions
	use ieee.std_logic_1164.all;

package DE2_CONSTANTS is
	
	type DE2_SDRAM_ADDR_BUS is array(11 downto 0) of std_logic;
	type DE2_SDRAM_DATA_BUS is array(15 downto 0) of std_logic;
	
	type DE2_LCD_DATA_BUS	is array(7 downto 0) of std_logic;
	
	type DE2_LED_GREEN		is array(7 downto 0)  of std_logic;
	
	type DE2_SRAM_ADDR_BUS	is array(17 downto 0) of std_logic;
	type DE2_SRAM_DATA_BUS  is array(15 downto 0) of std_logic;
	
end DE2_CONSTANTS;


