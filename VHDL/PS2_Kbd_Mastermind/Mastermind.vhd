----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    20:45:34 05/15/2021 
-- Design Name: 
-- Module Name:    Mastermind - Behavioral 
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

entity Mastermind is
    Port ( DI : in  STD_LOGIC_VECTOR (7 downto 0);
           DI_Rdy : in  STD_LOGIC;
           F0 : in  STD_LOGIC;
           Clk : in  STD_LOGIC;
           Rst : in  STD_LOGIC;
           DO : out  STD_LOGIC_VECTOR (7 downto 0);
           DO_Rdy : out  STD_LOGIC;
			  newline : out STD_LOGIC);
end Mastermind;

architecture Behavioral of Mastermind is

-- 12 guesses && 6 'colors' {A, B, C, D, E, F} && R for in-game reset
signal curr_num, lives : unsigned (3 downto 0);
signal abs_right, rel_right : unsigned (2 downto 0);
signal abs_ascii, rel_ascii, lives_ascii : STD_LOGIC_VECTOR (7 downto 0) := X"30";
signal set_code : STD_LOGIC := '1';
signal rst_counter : unsigned (3 downto 0) := X"F";
signal print_counter : unsigned (3 downto 0) := X"0";
signal code, pos : STD_LOGIC_VECTOR (11 downto 0); --each can use one bit less, but that solution is too complex
type state_type is (idle, processing, reset, done, summary);
signal state : state_type; --, next_state

begin

-- State machines --
--game_state : process(Clk)
--begin
--	if (falling_edge(Clk)) then
--		if (Rst = '1' or rst_counter = X"F") then --reset after 3 sec of helding down R, at startup or manual on module 
--			state <= reset;
--		else
--			state <= next_state;
--		end if;
--	end if;
--end process;

state_machine : process(DI_Rdy, Clk, F0, Rst, rst_counter)
begin
	
	if rising_edge(Clk) then
		if (Rst = '1' or rst_counter = X"F") then --reset after 3 sec of helding down R, at startup or manual on module 
				state <= reset;
		end if;
--	next_state <= state;
		case state is
		when idle =>
			print_counter <= X"0";
			if DI_Rdy = '1' then
				state <= processing;
			end if;
			
		when processing =>
			if DI_Rdy = '1' then
				case DI is
					when X"1C" => --A
						if F0 = '1' then
							if (set_code = '1') then
								code(to_integer(curr_num) downto to_integer(curr_num)-2) <= "000";
							else
								pos(to_integer(curr_num) downto to_integer(curr_num)-2) <= "000";
							end if;
							if (curr_num <= X"2" ) then
								state <= done;
								abs_right <= O"0";
								rel_right <= O"0";
								curr_num <= X"0";
							else
								curr_num <= curr_num - 3;
								state <= idle;
							end if;
						end if;
					when X"32" => --B
						if F0 = '1' then
							if (set_code = '1') then
								code(to_integer(curr_num) downto to_integer(curr_num)-2) <= "001";
							else
								pos(to_integer(curr_num) downto to_integer(curr_num)-2) <= "001";
							end if;
							if (curr_num <= X"2" ) then
								state <= done;
								abs_right <= O"0";
								rel_right <= O"0";
								curr_num <= X"0";
							else
								curr_num <= curr_num - 3;
								state <= idle;
							end if;
						end if;
					when X"21" => --C
						if F0 = '1' then
							if (set_code = '1') then
								code(to_integer(curr_num) downto to_integer(curr_num)-2) <= "010";
							else
								pos(to_integer(curr_num) downto to_integer(curr_num)-2) <= "010";
							end if;
							if (curr_num <= X"2" ) then
								state <= done;
								abs_right <= O"0";
								rel_right <= O"0";
								curr_num <= X"0";
							else
								curr_num <= curr_num - 3;
								state <= idle;
							end if;
						end if;	
					when X"23" => --D
						if F0 = '1' then
							if (set_code = '1') then
								code(to_integer(curr_num) downto to_integer(curr_num)-2) <= "011";
							else
								pos(to_integer(curr_num) downto to_integer(curr_num)-2) <= "011";
							end if;
							if (curr_num <= X"2" ) then
								state <= done;
								abs_right <= O"0";
								rel_right <= O"0";
								curr_num <= X"0";
							else
								curr_num <= curr_num - 3;
								state <= idle;
							end if;
						end if;
					when X"24" => --E
						if F0 = '1' then
							if (set_code = '1') then
								code(to_integer(curr_num) downto to_integer(curr_num)-2) <= "100";
							else
								pos(to_integer(curr_num) downto to_integer(curr_num)-2) <= "100";
							end if;
							if (curr_num <= X"2" ) then
								state <= done;
								abs_right <= O"0";
								rel_right <= O"0";
								curr_num <= X"0";
							else
								curr_num <= curr_num - 3;
								state <= idle;
							end if;
						end if;
					when X"2B" => --F
						if F0 = '1' then
							if (set_code = '1') then
								code(to_integer(curr_num) downto to_integer(curr_num)-2) <= "101";
							else
								pos(to_integer(curr_num) downto to_integer(curr_num)-2) <= "101";
							end if;
							if (curr_num <= X"2" ) then
								state <= done;
								abs_right <= O"0";
								rel_right <= O"0";
								curr_num <= X"0";
							else
								curr_num <= curr_num - 3;
								state <= idle;
							end if;
						end if;
					when X"2D" => --R
						if F0 = '1' then
							rst_counter <= X"0";
							state <= idle;
						else
							rst_counter <= rst_counter+1;
							state <= idle;
						end if;
					when others =>
						if F0 = '1' then
							state <= idle;
						end if;
				end case;
			end if;
			
		when reset =>
			curr_num <= X"B";
			lives <= X"C";
			rst_counter <= X"0";
			set_code <= '1';
			code <= O"0000";
			pos <= O"0000";
			state <= idle;
			
		when done =>
			if (set_code = '1') then
				state <= idle; 
				curr_num <= X"B";
				set_code <= '0';
			else
				case curr_num is 
					when X"C" =>
						if abs_right < X"4" then
							lives <= lives - 1;
						end if;
						curr_num <= X"B"; 
						state <= summary; 
					when X"0" =>
						if pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (2 downto 0) then
							abs_right <= abs_right + 1;
						elsif pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (5 downto 3) or
								pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (8 downto 6) or
								pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (11 downto 9) then
							rel_right <= rel_right + 1;
						end if;
					when X"3" =>
						if pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (5 downto 3) then
							abs_right <= abs_right + 1;
						elsif pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (2 downto 0) or
								pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (8 downto 6) or
								pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (11 downto 9) then
							rel_right <= rel_right + 1;
						end if;
					when X"6" =>
						if pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (8 downto 6) then
							abs_right <= abs_right + 1;
						elsif pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (2 downto 0) or
								pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (5 downto 3) or
								pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (11 downto 9) then
							rel_right <= rel_right + 1;
						end if;
					when X"9" =>
						if pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (11 downto 9) then
							abs_right <= abs_right + 1;
						elsif pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (2 downto 0) or
								pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (5 downto 3) or
								pos(to_integer(curr_num) + 2 downto to_integer(curr_num)) = code (8 downto 6) then
							rel_right <= rel_right + 1;
						end if;
					when others =>
					end case;
					if curr_num /= X"C" then	
						curr_num <= curr_num + 3;
					end if;
			end if;
			
			when summary =>
				if print_counter > X"C" then
					if abs_right = O"4" or lives = X"0" then
						state <= reset;
					else 
						state <= idle;
					end if;
				else 
					print_counter <= print_counter + 1;
				end if;
					
				
		end case;
	end if;
	
	
