----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    00:22:09 04/20/2021 
-- Design Name: 
-- Module Name:    PS2_RX - Behavioral 
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
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity PS2_RX is
    Port ( PS2_CLK : in  STD_LOGIC;
           PS2_DATA : in  STD_LOGIC;
           Clk : in  STD_LOGIC;
           DO : out  STD_LOGIC_VECTOR (7 downto 0);
           DO_RDY : out  STD_LOGIC);
end PS2_RX;

architecture Behavioral of PS2_RX is

signal rx_clk : STD_LOGIC_VECTOR(1 downto 0) := "11";
signal rx_data :STD_LOGIC_VECTOR(10 downto 0) := "00000000000";
signal mod11_count : unsigned(3 downto 0) := "0000";
signal parity_check : STD_LOGIC := '0';
type state_type is (idle, test, rx_ok);
signal state, next_state : state_type;

begin

--Forcing system to use keyboard clock 
timing : process(Clk)
begin
	if rising_edge(Clk) then
		rx_clk(1) <= PS2_CLK;
		rx_clk(0) <= rx_clk(1);
	end if;
end process;

parity_check <= not( rx_data(8) xor rx_data(7) xor rx_data(6) xor
						rx_data(5) xor rx_data(4) xor rx_data(3) xor
						rx_data(2) xor rx_data(1) );
						
--Counter mod11, for getting input data in 11b shift register
licznik : process(Clk, state)
begin
   if(rising_edge(Clk)) then
      if(state  = test) then
         mod11_count <= "0000";
      elsif(rx_clk(1) = '0' and rx_clk(0) = '1') then
         mod11_count <= mod11_count + 1;  
       end if;
    end if;
end process;

--11 bit shift register, for data input
input : process(Clk)
begin
   if(rising_edge(Clk)) then
		if(rx_clk = "01") then
			rx_data(9 downto 0) <= rx_data(10 downto 1);
			rx_data(10) <= PS2_Data;
		end if;
   end if;
end process;

--State machines--
rx_state : process(Clk)
begin
   if(rising_edge(Clk)) then
      state <= next_state;
   end if;
end process;


state_machine : process(state, mod11_count, rx_data, parity_check)
begin
   next_state <= state;
   case state is
      when idle =>
         if(mod11_count = "1011") then
            next_state <= test;
         end if;
      when test =>
         if(rx_data(0) = '0' and rx_data(9) = parity_check and rx_data(10) = '1') then
				next_state <= rx_ok;
         else
            next_state <= idle;
			end if;
      when rx_ok =>
         next_state <= idle;
   end case;
end process;

DO_Rdy <= '1' when state = rx_ok
   else '0';

DO <= rx_data(8 downto 1);

end Behavioral;

