#include <stdio.h>
#include <stdlib.h>

#include "./KISA_SHA256.h"

int main(int argc, char *argv[]) {
    BYTE data[2] = {
        0x0a, 0xbc
    };
    BYTE digest[SHA256_DIGEST_VALUELEN];

    for (int i = 0; i < 2; i++) {
        printf("%02x ", data[i]);
    }
    puts("");
    
    SHA256_Encrpyt(data, 0u, digest);

    for (int i = 0; i < SHA256_DIGEST_VALUELEN; i++) {
        printf("%02x ", digest[i]);
    }
    puts("");
    return 0;
}