#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "Display_Bibliotecas/ssd1306.h"
#include "Display_Bibliotecas/font.h"
#include "matriz_led.h"
#include "numeros.h"

// Definições para I2C e display SSD1306
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDR 0x3C

// Definições dos pinos para os botões e LEDs
#define BOTAO_A_PIN 5   // Botão A no GPIO 5
#define BOTAO_B_PIN 6   // Botão B no GPIO 6
#define LED_VERDE_PIN 11 // LED Verde no GPIO 11
#define LED_AZUL_PIN 12  // LED Azul no GPIO 12

// Tempo de debouncing em milissegundos
#define DEBOUNCE_DELAY 200

// Variáveis globais
volatile bool led_green_state = false;    // Estado do LED verde
volatile bool led_blue_state = false;       // Estado do LED azul
volatile uint32_t last_button_a_time = 0;   // Última vez que o botão A foi acionado
volatile uint32_t last_button_b_time = 0;   // Última vez que o botão B foi acionado
volatile bool flag_display_update = false;  // Flag para atualizar o display

// Função para atualizar a matriz de LEDs WS2812
void update_ws2812_matrix(char digit) {
    uint8_t num = digit - '0';
    mostrar_numero(num);
}

// Função de callback global para as interrupções dos botões
void gpio_callback(uint gpio, uint32_t events) {
    uint32_t now = to_ms_since_boot(get_absolute_time());
    
    if (gpio == BOTAO_A_PIN) {
        if ((now - last_button_a_time) < DEBOUNCE_DELAY)
            return;
        last_button_a_time = now;
        led_green_state = !led_green_state;
        gpio_put(LED_VERDE_PIN, led_green_state);
        printf("LED Verde: %s\n", led_green_state ? "LIGADO" : "DESLIGADO");
        flag_display_update = true;
    }
    else if (gpio == BOTAO_B_PIN) {
        if ((now - last_button_b_time) < DEBOUNCE_DELAY)
            return;
        last_button_b_time = now;
        led_blue_state = !led_blue_state;
        gpio_put(LED_AZUL_PIN, led_blue_state);
        printf("LED Azul: %s\n", led_blue_state ? "LIGADO" : "DESLIGADO");
        flag_display_update = true;
    }
}

int main(void) {
    stdio_init_all();
    sleep_ms(100);

    // Configuração do I2C e do display SSD1306
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_t display;
    ssd1306_init(&display, 128, 64, false, OLED_ADDR, I2C_PORT);
    ssd1306_config(&display);

    // Inicializa a matriz WS2812
    inicializar_matriz_led();

    // Configuração do Botão A
    gpio_init(BOTAO_A_PIN);
    gpio_set_dir(BOTAO_A_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_A_PIN);
    
    // Configuração do Botão B
    gpio_init(BOTAO_B_PIN);
    gpio_set_dir(BOTAO_B_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_B_PIN);
    
    // Registra a callback global para o botão A e habilita a interrupção
    gpio_set_irq_enabled_with_callback(BOTAO_A_PIN, GPIO_IRQ_EDGE_FALL, true, gpio_callback);
    // Para o botão B, apenas habilita a interrupção (a callback já está registrada)
    gpio_set_irq_enabled(BOTAO_B_PIN, GPIO_IRQ_EDGE_FALL, true);

    // Configuração dos LEDs
    gpio_init(LED_VERDE_PIN);
    gpio_set_dir(LED_VERDE_PIN, GPIO_OUT);
    gpio_init(LED_AZUL_PIN);
    gpio_set_dir(LED_AZUL_PIN, GPIO_OUT);

    char last_char = '\0';
    while (true) {
        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            last_char = (char)c;
            flag_display_update = true;
        }

        if (flag_display_update) {
            flag_display_update = false;
            ssd1306_fill(&display, false);

            if ((last_char >= 'A' && last_char <= 'Z') || (last_char >= 'a' && last_char <= 'z')) {
                ssd1306_draw_string(&display, "letra: ", 0, 0);
            } else if (last_char >= '0' && last_char <= '9') {
                ssd1306_draw_string(&display, "Numero: ", 0, 0);
            }

            ssd1306_draw_char(&display, last_char, 100, 0);
            ssd1306_draw_string(&display, "BOTAO VERDE:", 0, 20);
            char *estadoVerdeStr = led_green_state ? "Ativo" : "Nao ativo";
            int textWidthVerde = 6 * strlen(estadoVerdeStr);
            int posXVerde = (128 - textWidthVerde) / 2;
            ssd1306_draw_string(&display, estadoVerdeStr, posXVerde, 30);

            ssd1306_draw_string(&display, "BOTAO AZUL:", 0, 40);
            char *estadoAzulStr = led_blue_state ? "Ativo" : "Nao ativo";
            int textWidthAzul = 6 * strlen(estadoAzulStr);
            int posXAzul = (128 - textWidthAzul) / 2;
            ssd1306_draw_string(&display, estadoAzulStr, posXAzul, 50);
            ssd1306_send_data(&display);

            printf("Caractere digitado: %c\n", last_char);
            printf("LED Verde: %s\n", led_green_state ? "LIGADO" : "DESLIGADO");
            printf("LED Azul: %s\n", led_blue_state ? "LIGADO" : "DESLIGADO");

            if (last_char >= '0' && last_char <= '9') {
                update_ws2812_matrix(last_char);
            } else {
                desligar_matriz();
            }
        }
        sleep_ms(100);
    }
    return 0;
}
