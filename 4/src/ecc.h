#include <stdio.h>
#include <stdlib.h>

#define _GMP_ENABLE_

#ifdef _GMP_ENABLE_
#include "gmp.h"
#endif

#define ECC_PASS       1
#define ECC_FAIL      -1
#define ECC_WORD_LEN  20

typedef struct _ECC_BN
{
  unsigned int dat[ECC_WORD_LEN];
  unsigned int len;
}ECC_BN;

// FFFFFFFF 00000001 00000000 00000000 00000000 FFFFFFFF FFFFFFFF FFFFFFFF
static ECC_BN prime_p256 =
{
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF }, 8
};

#ifdef _GMP_ENABLE_
int mpz_to_ECC_BN(ECC_BN* c, mpz_t a);
int ECC_BN_to_mpz(mpz_t c, ECC_BN* a);
#endif

int ECC_BN_copy(ECC_BN* c, ECC_BN* a); // c = a
int ECC_BN_cmp(ECC_BN* a, ECC_BN* b); // a>b => 1 , a==b => 0, a<b => -1 
int ECC_BN_lshift_1bit(ECC_BN* c, ECC_BN* a); // c= a<<1 

void __ECC_BN_add(ECC_BN* c, ECC_BN* a, ECC_BN* b); // c = a+b 
void __ECC_BN_sub(ECC_BN* c, ECC_BN* a, ECC_BN* b); // c = a-b

int ECC_BN_add_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b, ECC_BN* p);  //c= a+b mod p
int ECC_BN_sub_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b, ECC_BN* p);  //c= a-b mod p
