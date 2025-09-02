#include "gpio.h"
#include "utils.h"

void gpio_pin_set_func(uint8_t pinNumber, GpioFunc func) {
    uint8_t bitStart = (pinNumber * 3) % 30;
    uint8_t reg = pinNumber / 10;

    uint32_t selector = REGS_GPIO->func_select[reg];
    selector &= ~(7 << bitStart);
    selector |= (func << bitStart);

    REGS_GPIO->func_select[reg] = selector;
}

void gpio_pin_enable(uint8_t pinNumber) {
    REGS_GPIO->pupd_enable = 0;
    delay(150);
    REGS_GPIO->pupd_enable_clocks[pinNumber / 32] = 1 << (pinNumber % 32);
    delay(150);
    REGS_GPIO->pupd_enable = 0;
    REGS_GPIO->pupd_enable_clocks[pinNumber / 32] = 0;
}
