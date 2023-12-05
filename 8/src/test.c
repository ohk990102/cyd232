#include "ecc.h"

void test___ecc_bn_add() {
  int i, len;
  mpz_t a, b, c;
  ECC_BN ec_a, ec_b, ec_c, tmp;
  gmp_randstate_t state;

  mpz_init(a);
  mpz_init(b);
  mpz_init(c);
  gmp_randinit_default(state);

  for (i = 0; i < 100000; i++) {
    mpz_urandomb(a, state, 256);
    mpz_urandomm(b, state, a);

    len = a->_mp_d[0] % 10;
    b->_mp_size = (len) ? (b->_mp_size % len) : (b->_mp_size);

    mpz_add(c, a, b);

    mpz_to_ECC_BN(&ec_a, a);
    mpz_to_ECC_BN(&ec_b, b);
    __ECC_BN_add(&ec_c, &ec_a, &ec_b);

    mpz_to_ECC_BN(&tmp, c);

    if (ECC_BN_cmp(&ec_c, &tmp)) {
      printf("\ntest ECC_BN_add FAIL\n");
      return;
    }

    gmp_printf("a=%Zx\n", a);
    gmp_printf("b=%Zx\n", b);
    gmp_printf("c=%Zx\n", c);
  }

  printf("\ntest ECC_BN_add PASS\n");

  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(c);
  gmp_randclear(state);
}

void test___ecc_bn_sub() {
  int i, len;
  mpz_t a, b, c;
  ECC_BN ec_a, ec_b, ec_c, tmp;
  gmp_randstate_t state;

  mpz_init(a);
  mpz_init(b);
  mpz_init(c);
  gmp_randinit_default(state);

  for (i = 0; i < 100000; i++) {
    mpz_urandomb(a, state, 256);
    mpz_urandomm(b, state, a);

    len = a->_mp_d[0] % 10;
    b->_mp_size = (len) ? (b->_mp_size % len) : (b->_mp_size);

    mpz_sub(c, a, b);

    mpz_to_ECC_BN(&ec_a, a);
    mpz_to_ECC_BN(&ec_b, b);
    __ECC_BN_sub(&ec_c, &ec_a, &ec_b);

    mpz_to_ECC_BN(&tmp, c);

    if (ECC_BN_cmp(&ec_c, &tmp)) {
      printf("\ntest ECC_BN_sub FAIL\n");
      return;
    }

    gmp_printf("a=%Zx\n", a);
    gmp_printf("b=%Zx\n", b);
    gmp_printf("c=%Zx\n", c);
  }

  printf("\ntest ECC_BN_sub PASS\n");

  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(c);
  gmp_randclear(state);
}

void test___ecc_bn_add_mod() {
  int i;
  mpz_t a, b, c, d;
  ECC_BN ec_a, ec_b, ec_c, tmp;
  gmp_randstate_t state;

  mpz_init(a);
  mpz_init(b);
  mpz_init(c);
  mpz_init(d);

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

    gmp_printf("a=%Zx\n", a);
    gmp_printf("b=%Zx\n", b);
    gmp_printf("c=%Zx\n", c);
  }

  printf("\ntest ECC_BN_add_mod PASS\n");

  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(c);
  gmp_randclear(state);
}

void test___ecc_bn_sub_mod() {
  int i;
  mpz_t a, b, c, d;
  ECC_BN ec_a, ec_b, ec_c, tmp;
  gmp_randstate_t state;

  mpz_init(a);
  mpz_init(b);
  mpz_init(c);
  mpz_init(d);

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

    gmp_printf("a=%Zx\n", a);
    gmp_printf("b=%Zx\n", b);
    gmp_printf("c=%Zx\n", c);
  }

  printf("\ntest ECC_BN_sub_mod PASS\n");

  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(c);
  gmp_randclear(state);
}

void test___ecc_bn_mul_mod_p256() {
  int i;
  mpz_t a, b, c, d;
  ECC_BN ec_a, ec_b, ec_c, tmp;
  gmp_randstate_t state;

  mpz_init(a);
  mpz_init(b);
  mpz_init(c);
  mpz_init(d);

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

    gmp_printf("a=%Zx\n", a);
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

  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(c);
  gmp_randclear(state);
}

void test___ecc_bn_binary_inv() {
  int i;
  mpz_t a, p, a_inv;
  gmp_randstate_t state;

  ECC_BN ec_a, ec_expect, ec_got;

  mpz_init(a);
  mpz_init(p);
  mpz_init(a_inv);
  gmp_randinit_default(state);

  ECC_BN_to_mpz(p, &prime_p256);

  for (i = 0; i < 100000; i++) {
    mpz_urandomm(a, state, p);
    // mpz_set_ui(a, 3);
    mpz_invert(a_inv, a, p);

    mpz_to_ECC_BN(&ec_a, a);
    mpz_to_ECC_BN(&ec_expect, a_inv);

    ECC_BN_binary_inv(&ec_got, &ec_a);

    gmp_printf("a=%Zx\n", a);
    gmp_printf("p=%Zx\n", p);
    gmp_printf("a_inv_expect=%Zx\n", a_inv);

    if (ECC_BN_cmp(&ec_got, &ec_expect)) {
      printf("\ntest ECC_BN_binary_inv FAIL\n");
      ECC_BN_to_mpz(p, &ec_got);
      gmp_printf("a_inv_got=%Zx\n", p);
      return;
    }
  }

  printf("\ntest ECC_BN_binary_inv PASS\n");

  mpz_clear(a);
  mpz_clear(p);
  mpz_clear(a_inv);
  gmp_randclear(state);
}

