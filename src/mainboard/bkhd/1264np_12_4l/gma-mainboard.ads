with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

package GMA.Mainboard is

   ports : constant Port_List :=
     (HDMI1,   -- DDI A
      HDMI2,   -- DDI B  <- board HDMI is here (confirmed via i915: ENCODER DDI B/PHY B)
      HDMI3,   -- DDI C
      DP1,
      DP2,
      DP3,
      others => Disabled);

end GMA.Mainboard;
