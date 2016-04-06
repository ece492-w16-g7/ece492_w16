--------------------------------------------------------------------------
--   This VHDL file was developed by Altera Corporation.  It may be
-- freely copied and/or distributed at no cost.  Any persons using this
-- file for any purpose do so at their own risk, and are responsible for
-- the results of such use.  Altera Corporation does not guarantee that
-- this file is complete, correct, or fit for any particular purpose.
-- NO WARRANTY OF ANY KIND IS EXPRESSED OR IMPLIED.  This notice must
-- accompany any copy of this file.
--
--------------------------------------------------------------------------
-- LPM Synthesizable Models (Support string type generic)
--------------------------------------------------------------------------
-- Version 2.0 (lpm 220)      Date 01/04/00
--
-- 1. Fixed LPM_RAM_DQ, LPM_RAM_DP, LPM_RAM_IO and LPM_ROM to correctly
--    read in values from LPM_FILE (*.hex) when the DATA width is greater
--    than 16 bits.
-- 2. Explicit sign conversions are added to standard logic vector
--    comparisons in LPM_RAM_DQ, LPM_RAM_DP, LPM_RAM_IO, LPM_ROM, and
--    LPM_COMPARE.
-- 3. LPM_FIFO_DC is rewritten to have correct outputs.
-- 4. LPM_FIFO outputs zeros when nothing has been read from it, and
--    outputs LPM_NUMWORDS mod exp(2, LPM_WIDTHU) when it is full.
-- 5. Fixed LPM_DIVIDE to divide correctly.
--------------------------------------------------------------------------
-- Version 1.9 (lpm 220)      Date 11/30/99
--
-- 1. Fixed UNUSED file not found problem and initialization problem
--    with LPM_RAM_DP, LPM_RAM_DQ, and LPM_RAM_IO.
-- 2. Fixed LPM_MULT when SUM port is not used.
-- 3. Fixed LPM_FIFO_DC to enable read when rdclock and wrclock rise
--    at the same time.
-- 4. Fixed LPM_COUNTER comparison problem when signed library is loaded
--    and counter is incrementing.
-- 5. Got rid of "Illegal Character" error message at time = 0 ns when
--    simulating LPM_COUNTER.
--------------------------------------------------------------------------
-- Version 1.8 (lpm 220)      Date 10/25/99
--
-- 1. Some LPM_PVALUE implementations were missing, and now implemented.
-- 2. Fixed LPM_COUNTER to count correctly without conversion overflow,
--    that is, when LPM_MODULUS = 2 ** LPM_WIDTH.
-- 3. Fixed LPM_RAM_DP sync process sensitivity list to detect wraddress
--    changes.
--------------------------------------------------------------------------
-- Version 1.7 (lpm 220)      Date 07/13/99
--
-- Changed LPM_RAM_IO so that it can be used to simulate both MP2 and
--   Quartus behaviour and LPM220-compliant behaviour.
--------------------------------------------------------------------------
-- Version 1.6 (lpm 220)      Date 06/15/99
--
-- 1. Fixed LPM_ADD_SUB sign extension problem and subtraction bug.
-- 2. Fixed LPM_COUNTER to use LPM_MODULUS value.
-- 3. Added CIN and COUT port, and discarded EQ port in LPM_COUNTER to
--    comply with the specfication.
-- 4. Included LPM_RAM_DP, LPM_RAM_DQ, LPM_RAM_IO, LPM_ROM, LPM_FIFO, and
--    LPM_FIFO_DC; they are all initialized to 0's.
--------------------------------------------------------------------------
-- Version 1.5 (lpm 220)      Date 05/10/99
--
-- Changed LPM_MODULUS from string type to integer.
--------------------------------------------------------------------------
-- Version 1.4 (lpm 220)      Date 02/05/99
-- 
-- 1. Added LPM_DIVIDE module.
-- 2. Added CLKEN port to LPM_MUX, LPM_DECODE, LPM_ADD_SUB, LPM_MULT
--    and LPM_COMPARE
-- 3. Replaced the constants holding string with the actual string.
--------------------------------------------------------------------------
-- Version 1.3                Date 07/30/96
--
-- Modification History
--
-- 1. Changed the DEFAULT value to "UNUSED" for LPM_SVALUE, LPM_AVALUE,
-- LPM_MODULUS, and LPM_NUMWORDS, LPM_HINT,LPM_STRENGTH, LPM_DIRECTION,
-- and LPM_PVALUE
--
-- 2. Added the two dimentional port components (AND, OR, XOR, and MUX).
--------------------------------------------------------------------------
-- Excluded Functions:
--
--   LPM_FSM and LPM_TTABLE
--
--------------------------------------------------------------------------
-- Assumptions:
--
-- 1. All ports and signal types are std_logic or std_logic_vector
--    from IEEE 1164 package.
-- 2. Synopsys std_logic_arith, std_logic_unsigned, and std_logic_signed
--    package are assumed to be accessible from IEEE library.
-- 3. lpm_component_package must be accessible from library work.
-- 4. The default value of LPM_SVALUE, LPM_AVALUE, LPM_MODULUS, LPM_HINT,
--    LPM_NUMWORDS, LPM_STRENGTH, LPM_DIRECTION, and LPM_PVALUE is
--    string "UNUSED".
--------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_signed.all;
use work.LPM_COMPONENTS.all;

