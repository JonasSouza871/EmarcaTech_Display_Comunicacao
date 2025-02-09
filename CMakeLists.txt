cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inicializa o pico_sdk a partir da localização instalada
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico CACHE STRING "Board type")

# Inclui o SDK do Raspberry Pi Pico (deve ser incluído antes do projeto)
include(pico_sdk_import.cmake)

project(DisplayC C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Adiciona o executável com o novo arquivo main.c (além do ssd1306.c)
add_executable(DisplayC 
    main.c 
    Display_Bibliotecas/ssd1306.c
)

pico_set_program_name(DisplayC "DisplayC")
pico_set_program_version(DisplayC "0.1")

# Configura saída via UART/USB (ajuste conforme necessário)
pico_enable_stdio_uart(DisplayC 0)
pico_enable_stdio_usb(DisplayC 1)

# Adiciona a biblioteca padrão ao compilado
target_link_libraries(DisplayC
    pico_stdlib
)

# Adiciona os diretórios de inclusão padrão
target_include_directories(DisplayC PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# Adiciona as bibliotecas de usuário (neste caso, hardware_i2c)
target_link_libraries(DisplayC 
    hardware_i2c
)

pico_add_extra_outputs(DisplayC)
