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
		-- Input ports and Clocks
		KEY		: in  std_logic_vector (0 downto 0);
		SW			: in 	std_logic_vector (17 downto 0);
		CLOCK_50	: in  std_logic;
		CLOCK_27 : in  std_logic;

		-- SRAM on board
		SRAM_ADDR	:	out	DE2_SRAM_ADDR_BUS;
		SRAM_DQ		:	inout DE2_SRAM_DATA_BUS;
		SRAM_WE_N	:	out	std_logic;
		SRAM_OE_N	:	out	std_logic;
		SRAM_UB_N	:	out 	std_logic;
		SRAM_LB_N	:	out	std_logic;
		SRAM_CE_N	:	out	std_logic;
		
		-- SDRAM on board
		DRAM_ADDR	:	out	DE2_SDRAM_ADDR_BUS;
		DRAM_BA_0	: 	out	std_logic;
		DRAM_BA_1	:	out	std_logic;
		DRAM_CAS_N	:	out	std_logic;
		DRAM_CKE		:	out	std_logic;
		DRAM_CLK		:	out	std_logic;
		DRAM_CS_N	:	out	std_logic;
		DRAM_DQ		:	inout DE2_SDRAM_DATA_BUS;
		DRAM_LDQM	: 	out	std_logic;
		DRAM_UDQM	: 	out	std_logic;
		DRAM_RAS_N	: 	out	std_logic;
		DRAM_WE_N	: 	out 	std_logic;
		
		-- FLASH
		FL_ADDR     :  out   std_logic_vector (21 downto 0);
		FL_CE_N     :  out   std_logic_vector (0 downto 0);
		FL_OE_N     :  out   std_logic_vector (0 downto 0);
		FL_DQ       :  inout std_logic_vector (7 downto 0);
		FL_RST_N    :  out   std_logic_vector (0 downto 0);
		FL_WE_N     :  out   std_logic_vector (0 downto 0);
		
		-- RED leds on board
		LEDR		   :  out   std_logic_vector(17 downto 0);
		
		-- LCD on board
		LCD_BLON	   : out std_logic;
		LCD_ON	   : out std_logic;
		LCD_DATA   	: inout DE2_LCD_DATA_BUS;
		LCD_RS	   : out std_logic;
		LCD_EN	   : out std_logic;
		LCD_RW	   : out std_logic;
		
		-- VGA Controller
		VGA_R       : OUT STD_LOGIC_VECTOR (9 downto 0);
		VGA_B       : OUT STD_LOGIC_VECTOR (9 downto 0);
		VGA_G       : OUT STD_LOGIC_VECTOR (9 downto 0);
		VGA_CLK     : OUT STD_LOGIC;
		VGA_BLANK   : OUT STD_LOGIC;
		VGA_HS      : OUT STD_LOGIC;
		VGA_VS      : OUT STD_LOGIC;
		VGA_SYNC    : OUT STD_LOGIC;
		
		-- SD CARD
		SD_DAT	: inout	std_logic;
		SD_DAT3	: inout 	std_logic;
		SD_CMD 	: inout 	std_logic;
		SD_CLK	: out 	std_logic;
		
		-- Audio CODEC
		AUD_DACLRCK   : inout std_logic;
		AUD_DACDAT    : out   std_logic;
		AUD_BCLK      : inout std_logic;
		AUD_XCK       : out   std_logic;	
		
		I2C_SDAT    : inout std_logic                     := 'X';            
      I2C_SCLK    : out   std_logic;
		
		-- TV Decoder
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
			clock50_clk_in_clk                       : in    std_logic                     := 'X';             -- clk
			clock50_clk_in_reset_reset_n             : in    std_logic                     := 'X';             -- reset_n
			clock27_clk_in_clk                       : in    std_logic                     := 'X';             -- clk
			clock27_clk_in_reset_reset_n             : in    std_logic                     := 'X';             -- reset_n
			
			sdramclk_out_clk_clk                     : out   std_logic;                                        -- clk
         audioclk_out_clk_clk                     : out   std_logic;                                        -- clk
			
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
			vga_controller_external_interface_B      : out   std_logic_vector(9 downto 0);                     -- B
			
			sdram_0_wire_addr                          : out   DE2_SDRAM_ADDR_BUS;                               -- addr
			sdram_0_wire_ba                            : out   std_logic_vector(1 downto 0);                     -- ba
			sdram_0_wire_cas_n                         : out   std_logic;                                        -- cas_n
			sdram_0_wire_cke                           : out   std_logic;                                        -- cke
			sdram_0_wire_cs_n                          : out   std_logic;                                        -- cs_n
			sdram_0_wire_dq                            : inout DE2_SDRAM_DATA_BUS := (others => 'X');            -- dq
			sdram_0_wire_dqm                           : out   std_logic_vector(1 downto 0);                     -- dqm
			sdram_0_wire_ras_n                         : out   std_logic;                                        -- ras_n
			sdram_0_wire_we_n                          : out   std_logic;                                        -- we_n
			
			character_lcd_0_external_interface_DATA    : inout DE2_LCD_DATA_BUS  := (others => 'X');             -- DATA
			character_lcd_0_external_interface_ON      : out   std_logic;                                        -- ON
			character_lcd_0_external_interface_BLON    : out   std_logic;                                        -- BLON
			character_lcd_0_external_interface_EN      : out   std_logic;                                        -- EN
			character_lcd_0_external_interface_RS      : out   std_logic;                                        -- RS
			character_lcd_0_external_interface_RW      : out   std_logic;                                        -- RW
			
			audio_0_external_interface_BCLK            : in    std_logic                     := 'X';             -- BCLK
         audio_0_external_interface_DACDAT          : out   std_logic;                                        -- DACDAT
         audio_0_external_interface_DACLRCK         : in    std_logic                     := 'X';             -- DACLRCK
			
			spi_0_external_MISO                        : in    std_logic                     := 'X';             -- MISO
			spi_0_external_MOSI                        : out   std_logic;                                        -- MOSI
			spi_0_external_SCLK                        : out   std_logic;                                        -- SCLK
			spi_0_external_SS_n                        : out   std_logic;                                        -- SS_n
			
			pll50_areset_conduit_export                : in    std_logic                     := 'X';             -- export
			pll50_locked_conduit_export                : out   std_logic;                                        -- export
			pll50_phasedone_conduit_export             : out   std_logic;                                        -- export
			pll27_areset_conduit_export                : in    std_logic                     := 'X';             -- export
			pll27_locked_conduit_export                : out   std_logic;                                        -- export
			pll27_phasedone_conduit_export             : out   std_logic;                                        -- export
			
			tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_read_n_out       : out   std_logic_vector(0 downto 0);                     -- generic_tristate_controller_0_tcm_read_n_out
			tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_data_out         : inout std_logic_vector(7 downto 0) := (others => 'X');  -- generic_tristate_controller_0_tcm_data_out
			tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_chipselect_n_out : out   std_logic_vector(0 downto 0);                     -- generic_tristate_controller_0_tcm_chipselect_n_out
			tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_write_n_out      : out   std_logic_vector(0 downto 0);                     -- generic_tristate_controller_0_tcm_write_n_out
			tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_address_out      : out   std_logic_vector(21 downto 0);                    -- generic_tristate_controller_0_tcm_address_out
			
			led_detector_thres_active_conduit_export   : in    std_logic                     := 'X';             -- export
         led_detector_thres_range_conduit_export    : in    std_logic_vector(9 downto 0)  := (others => 'X')  -- export
			
	  );
	end component niosII_system;

	--	These signals are for matching the provided IP core to
	-- The specific SDRAM chip in our system	 
	signal BA	     :  std_logic_vector (1 downto 0);
	signal DQM	     :  std_logic_vector (1 downto 0);
	signal RED       :  std_logic_vector (17 downto 0);
	
	signal P50ARES   :  std_logic;
	signal P50LOCK   :  std_logic;
	signal P50PHAS   :  std_logic;
	signal P27ARES   :  std_logic;
	signal P27LOCK   :  std_logic;
	signal P27PHAS   :  std_logic;
	 
