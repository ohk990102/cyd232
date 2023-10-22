// gcc -o test test.c ecc.c -g -m32 -lgmp

#include "ecc.h"

#define TEST_CASE 100000

void test___ecc_bn_add()
{
  mpz_t a, b;
  mpz_t c;

  ECC_BN ea, eb;
  ECC_BN expected, out;
  gmp_randstate_t state;

  mpz_init(a); mpz_init(b); mpz_init(c);
  gmp_randinit_default(state);

  for(int i = 0; i < TEST_CASE; i++) {
    if ((i + 1) % 1000 == 0) {
      printf("TEST %d / %d\n", i+1, TEST_CASE);
    }
    mpz_urandomb(a, state, 256);
    if (i % 2 == 0)
      mpz_urandomb(b, state, 128);
    else
      mpz_urandomm(b, state, a);

    mpz_add(c, a, b);

    mpz_to_ECC_BN(&ea, a);
    mpz_to_ECC_BN(&eb, b);
    mpz_to_ECC_BN(&expected, c);

    __ECC_BN_add(&out, &ea, &eb);
    if (ECC_BN_cmp(&expected, &out) != 0) {
      puts("Wrong!");
      printf("Length: %d %d\n", expected.len, out.len);
      for (int j = 0; j < expected.len; j++) {
        printf("%d: %u %u\n", j, expected.dat[j], out.dat[j]);
      }
      return;
    }
  }
  puts("Correct!");

  return;
}


void main()
{
  test___ecc_bn_add();
}