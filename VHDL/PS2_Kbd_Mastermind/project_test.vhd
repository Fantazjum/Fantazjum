-- Vhdl test bench created from schematic R:\Users\Dom\Downloads\Klawiatura\Kl\project.sch - Sun May 30 13:39:28 2021
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
ENTITY test IS
END test;
ARCHITECTURE behavioral OF test IS 

   COMPONENT project
   PORT(  CLK	:	IN	STD_LOGIC; 
          Rst	:	IN	STD_LOGIC; 
          Kbd_CLK	:	IN	STD_LOGIC; 
          Kbd_DATA	:	IN	STD_LOGIC; 
			 HS	:	OUT	STD_LOGIC;
          VS	:	OUT	STD_LOGIC; 
          COLOR	:	OUT	STD_LOGIC);
   END COMPONENT;

   
   SIGNAL CLK	:	STD_LOGIC := '0';
   SIGNAL Rst	:	STD_LOGIC := '0'; 
   SIGNAL Kbd_CLK	:	STD_LOGIC := '0';
   SIGNAL Kbd_DATA	:	STD_LOGIC := '0';
   SIGNAL HS	:	STD_LOGIC;
	SIGNAL VS	:	STD_LOGIC;
   SIGNAL COLOR	:	STD_LOGIC;
	 
	CONSTANT Clk_Period : DELAY_LENGTH := 20 ns;
	 -- Interface to VGAscaner
   component VGAscan
       Generic ( -- VGA mode params (defaults are for 800x600/72Hz)
                 pxWidth    : integer := 800;         -- visible pixels: width
                 pxHeight   : integer := 600;         -- visible pixels: height
                 pxHSfrontP : integer := 56;          -- HSync front porch
                 lnVSfrontP : integer := 37;          -- VSync front porch
                 pxTotal    : integer := 1040;        -- total horiz. pixels (visible & invisible)
                 lnTotal    : integer := 666;         -- total vert. lines (visible & invisible)
                 mhzPixelClk: real    := 50.000;      -- pixel clock
                 activeHS   : std_logic := '1';       -- HSync polarity
                 activeVS   : std_logic := '1';       -- VSync polarity
                 -- Aux params
                 pxBorder   : integer := 2;           -- bitmap border around the visible area
                 FileName   : string := "D:/VGA/Frame"    -- output file name (frame number & ".bmp" will be appended)
               );
       Port ( VS, HS : in  STD_LOGIC;
              R, G, B : in  STD_LOGIC);
   end component;

BEGIN

   UUT: project PORT MAP(
		
		CLK => CLK, 
		Rst => Rst, 
		Kbd_CLK => Kbd_CLK, 
		Kbd_DATA => Kbd_DATA, 
		HS => HS, 
		VS => VS, 
		COLOR => COLOR
   );
	 
	------------------------------------------------------------------
   -- Instance of the VGA scanner
   ------------------------------------------------------------------
   I_VGAscaner: VGAscan
      generic map(
         pxBorder => 3,
         FIleName => "D:/VGA/Frame" )    -- VGAtxt48x20 works in 800x600/72Hz mode => VGA generics can be left with defaults
      port map(
         VS => VS,
         HS => HS,
         R => COLOR,
         G => COLOR,
         B => COLOR );

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
