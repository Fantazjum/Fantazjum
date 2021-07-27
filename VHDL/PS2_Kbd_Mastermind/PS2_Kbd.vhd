----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    00:22:47 04/20/2021 
-- Design Name: 
-- Module Name:    PS2_Kbd - Behavioral 
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

entity PS2_Kbd is
    Port ( PS2_CLK : in  STD_LOGIC;
           PS2_DATA : in  STD_LOGIC; -- Nie wysy³amy danych do klawiatury, wystarcza input pin
           Clk : in  STD_LOGIC;
           DO : out  STD_LOGIC_VECTOR (7 downto 0);
           E0 : out  STD_LOGIC := '0';
           F0 : out  STD_LOGIC := '0';
           DO_RDY : out  STD_LOGIC := '0');
end PS2_Kbd;

architecture Behavioral of PS2_Kbd is

component PS2_RX 
  	  Port ( PS2_CLK : in  STD_LOGIC;
           PS2_DATA : in  STD_LOGIC;
           Clk : in  STD_LOGIC;
           DO : out  STD_LOGIC_VECTOR (7 downto 0);
           DO_RDY : out  STD_LOGIC);
end component PS2_RX;
	
signal val : STD_LOGIC_VECTOR (7 downto 0) := X"00";
signal rdy : STD_LOGIC := '0';	
signal waiting : STD_LOGIC := '0';
signal ps2_c, ps2_d, cl : STD_LOGIC;
	
begin
							
process(Clk)
begin
	if rising_edge(Clk) then
		if rdy = '1' then
			if val = X"F0" then
				F0 <= '1';
				waiting <= '1';
			elsif val = X"E0" then
				E0 <= '1';
				waiting <= '1';
			else 
				DO <= val;
				DO_RDY <= '1';
				waiting <= '0';
			end if;
		elsif waiting = '0' then
			F0 <= '0';
			E0 <= '0';
			DO_RDY <= '0';
		end if;
	end if;
end process;

mod_rx : PS2_RX port map (PS2_CLK => ps2_c, PS2_DATA => ps2_d,
									Clk => cl, DO => val, DO_RDY => rdy);

ps2_c <= PS2_CLK;
ps2_d <= PS2_DATA;
cl <= Clk;

end Behavioral;

