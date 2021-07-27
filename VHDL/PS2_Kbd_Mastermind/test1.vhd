--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   11:52:19 05/16/2021
-- Design Name:   
-- Module Name:   R:/Users/Dom/Documents/STUDIA/UCISW/Klawiatura/test1.vhd
-- Project Name:  Klawiatura
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: Mastermind
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
 
ENTITY test1 IS
END test1;
 
ARCHITECTURE behavior OF test1 IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT Mastermind
    PORT(
         DI : IN  std_logic_vector(7 downto 0);
         DI_Rdy : IN  std_logic;
         F0 : IN  std_logic;
         Clk : IN  std_logic;
         Rst : IN  std_logic;
         DO : OUT  std_logic_vector(7 downto 0);
         DO_Rdy : OUT  std_logic;
			newline : OUT std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal DI : std_logic_vector(7 downto 0) := (others => '0');
   signal DI_Rdy : std_logic := '0';
   signal F0 : std_logic := '0';
   signal Clk : std_logic := '0';
   signal Rst : std_logic := '0';
	

 	--Outputs
   signal DO : std_logic_vector(7 downto 0);
   signal DO_Rdy : std_logic;
	signal newline :  std_logic := '0';
	
   -- Clock period definitions
   constant Clk_period : time := 20 ns;
	
	--test signal
	type type_byte_array is array (NATURAL range <>) of STD_LOGIC_VECTOR (7 downto 0);
	signal test_array : type_byte_array(0 to 7) := ( X"1C", X"1C", X"21", X"23", X"1C", X"32", X"1C", X"21"); 
 -- A X"1C"
 -- B X"32"
 -- C X"21"
 -- D X"23"
 -- E X"24"
 -- F X"2B"
	signal test_array2 : type_byte_array(0 to 3) := ( X"1C", X"1C", X"21", X"23"); 
	signal test_array3 : type_byte_array(0 to 3) := ( X"32", X"21", X"23", X"24");
 
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: Mastermind PORT MAP (
          DI => DI,
          DI_Rdy => DI_Rdy,
          F0 => F0,
          Clk => Clk,
          Rst => Rst,
          DO => DO,
          DO_Rdy => DO_Rdy,
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
   stim_proc: process
   begin
		rst <= '1';
		wait for CLK_period;
		rst <= '0';
		wait for CLK_period*2;
		for i in test_array'RANGE loop
			DI <= test_array(i);
			DI_RDY <= '1';
			F0 <= '0';
			wait for CLK_period;
			DI_RDY <= '0';
			wait for CLK_period*3;
			DI_RDY <= '1';
			F0 <= '1';
			wait for CLK_period;
			DI_RDY <= '0';
			F0 <= '0';
			wait for CLK_period*7;
		end loop;
		wait for CLK_period*15;
		for i in test_array3'RANGE loop
			DI <= test_array3(i);
			DI_RDY <= '1';
			F0 <= '0';
			wait for CLK_period;
			DI_RDY <= '0';
			wait for CLK_period*3;
			DI_RDY <= '1';
			F0 <= '1';
			wait for CLK_period;
			DI_RDY <= '0';
			F0 <= '0';
			wait for CLK_period*7;
		end loop;
		wait for CLK_period*7;
		DI <= X"2D";
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		F0 <= '1';
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		F0 <= '0';
		wait for CLK_period*7;
		for i in test_array3'RANGE loop
			DI <= test_array3(i);
			DI_RDY <= '1';
			F0 <= '0';
			wait for CLK_period;
			DI_RDY <= '0';
			wait for CLK_period*3;
			DI_RDY <= '1';
			F0 <= '1';
			wait for CLK_period;
			DI_RDY <= '0';
			F0 <= '0';
			wait for CLK_period*7;
		end loop;
		wait for CLK_period*7;
		DI <= X"2D";
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		wait for CLK_period*7;
		F0 <= '1';
		DI_RDY <= '1';
		wait for CLK_period;
		DI_RDY <= '0';
		F0 <= '0';
		wait for CLK_period*7;
		for i in test_array3'RANGE loop
			DI <= test_array3(i);
			DI_RDY <= '1';
			F0 <= '0';
			wait for CLK_period;
			DI_RDY <= '0';
			wait for CLK_period*3;
			DI_RDY <= '1';
			F0 <= '1';
			wait for CLK_period;
			DI_RDY <= '0';
			F0 <= '0';
			wait for CLK_period*7;
		end loop;
	wait;
   end process;

END;
