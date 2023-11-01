#include "ecc.h"



void test___ecc_bn_add()
{
  int             i,len;
  mpz_t           a, b, c;
  ECC_BN          ec_a, ec_b, ec_c, tmp;
  gmp_randstate_t state;
  
  mpz_init(a); mpz_init(b); mpz_init(c);
  gmp_randinit_default(state);

  for (i = 0; i < 100000; i++) {

	mpz_urandomb(a, state, 256);
	mpz_urandomm(b, state, a);

	len = a->_mp_d[0] % 10;
	b->_mp_size = (len) ? (b->_mp_size%len) : (b->_mp_size);

	mpz_add(c, a, b);

	mpz_to_ECC_BN(&ec_a, a);
	mpz_to_ECC_BN(&ec_b, b);
	__ECC_BN_add(&ec_c, &ec_a, &ec_b);

	mpz_to_ECC_BN(&tmp, c);



	if (ECC_BN_cmp(&ec_c, &tmp)) {
	  printf("\ntest ECC_BN_add FAIL\n");
	  return;
	}

	gmp_printf("a=%Zx\n",a);
	gmp_printf("b=%Zx\n", b);
	gmp_printf("c=%Zx\n", c);
  }

  printf("\ntest ECC_BN_add PASS\n");

  mpz_clear(a); mpz_clear(b); mpz_clear(c);
  gmp_randclear(state);
}

void test___ecc_bn_sub() {
  int             i,len;
  mpz_t           a, b, c;
  ECC_BN          ec_a, ec_b, ec_c, tmp;
  gmp_randstate_t state;
  
  mpz_init(a); mpz_init(b); mpz_init(c);
  gmp_randinit_default(state);

  for (i = 0; i < 100000; i++) {

	mpz_urandomb(a, state, 256);
	mpz_urandomm(b, state, a);

	len = a->_mp_d[0] % 10;
	b->_mp_size = (len) ? (b->_mp_size%len) : (b->_mp_size);

	mpz_sub(c, a, b);

	mpz_to_ECC_BN(&ec_a, a);
	mpz_to_ECC_BN(&ec_b, b);
	__ECC_BN_sub(&ec_c, &ec_a, &ec_b);

	mpz_to_ECC_BN(&tmp, c);



	if (ECC_BN_cmp(&ec_c, &tmp)) {
	  printf("\ntest ECC_BN_sub FAIL\n");
	  return;
	}

	gmp_printf("a=%Zx\n",a);
	gmp_printf("b=%Zx\n", b);
	gmp_printf("c=%Zx\n", c);
  }

  printf("\ntest ECC_BN_sub PASS\n");

  mpz_clear(a); mpz_clear(b); mpz_clear(c);
  gmp_randclear(state);
}

void test___ecc_bn_add_mod() {
  int             i;
  mpz_t           a, b, c, d;
  ECC_BN          ec_a, ec_b, ec_c, tmp;
  gmp_randstate_t state;
  
  mpz_init(a); mpz_init(b); mpz_init(c); mpz_init(d);

  ECC_BN_to_mpz(d, &prime_p256);
  gmp_randinit_default(state);

  for (i = 0; i < 100000; i++) {

	// mpz_urandomb(a, state, 256);
	mpz_urandomm(a, state, d);
	mpz_urandomm(b, state, d);

	// len = a->_mp_d[0] % 10;
	// b->_mp_size = (len) ? (b->_mp_size%len) : (b->_mp_size);

	mpz_add(c, a, b);
	mpz_mod(c, c, d);

	mpz_to_ECC_BN(&ec_a, a);
	mpz_to_ECC_BN(&ec_b, b);
	ECC_BN_add_mod(&ec_c, &ec_a, &ec_b, &prime_p256);

	mpz_to_ECC_BN(&tmp, c);



	if (ECC_BN_cmp(&ec_c, &tmp)) {
	  printf("\ntest ECC_BN_add_mod FAIL\n");
	  return;
	}

	gmp_printf("a=%Zx\n",a);
	gmp_printf("b=%Zx\n", b);
	gmp_printf("c=%Zx\n", c);
  }

  printf("\ntest ECC_BN_add_mod PASS\n");

  mpz_clear(a); mpz_clear(b); mpz_clear(c);
  gmp_randclear(state);
}

void test___ecc_bn_sub_mod() {
  int             i;
  mpz_t           a, b, c, d;
  ECC_BN          ec_a, ec_b, ec_c, tmp;
  gmp_randstate_t state;
  
  mpz_init(a); mpz_init(b); mpz_init(c); mpz_init(d);

  ECC_BN_to_mpz(d, &prime_p256);
  gmp_randinit_default(state);

  for (i = 0; i < 100000; i++) {

	// mpz_urandomb(a, state, 256);
	mpz_urandomm(a, state, d);
	mpz_urandomm(b, state, d);

	// len = a->_mp_d[0] % 10;
	// b->_mp_size = (len) ? (b->_mp_size%len) : (b->_mp_size);

	mpz_sub(c, a, b);
	mpz_mod(c, c, d);

	mpz_to_ECC_BN(&ec_a, a);
	mpz_to_ECC_BN(&ec_b, b);
	ECC_BN_sub_mod(&ec_c, &ec_a, &ec_b, &prime_p256);

	mpz_to_ECC_BN(&tmp, c);



	if (ECC_BN_cmp(&ec_c, &tmp)) {
	  printf("\ntest ECC_BN_sub_mod FAIL\n");
	  return;
	}

	gmp_printf("a=%Zx\n",a);
	gmp_printf("b=%Zx\n", b);
	gmp_printf("c=%Zx\n", c);
  }

  printf("\ntest ECC_BN_sub_mod PASS\n");

  mpz_clear(a); mpz_clear(b); mpz_clear(c);
  gmp_randclear(state);
}


void test___ecc_bn_mul_mod_p256() {
  int             i;
  mpz_t           a, b, c, d;
  ECC_BN          ec_a, ec_b, ec_c, tmp;
  gmp_randstate_t state;
  
  mpz_init(a); mpz_init(b); mpz_init(c); mpz_init(d);

  ECC_BN_to_mpz(d, &prime_p256);
  gmp_randinit_default(state);

  for (i = 0; i < 100000; i++) {

	mpz_urandomm(a, state, d);
	mpz_urandomm(b, state, d);

	a->_mp_size = 2;
	b->_mp_size = 2;

	mpz_mul(c, a, b);
	mpz_mod(c, c, d);

	mpz_to_ECC_BN(&ec_a, a);
	mpz_to_ECC_BN(&ec_b, b);
	ECC_BN_mul_mod_p256(&ec_c, &ec_a, &ec_b);

	mpz_to_ECC_BN(&tmp, c);

	gmp_printf("a=%Zx\n",a);
	gmp_printf("b=%Zx\n", b);
	gmp_printf("c=%Zx\n", c);

	if (ECC_BN_cmp(&ec_c, &tmp)) {
	  printf("\ntest ECC_BN_mul_mod_p256 FAIL\n");
	  ECC_BN_to_mpz(d, &ec_c);
	  gmp_printf("ec_c=%Zx\n", d);
	  return;
	}
  }

  printf("\ntest ECC_BN_mul_mod_p256 PASS\n");

  mpz_clear(a); mpz_clear(b); mpz_clear(c);
  gmp_randclear(state);
}



int main()
{
  //test___ecc_bn_add();
  //test___ecc_bn_sub();
  // test___ecc_bn_add_mod();
  // test___ecc_bn_sub_mod();
  test___ecc_bn_mul_mod_p256();

}