//     $Date: 2018-03-26 08:32:18 +1100 (Mon, 26 Mar 2018) $
// $Revision: 1217 $
//   $Author: Peter $

#include "Ass-02.h"

//
// PLACE YOUR COMMON CODE HERE 
//

// STEPIEN: Added two touch panel functions to make the interface more
//          consistent with the LCD BSP.

#ifdef STM32F407xx
uint8_t BSP_TP_Init(void)
{
  // Initialise the interface and calibrate
  TP_Init(); // This is an empty function since done by STM32CubeMX
  TouchPanel_Calibrate();

  return 0;
}

uint8_t BSP_TP_GetDisplayPoint(Coordinate *pDisplay)
{
  Coordinate *pScreen;

  pScreen = Read_Ads7846();
  if (pScreen == NULL)
  {
    return 1; // Error reading the touch panel
  }
  if (getDisplayPoint(pDisplay, pScreen, &matrix ) == DISABLE)
  {
    return 1; // Error in LCD
  }
  return 0;
}
#endif
