#include "ecc.h"

#ifdef _GMP_ENABLE_
int mpz_to_ECC_BN(ECC_BN* c, mpz_t a) {
  int i;

  if (a->_mp_size < 0) return ECC_FAIL;

  if (a->_mp_size > ECC_WORD_LEN) return ECC_FAIL;

  for (i = 0; i < a->_mp_size; i++) {
    c->dat[i] = a->_mp_d[i];
  }
  c->len = a->_mp_size;

  return ECC_PASS;
}

int ECC_BN_to_mpz(mpz_t c, ECC_BN* a) {
  int i;

  if (c->_mp_alloc < a->len) {
    mpz_realloc2(c, a->len << 5);
  }

  for (i = 0; i < a->len; i++) {
    c->_mp_d[i] = a->dat[i];
  }
  c->_mp_size = a->len;

  return ECC_PASS;
}
#endif

// c = a
int ECC_BN_copy(ECC_BN* c, ECC_BN* a) {
  int i;

  for (i = 0; i < a->len; i++) {
    c->dat[i] = a->dat[i];
  }
  c->len = a->len;

  return ECC_PASS;
}

// a>b => 1 , a==b => 0, a<b => -1
int ECC_BN_cmp(ECC_BN* a, ECC_BN* b) {
  int i;

  if (a->len > b->len) {
    return 1;
  }

  if (a->len < b->len) {
    return -1;
  }

  for (i = a->len - 1; i >= 0; i--) {
    if (a->dat[i] > b->dat[i]) return 1;
    if (a->dat[i] < b->dat[i]) {
      return -1;
    }
  }

  return 0;
}

// c= a<<1
int ECC_BN_lshift_1bit(ECC_BN* c, ECC_BN* a) {
  int i;

  c->dat[a->len] = a->dat[a->len - 1] >> 31;
  for (i = a->len - 1; i > 0; i--) {
    c->dat[i] = (a->dat[i] << 1) | (a->dat[i - 1] >> 31);
  }
  c->dat[0] = a->dat[0] << 1;
  c->len = a->len + c->dat[a->len];

  return ECC_PASS;
}

// c = a+b , |a|>=|b|
void __ECC_BN_add(ECC_BN* c, ECC_BN* a, ECC_BN* b) {
  int i = 0;
  unsigned int carry = 0;
  ECC_BN out;

  // i<b->len
  for (i = 0; i < b->len; i++) {
    out.dat[i] = a->dat[i] + b->dat[i] + carry;
    if (carry) {
      carry = a->dat[i] >= (~b->dat[i]);
    } else {
      carry = out.dat[i] < a->dat[i];
    }
  }

  // i<a->len
  for (; i < a->len; i++) {
    out.dat[i] = a->dat[i] + carry;
    carry = out.dat[i] < carry;
  }
  out.dat[i] = carry;
  out.len = a->len + carry;

  ECC_BN_copy(c, &out);
}

// c = a-b, a>=b
void __ECC_BN_sub(ECC_BN* c, ECC_BN* a, ECC_BN* b) {
  int i = 0;
  unsigned int borrow = 0;

  ECC_BN out;

  for (i = 0; i < b->len; i++) {
    out.dat[i] = a->dat[i] - borrow - b->dat[i];

    if (borrow) {
      borrow = a->dat[i] <= b->dat[i];
    } else {
      borrow = a->dat[i] < b->dat[i];
    }
  }

  for (; i < a->len; i++) {
    out.dat[i] = a->dat[i] - borrow;
    borrow = a->dat[i] < borrow;
  }

  if (borrow != 0) {
    puts("Something wrong!");
    return;
  }
  for (i = a->len - 1; i >= 0; i--) {
    if (out.dat[i] != 0) break;
  }
  out.len = i + 1;

  ECC_BN_copy(c, &out);
}

