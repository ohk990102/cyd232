#include <stdio.h>
#include <stdlib.h>

#define _GMP_ENABLE_

#ifdef _GMP_ENABLE_
#include "gmp.h"
#endif

#define ECC_PASS 1
#define ECC_FAIL -1
#define ECC_WORD_LEN 20

typedef struct _ECC_BN {
  unsigned int dat[ECC_WORD_LEN];
  unsigned int len;
} ECC_BN;

typedef struct _ECC_POINT
{
  ECC_BN x;
  ECC_BN y;
  int    point_at_infinity; //1 = 무한원점
}ECC_POINT;
// FFFFFFFF 00000001 00000000 00000000 00000000 FFFFFFFF FFFFFFFF FFFFFFFF
static ECC_BN prime_p256 =
{
  {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF }, 8
};
// G x =  6b17d1f2 e12c4247 f8bce6e5 63a440f2 77037d81 2deb33a0 f4a13945 d898c296
// G y =  4fe342e2 fe1a7f9b 8ee7eb4a 7c0f9e16 2bce3357 6b315ece cbb64068 37bf51f5
static ECC_POINT base_p256 =
{
   {{0xd898c296, 0xf4a13945, 0x2deb33a0, 0x77037d81, 0x63a440f2, 0xf8bce6e5, 0xe12c4247, 0x6b17d1f2},8}
  ,{{0x37bf51f5, 0xcbb64068, 0x6b315ece, 0x2bce3357, 0x7c0f9e16, 0x8ee7eb4a, 0xfe1a7f9b, 0x4fe342e2},8}
  ,0 
};
//coeff a = -3
static ECC_BN ceff_a_p256 =
{
  {0xFFFFFFFC, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF }, 8
};
//coeff b = 5ac635d8 aa3a93e7 b3ebbd55 769886bc 651d06b0 cc53b0f6 3bce3c3e 27d2604b
static ECC_BN ceff_b_p256 =
{
  {0x27d2604b, 0x3bce3c3e, 0xcc53b0f6, 0x651d06b0, 0x769886bc, 0xb3ebbd55, 0xaa3a93e7, 0x5ac635d8 }, 8
};

#ifdef _GMP_ENABLE_
int mpz_to_ECC_BN(ECC_BN* c, mpz_t a);
int ECC_BN_to_mpz(mpz_t c, ECC_BN* a);
#endif

int ECC_BN_copy(ECC_BN* c, ECC_BN* a);  // c = a
int ECC_BN_cmp(ECC_BN* a, ECC_BN* b);   // a>b => 1 , a==b => 0, a<b => -1
int ECC_BN_lshift_1bit(ECC_BN* c, ECC_BN* a);  // c= a<<1

void __ECC_BN_add(ECC_BN* c, ECC_BN* a, ECC_BN* b);  // c = a+b
void __ECC_BN_sub(ECC_BN* c, ECC_BN* a, ECC_BN* b);  // c = a-b

int ECC_BN_add_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b, ECC_BN* p);  // c= a+b mod p
int ECC_BN_sub_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b, ECC_BN* p);  // c= a-b mod p

void __ECC_BN_mul(ECC_BN* c, ECC_BN* a, ECC_BN* b);        // c = a+b
int ECC_BN_mod_p256(ECC_BN* c, ECC_BN* a);                 // c = a mod p256
int ECC_BN_mul_mod_p256(ECC_BN* c, ECC_BN* a, ECC_BN* b);  // c = a+b mod p256
int ECC_BN_mul_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b,
                   ECC_BN* p);  // c = a+b mod p

int ECC_BN_rshift_1bit(ECC_BN* c, ECC_BN* a); // c= a>>1
int ECC_BN_binary_inv(ECC_BN* c, ECC_BN* a); // c= a^(-1) mod p 

/*---------------------------------- - ecc -------------------------------------*/
int ECC_point_init(ECC_POINT *P);
int ECC_point_copy(ECC_POINT *R, ECC_POINT* P);
int ECC_point_add(ECC_POINT* R, ECC_POINT* P, ECC_POINT* Q); //P+Q=R
int ECC_point_dbl(ECC_POINT* R, ECC_POINT* P);               //2P=R