entity LPM_MULT_SIGNED is
	generic (LPM_WIDTHA : positive;
			 LPM_WIDTHB : positive;
			 --LPM_WIDTHS : positive;
			 LPM_WIDTHS : natural := 0;
			 LPM_WIDTHP : positive;
			 LPM_PIPELINE : integer := 0;
			 LPM_TYPE: string := "LPM_MULT";
			 LPM_HINT : string := "UNUSED");
	port (DATAA : in std_logic_vector(LPM_WIDTHA-1 downto 0);
		  DATAB : in std_logic_vector(LPM_WIDTHB-1 downto 0);
		  ACLR : in std_logic := '0';
		  CLOCK : in std_logic := '0';
		  CLKEN : in std_logic := '1';
		  SUM : in std_logic_vector(LPM_WIDTHS-1 downto 0) := (OTHERS => '0');
		  RESULT : out std_logic_vector(LPM_WIDTHP-1 downto 0));
end LPM_MULT_SIGNED;

architecture LPM_SYN of LPM_MULT_SIGNED is

signal FP : std_logic_vector(LPM_WIDTHS-1 downto 0);
type t_resulttmp IS ARRAY (0 to LPM_PIPELINE) of std_logic_vector(LPM_WIDTHP-1 downto 0);

begin

	process (CLOCK, ACLR, DATAA, DATAB, SUM)
	variable resulttmp : t_resulttmp;
	begin
		if LPM_PIPELINE >= 0 then
			if LPM_WIDTHP >= LPM_WIDTHS then
				if LPM_WIDTHS > 0 then
					resulttmp(LPM_PIPELINE) := (DATAA * DATAB) + SUM;
				else
					resulttmp(LPM_PIPELINE) := (DATAA * DATAB);
				end if;
			else
				FP <= (DATAA * DATAB) + SUM;
				resulttmp(LPM_PIPELINE) := FP(LPM_WIDTHS-1 downto LPM_WIDTHS-LPM_WIDTHP);
			end if;

			if LPM_PIPELINE > 0 then
				if ACLR = '1' then
					for i in 0 to LPM_PIPELINE loop
						resulttmp(i) := (OTHERS => '0');
					end loop;
				elsif CLOCK'event and CLOCK = '1' and CLKEN = '1' then
					resulttmp(0 to LPM_PIPELINE - 1) := resulttmp(1 to LPM_PIPELINE);
				end if;
			end if;
		end if;

		RESULT <= resulttmp(0);
	end process;

end LPM_SYN;


-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;
use work.LPM_COMPONENTS.all;

entity LPM_MULT_UNSIGNED is
	generic (LPM_WIDTHA : positive;
			 LPM_WIDTHB : positive;
			 --LPM_WIDTHS : positive;
			 LPM_WIDTHS : natural := 0;
			 LPM_WIDTHP : positive;
			 LPM_PIPELINE : integer := 0;
			 LPM_TYPE: string := "LPM_MULT";
			 LPM_HINT : string := "UNUSED");
	port (DATAA : in std_logic_vector(LPM_WIDTHA-1 downto 0);
		  DATAB : in std_logic_vector(LPM_WIDTHB-1 downto 0);
		  ACLR : in std_logic := '0';
		  CLOCK : in std_logic := '0';
		  CLKEN : in std_logic := '1';
		  SUM : in std_logic_vector(LPM_WIDTHS-1 downto 0) := (OTHERS => '0');
		  RESULT : out std_logic_vector(LPM_WIDTHP-1 downto 0));
end LPM_MULT_UNSIGNED;

architecture LPM_SYN of LPM_MULT_UNSIGNED is

signal FP : std_logic_vector(LPM_WIDTHS-1 downto 0);
type t_resulttmp IS ARRAY (0 to LPM_PIPELINE) of std_logic_vector(LPM_WIDTHP-1 downto 0);

begin

	process (CLOCK, ACLR, DATAA, DATAB, SUM)
	variable resulttmp : t_resulttmp;
	begin
		if LPM_PIPELINE >= 0 then
			if LPM_WIDTHP >= LPM_WIDTHS then
				if LPM_WIDTHS > 0 then
					resulttmp(LPM_PIPELINE) := (DATAA * DATAB) + SUM;
				else
					resulttmp(LPM_PIPELINE) := (DATAA * DATAB);
				end if;
			else
				FP <= (DATAA * DATAB) + SUM;
				resulttmp(LPM_PIPELINE) := FP(LPM_WIDTHS-1 downto LPM_WIDTHS-LPM_WIDTHP);
			end if;

			if LPM_PIPELINE > 0 then
				if ACLR = '1' then
					for i in 0 to LPM_PIPELINE loop
						resulttmp(i) := (OTHERS => '0');
					end loop;
				elsif CLOCK'event and CLOCK = '1' and CLKEN = '1' then
					resulttmp(0 to LPM_PIPELINE - 1) := resulttmp(1 to LPM_PIPELINE);
				end if;
			end if;
		end if;

		RESULT <= resulttmp(0);
	end process;