// c= a+b mod p
int ECC_BN_add_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b, ECC_BN* p) {
  ECC_BN out;

  if (ECC_BN_cmp(a, b) >= 0)
    __ECC_BN_add(&out, a, b);
  else
    __ECC_BN_add(&out, b, a);

  /**
  while (ECC_BN_cmp(&out, p) >= 0) {
    __ECC_BN_sub(&out, &out, p);
  }
  */
  // Optimize to cases where a < p, b < p
  if (ECC_BN_cmp(&out, p) >= 0) {
    __ECC_BN_sub(&out, &out, p);
  }

  ECC_BN_copy(c, &out);
}

// c= a-b mod p
int ECC_BN_sub_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b, ECC_BN* p) {
  
  ECC_BN out;

  if (ECC_BN_cmp(a, b) >= 0)
    __ECC_BN_sub(&out, a, b);
  else {
    __ECC_BN_add(&out, p, a);
    __ECC_BN_sub(&out, &out, b);
  }

  /**
  while (ECC_BN_cmp(&out, p) >= 0) {
    __ECC_BN_sub(&out, &out, p);
  }
  */
  // Optimize to cases where a < p, b < p
  // as a - b <= a < p, we don't need to compare with p

  ECC_BN_copy(c, &out);
}

// c=a*b
void __ECC_BN_mul(ECC_BN* c, ECC_BN* a, ECC_BN* b) {
  int i, j;
  ECC_BN out;
  unsigned long long int carry = 0;
  if ((a->len == 0) || (b->len == 0)) {
    c->len = 0;
    return;
  }

  /*
  // a* b0
  for (i = 0; i < a->len; i++)
  {
    carry = (unsigned long long int)carry
      + (unsigned long long int)a->dat[i] * b->dat[0];
    out.dat[i] = (unsigned int)carry;
    carry = (unsigned long long int)carry >> 32;
  }
  */
  for (i = 0; i < ECC_WORD_LEN; i++) out.dat[i] = 0;
  for (i = 0; i < b->len; i++) {
    carry = 0;
    for (j = 0; j < a->len; j++) {
      carry = (unsigned long long int)carry +
              (unsigned long long int)a->dat[j] * b->dat[i] +
              (unsigned long long int)out.dat[i + j];
      out.dat[i + j] = (unsigned int)carry;
      carry = (unsigned long long int)carry >> 32;
    }
    out.dat[i + j] = carry;
  }
  out.len = a->len + b->len;
  if (out.dat[out.len - 1] == 0) out.len--;
  ECC_BN_copy(c, &out);
}

