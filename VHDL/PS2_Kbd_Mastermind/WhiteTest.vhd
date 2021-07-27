--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   23:13:03 05/16/2021
-- Design Name:   
-- Module Name:   C:/Users/Slize/Downloads/Kl/WhiteTest.vhd
-- Project Name:  Klawiatura
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: WhiteBox
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
 
ENTITY WhiteTest IS
END WhiteTest;
 
ARCHITECTURE behavior OF WhiteTest IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT WhiteBox
    PORT(
         Kbd_CLK : IN  std_logic;
         Kbd_DATA : IN  std_logic;
         CLK : IN  std_logic;
         Rst : IN  std_logic;
         DO : OUT  std_logic_vector(7 downto 0);
         DO_RDY : OUT  std_logic;
         newline : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal Kbd_CLK : std_logic := '0';
   signal Kbd_DATA : std_logic := '0';
   signal CLK : std_logic := '0';
   signal Rst : std_logic := '0';

 	--Outputs
   signal DO : std_logic_vector(7 downto 0);
   signal DO_RDY : std_logic;
   signal newline : std_logic;

   -- Clock period definitions
   constant CLK_period : time := 20 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: WhiteBox PORT MAP (
          Kbd_CLK => Kbd_CLK,
          Kbd_DATA => Kbd_DATA,
          CLK => CLK,
          Rst => Rst,
          DO => DO,
          DO_RDY => DO_RDY,
          newline => newline
        );

   -- Clock process definitions 
   Clk_process :process
   begin
		Clk <= '0';
		wait for Clk_period/2;
		Clk <= '1';
		wait for Clk_period/2;
   end process;
 

   -- Stimulus process
   PROCESS
		procedure TransmPS2( Byte : std_logic_vector( 7 downto 0 ) ) is
		  variable Frame : std_logic_vector( 10 downto 0 ) := "11" & Byte & '0';
		begin
		  -- Parity calculation
		  for i in 0 to 7 loop
			Frame( 9 ) := Frame( 9 ) xor Byte( i );
		  end loop;
		  -- Transmission of the frame; Fclk = 10kHz
		  for i in 0 to 10 loop
			Kbd_Data <= Frame( i );
			wait for 5 us;
			Kbd_Clk <= '0', '1' after 50 us;
			wait for 95 us;
		  end loop;
		end procedure;
	BEGIN
		wait for 15 us;
		TransmPS2( X"1C" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"1C" ); --1
		wait for 15 us;
		TransmPS2( X"32" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"32" ); --2
		wait for 15 us;
		TransmPS2( X"1C" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"1C" ); --3
		wait for 15 us;
		TransmPS2( X"1C" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"1C" ); --4
		wait for 15 us;
		TransmPS2( X"1C" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"1C" ); --5
		wait for 15 us;
		TransmPS2( X"32" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"32" ); --6
		wait for 15 us;
		TransmPS2( X"1C" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"1C" ); --7
		wait for 15 us;
		TransmPS2( X"2B" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"2B" ); --8
		wait; -- will wait forever
	END PROCESS;

END;
