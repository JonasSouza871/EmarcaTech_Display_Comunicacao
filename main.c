#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "Display_Bibliotecas/ssd1306.h"
#include "Display_Bibliotecas/font.h"
#include "matriz_led.h"
#include "numeros.h"

#define PORTA_I2C i2c1
#define PINO_SDA 14 //Declaração do pino SDA
#define PINO_SCL 15 //Declaração pino SCL
#define ENDERECO_DISPLAY 0x3C //Endereço I2C
#define PINO_BOTAO_A 5 //Pino do botao A
#define PINO_BOTAO_B 6 //Pino do botao B
#define PINO_LED_VERDE 11 // Pino do led verde
#define PINO_LED_AZUL 12 // pino do led azul
#define TEMPO_ESPERA_BOTAO 200 //Debounce em 200 ms

// Variáveis marcadas como volatile porque são modificadas em interrupções
// O volatile avisa o compilador que estas variáveis podem mudar a qualquer momento
volatile bool estado_led_verde = false;    // Estado do LED verde (ligado/desligado)
volatile bool estado_led_azul = false;     // Estado do LED azul (ligado/desligado)
volatile uint32_t ultima_vez_botao_a = 0;  // Último momento que o botão A foi pressionado
volatile uint32_t ultima_vez_botao_b = 0;  // Último momento que o botão B foi pressionado
volatile bool atualizar_display = false;    // Flag para indicar necessidade de atualizar o display

void atualizar_matriz_led(char digito) { //Função para atualizar o valor do numero na matriz de led.
    uint8_t numero = digito - '0';
    mostrar_numero(numero);
}

void funcao_botoes(uint pino, uint32_t eventos) { //Funçoes para contro do estado do botao
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time()); 
    
    if (pino == PINO_BOTAO_A) {
        if ((tempo_atual - ultima_vez_botao_a) < TEMPO_ESPERA_BOTAO)
            return;
        
        ultima_vez_botao_a = tempo_atual;
        estado_led_verde = !estado_led_verde;
        gpio_put(PINO_LED_VERDE, estado_led_verde);
        printf("LED Verde: %s\n", estado_led_verde ? "LIGADO" : "DESLIGADO");
        atualizar_display = true;
    }
    else if (pino == PINO_BOTAO_B) {
        if ((tempo_atual - ultima_vez_botao_b) < TEMPO_ESPERA_BOTAO)
            return;
        ultima_vez_botao_b = tempo_atual;
        estado_led_azul = !estado_led_azul;
        gpio_put(PINO_LED_AZUL, estado_led_azul);
        printf("LED Azul: %s\n", estado_led_azul ? "LIGADO" : "DESLIGADO");
        atualizar_display = true;
    }
}

int main(void) {
    stdio_init_all(); //Inicia comunicação seria
    sleep_ms(100);
    i2c_init(PORTA_I2C, 400000); //Inicia display 128x64
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C); //Seta função sda
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C); // seta função scl
    gpio_pull_up(PINO_SDA); //Pino sda como pull_up
    gpio_pull_up(PINO_SCL); // Pino SCL como pull_up
    ssd1306_t display;
    ssd1306_init(&display, 128, 64, false, ENDERECO_DISPLAY, PORTA_I2C); //Inicia portas do display
    ssd1306_config(&display); //configura display
    inicializar_matriz_led(); //Função da bibliotecatecla matriz de led para inciializar a patra
    gpio_init(PINO_BOTAO_A); //Inicia botao A
    gpio_set_dir(PINO_BOTAO_A, GPIO_IN); // Seta botao A como entrada de dados
    gpio_pull_up(PINO_BOTAO_A); // Define botao A como pull up
    gpio_init(PINO_BOTAO_B); // Inicia botao B
    gpio_set_dir(PINO_BOTAO_B, GPIO_IN); // Seta botao B como entrada de dados
    gpio_pull_up(PINO_BOTAO_B); // Define botao B como pull_up
    gpio_set_irq_enabled_with_callback(PINO_BOTAO_A, GPIO_IRQ_EDGE_FALL, true, funcao_botoes); // Aplicação da interrupção no Botao A
    gpio_set_irq_enabled(PINO_BOTAO_B, GPIO_IRQ_EDGE_FALL, true); //Aplicação da interrupção no botao B

    gpio_init(PINO_LED_VERDE);  //Inicia pino do led verde
    gpio_set_dir(PINO_LED_VERDE, GPIO_OUT); // Seta pino verde como saída
    gpio_init(PINO_LED_AZUL); // inicia pino do led azul
    gpio_set_dir(PINO_LED_AZUL, GPIO_OUT); // Seta pino azul como saida
 
    char caractere_atual = '\0';
    
    while (true) {
        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            caractere_atual = (char)c;
            atualizar_display = true;
        }

        if (atualizar_display) { // para ataulizar dispaly
            atualizar_display = false;
            ssd1306_fill(&display, false);

            if ((caractere_atual >= 'A' && caractere_atual <= 'Z') || //Caracteres maiusculos
                (caractere_atual >= 'a' && caractere_atual <= 'z')) { //Caracteres minuscuslo
                ssd1306_draw_string(&display, "Letra: ", 0, 0); //Desenha letras digitadas
            } else if (caractere_atual >= '0' && caractere_atual <= '9') { //Numero para aparreze na matriz e display
                ssd1306_draw_string(&display, "Numero: ", 0, 0); //desenha o numero digitado
            }

            ssd1306_draw_char(&display, caractere_atual, 100, 0); //Print caractere na matriz

            ssd1306_draw_string(&display, "BOTAO A :", 0, 20); 
            char *texto_verde = estado_led_verde ? "Ativo" : "Nao ativo"; //Printa estado do led verde
            int largura_texto = 6 * strlen(texto_verde);
            int posicao_x = (128 - largura_texto) / 2;
            ssd1306_draw_string(&display, texto_verde, posicao_x, 30);

            ssd1306_draw_string(&display, "BOTAO B:", 0, 40);
            char *texto_azul = estado_led_azul ? "Ativo" : "Nao ativo";//Printa estado do led azul
            largura_texto = 6 * strlen(texto_azul);
            posicao_x = (128 - largura_texto) / 2;
            ssd1306_draw_string(&display, texto_azul, posicao_x, 50);
            
            ssd1306_send_data(&display);
            //para mandar para o serial
            printf("Caractere digitado: %c\n", caractere_atual);
            printf("LED Verde: %s\n", estado_led_verde ? "LIGADO" : "DESLIGADO");
            printf("LED Azul: %s\n", estado_led_azul ? "LIGADO" : "DESLIGADO");

            if (caractere_atual >= '0' && caractere_atual <= '9') {
                atualizar_matriz_led(caractere_atual); //Atualiza matriz de led com valores entre 0 e 9
            } else {
                desligar_matriz(); //Se não colocar nenhum numero a matriz desliga
                
            }
        }
        sleep_ms(100);
    }
    return 0;
}