int ECC_BN_mod_p256(ECC_BN* c, ECC_BN* _a) {
  ECC_BN t1, t2, t3;
  // s2+s3    7    6    5    4    3  2   1   0
  // s2 = (c15, c14, c13, c12, c11, 0, 0, 0), t1
  // s3 = (0,   c15, c14, c13, c12, 0, 0, 0), t2

  ECC_BN __a;
  ECC_BN_copy(&__a, _a);
  ECC_BN* a = &__a;

  for (int i = _a->len; i < ECC_WORD_LEN; i++) {
    a->dat[i] = 0;
  }

  t1.dat[0] = t2.dat[0] = t2.dat[7] = 0;
  t1.dat[1] = t2.dat[1] = 0;
  t1.dat[2] = t2.dat[2] = 0;
  t1.dat[3] = a->dat[11];
  t1.dat[4] = t2.dat[3] = a->dat[12];
  t1.dat[5] = t2.dat[4] = a->dat[13];
  t1.dat[6] = t2.dat[5] = a->dat[14];
  t1.dat[7] = t2.dat[6] = a->dat[15];
  t1.len = t2.len = 8;
  while ((t1.len > 0) && (t1.dat[t1.len - 1] == 0)) t1.len--;
  while ((t2.len > 0) && (t2.dat[t2.len - 1] == 0)) t2.len--;
  ECC_BN_add_mod(&t3, &t1, &t2, &prime_p256);
  ECC_BN_add_mod(&t3, &t3, &t3, &prime_p256);

  t1.dat[0] = a->dat[0];
  t1.dat[1] = a->dat[1];
  t1.dat[2] = a->dat[2];
  t1.dat[3] = a->dat[3];
  t1.dat[4] = a->dat[4];
  t1.dat[5] = a->dat[5];
  t1.dat[6] = a->dat[6];
  t1.dat[7] = a->dat[7];

  t1.len = 8;
  while ((t1.len > 0) && (t1.dat[t1.len - 1] == 0)) t1.len--;
  ECC_BN_add_mod(&t3, &t3, &t1, &prime_p256);

  t1.dat[0] = a->dat[8];
  t1.dat[1] = a->dat[9];
  t1.dat[2] = a->dat[10];
  t1.dat[3] = 0;
  t1.dat[4] = 0;
  t1.dat[5] = 0;
  t1.dat[6] = a->dat[14];
  t1.dat[7] = a->dat[15];

  t1.len = 8;
  while ((t1.len > 0) && (t1.dat[t1.len - 1] == 0)) t1.len--;
  ECC_BN_add_mod(&t3, &t3, &t1, &prime_p256);

  t1.dat[0] = a->dat[9];
  t1.dat[1] = a->dat[10];
  t1.dat[2] = a->dat[11];
  t1.dat[3] = a->dat[13];
  t1.dat[4] = a->dat[14];
  t1.dat[5] = a->dat[15];
  t1.dat[6] = a->dat[13];
  t1.dat[7] = a->dat[8];

  t1.len = 8;
  while ((t1.len > 0) && (t1.dat[t1.len - 1] == 0)) t1.len--;
  ECC_BN_add_mod(&t3, &t3, &t1, &prime_p256);

  t1.dat[0] = a->dat[11];
  t1.dat[1] = a->dat[12];
  t1.dat[2] = a->dat[13];
  t1.dat[3] = 0;
  t1.dat[4] = 0;
  t1.dat[5] = 0;
  t1.dat[6] = a->dat[8];
  t1.dat[7] = a->dat[10];

  t1.len = 8;
  while ((t1.len > 0) && (t1.dat[t1.len - 1] == 0)) t1.len--;
  ECC_BN_sub_mod(&t3, &t3, &t1, &prime_p256);

  t1.dat[0] = a->dat[12];
  t1.dat[1] = a->dat[13];
  t1.dat[2] = a->dat[14];
  t1.dat[3] = a->dat[15];
  t1.dat[4] = 0;
  t1.dat[5] = 0;
  t1.dat[6] = a->dat[9];
  t1.dat[7] = a->dat[11];

  t1.len = 8;
  while ((t1.len > 0) && (t1.dat[t1.len - 1] == 0)) t1.len--;
  ECC_BN_sub_mod(&t3, &t3, &t1, &prime_p256);

  t1.dat[0] = a->dat[13];
  t1.dat[1] = a->dat[14];
  t1.dat[2] = a->dat[15];
  t1.dat[3] = a->dat[8];
  t1.dat[4] = a->dat[9];
  t1.dat[5] = a->dat[10];
  t1.dat[6] = 0;
  t1.dat[7] = a->dat[12];

  t1.len = 8;
  while ((t1.len > 0) && (t1.dat[t1.len - 1] == 0)) t1.len--;
  ECC_BN_sub_mod(&t3, &t3, &t1, &prime_p256);

  t1.dat[0] = a->dat[14];
  t1.dat[1] = a->dat[15];
  t1.dat[2] = 0;
  t1.dat[3] = a->dat[9];
  t1.dat[4] = a->dat[10];
  t1.dat[5] = a->dat[11];
  t1.dat[6] = 0;
  t1.dat[7] = a->dat[13];

  t1.len = 8;
  while ((t1.len > 0) && (t1.dat[t1.len - 1] == 0)) t1.len--;
  ECC_BN_sub_mod(&t3, &t3, &t1, &prime_p256);

  ECC_BN_copy(c, &t3);

  // s1 = (c7, c6, c5, c4, c3, c2, c1, c0),
  // s4 = (c15, c14, 0, 0, 0, c10, c9, c8),
  // s5 = (c8, c13, c15, c14, c13, c11, c10, c9),
  // s6 = (c10, c8, 0, 0, 0, c13, c12, c11),
  // s7 = (c11, c9, 0, 0, c15, c14, c13, c12),
  // s8 = (c12, 0, c10, c9, c8, c15, c14, c13),
  // s9 = (c13, 0, c11, c10, c9, 0, c15, c14).
  //  Return(s1 + 2s2 + 2s3 + s4 + s5 −s6 −s7 −s8 −s9 mod p256).
  return ECC_PASS;
}
// c = a+b mod p256
int ECC_BN_mul_mod_p256(ECC_BN* c, ECC_BN* a, ECC_BN* b) {
  __ECC_BN_mul(c, a, b);
  ECC_BN_mod_p256(c, c);
}


