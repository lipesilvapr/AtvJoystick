#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pwm.h"

// Definições de pinos e constantes
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDRESS 0x3C
#define Y_PIN 26  // GPIO para eixo Y do joystick
#define X_PIN 27  // GPIO para eixo X do joystick
#define JOYSTICK_CLICK 22  // GPIO para botão do joystick
#define BUTTON_A 5  // GPIO para botão A
#define LED_BLUE 12
#define LED_GREEN 11
#define LED_RED 13

#define PWM_WRAP 4095
#define PWM_CLK_DIV 30.52f

// Variáveis globais
bool led_state = 0;
bool rgb_leds_enabled = 1;
bool border_active = 0;
absolute_time_t last_interrupt_time = 0;

ssd1306_t display;  // Estrutura do display OLED

// Protótipos de funções
void setup_pwm(uint gpio, uint wrap, float clkdiv);
int32_t calculate_joystick_value(uint16_t value, bool is_y_axis);
void handle_gpio_interrupt(uint gpio, uint32_t events);
void setup_gpio();
void setup_adc();
void setup_i2c();
void setup_display();
void update_leds(uint16_t x_value, uint16_t y_value);
void draw_border();
void draw_square(int x, int y, uint size);

// Função principal
int main() {
    stdio_init_all();
    sleep_ms(1000);

    setup_gpio();
    setup_adc();
    setup_i2c();
    setup_display();

    uint16_t adc_value_x, adc_value_y;
    const uint square_size = 8;
    const uint center_x = (WIDTH - square_size) / 2;
    const uint center_y = (HEIGHT - square_size) / 2;

    while (true) {
        adc_select_input(0);  // Seleciona o eixo Y
        adc_value_y = adc_read();
        adc_select_input(1);  // Seleciona o eixo X
        adc_value_x = adc_read();

        update_leds(adc_value_x, adc_value_y);

        int square_x = center_x + ((int)adc_value_x - 2048) * center_x / 2048;
        int square_y = center_y + (2048 - (int)adc_value_y) * center_y / 2048;

        draw_border();
        draw_square(square_y, square_x, square_size);

        ssd1306_send_data(&display);
        ssd1306_fill(&display, false);

        sleep_ms(100);
    }
}

// Configuração do PWM
void setup_pwm(uint gpio, uint wrap, float clkdiv) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, wrap);
    pwm_config_set_clkdiv(&config, clkdiv);
    pwm_init(slice_num, &config, true);
}

// Calcula o valor do joystick (eixo X ou Y)
int32_t calculate_joystick_value(uint16_t value, bool is_y_axis) {
    int16_t axis_value;
    if (value < 2047) {
        axis_value = 2047 - value;
    } else if (value > 2047) {
        axis_value = value - 2047;
    } else {
        axis_value = 0;
    }
    return axis_value;
}

// Manipulador de interrupção GPIO
void handle_gpio_interrupt(uint gpio, uint32_t events) {
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_interrupt_time, now) < 2500000) {
        last_interrupt_time = now;
        return;
    }

    if (gpio == JOYSTICK_CLICK) {
        led_state = !led_state;
        gpio_put(LED_GREEN, led_state);
        border_active = !border_active;
    } else if (gpio == BUTTON_A) {
        rgb_leds_enabled = !rgb_leds_enabled;
        pwm_set_gpio_level(LED_RED, rgb_leds_enabled ? calculate_joystick_value(adc_read(), false) : 0);
        pwm_set_gpio_level(LED_BLUE, rgb_leds_enabled ? calculate_joystick_value(adc_read(), true) : 0);
    }
}

// Configuração dos GPIOs
void setup_gpio() {
    gpio_init(JOYSTICK_CLICK);
    gpio_set_dir(JOYSTICK_CLICK, GPIO_IN);
    gpio_pull_up(JOYSTICK_CLICK);
    gpio_set_irq_enabled_with_callback(JOYSTICK_CLICK, GPIO_IRQ_EDGE_FALL, true, handle_gpio_interrupt);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, handle_gpio_interrupt);

    setup_pwm(LED_BLUE, PWM_WRAP, PWM_CLK_DIV);
    setup_pwm(LED_RED, PWM_WRAP, PWM_CLK_DIV);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
}

// Configuração do ADC
void setup_adc() {
    adc_init();
    adc_gpio_init(Y_PIN);
    adc_gpio_init(X_PIN);
}

// Configuração do I2C
void setup_i2c() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// Configuração do display OLED
void setup_display() {
    ssd1306_init(&display, WIDTH, HEIGHT, false, OLED_ADDRESS, I2C_PORT);
    ssd1306_config(&display);
    ssd1306_send_data(&display);
    ssd1306_fill(&display, false);
    ssd1306_send_data(&display);
}

// Atualiza os LEDs RGB com base nos valores do joystick
void update_leds(uint16_t x_value, uint16_t y_value) {
    if (x_value >= 2060 - 100 && x_value <= 2060 + 100 && y_value >= 2060 - 100 && y_value <= 2060 + 100) {
        pwm_set_gpio_level(LED_BLUE, 0);
        pwm_set_gpio_level(LED_RED, 0);
    } else if (rgb_leds_enabled) {
        pwm_set_gpio_level(LED_BLUE, calculate_joystick_value(y_value, true));
        pwm_set_gpio_level(LED_RED, calculate_joystick_value(x_value, false));
    }
}

// Desenha a borda no display
void draw_border() {
    if (border_active) {
        ssd1306_circle(&display, WIDTH / 2, HEIGHT / 2, 30, true);
    } else {
        ssd1306_rect(&display, 3, 3, 122, 60, true, false);
    }
}

// Desenha um quadrado no display
void draw_square(int x, int y, uint size) {
    ssd1306_rect(&display, x, y, size, size, true, false);
}