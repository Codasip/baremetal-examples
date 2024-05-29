/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/i2c.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define INA219_REG_CONFIG                       0x00
#define INA219_REG_SHUNTVOLTAGE                 0x01
#define INA219_REG_BUSVOLTAGE                   0x02
#define INA219_REG_POWER                        0x03
#define INA219_REG_CURRENT                      0x04
#define INA219_REG_CALIBRATION                  0x05

#define INA219_CONFIG_RESET                     0x8000
#define INA219_CONFIG_RESET_VALUE               0x399f

#define INA219_CONFIG_BVOLTAGERANGE_16V         0x0000 // 0-16V Range
#define INA219_CONFIG_BVOLTAGERANGE_32V         0x4000 // 0-32V Range
#define INA219_CONFIG_GAIN_1_40MV               0x0000 // Gain 1, 40mV Range
#define INA219_CONFIG_GAIN_2_80MV               0x0800 // Gain 2, 80mV Range
#define INA219_CONFIG_GAIN_4_160MV              0x1000 // Gain 4, 160mV Range
#define INA219_CONFIG_GAIN_8_320MV              0x1800 // Gain 8, 320mV Range
#define INA219_CONFIG_GAIN_MASK                 0x1800

#define INA219_CONFIG_SADCRES_9BIT_1S_84US      0x0000 // 1 x 9-bit shunt sample
#define INA219_CONFIG_SADCRES_10BIT_1S_148US    0x0008 // 1 x 10-bit shunt sample
#define INA219_CONFIG_SADCRES_11BIT_1S_276US    0x0010 // 1 x 11-bit shunt sample
#define INA219_CONFIG_SADCRES_12BIT_1S_532US    0x0018 // 1 x 12-bit shunt sample
#define INA219_CONFIG_SADCRES_12BIT_2S_1060US   0x0048 // 2 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_4S_2130US   0x0050 // 4 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_8S_4260US   0x0058 // 8 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_16S_8510US  0x0060 // 16 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_32S_17MS    0x0068 // 32 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_64S_34MS    0x0070 // 64 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_128S_69MS   0x0078 // 128 x 12-bit shunt samples averaged together

#define INA219_CONFIG_MODE_POWERDOWN            0x0000
#define INA219_CONFIG_MODE_SVOLT_TRIGGERED      0x0001
#define INA219_CONFIG_MODE_BVOLT_TRIGGERED      0x0002
#define INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED  0x0003
#define INA219_CONFIG_MODE_ADCOFF               0x0004
#define INA219_CONFIG_MODE_SVOLT_CONTINUOUS     0x0005
#define INA219_CONFIG_MODE_BVOLT_CONTINUOUS     0x0006
#define INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS 0x0007

uint16_t ina219_read_reg(bm_i2c_t *i2c, uint8_t reg, uint8_t addr)
{
    bm_i2c_init(i2c);

    // Select register
    bm_i2c_send(i2c, addr, &reg, 1, false);

    uint16_t tmp;
    bm_i2c_recv(i2c, addr, (uint8_t *)&tmp, 2, true);

    // Received data - LSB, MSB
    return ((tmp & 0xff) << 8) | ((tmp & 0xff00) >> 8);
}

void ina219_write_reg(bm_i2c_t *i2c, uint8_t reg, uint16_t val, uint8_t addr)
{
    // Data to transmit - register, MSB, LSB
    uint32_t tmp = ((val & 0xff) << 16) | (val & 0xff00) | reg;

    bm_i2c_init(i2c);
    bm_i2c_send(i2c, addr, (uint8_t *)&tmp, 3, true);
}

int main(void)
{
    puts("Welcome to the I2C demo!");

    bm_i2c_t *i2c = (bm_i2c_t *)target_peripheral_get(BM_PERIPHERAL_I2C_PWR);

    // Slave addresses for sensors on the Genesys2 board
    // see https://digilent.com/reference/programmable-logic/genesys-2/reference-manual
    const uint8_t slaves[]       = {0x45, 0x4c, 0x48, 0x41, 0x44, 0x40};
    const float   shunt_resistor = 0.005; // 5mOhm

    for (unsigned i = 0; i < sizeof(slaves); ++i)
    {
        printf("\nTesting sensor #%u: 0x%x\n", i, slaves[i]);

        // Reset the sensor
        ina219_write_reg(i2c, INA219_REG_CONFIG, INA219_CONFIG_RESET, slaves[i]);

        if (ina219_read_reg(i2c, INA219_REG_CONFIG, slaves[i]) != INA219_CONFIG_RESET_VALUE)
        {
            puts("Error: Incorrect value after reset!");
            return EXIT_FAILURE;
        }

        // Selected configuration
        uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V | INA219_CONFIG_GAIN_2_80MV |
                          INA219_CONFIG_SADCRES_12BIT_16S_8510US |
                          INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

        float gain;
        switch (config & INA219_CONFIG_GAIN_MASK)
        {
            case INA219_CONFIG_GAIN_1_40MV:
                gain = 0.040;
                break;
            case INA219_CONFIG_GAIN_2_80MV:
                gain = 0.080;
                break;
            case INA219_CONFIG_GAIN_4_160MV:
                gain = 0.160;
                break;
            case INA219_CONFIG_GAIN_8_320MV:
                gain = 0.320;
                break;
        }

        // Values corresponding to the configuration
        // see https://www.ti.com/lit/ds/symlink/ina219.pdf
        uint16_t calibration = 0.04096 / (gain / (float)(2 << 14));
        float    current_lsb = gain / shunt_resistor / (2 << 14) * 1000;

        // Configure the sensor
        ina219_write_reg(i2c, INA219_REG_CONFIG, config, slaves[i]);
        ina219_write_reg(i2c, INA219_REG_CALIBRATION, calibration, slaves[i]);

        // Wait for measurements
        bm_delay_ms(100);

        // Raw shunt voltage value is a multiple of 10uV
        int   shunt_voltage_reg = (int16_t)ina219_read_reg(i2c, INA219_REG_SHUNTVOLTAGE, slaves[i]);
        float shunt_voltage     = (float)shunt_voltage_reg / 100;

        // Raw bus voltage is a multiple of 4 mV, stored in upper 13 bits
        unsigned bus_voltage_reg = ina219_read_reg(i2c, INA219_REG_BUSVOLTAGE, slaves[i]);
        float    bus_voltage     = (float)(bus_voltage_reg >> 3) * 4 / 1000;

        int   current_reg = (int16_t)ina219_read_reg(i2c, INA219_REG_CURRENT, slaves[i]);
        float current     = current_reg * current_lsb;

        unsigned power_reg = ina219_read_reg(i2c, INA219_REG_POWER, slaves[i]);
        float    power     = power_reg * current_lsb * 20; // Power lsb = 20 * current lsb

        puts("Measured:");
        printf(" - shunt voltage: %.2fmV\n", shunt_voltage);
        printf(" - bus voltage:   %.2fV\n", bus_voltage);
        printf(" - current:       %.2fmA\n", current);
        printf(" - power:         %.2fmW\n", power);
    }
    return EXIT_SUCCESS;
}
