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

/**
 * @brief Prints out the raw bytes block in hex characters
 *        and assumes the input is in MSB format
 * 
 * @param input 
 */
void print_raw(char *input) {
    for (int i = 0; i < strlen(input); i += 4) {

        // Since RMII is 2 bits wide, values are 0,1,2,3 
        // 4 sets of 2bits in 1 byte.
        uint8_t val = 0;
        val |= get_bits(input[i] - '0') << 6;
        val |= get_bits(input[i + 1] - '0') << 4;
        val |= get_bits(input[i + 2] - '0') << 2;
        val |= get_bits(input[i + 3] - '0');


        if (i % (4 * 8) == 0)
            printf("\n");

        printf("%02x ", val);

    }
}

/**
 * @brief Prints out the raw bytes block in hex characters
 *        and assumes the input is in LSB format
 * 
 * @param input 
 */
void print_raw_rev(char *input) {
    for (int i = 0; i < strlen(input); i += 4) {

        // Since RMII is 2 bits wide, values are 0,1,2,3 
        // 4 sets of 2bits in 1 byte.
        uint8_t val = 0;
        val |= get_bits(input[i] - '0');
        val |= get_bits(input[i + 1] - '0') << 2;
        val |= get_bits(input[i + 2] - '0') << 4;
        val |= get_bits(input[i + 3] - '0') << 6;


        if (i % (4 * 8) == 0)
            printf("\n");

        printf("%02x ", val);

    }
}


/**
 * @brief Prints out the headers of the MAC data
 *        and assumes the input is in MSB format
 * 
 * @param input 
 */
void print_head(char *input) {
    
    for (int i = 0; i < strlen(input); i += 4) {

        uint8_t val = 0;
        val |= get_bits(input[i] - '0') << 6;
        val |= get_bits(input[i + 1] - '0') << 4;
        val |= get_bits(input[i + 2] - '0') << 2;
        val |= get_bits(input[i + 3] - '0');


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
}

/**
 * @brief Prints out the headers of the MAC data
 *        and assumes the input is in LSB format
 * 
 * @param input 
 */
void print_head_rev(char *input) {
    
    for (int i = 0; i < strlen(input); i += 4) {

        uint8_t val = 0;
        val |= get_bits(input[i] - '0') ;
        val |= get_bits(input[i + 1] - '0') << 2;
        val |= get_bits(input[i + 2] - '0') << 4;
        val |= get_bits(input[i + 3] - '0') << 6;


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
}


int main(int argc, char **argv) {

    switch (argv[1][0]) {
        
        case 'r':
        case 'l':
            // Print out the headers but LSB first
            print_raw_rev(argv[2]);
            printf("\n\n");
            print_head_rev(argv[2]);
            break;

        case 'n':
        case 'm':
            print_raw(argv[2]);
            printf("\n\n");
            print_head(argv[2]);
            break;
        
        default: 
            printf("No order was specified\n\tUsage: ./decodeRaw [r|n] quaterbytes");
        
    }

    printf("\n");

}