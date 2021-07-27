-- Vhdl test bench created from schematic R:\Users\Dom\Documents\STUDIA\UCISW\Klawiatura\sch1.sch - Sat May 15 21:31:15 2021
--
-- Notes: 
-- 1) This testbench template has been automatically generated using types
-- std_logic and std_logic_vector for the ports of the unit under test.
-- Xilinx recommends that these types always be used for the top-level
-- I/O of a design in order to guarantee that the testbench will bind
-- correctly to the timing (post-route) simulation model.
-- 2) To use this template as your testbench, change the filename to any
-- name of your choice with the extension .vhd, and use the "Source->Add"
-- menu in Project Navigator to import the testbench. Then
-- edit the user defined section below, adding code to generate the 
-- stimulus for your design.
--
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
LIBRARY UNISIM;
USE UNISIM.Vcomponents.ALL;
ENTITY sch1_sch1_sch_tb IS
END sch1_sch1_sch_tb;
ARCHITECTURE behavioral OF sch1_sch1_sch_tb IS 

   COMPONENT sch1
   PORT( PS2_CLK	:	IN	STD_LOGIC; 
          PS2_Data	:	IN	STD_LOGIC; 
          Clk	:	IN	STD_LOGIC; 
          DO_Rdy	:	OUT	STD_LOGIC; 
          DO	:	OUT	STD_LOGIC_VECTOR (7 DOWNTO 0));
   END COMPONENT;

   SIGNAL PS2_CLK	:	STD_LOGIC;
   SIGNAL PS2_Data	:	STD_LOGIC;
   SIGNAL Clk	:	STD_LOGIC;
   SIGNAL DO_Rdy	:	STD_LOGIC;
   SIGNAL DO	:	STD_LOGIC_VECTOR (7 DOWNTO 0);

	constant Clk_period : time := 20 ns;

BEGIN

   UUT: sch1 PORT MAP(
		PS2_CLK => PS2_CLK, 
		PS2_Data => PS2_Data, 
		Clk => Clk, 
		DO_Rdy => DO_Rdy, 
		DO => DO
   );

-- *** Test Bench - User Defined Section ***
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
			PS2_Data <= Frame( i );
			wait for 5 us;
			PS2_Clk <= '0', '1' after 50 us;
			wait for 95 us;
		  end loop;
		end procedure;
	BEGIN
		wait for 15 us;
		TransmPS2( X"1C" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"1C" );
		wait for 200 us;
		TransmPS2( X"32" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"32" );
		wait for 200 us;
		TransmPS2( X"21" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"21" );
		wait for 200 us;
		TransmPS2( X"23" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"23" );
		wait; -- will wait forever
	END PROCESS;
-- *** End Test Bench - User Defined Section ***

END;