void ECC_BN_print(ECC_BN *p) {
  mpz_t x;
  mpz_init(x);

  ECC_BN_to_mpz(x, p);

  gmp_printf("%Zx\n", x);

  mpz_clear(x);
}

void ECC_point_print(ECC_POINT *p) {
  mpz_t x, y;
  mpz_init(x);
  mpz_init(y);

  ECC_BN_to_mpz(x, &(p->x));
  ECC_BN_to_mpz(y, &(p->y));

  puts("R: ");
  gmp_printf("x=%Zx\n", x);
  gmp_printf("y=%Zx\n", y);
  printf("point_at_infinity=%d\n", p->point_at_infinity);

  mpz_clear(x);
  mpz_clear(y);
}

void test___ecc_point_add_dbl() {
  ECC_POINT R, minus_P;

  ECC_point_dbl(&R, &base_p256);
  ECC_point_copy(&minus_P, &base_p256);
  ECC_BN_sub_mod(&(minus_P.y), &prime_p256, &(minus_P.y), &prime_p256);
  ECC_point_add(&R, &R, &minus_P);


  if (!ECC_BN_cmp(&R.x, &base_p256.x) && !ECC_BN_cmp(&R.y, &base_p256.y) && (R.point_at_infinity == base_p256.point_at_infinity)) {
    puts("test ECC_point_add_dbl PASS");
  }
  else {
    puts("test ECC_point_add_dbl FAIL\n");

    puts("R: ");
    ECC_point_print(&R);

    puts("base_p256: ");
    ECC_point_print(&base_p256);
    return;
  }
}


void test___ecc_point_smul_2() {
  ECC_POINT result;

  ECC_point_smul(&result, &order_p256, &base_p256);

  if (result.point_at_infinity == 1) {
    puts("test ECC_point_smul_2 PASS");
  }
  else {
    puts("test ECC_point_smul_2 FAIL");
  }
}

void test___ecc_point_smul_1() {
  mpz_t a, b, order;
  
  gmp_randstate_t state;

  ECC_POINT pa, pb;
  ECC_BN ka, kb;

  mpz_init(a);
  mpz_init(b);
  mpz_init(order);
  gmp_randinit_default(state);

  ECC_BN_to_mpz(order, &order_p256);

  for(int i = 0; i < 20; i++) {
    mpz_urandomm(a, state, order);
    mpz_urandomm(b, state, order);

    gmp_printf("a=%Zx\n", a);
    gmp_printf("b=%Zx\n", b);

    mpz_to_ECC_BN(&ka, a);
    mpz_to_ECC_BN(&kb, b);

    ECC_point_smul(&pa, &ka, &base_p256);
    ECC_point_smul(&pb, &kb, &base_p256);

    // Swap two
    ECC_point_smul(&pa, &kb, &pa);
    ECC_point_smul(&pb, &ka, &pb);

    if (!(!ECC_BN_cmp(&pa.x, &pb.x) && !ECC_BN_cmp(&pa.y, &pb.y) && (pa.point_at_infinity == pb.point_at_infinity))) {
      puts("test ECC_point_smul_1 FAIL");
      goto EXIT;
    }
  }

  puts("test ECC_point_smul_1 PASS");

EXIT:
  
  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(order);
  gmp_randclear(state);
}

void test___ecc_pubkey_validation() {
  if (ECC_pubkey_validation(&base_p256) != ECC_PASS) {
     puts("test ECC_pubkey_validation FAIL");
     return;
  } 

  ECC_POINT mal_p;
  ECC_point_copy(&mal_p, &base_p256);

  mal_p.x.dat[4] = (mal_p.x.dat[4] ^ (0b0101010101010101010101010101010101010101010101010101010101010101));

  if (ECC_pubkey_validation(&mal_p) != ECC_FAIL) {
     puts("test ECC_pubkey_validation FAIL");
     return;
  } 

  puts("test ECC_pubkey_validation PASS");
}

int main() {
  // test___ecc_bn_add();
  // test___ecc_bn_sub();
  //  test___ecc_bn_add_mod();
  //  test___ecc_bn_sub_mod();
  // test___ecc_bn_mul_mod_p256();
  // test___ecc_bn_binary_inv();
  // test___ecc_point_add_dbl();
  // test___ecc_point_smul_1();
  // test___ecc_point_smul_2();
  test___ecc_pubkey_validation();
}