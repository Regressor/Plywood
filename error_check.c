/* Файл для проверки надежности различных способов 
   расчета контрольных сумм */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define Iterrations (1*1000*1000)
#define array_work 15

unsigned char crc8(unsigned char *data) {
    unsigned short crc8 = 0;

    for (int i=0; i<15; i++) {
        //[1]======= simple num mul + mix
        crc8 = crc8 + data[i] * 211;
        crc8 = (crc8 ^ (crc8 >> 8));

        //[2]======= sum
        // crc8 = crc8 + data[i];

        //[3]======= xor
        // crc8 = crc8 ^ data[i];
    }
    return (crc8 & 0xFF);
}


// random int from 0 to n
int randint(int n) {
    if ((n - 1) == RAND_MAX) {
        return rand();
    } else {
        long end = RAND_MAX / n;
        end *= n;
        int r;
        while ((r = rand()) >= end);
        return r % n;
    }
}


// Main function to test
int main() {
    int j = 0;
    unsigned char a = 0;
    unsigned char b = 0;
    unsigned int er_c = 0;
    unsigned char TeleTX8[16];
    unsigned char TeleRX8[16];
    unsigned short delta;
    srand(time(NULL));

    for (int I=1; I < Iterrations; I++) {

        // packet 16 bytes long
        // packet format: 0xAA 0xBB CMD XX XX XX XX XX XX XX XX XX XX XX XX SUM

        // Init arrays
        TeleTX8[0] = 0xAA;
        TeleTX8[1] = 0xBB;
        TeleRX8[0] = 0xAA;
        TeleRX8[1] = 0xBB;
        TeleRX8[2] = randint(64)+1;
        TeleTX8[2] = TeleRX8[2];

        // Random data for payload
        for (j=2; j < 15; j++) {
            a = randint(256);
            TeleTX8[j] = a;
            TeleRX8[j] = a;
        }
        
        // Add crc8
        TeleTX8[15] = crc8(TeleTX8);
        TeleRX8[15] = crc8(TeleRX8);

        // *************** add 1-10 errors *********
        do {
            for (int j = 1; j < randint(10) + 1; j++) {
                int e_num = randint(16);
                a = randint(8);
                b = 1 << a;
                TeleTX8[e_num] = TeleRX8[e_num] ^ b;
            }

            delta = 0;
            for (int j = 1; j < array_work; j++) {
                if (TeleRX8[j] != TeleTX8[j]) { delta++; break; };
            }

        } while (delta == 0);
        // *****************************************

        // We allways have changed packet here, so if it cannot be detected
        // it is a error. Check crc, check signature, check cmd range
        if (crc8(TeleTX8) == TeleTX8[15]) {.
            if (TeleTX8[0] == 0xAA && TeleTX8[1] == 0xBB)
                if (TeleTX8[2] < 1 || TeleTX8[2] > 64)
                    er_c++;
        }
    }

    printf("Iterrations: %d, errors: %d\n", Iterrations, er_c);
}
