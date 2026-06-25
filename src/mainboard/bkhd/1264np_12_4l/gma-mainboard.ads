with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

package GMA.Mainboard is

   ports : constant Port_List :=
     (HDMI2,    -- DDI B - board HDMI
      DP_TC1,   -- board DP via Type-C/Dekel PHY (DP-1), Nico's TC stack
      others => Disabled);

end GMA.Mainboard;
