#ifndef SRC_RESETSOURCE_H_
#define SRC_RESETSOURCE_H_
    enum ResetCause {
        RESET_CAUSE_UNKNOWN = 0,
        RESET_CAUSE_LOW_POWER_RESET,
        RESET_CAUSE_WINDOW_WATCHDOG_RESET,
        RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
        RESET_CAUSE_SOFTWARE_RESET,
        RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
        RESET_CAUSE_EXTERNAL_RESET_PIN_RESET,
        RESET_CAUSE_BROWNOUT_RESET,
    };

    enum ResetCause resetCauseGet(void);
    const char *resetCauseGetName(enum ResetCause cause);
#endif /* SRC_RESETSOURCE_H_ */
