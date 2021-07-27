--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   00:23:16 04/20/2021
-- Design Name:   
-- Module Name:   R:/Users/Dom/Documents/STUDIA/UCISW/Klawiatura/rx_test.vhd
-- Project Name:  Klawiatura
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: PS2_Kbd
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY rx_test IS
END rx_test;
 
ARCHITECTURE behavior OF rx_test IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT PS2_Kbd
    PORT(
         PS2_CLK : IN  std_logic;
         PS2_DATA : IN  std_logic;
         Clk : IN  std_logic;
         DO : OUT  std_logic_vector(7 downto 0);
         E0 : OUT  std_logic;
         F0 : OUT  std_logic;
         DO_RDY : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal PS2_CLK : std_logic := '0';
   signal PS2_DATA : std_logic := '0';
   signal Clk : std_logic := '0';

 	--Outputs
   signal DO : std_logic_vector(7 downto 0);
   signal E0 : std_logic;
   signal F0 : std_logic;
   signal DO_RDY : std_logic;

   -- Clock period definitions
   constant PS2_CLK_period : time := 10 ns;
   constant Clk_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: PS2_Kbd PORT MAP (
          PS2_CLK => PS2_CLK,
          PS2_DATA => PS2_DATA,
          Clk => Clk,
          DO => DO,
          E0 => E0,
          F0 => F0,
          DO_RDY => DO_RDY
        );

   -- Clock process definitions
   PS2_CLK_process :process
   begin
		PS2_CLK <= '0';
		wait for PS2_CLK_period/2;
		PS2_CLK <= '1';
		wait for PS2_CLK_period/2;
   end process;
 
   Clk_process :process
   begin
		Clk <= '0';
		wait for Clk_period/2;
		Clk <= '1';
		wait for Clk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for PS2_CLK_period*10;

      -- insert stimulus here 

      wait;
   end process;

END;
