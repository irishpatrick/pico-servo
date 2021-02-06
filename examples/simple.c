#include "pico/stdlib.h"
#include "pico_servo.h"

int main()
{
    stdio_init_all();

    servo_init();
    servo_attach(2);
    servo_attach(3);

    while (1)
    {
        servo_move_to(2, 0);
        servo_move_to(15, 0);
        sleep_ms(1000);
        servo_move_to(2, 180);
        servo_move_to(15, 90);
        sleep_ms(1000);
    }
    return 0;
}