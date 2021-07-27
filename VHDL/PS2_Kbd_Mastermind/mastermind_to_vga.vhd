----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    16:20:33 05/17/2021 
-- Design Name: 
-- Module Name:    mastermind_to_vga - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity mastermind_to_vga is
    Port ( Kbd_CLK : in  STD_LOGIC;
           Kbd_DATA : in  STD_LOGIC;
           CLK : in  STD_LOGIC;
           Rst : in  STD_LOGIC;
           VGA_HS : out  STD_LOGIC;
           VGA_VS : out  STD_LOGIC;
           VGA_RGB : out  STD_LOGIC);
end mastermind_to_vga;

architecture Behavioral of mastermind_to_vga is

	component WhiteBox
		Port ( Kbd_CLK : in  STD_LOGIC;
				  Kbd_DATA : in  STD_LOGIC;
				  CLK : in  STD_LOGIC;
				  Rst : in  STD_LOGIC;
				  DO : out  STD_LOGIC_VECTOR (7 downto 0);
				  DO_RDY : out  STD_LOGIC;
				  newline : out  STD_LOGIC);
	end component WhiteBox;

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
		
	component VGAtxt48x20 is
	  port (
		 VGA_HS : out STD_LOGIC; 
		 Home : in STD_LOGIC := 'X'; 
		 CursorOn : in STD_LOGIC := 'X'; 
		 Busy : out STD_LOGIC; 
		 VGA_VS : out STD_LOGIC; 
		 Char_WE : in STD_LOGIC := 'X'; 
		 Goto00 : in STD_LOGIC := 'X'; 
		 Clk_Sys : in STD_LOGIC := 'X'; 
		 NewLine : in STD_LOGIC := 'X'; 
		 ScrollEn : in STD_LOGIC := 'X'; 
		 VGA_RGB : out STD_LOGIC; 
		 ScrollClear : in STD_LOGIC := 'X'; 
		 Clk_50MHz : in STD_LOGIC := 'X'; 
		 Char_DI : in STD_LOGIC_VECTOR ( 7 downto 0 ) 
	  );
	end component VGAtxt48x20;
	
	signal Data : STD_LOGIC_VECTOR (7 downto 0);
	signal rdyO : STD_LOGIC := '0';
	signal nl : STD_LOGIC := '0';
	signal gRst : STD_LOGIC := '0';
	signal ps2_c, ps2_d, cl : STD_LOGIC;
	
--	mod_wb : WhiteBox port map(Kbd_CLK => ps2_c, Kbd_DATA => ps2_d,
--									 Clk => cl ,Rst => gRst, DO => Data, 
--									 DO_Rdy => rdyO, newline => nl);
									
	--mod_VGA : VGAtxt48x20 port map(
	--								VGA_HS ,
	--								Home ,
	--								CursorOn , 
	--								Busy, 
	--								VGA_VS ,
	--								Char_WE, 
	--								Goto00 ,
	--								Clk_Sys,
	--								NewLine ,
	--								ScrollEn ,
	--								VGA_RGB ,
	---								ScrollClear ,
		--							Clk_50MHz , 
		--							Char_DI );
--
--	ps2_c <= Kbd_CLK;
--	ps2_d <= Kbd_DATA;
--	cl <= CLK;
--	gRst <= Rst;
	


begin


end Behavioral;

