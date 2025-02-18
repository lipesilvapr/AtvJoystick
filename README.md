# Projeto: Controle de LEDs RGB e Display com Joystick no BitDogLab

Este projeto tem como objetivo consolidar os conceitos de uso de conversores analógico-digitais (ADC), PWM, e comunicação I2C no microcontrolador RP2040, utilizando a placa de desenvolvimento BitDogLab. O projeto consiste em controlar a intensidade de LEDs RGB com base nos valores de um joystick, exibir a posição do joystick em um display SSD1306 e implementar funcionalidades adicionais com botões.

## Funcionalidades

1. **Controle de LEDs RGB:**
   - O LED Azul é controlado pelo eixo Y do joystick. Quando o joystick está na posição central (valor 2048), o LED permanece apagado. Movendo o joystick para cima ou para baixo, o brilho do LED aumenta gradualmente.
   - O LED Vermelho é controlado pelo eixo X do joystick, seguindo o mesmo princípio do LED Azul.
   - O LED Verde alterna seu estado (ligado/desligado) a cada pressionamento do botão do joystick.

2. **Display SSD1306:**
   - Um quadrado de 8x8 pixels é exibido no display, movendo-se proporcionalmente aos valores do joystick.
   - O botão do joystick alterna o estilo da borda do display a cada pressionamento.

3. **Botão A:**
   - Ativa ou desativa o controle dos LEDs RGB via PWM a cada pressionamento.

## Componentes Utilizados

- **LED RGB:** Conectado às GPIOs 11 (Verde), 12 (Azul) e 13 (Vermelho).
- **Joystick:** Conectado às GPIOs 26 (Eixo Y) e 27 (Eixo X).
- **Botão do Joystick:** Conectado à GPIO 22.
- **Botão A:** Conectado à GPIO 5.
- **Display SSD1306:** Conectado via I2C (GPIO 14 - SDA, GPIO 15 - SCL).

## Requisitos do Projeto

1. **Uso de Interrupções:**
   - Todas as funcionalidades relacionadas aos botões são implementadas usando rotinas de interrupção (IRQ).
   
2. **Debouncing:**
   - O tratamento do bouncing dos botões é implementado via software.

3. **Display 128x64:**
   - O display SSD1306 é utilizado para exibir a posição do joystick e alternar estilos de borda.

4. **Organização do Código:**
   - O código está bem estruturado e comentado para facilitar o entendimento.

## Estrutura do Código

O código está organizado em funções modulares para facilitar a manutenção e o entendimento. Aqui estão as principais funções:

- **`setup_pwm`:** Configura o PWM para controlar a intensidade dos LEDs.
- **`calculate_joystick_value`:** Calcula o valor do joystick para ajustar o brilho dos LEDs.
- **`handle_gpio_interrupt`:** Manipula as interrupções dos botões.
- **`setup_gpio`:** Configura os GPIOs para os botões e LEDs.
- **`setup_adc`:** Configura o ADC para ler os valores do joystick.
- **`setup_i2c`:** Configura a comunicação I2C com o display.
- **`setup_display`:** Inicializa e configura o display SSD1306.
- **`update_leds`:** Atualiza o brilho dos LEDs com base nos valores do joystick.
- **`draw_border`:** Desenha a borda no display, alternando entre estilos.
- **`draw_square`:** Desenha um quadrado no display, representando a posição do joystick.

## Como Executar

1. **Compilação:**
   - Utilize o ambiente de desenvolvimento compatível com o RP2040 (por exemplo, o Pico SDK) para compilar o código.

2. **Upload:**
   - Carregue o código compilado na placa BitDogLab.

3. **Execução:**
   - O projeto deve começar a funcionar automaticamente após o upload. O quadrado no display deve se mover conforme o joystick, e os LEDs devem ajustar seu brilho de acordo com os valores dos eixos X e Y.

## Vídeo de Demonstração

Um vídeo de demonstração foi produzido para mostrar o funcionamento do projeto. O vídeo inclui:
- A execução do projeto na placa BitDogLab.
- Uma breve explicação das funcionalidades implementadas.
- O aluno aparecendo pessoalmente para demonstrar o projeto.

[Link para o vídeo no Google Drive](#https://drive.google.com/file/d/1exIJpgIJm7UKt9Ue1Wwu_mItnia-SnZL/view?usp=sharing)

---
