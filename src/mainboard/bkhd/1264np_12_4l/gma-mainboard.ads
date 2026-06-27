with HW.GFX.GMA;
with HW.GFX.GMA.Display_Probing;

use HW.GFX.GMA;
use HW.GFX.GMA.Display_Probing;

package GMA.Mainboard is

   ports : constant Port_List :=
     (HDMI2,     -- DDI B - board HDMI
      HDMI_TC1,  -- FENN (Basti 2026-06-25): soldered DP carries a DP-to-HDMI
                 -- (DP++) adapter -> HDMI sink on the Dekel PHY. i915 lights it
                 -- as HDMI-A-2. The HDMI detect reads EDID via I2C/DDC (not the
                 -- DP AUX) so it does not hit the native-DP AUX timeout.
      DP_TC1,    -- board DP via Type-C/Dekel PHY (DP-1) - for a NATIVE DP sink
      others => Disabled);

end GMA.Mainboard;
