## Projeto Display de Matriz LED e OLED com Raspberry Pi Pico

**Descrição:** Este projeto demonstra o controle simultâneo de um display de matriz LED 5x5 (WS2812) e um display OLED 128x64 (SSD1306) usando uma Raspberry Pi Pico. Ele cria uma interface interativa onde:

* Caracteres alfanuméricos (A-Z, a-z, 0-9) inseridos via serial são exibidos no OLED.
* Dois botões controlam o estado de LEDs verde e azul, também refletidos no OLED.
* Números (0-9) inseridos são mostrados na matriz de LED.

Este projeto exemplifica o uso de diferentes periféricos da Pico, incluindo PIO para controle preciso de timing, I2C para comunicação com o OLED, e GPIO para entrada de botões e saída para LEDs.

---

## 🔧 Hardware e Pinagem

### Componentes Necessários:

* Raspberry Pi Pico
* Display OLED 128x64 (SSD1306) I2C
* Matriz de LED 5x5 (WS2812)
* 2 Botões de pressão (push-button)
* 1 LED Verde
* 1 LED Azul
* Resistores apropriados para os LEDs (ex: 220Ω - *verifique a folha de dados dos seus LEDs*)
* Fios de ligação
* Protoboard (recomendado)

### 📍 Pinagem:

A pinagem é configurável no código-fonte, mas a configuração padrão é:

| Componente         | Pino Pico |  Função      |
|----------------------|-----------|---------------|
| OLED SDA           | GPIO14    | I2C Data     |
| OLED SCL           | GPIO15    | I2C Clock     |
| Botão A             | GPIO5     | Entrada       |
| Botão B             | GPIO6     | Entrada       |
| LED Verde          | GPIO11    | Saída        |
| LED Azul           | GPIO12    | Saída        |
| Matriz de LED (DIN)| GPIO7     | PIO (WS2812) |


---

## 💻 Software e Instalação

**Pré-requisitos:**

* **Pico SDK:** Instale o Pico SDK seguindo as instruções em: [https://github.com/raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk)
    * Certifique-se de definir a variável de ambiente `PICO_SDK_PATH`.
* **CMake:**  Tenha o CMake instalado em seu sistema.
* **VS Code (Recomendado):**  VS Code com as extensões "CMake" e "CMake Tools" simplifica o desenvolvimento.
* **Git:** Para clonar o repositório.


**Etapas:**

1. **Clonar:**
   ```bash
   git clone [URL do seu repositório]
   ```

2. **VS Code (ou outra IDE):** Abra a pasta do projeto. Configure o CMake para usar o Pico SDK. O arquivo `pico_sdk_import.cmake` auxilia nesse processo.

3. **Compilar:**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. **Flash:** Entre no modo bootloader do Pico (pressione BOOTSEL enquanto conecta ao USB). Copie o arquivo `.uf2` da pasta `build` para o Pico.


---

## 📁 Estrutura do Projeto
- O projeto está organizado da seguinte forma:

.
├── .vscode/             # Configurações do VS Code (se aplicável)
├── build/               # Diretório para os arquivos compilados
├── CMakeLists.txt       # Arquivo de configuração do CMake
├── main.c              # Arquivo principal do programa
├── Display_Bibliotecas/ # Biblioteca para o display OLED
│   ├── ssd1306.c
│   └── ssd1306.h
├── matriz_led.c        # Código para controlar a matriz de LED
├── matriz_led.h        # Header da biblioteca da matriz de LED
├── numeros.c           # Padrões numéricos para a matriz de LED
├── numeros.h           # Header para os padrões numéricos
├── pico_sdk_import.cmake # Script para importar o Pico SDK
├── ws2812.pio          # Programa PIO para o WS2812
└── README.md            # Este arquivo


## 🚀 Funcionamento Detalhado

**Loop Principal (`main.c`):**

* **Leitura Serial (Não Bloqueante):**  `getchar_timeout_us(0)`  verifica por novos caracteres sem interromper o fluxo do programa.

* **Atualização do Display (OLED):** O OLED é atualizado periodicamente e após eventos (botão pressionado, novo caractere) para exibir:
    * Caractere digitado.
    * Estado dos LEDs ("Ativo"/"Não ativo").  Centralizado para melhor visualização.

* **Controle da Matriz (LED):**
    * **Números (0-9):**  `atualizar_matriz_led()`  desenha o número na matriz.
    * **Outros Caracteres:** `desligar_matriz()` apaga a matriz.


**Interrupções de Botão:**

* **Borda de Descida:**  Configuradas para acionar na borda de descida (pressionamento).
* **`funcao_botoes()`:**
    * **Debouncing:** Previne leituras múltiplas com `ultima_vez_botao_` e `TEMPO_ESPERA_BOTAO`.
    * **Alternância de Estado (LED):** Inverte o estado do LED associado ao botão.
    * **Flag de Atualização (OLED):** Define `atualizar_display` para que o OLED seja atualizado no próximo ciclo do loop principal.

**Bibliotecas:**

* **`ssd1306.c/h`:** Abstrai a comunicação I2C com o OLED, oferecendo funções de desenho.
* **`matriz_led.c/h`:**  Gerencia o PIO e o protocolo WS2812 para controlar a matriz de LED.
* **`numeros.c/h`:** Define os padrões de bits para os números na matriz.


**Fluxo de Dados:**

```
Serial Input -> main.c -> (OLED Display, Matriz LED)
Botões -> Interrupção -> funcao_botoes() -> (LEDs, Flag Atualização OLED) -> main.c -> OLED Display
```

---


## 🗺️ Esquemático (Adicione seu esquema aqui)

---

## 🎬 Vídeo de Funcionamento:

## 📞 Contato (Adicione suas informações de contato):
👤 Autor: Jonas Souza Pinto

📧 E-mail: jonassouza871@hotmail.com
