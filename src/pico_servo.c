// MIT License

// Copyright (c) 2021 Patrick Roche

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include "pico_servo.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"

#include "fixmath.h"

#include <string.h>

#define KILO 1e3
#define MICRO 1e-6
#define WRAP 10000
#define PWM_FREQ 50 // PWM frequency in hertz

static float clkdiv;
static uint min;
static uint max;

static int slice_map[30];
static uint slice_active[8];
static uint servo_pos[32];
static uint servo_pos_buf[16];
static pwm_config slice_cfg[8];

static uint min_us = 500;
static uint max_us = 2500;
static fix16_t us_per_unit = 0.f;

static void wrap_cb(void)
{
    uint offset;

    for (int i = 0; i < 8; ++i)
    {
        if (slice_active[i] == 0)
	{
            continue;
        }

        pwm_clear_irq(i);
        offset = 16 * ((servo_pos_buf[i + 0] + 1) % 2);
        pwm_set_chan_level(i, 0, servo_pos[offset + i + 0]);

        offset = 16 * ((servo_pos_buf[i + 1] + 1) % 2);
        pwm_set_chan_level(i, 1, servo_pos[offset + i + 1]);
    }
}

/**
 * @brief Set min and max microseconds.
 * 
 * Set min and max microseconds.
 * 
 * @param a the min time in microseconds
 * @param b the max time in microseconds
 */
void servo_set_bounds(uint a, uint b)
{
    min_us = a;
    max_us = b;
    if (fix16_to_float(us_per_unit) > 0.0f)
    {
        min = min_us / us_per_unit;
	    max = max_us / us_per_unit;
    }
}

/**
 * @brief Set up the servo system.
 * 
 * Attach IRQ handler, allocate and initialize memory. * 
 * 
 */
int servo_init(void)
{
    for (int i = 0; i < 30; ++i)
    {
        slice_map[i] = -1;
    }
    memset(slice_active, 0, 8 * sizeof(uint));
    memset(servo_pos, 0, 32 * sizeof(uint));
    memset(servo_pos_buf, 0, 16 * sizeof(uint));

    irq_add_shared_handler(PWM_IRQ_WRAP, wrap_cb, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);

    return 0;
}

/**
 * @brief Reference the primary clock.
 * 
 * Set the clock source to CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY
 * 
 */
int servo_clock_auto(void)
{
    return servo_clock_source(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
}

/**
 * @brief Specify the clock source.
 *
 * Specify the clock source. 
 * 
 */
int servo_clock_source(uint src)
{
    clkdiv = (float)frequency_count_khz(src) * (float)KILO / (PWM_FREQ * WRAP);
    if (clkdiv == 0)
    {
        return 1;
    }
    us_per_unit = 1.f / (PWM_FREQ * WRAP) / MICRO;

    min = min_us / us_per_unit;
    max = max_us / us_per_unit;

    return 0;
}

/**
 * @brief Probably don't use this
 *
 * Not a good idea
 * 
 */
int servo_clock_manual(uint freq)
{
    clkdiv = (float)(freq * KILO) / (float)(PWM_FREQ * WRAP);
    if (clkdiv == 0)
    {
        return 1;
    }
    min = 0.025f * (float)WRAP;
    max = 0.125f * (float)WRAP;

    return 0;
}

/**
 * @brief Allocate a pin to controlling a servo
 *
 * Binds the specified pin to PWM output.
 * 
 * @param pin The pin to make PWM output.
 */
int servo_attach(uint pin)
{
    uint slice = pwm_gpio_to_slice_num(pin);
    if (slice_active[slice] >= 2)
    {
        return 1;
    }

    gpio_set_function(pin, GPIO_FUNC_PWM);
    slice_map[pin] = slice;

    if (slice_active[slice] == 0)
    {
        pwm_clear_irq(slice);
        pwm_set_irq_enabled(slice, true);

        slice_cfg[slice] = pwm_get_default_config();
        pwm_config_set_wrap(&slice_cfg[slice], WRAP);
        pwm_config_set_clkdiv(&slice_cfg[slice], clkdiv);
        pwm_init(slice, &slice_cfg[slice], true);
        pwm_set_chan_level(slice, pin % 2, 90);
    }

    ++slice_active[slice];

    irq_set_enabled(PWM_IRQ_WRAP, true);

    return 0;
}

/**
 * @brief Move a servo.
 * Move the servo on the specified pin to the specified angle in degrees.
 *
 * @param pin The PWM pin controlling a servo.
 * @param angle The angle to move to.
 */
int servo_move_to(uint pin, uint angle)
{
    if (slice_map[pin] < 0)
    {
        return 1;
    }

    uint val = (uint)fix16_to_int(
        fix16_mul(
            fix16_div(fix16_from_int(angle), fix16_from_int(180)),
            fix16_from_int(max - min)
        )
    ) + min;

    uint pos = slice_map[pin] + (pin % 2);
    servo_pos[16 * servo_pos_buf[pos] + pos] = val;
    servo_pos_buf[pos] = (servo_pos_buf[pos] + 1) % 2;
    return 0;
}

/**
 * @brief Move a servo.
 *
 * Move a servo by specifing microseconds.
 * Note that this is dangerous and can damage your servo if you are not careful!
 *
 * @param pin The PWM pin.
 * @param us The amount of time in microseconds the PWM signal is high.
 */
int servo_microseconds(uint pin, uint us)
{
    if (slice_map[pin] < 0)
    {
        return 1;
    }
    
    uint pos = slice_map[pin] + (pin % 2);
    servo_pos[16 * servo_pos_buf[pos] + pos] = us;
    servo_pos_buf[pos] = (servo_pos_buf[pos] + 1) % 2;
    return 0;
}

