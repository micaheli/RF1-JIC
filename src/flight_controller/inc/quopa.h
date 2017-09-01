#pragma once

#define QUOPA_FLIGHT_LIMIT 5000

typedef enum quopa_state_t
{
    QUOPA_INACTIVE = 0,
    QUOPA_INIT     = 1,
    QUOPA_ACTIVE   = 2,
    QUOPA_FAILED   = 3,
} quopa_state;


extern volatile quopa_state quopaState;

extern int InitQuopaMode(void);
extern int StartQuopaMode(void);
extern int CleanupQuopaMode(void);
extern int HandleQuopaMode(void);