-- Vhdl test bench created from schematic R:\Users\Dom\Downloads\Klawiatura\Kl\project.sch - Mon May 17 16:53:57 2021
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

-- grupa 4
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
LIBRARY UNISIM;
USE UNISIM.Vcomponents.ALL;
ENTITY project_project_sch_tb IS
END project_project_sch_tb;
ARCHITECTURE behavioral OF project_project_sch_tb IS 

   COMPONENT project
   PORT(	CLK	:	IN	STD_LOGIC; 
          Rst	:	IN	STD_LOGIC; 
          Kbd_CLK	:	IN	STD_LOGIC; 
          Kbd_DATA	:	IN	STD_LOGIC
			 );
   END COMPONENT;

   SIGNAL CLK	:	STD_LOGIC := '0';
   SIGNAL Rst	:	STD_LOGIC := '0'; 
   SIGNAL Kbd_CLK	:	STD_LOGIC := '0';
   SIGNAL Kbd_DATA	:	STD_LOGIC := '0';
	
	CONSTANT Clk_Period : DELAY_LENGTH := 20 ns;

BEGIN

   UUT: project PORT MAP(
		CLK => CLK, 
		Rst => Rst, 
		Kbd_CLK => Kbd_CLK, 
		Kbd_DATA => Kbd_DATA
   );

-- *** Test Bench - User Defined Section ***
    -- Clock wave
   Clk <= not Clk after Clk_Period / 2;

   -- Inputs
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
		wait for 15 us;
		TransmPS2( X"2B" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"2B" ); --9
		wait for 15 us;
		TransmPS2( X"2B" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"2B" ); --10
		wait for 15 us;
		TransmPS2( X"2B" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"2B" ); --11
		wait for 15 us;
		TransmPS2( X"2B" );
		wait for 200 us;
		TransmPS2( X"F0" );
		wait for 200 us;
		TransmPS2( X"2B" ); --12
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
		
		wait; -- will wait forever
	END PROCESS;

-- *** End Test Bench - User Defined Section ***

END;
