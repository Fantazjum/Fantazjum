----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:52:13 05/16/2021 
-- Design Name: 
-- Module Name:    WhiteBox - Behavioral 
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

entity WhiteBox is
    Port ( Kbd_CLK : in  STD_LOGIC;
           Kbd_DATA : in  STD_LOGIC;
           CLK : in  STD_LOGIC;
           Rst : in  STD_LOGIC;
           DO : out  STD_LOGIC_VECTOR (7 downto 0);
           DO_RDY : out  STD_LOGIC;
           newline : out  STD_LOGIC);
end WhiteBox;

architecture Behavioral of WhiteBox is

component PS2_Kbd 
  	  Port ( PS2_CLK : in  STD_LOGIC;
           PS2_DATA : in  STD_LOGIC;
           Clk : in  STD_LOGIC;
           DO : out  STD_LOGIC_VECTOR (7 downto 0);
			  E0 : out	STD_LOGIC;
			  F0 : out	STD_LOGIC;
           DO_RDY : out  STD_LOGIC);
end component PS2_Kbd;

component Mastermind
		Port ( DI : in  STD_LOGIC_VECTOR (7 downto 0);
           DI_Rdy : in  STD_LOGIC;
           F0 : in  STD_LOGIC;
           Clk : in  STD_LOGIC;
           Rst : in  STD_LOGIC;
           DO : out  STD_LOGIC_VECTOR (7 downto 0);
           DO_Rdy : out  STD_LOGIC;
			  newline : out STD_LOGIC);
end component Mastermind;
	
signal DT, CO : STD_LOGIC_VECTOR (7 downto 0);
signal rdyT : STD_LOGIC := '0';
signal rdyO : STD_LOGIC := '0';	
signal F0T : STD_LOGIC := '0';
signal nl : STD_LOGIC := '0';
signal gRst : STD_LOGIC := '0';
signal ps2_c, ps2_d, cl : STD_LOGIC;

begin


mod_kbd : PS2_Kbd port map(PS2_CLK => ps2_c, PS2_DATA => ps2_d,
									Clk => cl, DO => DT, DO_RDY => rdyT, F0 => F0T);
									
mod_msm : Mastermind port map(DI => DT, DI_Rdy => rdyT, F0 => F0T,
										Clk => cl, Rst => gRst, DO => CO,
										DO_Rdy => rdyO, newline => nl);

ps2_c <= Kbd_CLK;
ps2_d <= Kbd_DATA;
cl <= CLK;
gRst <= Rst;
newline <= nl;
DO <= CO;
DO_Rdy <= rdyO;

end Behavioral;

