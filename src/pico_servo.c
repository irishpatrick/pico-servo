#include "pico_servo.h"
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"


#define WRAP 10000
#define CLKDIV 250

#define _WRAP_HANDLER(x) pwm##x##_cb
#define WRAP_HANDLER(x) _WRAP_HANDLER(x)

static uint slice_map[30];
static uint slice_active[8];
static void (*pwm_cb[8])(void);
static uint servo_pos[32];
static uint servo_pos_buf[16];

static pwm_generic_cb(uint slice)
{
    pwm_clear_irq(slice);

    uint offset;

    offset = 16 * ((servo_pos_buf[slice + 0] + 1) % 2);
    pwm_set_chan_level(slice, 0, servo_pos[offset + slice + 0]);
    //servo_pos_buf[slice + 0] = (servo_pos_buf[slice + 0] + 16) % 32; // flip buffer

    offset = 16 * ((servo_pos_buf[slice + 1] + 1) % 2);
    pwm_set_chan_level(slice, 1, servo_pos[offset + slice + 1]);
    //servo_pos_buf[slice + 1] = (servo_pos_buf[slice + 1] + 16) % 32; // flip buffer
}

static void pwm0_cb()
{
    pwm_generic_cb(0);
}

static void pwm1_cb()
{
    pwm_generic_cb(1);
}


static void pwm2_cb()
{
    pwm_generic_cb(2);
}


static void pwm3_cb()
{
    pwm_generic_cb(3);
}


static void pwm4_cb()
{
    pwm_generic_cb(4);
}


static void pwm5_cb()
{
    pwm_generic_cb(5);
}


static void pwm6_cb()
{
    pwm_generic_cb(6);
}


static void pwm7_cb()
{
    pwm_generic_cb(7);
}

int servo_init()
{
    memset(slice_map, 0xFF, 30 * sizeof(uint));
    memset(slice_active, 0, 8 * sizeof(uint));
    memset(servo_pos, 0, 32 * sizeof(uint));
    memset(servo_pos_buf, 0, 16 * sizeof(uint));
    pwm_cb[0] = pwm0_cb;
    pwm_cb[1] = pwm1_cb;
    pwm_cb[2] = pwm2_cb;
    pwm_cb[3] = pwm3_cb;
    pwm_cb[4] = pwm4_cb;
    pwm_cb[5] = pwm5_cb;
    pwm_cb[6] = pwm6_cb;
    pwm_cb[7] = pwm7_cb;
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
        irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_cb[slice]);
        irq_set_enabled(PWM_IRQ_WRAP, true);

        pwm_config cfg = pwm_get_default_config();
        pwm_config_set_wrap(&cfg, WRAP);
        pwm_config_set_clkdiv(&cfg, CLKDIV);
        pwm_init(slice, &cfg, true);
    }

    ++slice_active[slice];

    return 0;
}

int servo_move_to(uint pin, uint angle)
{
    uint pos = slice_map[pin] + (pin % 2);
    servo_pos[16 * servo_pos_buf[pos] + pos] = angle;
    servo_pos_buf[pos] = (servo_pos_buf[pos] + 1) % 2;
    return 0;
}