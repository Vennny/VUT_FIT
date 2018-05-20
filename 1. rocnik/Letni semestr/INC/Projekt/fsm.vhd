-- fsm.vhd: Finite State Machine
-- Author(s): Vaclav Trampeska (xtramp00)
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST_1, TEST_2, TEST_3, TEST_4, TEST_5, TEST_6, TEST_7, TEST_8_1, TEST_9_1, TEST_8_2, TEST_9_2, TEST_10_2, PRINT_FAIL, PRINT_SUC, FINISH, FAIL, FINAL);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST_1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_1 =>
      next_state <= TEST_1;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(9) = '1') then 
	 next_state <= TEST_2;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_2 =>
      next_state <= TEST_2;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(8) = '1') then 
	 next_state <= TEST_3;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_3 =>
      next_state <= TEST_3;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(6) = '1') then 
	 next_state <= TEST_4;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_4 =>
      next_state <= TEST_4;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(8) = '1') then 
	 next_state <= TEST_5;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_5 =>
      next_state <= TEST_5;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(0) = '1') then 
	 next_state <= TEST_6;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_6 =>
      next_state <= TEST_6;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(1) = '1') then 
	 next_state <= TEST_7;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_7 =>
      next_state <= TEST_7;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(4) = '1') then 
	 next_state <= TEST_8_1;
      elsif (KEY(5) = '1') then 
	 next_state <= TEST_8_2;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
      -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_8_1 =>
      next_state <= TEST_8_1;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(6) = '1') then 
	 next_state <= TEST_9_1;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
      -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_9_1 =>
      next_state <= TEST_9_1;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(7) = '1') then 
	 next_state <= FINAL;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
      -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_8_2 =>
      next_state <= TEST_8_2;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(1) = '1') then 
	 next_state <= TEST_9_2;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
      -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_9_2 =>
      next_state <= TEST_9_2;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(3) = '1') then 
	 next_state <= TEST_10_2;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
      -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_10_2 =>
      next_state <= TEST_10_2;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL;
      elsif (KEY(6) = '1') then 
	 next_state <= FINAL;
      elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_FAIL =>
      next_state <= PRINT_FAIL;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_SUC =>
      next_state <= PRINT_SUC;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST_1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FAIL =>
      next_state <= FAIL;
      if (KEY(15) = '1') then
         next_state <= PRINT_FAIL; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINAL =>
      next_state <= FINAL;
      if (KEY(15) = '1') then
         next_state <= PRINT_SUC;
   elsif KEY(15 downto 0) /= "0000000000000000" then
	 next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST_1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_FAIL =>
      FSM_MX_MEM     <= '0';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_SUC =>
      FSM_MX_MEM     <= '1';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;