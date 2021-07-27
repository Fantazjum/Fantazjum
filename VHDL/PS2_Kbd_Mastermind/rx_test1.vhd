--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   00:24:18 04/20/2021
-- Design Name:   
-- Module Name:   R:/Users/Dom/Documents/STUDIA/UCISW/Klawiatura/rx_test1.vhd
-- Project Name:  Klawiatura
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: PS2_RX
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
 
    COMPONENT PS2_RX
    PORT(
         PS2_CLK : IN  std_logic;
         PS2_DATA : IN  std_logic;
         Clk : IN  std_logic;
         DO : OUT  std_logic_vector(7 downto 0);
         DO_RDY : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal PS2_CLK : std_logic := '0';
   signal PS2_DATA : std_logic := '0';
   signal Clk : std_logic := '0';

 	--Outputs
   signal DO : std_logic_vector(7 downto 0);
   signal DO_RDY : std_logic;

   -- Clock period definitions
   constant Clk_period : time := 20 ns;
	
	
 
BEGIN

	-- Instantiate the Unit Under Test (UUT)
		UUT: PS2_RX PORT MAP (
			PS2_CLK => PS2_CLK,
			PS2_DATA => PS2_DATA,
			Clk => Clk,
			DO => DO,
			DO_RDY => DO_RDY
		);


 
	-- Clock process definitions
		CLK_process :process
		begin 
			CLK <= '0';
			wait for CLK_period/2;
			CLK <= '1';
			wait for CLK_period/2;
		end process;
		
 
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
			PS2_Data <= Frame( i );
			wait for 5 us; 
			PS2_Clk <= '0', '1' after 50 us;
			wait for 95 us;
		  end loop;
		end procedure;
	BEGIN
		wait for 15 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"81" );
		wait; -- will wait forever
	END PROCESS;

END;
