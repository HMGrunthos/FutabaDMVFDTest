#include "main.h"

#include "ResetSource.h"

enum ResetCause resetCauseGet(void)
{
    enum ResetCause cause;

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {
    	cause = RESET_CAUSE_LOW_POWER_RESET;
    } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {
    	cause = RESET_CAUSE_WINDOW_WATCHDOG_RESET;
    } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
    	cause = RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET;
    } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
        // This reset is induced by calling the ARM CMSIS
        // `NVIC_SystemReset()` function!
    	cause = RESET_CAUSE_SOFTWARE_RESET;
    } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {
    	cause = RESET_CAUSE_POWER_ON_POWER_DOWN_RESET;
    } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {
    	cause = RESET_CAUSE_EXTERNAL_RESET_PIN_RESET;
/*    } else if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST)) {
		// Needs to come *after* checking the `RCC_FLAG_PORRST` flag in order to
		// ensure first that the reset cause is NOT a POR/PDR reset. See note
		// below.
    	cause = RESET_CAUSE_BROWNOUT_RESET;*/
    } else {
    	cause = RESET_CAUSE_UNKNOWN;
    }

    // Clear all the reset flags or else they will remain set during future
    // resets until system power is fully removed.
    __HAL_RCC_CLEAR_RESET_FLAGS();

    return cause;
}

// Note: any of the STM32 Hardware Abstraction Layer (HAL) Reset and Clock
// Controller (RCC) header files, such as
// "STM32Cube_FW_F7_V1.12.0/Drivers/STM32F7xx_HAL_Driver/Inc/stm32f7xx_hal_rcc.h",
// "STM32Cube_FW_F2_V1.7.0/Drivers/STM32F2xx_HAL_Driver/Inc/stm32f2xx_hal_rcc.h",
// etc., indicate that the brownout flag, `RCC_FLAG_BORRST`, will be set in
// the event of a "POR/PDR or BOR reset". This means that a Power-On Reset
// (POR), Power-Down Reset (PDR), OR Brownout Reset (BOR) will trip this flag.
// See the doxygen just above their definition for the
// `__HAL_RCC_GET_FLAG()` macro to see this:
//      "@arg RCC_FLAG_BORRST: POR/PDR or BOR reset." <== indicates the Brownout
//      Reset flag will *also* be set in the event of a POR/PDR.
// Therefore, you must check the Brownout Reset flag, `RCC_FLAG_BORRST`, *after*
// first checking the `RCC_FLAG_PORRST` flag in order to ensure first that the
// reset cause is NOT a POR/PDR reset.


/// @brief      Obtain the system reset cause as an ASCII-printable name string
///             from a reset cause type
/// @param[in]  reset_cause     The previously-obtained system reset cause
/// @return     A null-terminated ASCII name string describing the system
///             reset cause
const char *resetCauseGetName(enum ResetCause cause)
{
    const char *causeName = "TBD";

    switch(cause) {
        case RESET_CAUSE_UNKNOWN:
        	causeName = "UNKNOWN";
            break;
        case RESET_CAUSE_LOW_POWER_RESET:
        	causeName = "LOW_POWER_RESET";
            break;
        case RESET_CAUSE_WINDOW_WATCHDOG_RESET:
        	causeName = "WINDOW_WATCHDOG_RESET";
            break;
        case RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET:
        	causeName = "INDEPENDENT_WATCHDOG_RESET";
            break;
        case RESET_CAUSE_SOFTWARE_RESET:
        	causeName = "SOFTWARE_RESET";
            break;
        case RESET_CAUSE_POWER_ON_POWER_DOWN_RESET:
        	causeName = "POWER-ON_RESET (POR) / POWER-DOWN_RESET (PDR)";
            break;
        case RESET_CAUSE_EXTERNAL_RESET_PIN_RESET:
        	causeName = "EXTERNAL_RESET_PIN_RESET";
            break;
        case RESET_CAUSE_BROWNOUT_RESET:
        	causeName = "BROWNOUT_RESET (BOR)";
            break;
    }

    return causeName;
}
