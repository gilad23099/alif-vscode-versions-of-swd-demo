#include "RTE_Components.h"
#include CMSIS_device_header

#include "Driver_GPIO.h"
#include "board.h"
#include "pinconf.h"

// define the port and pin numbers for the button
#define GPIO15_PORT 15
#define PIN4 4
#define PIN3 3

#define _GET_DRIVER_REF(ref, peri, chan)          \
    extern ARM_DRIVER_##peri Driver_##peri##chan; \
    static ARM_DRIVER_##peri *ref = &Driver_##peri##chan;
#define GET_DRIVER_REF(ref, peri, chan) _GET_DRIVER_REF(ref, peri, chan)

GET_DRIVER_REF(gpio_b, GPIO, BOARD_LEDRGB0_B_GPIO_PORT);
GET_DRIVER_REF(gpio_r, GPIO, BOARD_LEDRGB0_R_GPIO_PORT);
GET_DRIVER_REF(gpio_g, GPIO, BOARD_LEDRGB0_G_GPIO_PORT);
GET_DRIVER_REF(gpio_r2, GPIO, BOARD_LEDRGB1_R_GPIO_PORT);
GET_DRIVER_REF(gpio_button_down, GPIO, BOARD_BUTTON1_GPIO_PORT);
GET_DRIVER_REF(gpio_button_click, GPIO, BOARD_BUTTON2_GPIO_PORT);

void initialize_pins(void)
{
    gpio_r->Initialize(BOARD_LEDRGB0_R_PIN_NO, NULL);
    gpio_r->PowerControl(BOARD_LEDRGB0_R_PIN_NO, ARM_POWER_FULL);
    gpio_r->SetDirection(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
    gpio_r->SetValue(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);

    gpio_g->Initialize(BOARD_LEDRGB0_G_PIN_NO, NULL);
    gpio_g->PowerControl(BOARD_LEDRGB0_G_PIN_NO, ARM_POWER_FULL);
    gpio_g->SetDirection(BOARD_LEDRGB0_G_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
    gpio_g->SetValue(BOARD_LEDRGB0_G_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);

    gpio_b->Initialize(BOARD_LEDRGB0_B_PIN_NO, NULL);
    gpio_b->PowerControl(BOARD_LEDRGB0_B_PIN_NO, ARM_POWER_FULL);
    gpio_b->SetDirection(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
    gpio_b->SetValue(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);

    // initialize the second RGB red LED
    gpio_r2->Initialize(BOARD_LEDRGB1_R_PIN_NO, NULL);
    gpio_r2->PowerControl(BOARD_LEDRGB1_R_PIN_NO, ARM_POWER_FULL);
    gpio_r2->SetDirection(BOARD_LEDRGB1_R_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
    gpio_r2->SetValue(BOARD_LEDRGB1_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);

    // Set P15_4 to GPIO mode with pull-up
    pinconf_set(GPIO15_PORT, PIN4,
                PINMUX_ALTERNATE_FUNCTION_7, // GPIO
                PADCTRL_DRIVER_DISABLED_PULL_UP | PADCTRL_READ_ENABLE);

    // Initialize and set as input
    gpio_button_click->Initialize(BOARD_BUTTON2_PIN_NO, NULL);
    gpio_button_click->PowerControl(BOARD_BUTTON2_PIN_NO, ARM_POWER_FULL);
    gpio_button_click->SetDirection(BOARD_BUTTON2_PIN_NO, GPIO_PIN_DIRECTION_INPUT);


    // Set P15_3 to GPIO mode with pull-up
    pinconf_set(GPIO15_PORT, PIN3,
                PINMUX_ALTERNATE_FUNCTION_7, // GPIO
                PADCTRL_DRIVER_DISABLED_PULL_UP | PADCTRL_READ_ENABLE);

    // Initialize and set as input
    gpio_button_down->Initialize(BOARD_BUTTON1_PIN_NO, NULL);
    gpio_button_down->PowerControl(BOARD_BUTTON1_PIN_NO, ARM_POWER_FULL);
    gpio_button_down->SetDirection(BOARD_BUTTON1_PIN_NO, GPIO_PIN_DIRECTION_INPUT);
}

void delay(uint32_t delay)
{
    volatile uint32_t i;
    for (i = 0; i < delay; i++)
    {
        __NOP();
    }
}

void clock_cycle(void)
{
    gpio_g->SetValue(BOARD_LEDRGB0_G_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
    delay(1000000);
    gpio_g->SetValue(BOARD_LEDRGB0_G_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
    delay(1000000);
}

bool is_button_click_pressed(void)
{
    uint32_t val = 1;
    gpio_button_click->GetValue(BOARD_BUTTON2_PIN_NO, &val);
    return (val == 0); // Active-low logic
}

bool is_button_down_pressed(void)
{
    uint32_t val = 1;
    gpio_button_click->GetValue(BOARD_BUTTON1_PIN_NO, &val);
    return (val == 0); // Active-low logic
}

void swdio_send_one_to_blue(bool is_pressed)
{
    gpio_b->SetValue(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
    delay(1000000);
    gpio_b->SetValue(BOARD_LEDRGB0_B_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
}

void swdio_send_one_to_red(bool is_pressed)
{
    gpio_r2->SetValue(BOARD_LEDRGB1_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
    delay(1000000);
    gpio_r2->SetValue(BOARD_LEDRGB1_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
}

int main(void)
{
    // Initialize the board
    initialize_pins();
    
    bool is_button_click=false;
    bool is_button_down=false;

    while(1)
    {
        // swdio input from "host"
        is_button_click = is_button_click_pressed();
        is_button_down = is_button_down_pressed();
        if(is_button_click){

            // one clock cycle for the target to get the data and output it
            clock_cycle();
            
            // swdio output 1 in RGB led, blue light, D10
            swdio_send_one_to_blue(1);
        }
        else if (is_button_down)
        {
            // one clock cycle for the target to get the data and output it
            clock_cycle();
            
            // swdio output 1 in RGB led, red light, D11
            swdio_send_one_to_red(1);
        }

    }



#ifdef CORE_M55_HE
    SysTick_Config(SystemCoreClock / 10);
#else
    SysTick_Config(SystemCoreClock / 25);
#endif

    while (1)
        __WFI();
}

void SysTick_Handler(void)
{
#ifdef CORE_M55_HE
#else
    gpio_r->SetValue(BOARD_LEDRGB0_R_PIN_NO, GPIO_PIN_OUTPUT_STATE_TOGGLE);
#endif
}

// Stubs to suppress missing stdio definitions for nosys
#define TRAP_RET_ZERO \
    {                 \
        __BKPT(0);    \
        return 0;     \
    }
int _close(int val) TRAP_RET_ZERO
    int _lseek(int val0, int val1, int val2) TRAP_RET_ZERO
    int _read(int val0, char *val1, int val2) TRAP_RET_ZERO
    int _write(int val0, char *val1, int val2) TRAP_RET_ZERO