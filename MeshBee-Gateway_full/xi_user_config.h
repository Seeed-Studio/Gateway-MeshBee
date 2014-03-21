#ifndef __XI_USER_CONFIG_H__
#define __XI_USER_CONFIG_H__

// The following settings should lower memory footprint.
// The library currently allows one to send batch datapoint
// and feed updates, but it's not needed in most use cases
#define XI_MAX_DATAPOINTS         1
// The number of channels can be increased if needed
#define XI_MAX_DATASTREAMS        5

// Below are optimisations that reduce some minor functionality
#define XI_OPT_NO_ERROR_STRINGS

// If you wish to enable assertions, set this to 1
#define XI_DEBUG_ASSERT 0
// If you wish to disable debug output, set this to 0
#define XI_DEBUG_OUTPUT 0

// On the mbed app board we can use the LCD for debug output,
// but one may wish to modify this and write to file instead
#include "app_board_io.h"
#define XI_DEBUG_PRINTF lcd_printf //TODO: use serial port...

#endif /* __XI_USER_CONFIG_H__ */
