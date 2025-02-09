#include "matriz_led.h"
#include "numeros.h"

static inline uint32_t rgb_para_uint32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void inicializar_matriz_led() {
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, PINO_WS2812, 800000, RGBW_ATIVO); // configurações da matriz de LED
}

void enviar_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

void mostrar_numero(uint8_t num) {
    uint32_t cor = rgb_para_uint32(0, 0, 64);

    for (int linha = 0; linha < 5; linha++) {  // Configurações de linhas e colunas da matriz de LED
        for (int coluna = 0; coluna < 5; coluna++) {
            int indice = linha * 5 + coluna;
            enviar_pixel(padrao_numeros[num][indice] ? cor : 0);
        }
    }
}

void desligar_matriz() {
    for (int i = 0; i < NUM_PIXELS; i++) { // Usa a constante NUM_PIXELS para maior flexibilidade
        enviar_pixel(0); // Envia preto (LEDs desligados)
    }
}
