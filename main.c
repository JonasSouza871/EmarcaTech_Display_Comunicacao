#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "Display_Bibliotecas/ssd1306.h"
#include "Display_Bibliotecas/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDR 0x3C

int main() {
    stdio_init_all();
    sleep_ms(100);

    // Inicializa a interface I2C
    i2c_init(I2C_PORT, 400000); //Incia comunicação serial
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); //Inicia pino SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); //Inicia pino SCL
    gpio_pull_up(I2C_SDA); //Coloca pino SDA como pull up
    gpio_pull_up(I2C_SCL); //Pino scl como pull up

    // Configura o display SSD1306
    ssd1306_t display;
    ssd1306_init(&display, 128, 64, false, OLED_ADDR, I2C_PORT);
    ssd1306_config(&display);

    while (true) {
        // Limpa o display e desenha a string "hello world"
        ssd1306_fill(&display, false);
        ssd1306_draw_string(&display, "hello world", 0, 0);
        ssd1306_send_data(&display);

        // Envia para o Serial Monitor a mesma mensagem
        printf("\rhello world");
        sleep_ms(1000);
    }
    return 0;
}
