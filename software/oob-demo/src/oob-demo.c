/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/gpio.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdio.h>

int main(void)
{
    puts("Welcome to your first Codasip demo!\n\n\n");

    // clang-format off
    puts("                          ckKXX0d;                                                                                                                              ");
    puts("                       cOMMMMMMMMMNx,                                                                                                                           ");
    puts("                     ,NMMMMMMMMMMMMMM0                                                                                                                          ");
    puts("                    'MMMMMMMMMMMMMMMMMN                                                                                                                         ");
    puts("                    WMMMMMMMMMMMMMMMMMMc                                                                                                                        ");
    puts("                    MMMMMMMMMMMMMMMMMMMo                                                                                                                        ");
    puts("                    :MMMMMMMMMMMMMMMMMM                                                                                                                         ");
    puts("                     :MMMMMMMMMMMMMMMM                                                                                                                          ");
    puts("                       xMMMMMMMMMMMM'                                                                                                                           ");
    puts("                           dMMMM.                     .,;::;,.                                     ..                                     ..                    ");
    puts("     :dOKXXXOd:.                                   .dNMMMKKMMMXd                                  :MM.                                   cMMM.                  ");
    puts("  .OMMMMMMMMMMMM0.                                'WMN       .MMX                                 :MM.                                                          ");
    puts(" lMMMMMMMMMMMMMMMMo                               WMM         ;MMc       .;::;'           .,::;.  :MM.      .,::;,.         .,;::,.       ...    ...  .,::;.    ");
    puts("'MMMMMMMMMMMMMMMMMM:                              MMN                 ,OWMM. KMMKc      dNMMO  XNd0MM.    oNM0   MMK,     .KMW   dMNo     MMX    MMM;KM. :MMWO. ");
    puts("kMMMMMMMMMMMMMMMMMMN                              MMN                kMM'       MMX    NMM       .MMM.     X      cMM.    XMM             MMX    MMMl       xMM'");
    puts("lMMMMMMMMMMMMMMMMMMk                              MMN               .MM0        ,MMc  :MM'        lMM.     ;,;;;;;kMM'     WMXdc:,.       MMX    MMN         MMW");
    puts(" NMMMMMMMMMMMMMMMMM                               MMM         'XXc  .MM0        'MMc  :MM'        cMM.   dWM:     cMM'          .WMW0:    MMX    MMM         MMW");
    puts("  xMMMMMMMMMMMMMMx                                :MMO        NMM    KMM.       XMM    MMX       .WMM.  ,MMc      OMM'    ox,      0MM    MMX    MMMl       dMM;");
    puts("    cMMMMMMMMMMo                                   .MMWxc;;cxWMM      lMMkc,,;dWMN      MMNd;'';xXlMM.   NMNc..':KcMM0;.  cMMx,...lMMO    MMX    MMNo0c'.,lKMM, ");
    puts("                           ..''.                       'MMMM'             XMMM'            OMMW.            NMMK              XMMMc              MMN   OMMW     ");
    puts("                       .l0WMMMMMMNk:                                                                                                             MMN            ");
    puts("                      OMMMMMMMMMMMMMWo                                                                                                           MMN            ");
    puts("                    .WMMMMMMMMMMMMMMMM0                                                                                                                         ");
    puts("                    XMMMMMMMMMMMMMMMMMM;                                                                                                                        ");
    puts("                    MMMMMMMMMMMMMMMMMMMd                                                                                                                        ");
    puts("                    dMMMMMMMMMMMMMMMMMM.                                                                                                                        ");
    puts("                     0MMMMMMMMMMMMMMMM,                                                                                                                         ");
    puts("                      .MMMMMMMMMMMMMN                                                                                                                           ");
    puts("                         .MMMMMMM0                                                                                                                              ");
    // clang-format on

    bm_gpio_t *gpio = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);

    const unsigned leds[] = {TARGET_GPIO_PORT_LED0, TARGET_GPIO_PORT_LED1, TARGET_GPIO_PORT_LED2};
    unsigned       led_on = 0;
    while (1)
    {
        bm_delay_ms(250);

        bm_gpio_clear(gpio, leds[led_on]);

        led_on = (led_on + 1) % (sizeof(leds) / sizeof(leds[0]));
        bm_gpio_set(gpio, leds[led_on]);
    }
}
