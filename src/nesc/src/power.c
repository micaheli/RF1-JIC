
#include "includes.h"

void InitFets(void)
{
	InitializeGpio(A_FET_LO_GPIO, A_FET_LO_PIN, 0);
	InitializeGpio(B_FET_LO_GPIO, B_FET_LO_PIN, 0);
	InitializeGpio(C_FET_LO_GPIO, C_FET_LO_PIN, 0);
    DelayMs(2);
	InitializeGpio(A_FET_HI_GPIO, A_FET_HI_PIN, 0);
	InitializeGpio(B_FET_HI_GPIO, B_FET_HI_PIN, 0);
	InitializeGpio(C_FET_HI_GPIO, C_FET_HI_PIN, 0);
    DelayMs(2);
    PrechargeBootstrap();
}

inline void FreeWheel(void)
{
    AFetHiOff();
    BFetHiOff();
    CFetHiOff();
    AFetLoOff();
    BFetLoOff();
    CFetLoOff();
}

inline void FullBrake(void)
{
    AFetLoOn();
    BFetLoOn();
    CFetLoOn();
}

void PrechargeBootstrap(void)
{
    AFetLoOff();
    BFetLoOff();
    CFetLoOff();
    DelayMs(5);
    AFetHiOn();
    BFetHiOn();
    CFetHiOn();
    DelayMs(5);
    AFetHiOff();
    BFetHiOff();
    CFetHiOff();
}

inline void AFetHiOff(void)
{
    inlineDigitalLo(A_FET_HI_GPIO, A_FET_HI_PIN);
}

inline void BFetHiOff(void)
{
    inlineDigitalLo(B_FET_HI_GPIO, B_FET_HI_PIN);
}

inline void CFetHiOff(void)
{
    inlineDigitalLo(C_FET_HI_GPIO, C_FET_HI_PIN);
}

inline void AFetHiOn(void)
{
    inlineDigitalHi(A_FET_HI_GPIO, A_FET_HI_PIN);
}

inline void BFetHiOn(void)
{
    inlineDigitalHi(B_FET_HI_GPIO, B_FET_HI_PIN);
}

inline void CFetHiOn(void)
{
    inlineDigitalHi(C_FET_HI_GPIO, C_FET_HI_PIN);
}

inline void AFetLoOff(void)
{
    inlineDigitalLo(A_FET_LO_GPIO, A_FET_LO_PIN);
}

inline void BFetLoOff(void)
{
    inlineDigitalLo(B_FET_LO_GPIO, B_FET_LO_PIN);
}

inline void CFetLoOff(void)
{
    inlineDigitalLo(C_FET_LO_GPIO, C_FET_LO_PIN);
}

inline void AFetLoOn(void)
{
    inlineDigitalHi(A_FET_LO_GPIO, A_FET_LO_PIN);
}

inline void BFetLoOn(void)
{
    inlineDigitalHi(B_FET_LO_GPIO, B_FET_LO_PIN);
}

inline void CFetLoOn(void)
{
    inlineDigitalHi(C_FET_LO_GPIO, C_FET_LO_PIN);
}