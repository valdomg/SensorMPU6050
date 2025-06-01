#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

//Endereço do sensor MPU
const int I2C_ADDR = 0x68;

//Pinos do sensor

//Atributos do sensor
//Aceleração
const int accelX = 0;
const int accelY = 0;
const int accelZ = 1;

//Rotação
const int rotationX = 0;
const int rotationY = 0;
const int rotationZ = 0;

//Reset da MPU6050
static void mpu605,_rest(){

  uint8_t buf[] = {0x6B, 0x80};

  /*
  Função para tentar escrever um número especificado de bytes no endereço
  do Sensor(neste caso o MPU6050)
  */
  i2c_write_blocking(i2c_default, I2C_ADDR, buf, 2 , false);
  sleep_ms(100);

  //Limpa o modo de baixa atividade (sleep mode)
  buf[1] = 0x00;
  i2c_write_blocking(i2c_default, I2C_ADDR, buf, 2 , false);
  sleep_ms(10);

}

/*
Função para ler os valores raw do sensor MPU6050
colocar os parâmetros aqui também
*/

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3]){

  uint8_t buffer[6];


  //Começo para ler os valores de aceleração do sensor
  uint8_t val = 0x3B;
  i2c_write_blocking(i2c_default, I2C_ADDR, &val, 1, true);
  i2c_write_blocking(i2c_default, I2C_ADDR, buffer, 6, false);

  for(int i =0; i < 3; i ++){
    accel[i] = (buffer[i + 2] << 8 | buffer[(i * 2) + 1]);
  }


  //O mesmo para o giroscópio
  uint8_t val = 0x43;
  i2c_write_blocking(i2c_default, I2C_ADDR, &val, 1, true);
  i2c_write_blocking(i2c_default, I2C_ADDR, buffer, 6, false);

  for(int i = 0; i<3; i++){
    gyro[i] = (buffer[i + 2] << 8 | buffer[(i * 2) + 1]);
  }

}

int main() {
  stdio_init_all();


  while (true) {
    printf("Hello, Wokwi!\n");
    sleep_ms(250);
  }
}