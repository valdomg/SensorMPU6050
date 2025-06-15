#include <stdio.h>
#include <string.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "hardware/i2c.h"
#include "hardware/pwm.h"

// Endereço do sensor MPU
const int addr = 0x68;

// Pinos do sensor
const int SCL = 27;
const int SDA = 28;

//Pino do Servo
const int SERVO = 15;

//Período em Microssegundos do Servo
const int PERIODO_SERVO = 20;

const float VALOR_BRUTO_GRAVIDADE = 16384.0;
const float PI = 3.14;

// Reset da MPU6050
#ifdef i2c_default
static void mpu6050_reset()
{

  uint8_t buf[] = {0x6B, 0x80};

  /*
    Função para tentar escrever um número especificado de bytes no endereço
    do Sensor(neste caso o MPU6050)
  */
  i2c_write_blocking(i2c_default, addr, buf, 2, false);
  sleep_ms(100);

  // Limpa o modo de baixa atividade (sleep mode)
  buf[1] = 0x00;
  i2c_write_blocking(i2c_default, addr, buf, 2, false);
  sleep_ms(10);
}

/*
  Função para ler os valores raw do sensor MPU6050
  colocar os parâmetros aqui também
*/

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3])
{

  uint8_t buffer[6];

  // Começo para ler os valores de aceleração do sensor
  uint8_t val = 0x3B;
  i2c_write_blocking(i2c_default, addr, &val, 1, true);
  i2c_read_blocking(i2c_default, addr, buffer, 6, false);

  for (int i = 0; i < 3; i++)
  {
    accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
  }

  // O mesmo para o giroscópio
  val = 0x43;
  i2c_write_blocking(i2c_default, addr, &val, 1, true);
  i2c_read_blocking(i2c_default, addr, buffer, 6, false);

  for (int i = 0; i < 3; i++)
  {
    gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
  }
}
#endif

uint16_t calcular_angulo(int16_t accel_x, int16_t accel_y, int16_t accel_z) {

  float accel_x_g = accel_x / VALOR_BRUTO_GRAVIDADE;
  float accel_y_g = accel_y / VALOR_BRUTO_GRAVIDADE;
  float accel_z_g = accel_z / VALOR_BRUTO_GRAVIDADE;

  /*
    O retorno considera o valor do sinal dos dois parâmetros dados,
    assim retornando um valor com mais precição , ângulo em radianos.
  */

  float angulo = atan2(accel_x_g, sqrt(accel_y_g * accel_y_g + accel_z_g * accel_z_g)) * (180.0 / M_PI);

  return angulo;

}

// Converte ângulo (0–180) para pulso PWM em microssegundos (us)
uint16_t angle_to_pwm_us(uint8_t angle) {
    // Servo geralmente opera entre 1000us (0°) a 2000us (180°)
    return 1000 + (angle * 1000 / 180);
}

void init_i2c()
{
  i2c_init(i2c_default, 400 * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

  gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
  gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

  bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

  mpu6050_reset();
}


int main() {
  stdio_init_all();

  init_i2c();
  init_servo();

  int16_t acceleration[3], gyro[3];

  while (true)
  {

    mpu6050_read_raw(acceleration, gyro);
    float angle = calcular_angulo(acceleration[0], acceleration[1], acceleration[2]);
    
    printf("Accel -> X: %d, Y: %d, Z: %d \n", acceleration[0], acceleration[1], acceleration[2]);
    printf("Gyro -> X: %d, Y: %d, Z: %d \n", gyro[0], gyro[1], gyro[2]);

    int16_t duty = angle_to_pwm_us((int) angle);
    printf("Inclinação/Ângulo: %.2f || Duty: %d \n", angle, duty );

    set_servo_pwm_us(duty);  

    sleep_ms(1000);
  }
}