// c = a mod p 
// a's MSB location % 32 <= p's MSB location % 32
int ECC_BN_shift_sub_mod(ECC_BN *c, ECC_BN *a, ECC_BN *p) {
  ECC_BN result;
  ECC_BN tmp;

  if (ECC_BN_cmp(a, p) < 0) {
    ECC_BN_copy(c, a);
    return ECC_PASS;
  }
  
  ECC_BN_copy(&result, a);
  ECC_BN_copy(&tmp, p);

  for(int i = p->len - 1; i >= 0; i--) {
    tmp.dat[a->len - p->len + i] = tmp.dat[i];
  }

  for(int i = 0; i < a->len; i++) {
    if (i < a->len - p->len) {
      tmp.dat[i] = 0;
    }
    else {
      tmp.dat[i] = p->dat[i - (a->len - p->len)];
    }
  }

  tmp.len = a->len;

  for(int i = 0; i < (a->len - p->len) * 32; i++) {
    if (ECC_BN_cmp(&result, &tmp) >= 0) {
      __ECC_BN_sub(&result, &result, &tmp);
    }
    ECC_BN_rshift_1bit(&tmp, &tmp);
  }
  if (ECC_BN_cmp(&result, &tmp) >= 0) {
    __ECC_BN_sub(&result, &result, &tmp);
  }
  ECC_BN_copy(c, &result);
}


int ECC_BN_mul_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b, ECC_BN* p) {
  ECC_BN result;
  __ECC_BN_mul(&result, a, b);
  ECC_BN_shift_sub_mod(&result, &result, p);
  ECC_BN_copy(c, &result);
  
  return ECC_PASS;
}

int ECC_BN_rshift_1bit(ECC_BN* c, ECC_BN* a)
{
  int i;

  if (a->len == 0) {
    c->len = 0;
    return ECC_PASS;
  }
  for (i = 0; i < (a->len - 1); i++) {
    c->dat[i] = (a->dat[i]>>1) | (a->dat[i+1]<<31);
  }
  c->dat[i] = a->dat[i] >> 1;
  if (c->dat[i]) {
    c->len = a->len;
  }
  else {
    c->len = a->len-1;
  }
  return ECC_PASS;
}

#define CHECK_IF_1(a) ((a.len == 1) && (a.dat[0] == 1))
#define CHECK_IF_EVEN(a) ((a.dat[0] & 1) == 0)
#define CHECK_IF_ODD(a) (!CHECK_IF_EVEN(a))

