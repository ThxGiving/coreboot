with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

package GMA.Mainboard is

   ports : constant Port_List :=
     (HDMI2,    -- DDI B (gmbus dpb) - board HDMI, confirmed via i915 (ENCODER DDI B/PHY B)
      DP_TC1,   -- USB-C PHY / DDI TC1 - board DP routed through the Type-C PHY
      USBC1,    -- same lanes as DP-Alt-Mode, in case the sink enumerates as USB-C
      others => Disabled);

end GMA.Mainboard;