end process;


print : process (state, DI_Rdy, print_counter, set_code, F0, DI, abs_ascii, rel_ascii, lives, lives_ascii)
begin
	DO<=X"00";
	case state is 
		when processing =>
			if set_code = '1' then
				DO <= X"58";
				if F0 = '1' and DI_Rdy = '1' and (DI = X"1C" or DI = X"32" or DI = X"21" or DI = X"23" or DI = X"24" or DI = X"2B") then
					DO_Rdy <= '1';
				else
					DO_Rdy <= '0';
				end if;
			else
				case DI is
					when X"1C" => DO <= X"41";
					when X"32" => DO <= X"42";
					when X"21" => DO <= X"43";
					when X"23" => DO <= X"44";
					when X"24" => DO <= X"45";
					when X"2B" => DO <= X"46";
					when X"2D" => null;
					when others => DO <= X"58";
				end case;
				if F0 = '1' and DI_Rdy = '1' then
					if DI = X"2D" then
						DO_Rdy <= '0';
					else 
						DO_Rdy <= '1';
					end if;
				else
					DO_Rdy <= '0';
				end if;
			end if;
		when summary =>
			case print_counter is
				when X"0" | X"4" | X"8" => DO <= X"20";
				when X"2" => DO <= abs_ascii;
				when X"6" => DO <= rel_ascii;
				when X"A" => 
					if lives < X"A" then
						DO <= X"30";
					else
						DO <= X"31";
					end if;
				when X"C" => DO <= lives_ascii;
				when others =>
			end case;
			if print_counter(0) = '0' then
				DO_Rdy <= '1';
			else
				DO_Rdy <= '0';
			end if;
		when others =>
			DO_Rdy <= '0';
	end case;
end process;

abs_ascii(2 downto 0) <= std_logic_vector(abs_right);
rel_ascii(2 downto 0) <= std_logic_vector(rel_right);

with lives select lives_ascii(3 downto 0) <=
	X"0" when X"A",
	X"1" when X"B",
	X"2" when X"C",
	std_logic_vector(lives) when others;

newline <= '1' when (print_counter = X"D" and state = summary) or (set_code = '1' and state = done)
		else '0';

end Behavioral;