int ECC_BN_binary_inv(ECC_BN* c, ECC_BN* a) {
  ECC_BN u, v;
  ECC_BN x1, x2;

  ECC_BN_copy(&u, a);
  ECC_BN_copy(&v, &prime_p256);

  x1.len = 1;
  x1.dat[0] = 1;
  x2.len = 0;
  x2.dat[0] = 0;

  while ((!CHECK_IF_1(u)) && (!CHECK_IF_1(v))) {
    // printf("Loop...\n");
    while (CHECK_IF_EVEN(u)) {
      ECC_BN_rshift_1bit(&u, &u);
      if (CHECK_IF_EVEN(x1)) {
        ECC_BN_rshift_1bit(&x1, &x1);
      }
      else {
        __ECC_BN_add(&x1, &prime_p256, &x1);
        ECC_BN_rshift_1bit(&x1, &x1);
      }
    }
    while (CHECK_IF_EVEN(v)) {
      ECC_BN_rshift_1bit(&v, &v);
      if (CHECK_IF_EVEN(x2)) {
        ECC_BN_rshift_1bit(&x2, &x2);
      }
      else {
        __ECC_BN_add(&x2, &prime_p256, &x2);
        ECC_BN_rshift_1bit(&x2, &x2);
      }
    }

    if (ECC_BN_cmp(&u, &v) >= 0) {
      ECC_BN_sub_mod(&u, &u, &v, &prime_p256);
      ECC_BN_sub_mod(&x1, &x1, &x2, &prime_p256);
    }
    else {
      ECC_BN_sub_mod(&v, &v, &u, &prime_p256);
      ECC_BN_sub_mod(&x2, &x2, &x1, &prime_p256);
    }
  }

  if (CHECK_IF_1(u)) {
    ECC_BN_copy(c, &x1);
  }
  else {
    ECC_BN_copy(c, &x2);
  }
}

int ECC_point_init(ECC_POINT* P)
{
  P->point_at_infinity = 1;
  return ECC_PASS;
}
int ECC_point_copy(ECC_POINT* R, ECC_POINT* P)
{
  ECC_BN_copy(&R->x, &P->x);
  ECC_BN_copy(&R->y, &P->y);
  R->point_at_infinity = P->point_at_infinity;
  return ECC_PASS;
}

//P+Q=R
int ECC_point_add(ECC_POINT* R, ECC_POINT* P, ECC_POINT* Q)
{
  if (P->point_at_infinity == 1) {
    ECC_point_copy(R, Q);
    return ECC_PASS;
  }
  if (Q->point_at_infinity == 1) {
    ECC_point_copy(R, P);
    return ECC_PASS;
  }
  if (!ECC_BN_cmp(&P->x, &Q->x))
  {
    if (!ECC_BN_cmp(&P->y, &Q->y)) {
      ECC_point_dbl(R, P);
      return ECC_PASS;
    }
    else {
      R->point_at_infinity = 1;
      return ECC_PASS;
    }
  }
  else {
    ECC_POINT RESULT;
    ECC_BN l, d;

    // Calculate lambda 
    ECC_BN_sub_mod(&d, &(Q->x), &(P->x), &prime_p256);
    ECC_BN_binary_inv(&d, &d);
    ECC_BN_sub_mod(&l, &(Q->y), &(P->y), &prime_p256);
    ECC_BN_mul_mod_p256(&l, &l, &d);

    // x_3
    ECC_BN_mul_mod_p256(&(RESULT.x), &l, &l);
    ECC_BN_sub_mod(&(RESULT.x), &(RESULT.x), &(P->x), &prime_p256);
    ECC_BN_sub_mod(&(RESULT.x), &(RESULT.x), &(Q->x), &prime_p256);

    // y_3
    ECC_BN_sub_mod(&(RESULT.y), &(P->x), &(RESULT.x), &prime_p256);
    ECC_BN_mul_mod_p256(&(RESULT.y), &l, &(RESULT.y));
    ECC_BN_sub_mod(&(RESULT.y), &(RESULT.y), &(P->y), &prime_p256);

    
    RESULT.point_at_infinity = 0;

    ECC_point_copy(R, &RESULT);
  }
  return ECC_PASS;
}
//2P=R
int ECC_point_dbl(ECC_POINT* R, ECC_POINT* P)
{
  if (P->point_at_infinity == 1) {
    R->point_at_infinity = 1;
    return ECC_PASS;
  }

  ECC_BN l, t, d;
  ECC_POINT RESULT;

  // Calculate lambda
  ECC_BN_lshift_1bit(&d, &(P->y));
  ECC_BN_mod_p256(&d, &d);
  ECC_BN_binary_inv(&d, &d);
  ECC_BN_mul_mod_p256(&t, &(P->x), &(P->x));
  ECC_BN_lshift_1bit(&l, &t);
  ECC_BN_mod_p256(&l, &l);
  ECC_BN_add_mod(&l, &l, &t, &prime_p256);
  ECC_BN_add_mod(&l, &l, &ceff_a_p256, &prime_p256);
  ECC_BN_mul_mod_p256(&l, &l, &d);

  // x_3
  ECC_BN_mul_mod_p256(&(RESULT.x), &l, &l);
  ECC_BN_lshift_1bit(&t, &(P->x));
  ECC_BN_mod_p256(&t, &t);
  ECC_BN_sub_mod(&(RESULT.x), &(RESULT.x), &t, &prime_p256);

  // y_3
  ECC_BN_sub_mod(&(RESULT.y), &(P->x), &(RESULT.x), &prime_p256);
  ECC_BN_mul_mod_p256(&(RESULT.y), &l, &(RESULT.y));
  ECC_BN_sub_mod(&(RESULT.y), &(RESULT.y), &(P->y), &prime_p256);

  RESULT.point_at_infinity = 0;
  ECC_point_copy(R, &RESULT);
  return ECC_PASS;
}