begin

	DRAM_BA_1 <= BA(1);
	DRAM_BA_0 <= BA(0);
	
	DRAM_UDQM <= DQM(1);
	DRAM_LDQM <= DQM(0);
	
	FL_RST_N(0) <= '1';
	
	LEDR <= RED;
	RED <= SW;
	
	P50ARES <= '0';
	P27ARES <= '0';
	 
    u0 : component niosII_system
        port map (
				clock50_clk_in_clk                       => CLOCK_50,   --   clock50_clk_in.clk
            clock50_clk_in_reset_reset_n             => KEY(0),     --   clock50_clk_in_reset.reset_n
            clock27_clk_in_clk                       => CLOCK_27,   --   clock27_clk_in.clk
            clock27_clk_in_reset_reset_n             => KEY(0),     --   clock27_clk_in_reset.reset_n
		  
				sdramclk_out_clk_clk                     => DRAM_CLK,   --   sdramclk_out_clk.clk
            audioclk_out_clk_clk                     => AUD_XCK,    --   audioclk_out_clk.clk
				
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
           
			   vga_controller_external_interface_CLK    => VGA_CLK,    --  .CLK
            vga_controller_external_interface_HS     => VGA_HS,     --  .HS
            vga_controller_external_interface_VS     => VGA_VS,     --  .VS
            vga_controller_external_interface_BLANK  => VGA_BLANK,  --  .BLANK
            vga_controller_external_interface_SYNC   => VGA_SYNC,   --  .SYNC
            vga_controller_external_interface_R      => VGA_R,      --  .R
            vga_controller_external_interface_G      => VGA_G,      --  .G
            vga_controller_external_interface_B      => VGA_B,      --  .B
				
				sdram_0_wire_addr                        => DRAM_ADDR,
				sdram_0_wire_ba                          => BA,
				sdram_0_wire_cas_n                       => DRAM_CAS_N,
				sdram_0_wire_cke                         => DRAM_CKE,
				sdram_0_wire_cs_n                        => DRAM_CS_N,
				sdram_0_wire_dq                          => DRAM_DQ,
				sdram_0_wire_dqm                         => DQM,
				sdram_0_wire_ras_n                       => DRAM_RAS_N,
				sdram_0_wire_we_n                        => DRAM_WE_N,
			
				character_lcd_0_external_interface_DATA  => LCD_DATA, 
            character_lcd_0_external_interface_ON    => LCD_ON,   
            character_lcd_0_external_interface_BLON  => LCD_BLON, 
            character_lcd_0_external_interface_EN    => LCD_EN,   
            character_lcd_0_external_interface_RS    => LCD_RS,   
            character_lcd_0_external_interface_RW    => LCD_RW,
			
				audio_0_external_interface_BCLK          => AUD_BCLK,
				audio_0_external_interface_DACDAT        => AUD_DACDAT,
				audio_0_external_interface_DACLRCK       => AUD_DACLRCK,
				
				spi_0_external_MISO                      => SD_DAT,        --  .MISO
            spi_0_external_MOSI                      => SD_CMD,        --  .MOSI
            spi_0_external_SCLK                      => SD_CLK,        --  .SCLK
            spi_0_external_SS_n                      => SD_DAT3,       --  .SS_n
			
				pll50_areset_conduit_export              => P50ARES,       --  pll50_areset_conduit.export
            pll50_locked_conduit_export              => P50LOCK,       --  pll50_locked_conduit.export
            pll50_phasedone_conduit_export           => P50PHAS,       --  pll50_phasedone_conduit.export
            pll27_areset_conduit_export              => P27ARES,       --  pll27_areset_conduit.export
            pll27_locked_conduit_export              => P27LOCK,       --  pll27_locked_conduit.export
            pll27_phasedone_conduit_export           => P27PHAS,       --  pll27_phasedone_conduit.export
			
			   tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_read_n_out       => FL_OE_N, --      tristate_conduit_bridge_0_out.generic_tristate_controller_0_tcm_read_n_out
            tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_data_out         => FL_DQ,   --                                   .generic_tristate_controller_0_tcm_data_out
            tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_chipselect_n_out => FL_CE_N, --                                   .generic_tristate_controller_0_tcm_chipselect_n_out
            tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_write_n_out      => FL_WE_N, --                                   .generic_tristate_controller_0_tcm_write_n_out
            tristate_conduit_bridge_0_out_generic_tristate_controller_0_tcm_address_out      => FL_ADDR, --                                   .generic_tristate_controller_0_tcm_address_out			
			
				led_detector_thres_active_conduit_export => SW(17),
				led_detector_thres_range_conduit_export  => SW(9 DOWNTO 0)
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

