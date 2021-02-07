#include "pico_servo.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include <string.h>


#define WRAP 10000
#define FREQ 50

float clkdiv;
uint min;
uint max;

static int slice_map[30];
static uint slice_active[8];
static uint servo_pos[32];
static uint servo_pos_buf[16];
static pwm_config slice_cfg[8];

static void wrap_cb()
{
    uint offset;

    for (int i = 0; i < 8; ++i)
    {
        if (slice_active[i] == 0) continue;

        pwm_clear_irq(i);
        offset = 16 * ((servo_pos_buf[i + 0] + 1) % 2);
        pwm_set_chan_level(i, 0, servo_pos[offset + i + 0]);

        offset = 16 * ((servo_pos_buf[i + 1] + 1) % 2);
        pwm_set_chan_level(i, 1, servo_pos[offset + i + 1]);
    }
}

int servo_init()
{
    for (int i = 0; i < 30; ++i)
    {
        slice_map[i] = -1;
    }
    memset(slice_active, 0, 8 * sizeof(uint));
    memset(servo_pos, 0, 32 * sizeof(uint));
    memset(servo_pos_buf, 0, 16 * sizeof(uint));

    irq_set_exclusive_handler(PWM_IRQ_WRAP, wrap_cb);

    return 0;
}

int servo_clock_auto()
{
    return servo_clock_source(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
}

int servo_clock_source(uint src)
{
    clkdiv = (float)frequency_count_khz(src) * 1000.f / (FREQ * WRAP);
    if (clkdiv == 0)
    {
        return 1;
    }
    min = 0.025f * WRAP;
    max = 0.125f * WRAP;

    return 0;
}

int servo_clock_manual(uint freq)
{
    clkdiv = (float)freq * 1000.f / (FREQ * WRAP);
    if (clkdiv == 0)
    {
        return 1;
    }
    min = 0.025f * WRAP;
    max = 0.125f * WRAP;

    return 0;
}

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

int servo_move_to(uint pin, uint angle)
{
    if (slice_map[pin] < 0)
    {
        return 1;
    }

    uint val = (float)angle / 180.f * (max - min) + min;
    uint pos = slice_map[pin] + (pin % 2);
    servo_pos[16 * servo_pos_buf[pos] + pos] = val;
    servo_pos_buf[pos] = (servo_pos_buf[pos] + 1) % 2;
    return 0;
}