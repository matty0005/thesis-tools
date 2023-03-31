/**
 * @file decodeRaw.c
 * @author Matt Gilpin (matt@matthewgilpin.com)
 * @brief A simple program that takes in raw bus data from an RMII
 * interface and converts it into hex data output.
 * 
 * @version 0.1
 * @date 2023-03-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/**
 * @brief Returns the 4 least significant bits.
 * 
 * @param a 
 * @return uint8_t 
 */
uint8_t get_bits(uint8_t a) {
    return a & 0x3;
}

int main(int argc, char **argv) {


    // Print out the raw data. 
    for (int i = 0; i < strlen(argv[1]); i += 4) {

        // Since RMII is 2 bits wide, values are 0,1,2,3 
        // 4 sets of 2bits in 1 byte.
        uint8_t val = 0;
        val |= get_bits(argv[1][i] - '0') << 6;
        val |= get_bits(argv[1][i + 1] - '0') << 4;
        val |= get_bits(argv[1][i + 2] - '0') << 2;
        val |= get_bits(argv[1][i + 3] - '0');


        if (i % (4 * 8) == 0)
            printf("\n");

        printf("%02x ", val);

    }

    printf("\n\n\n");

    // Print out the different headers.
    for (int i = 0; i < strlen(argv[1]); i += 4) {

        uint8_t val = 0;
        val |= get_bits(argv[1][i] - '0') << 6;
        val |= get_bits(argv[1][i + 1] - '0') << 4;
        val |= get_bits(argv[1][i + 2] - '0') << 2;
        val |= get_bits(argv[1][i + 3] - '0');


        switch(i / 4) {
            case 0:
                printf("Preamble: ");
                break;
            case 7:
                printf("\nSFD: ");
                break;
            case 8:
                printf("\nDMAC: ");
                break;
            case 14:
                printf("\nSMAC: ");
                break;
            case 20:
                printf("\nLength: ");
                break;
            case 22:
                printf("\nData: ");
                break;
        }

        printf("%02x ", val);

    }

    printf("\n");

}