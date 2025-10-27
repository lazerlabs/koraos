#pragma once

#include "common.h"

void uart_init(void);
void uart_putc(unsigned char c);
unsigned char uart_getc(void);