end LPM_SYN;


--------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use work.LPM_COMPONENTS.all;

entity LPM_MULT is
	generic (LPM_WIDTHA : positive;
			 LPM_WIDTHB : positive;
			 --LPM_WIDTHS : positive;
			 LPM_WIDTHS : natural := 0;
			 LPM_WIDTHP : positive;
			 LPM_REPRESENTATION : string := "UNSIGNED";
			 LPM_PIPELINE : integer := 0;
			 LPM_TYPE: string := "LPM_MULT";
			 LPM_HINT : string := "UNUSED");
	port (DATAA : in std_logic_vector(LPM_WIDTHA-1 downto 0);
		  DATAB : in std_logic_vector(LPM_WIDTHB-1 downto 0);
		  ACLR : in std_logic := '0';
		  CLOCK : in std_logic := '0';
		  CLKEN : in std_logic := '1';
		  SUM : in std_logic_vector(LPM_WIDTHS-1 downto 0) := (OTHERS => '0');
		  RESULT : out std_logic_vector(LPM_WIDTHP-1 downto 0));
end LPM_MULT;

architecture LPM_SYN of LPM_MULT is

	component LPM_MULT_UNSIGNED
		generic (LPM_WIDTHA : positive;
				 LPM_WIDTHB : positive;
				 --LPM_WIDTHS : positive;
				 LPM_WIDTHS : natural := 0;
				 LPM_WIDTHP : positive;
				 LPM_PIPELINE : integer := 0;
				 LPM_TYPE: string := "LPM_MULT";
				 LPM_HINT : string := "UNUSED");
		port (DATAA : in std_logic_vector(LPM_WIDTHA-1 downto 0);
			  DATAB : in std_logic_vector(LPM_WIDTHB-1 downto 0);
			  ACLR : in std_logic := '0';
			  CLOCK : in std_logic := '0';
			  CLKEN : in std_logic := '1';
			  SUM : in std_logic_vector(LPM_WIDTHS-1 downto 0) := (OTHERS => '0');
			  RESULT : out std_logic_vector(LPM_WIDTHP-1 downto 0));
	end component;

	component LPM_MULT_SIGNED
		generic (LPM_WIDTHA : positive;
				 LPM_WIDTHB : positive;
				 --LPM_WIDTHS : positive;
				 LPM_WIDTHS : natural := 0;
				 LPM_WIDTHP : positive;
				 LPM_PIPELINE : integer := 0;
				 LPM_TYPE: string := "LPM_MULT";
				 LPM_HINT : string := "UNUSED");
		port (DATAA : in std_logic_vector(LPM_WIDTHA-1 downto 0);
			  DATAB : in std_logic_vector(LPM_WIDTHB-1 downto 0);
			  ACLR : in std_logic := '0';
			  CLOCK : in std_logic := '0';
			  CLKEN : in std_logic := '1';
			  SUM : in std_logic_vector(LPM_WIDTHS-1 downto 0) := (OTHERS => '0');
			  RESULT : out std_logic_vector(LPM_WIDTHP-1 downto 0));
	end component;

begin

L1: if LPM_REPRESENTATION = "UNSIGNED" generate

U1: LPM_MULT_UNSIGNED
	generic map (LPM_WIDTHA => LPM_WIDTHA,
				 LPM_WIDTHB => LPM_WIDTHB, LPM_WIDTHS => LPM_WIDTHS,
				 LPM_WIDTHP => LPM_WIDTHP, LPM_PIPELINE => LPM_PIPELINE,
				 LPM_TYPE => LPM_TYPE, LPM_HINT => LPM_HINT)
	port map (DATAA => DATAA, DATAB => DATAB, ACLR => ACLR, CLKEN => CLKEN,
			  SUM => SUM, CLOCK => CLOCK, RESULT => RESULT);
	end generate;

L2: if LPM_REPRESENTATION = "SIGNED" generate

U1: LPM_MULT_SIGNED
	generic map (LPM_WIDTHA => LPM_WIDTHA,
				 LPM_WIDTHB => LPM_WIDTHB, LPM_WIDTHS => LPM_WIDTHS,
				 LPM_WIDTHP => LPM_WIDTHP, LPM_PIPELINE => LPM_PIPELINE,
				 LPM_TYPE => LPM_TYPE, LPM_HINT => LPM_HINT)
	port map (DATAA => DATAA, DATAB => DATAB, ACLR => ACLR, CLKEN => CLKEN,
			  SUM => SUM, CLOCK => CLOCK, RESULT => RESULT);
	end generate;

end LPM_SYN;



