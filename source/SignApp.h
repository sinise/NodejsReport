#ifndef SIGNAPP_H
#define SIGNAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

/*********************************************************************
 * CONSTANTS
 */

// These constants are only for example and should be changed to the
// device's needs
#define SIGNAPP_ENDPOINT           10

#define SIGNAPP_PROFID             0x0F04
#define SIGNAPP_DEVICEID           0x0001
#define SIGNAPP_DEVICE_VERSION     0
#define SIGNAPP_FLAGS              0

#define SIGNAPP_MAX_CLUSTERS       1
#define SIGNAPP_CLUSTERID          1

// Send Message Timeout
#define SIGNAPP_CHECK_SHIP_INTERVAL   5000     // Every 5 seconds

// Application Events (OSAL) - These are bit weighted definitions.
#define SIGNAPP_CHECK_SHIP_EVT       0x0001

#if defined( IAR_ARMCM3_LM )
#define SIGNAPP_RTOS_MSG_EVT       0x0002
#endif

//static uint16 sleepCount = 0;
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Sign Application
 */
extern void SignApp_Init( byte task_id );

/*
 * Task Event Processor for the Generic Application
 */
extern UINT16 SignApp_ProcessEvent( byte task_id, UINT16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SIGNAPP_H */
