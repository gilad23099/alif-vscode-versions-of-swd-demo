#include "RTE_Components.h"
#include CMSIS_device_header

#include "Driver_GPIO.h"

#define GET_DRIVER_REF(ref, peri, chan) \
    extern ARM_DRIVER_##peri Driver_##peri##chan; \
    static ARM_DRIVER_##peri * ref = &Driver_##peri##chan;

GET_DRIVER_REF(gpio, GPIO, 12);

void main (void)
{
    gpio->Initialize(0, NULL);
    gpio->PowerControl(0, ARM_POWER_FULL);
    gpio->SetDirection(0, GPIO_PIN_DIRECTION_OUTPUT);
    gpio->SetValue(0, GPIO_PIN_OUTPUT_STATE_LOW);

    gpio->Initialize(3, NULL);
    gpio->PowerControl(3, ARM_POWER_FULL);
    gpio->SetDirection(3, GPIO_PIN_DIRECTION_OUTPUT);
    gpio->SetValue(3, GPIO_PIN_OUTPUT_STATE_LOW);

#ifdef CORE_M55_HE
    SysTick_Config(SystemCoreClock/10);
#else
    SysTick_Config(SystemCoreClock/25);
#endif

    while (1) __WFI();
}

void SysTick_Handler (void)
{
#ifdef CORE_M55_HE
    gpio->SetValue(0, GPIO_PIN_OUTPUT_STATE_TOGGLE);
#else
    gpio->SetValue(3, GPIO_PIN_OUTPUT_STATE_TOGGLE);
#endif
}

// Stubs to suppress missing stdio definitions for nosys
#define TRAP_RET_ZERO  {__BKPT(0); return 0;}
int _close(int val) TRAP_RET_ZERO
int _lseek(int val0, int val1, int val2) TRAP_RET_ZERO
int _read(int val0, char * val1, int val2) TRAP_RET_ZERO
int _write(int val0, char * val1, int val2) TRAP_RET_ZERO