int ECC_point_smul(ECC_POINT* R, ECC_BN* k, ECC_POINT* P)
{
  ECC_POINT RESULT;
  RESULT.point_at_infinity = 1;

  for(int i = k->len - 1; i >= 0; i--) {
    for(int j = sizeof(*(k->dat))*8 - 1; j >= 0; j--) {
      ECC_point_dbl(&RESULT, &RESULT);
      if (((k->dat[i]) >> j) & 1) {
        ECC_point_add(&RESULT, &RESULT, P);
      }
    }
  }
  ECC_point_copy(R, &RESULT);
  return ECC_PASS;
}

int ECC_pubkey_validation(ECC_POINT* P)
{
  //1. Verify that Q = ∞.
  if (P->point_at_infinity == 1)
    return ECC_FAIL;
  //2. represented elements of Fq

  if (ECC_BN_cmp(&(P->x), &prime_p256) >= 0) {
    return ECC_FAIL;
  }
  
  if (ECC_BN_cmp(&(P->y), &prime_p256) >= 0) {
    return ECC_FAIL;
  }

  //3. Verify that Q satisfies the elliptic curve equation defined by a and b.

  ECC_BN left, right, tmp;

  ECC_BN_mul_mod_p256(&left, &(P->y), &(P->y));
  ECC_BN_mul_mod_p256(&right, &(P->x), &(P->x));
  ECC_BN_mul_mod_p256(&right, &right, &(P->x));
  ECC_BN_mul_mod_p256(&tmp, &ceff_a_p256, &(P->x));
  ECC_BN_add_mod(&right, &right, &tmp, &prime_p256);
  ECC_BN_add_mod(&right, &right, &(ceff_b_p256), &prime_p256);

  if (ECC_BN_cmp(&left, &right) != 0) {
    return ECC_FAIL;
  }
  
  //4. Verify that nQ = ∞.

  ECC_POINT result;

  ECC_point_smul(&result, &order_p256, P);
  if (result.point_at_infinity != 1) {
    return ECC_FAIL;
  }
  
  //5. If any verification fails then return(“Invalid”); else return(“Valid”).
  return ECC_PASS;
}
