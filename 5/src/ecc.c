#include "ecc.h"



#ifdef _GMP_ENABLE_
int mpz_to_ECC_BN(ECC_BN* c, mpz_t a) 
{
  int i;

  if (a->_mp_size < 0)
	return ECC_FAIL;

  if (a->_mp_size > ECC_WORD_LEN)
	return ECC_FAIL;
  
  for (i = 0; i < a->_mp_size; i++)
  {
	c->dat[i] = a->_mp_d[i];
  }
  c->len = a->_mp_size;

  return ECC_PASS;
}

int ECC_BN_to_mpz(mpz_t c, ECC_BN* a)
{
  int i;

  if (c->_mp_alloc < a->len) {
	mpz_realloc2(c, a->len << 5);
  }

  for (i = 0; i < a->len; i++) 
  {
	c->_mp_d[i] = a->dat[i];
  }
  c->_mp_size = a->len;

  return ECC_PASS;
}
#endif

// c = a
int ECC_BN_copy(ECC_BN* c, ECC_BN* a) 
{
  int i;

  for (i = 0; i < a->len; i++)
  {
	c->dat[i] = a->dat[i];
  }
  c->len = a->len;

  return ECC_PASS;
}

// a>b => 1 , a==b => 0, a<b => -1 
int ECC_BN_cmp(ECC_BN* a, ECC_BN* b)
{
  int i;

  if (a->len > b->len) {
	return 1;
  }
  
  if (a->len < b->len) {
	return -1;
  }

  for (i = a->len - 1; i >= 0; i--) {
	if (a->dat[i] > b->dat[i])
	  return 1;
	if (a->dat[i] < b->dat[i]) {
	  return -1;
	}
  }

  return 0;
}

// c= a<<1 
int ECC_BN_lshift_1bit(ECC_BN* c, ECC_BN* a)
{
  int i;
  
  c->dat[a->len] = a->dat[a->len - 1] >> 31;
  for (i = a->len-1; i > 0; i--)
  {
	c->dat[i] = (a->dat[i] << 1) | (a->dat[i-1] >> 31);
  }
  c->dat[0] = a->dat[0] << 1;
  c->len = a->len + c->dat[a->len];

  return ECC_PASS;
}

// c = a+b , |a|>=|b|
void __ECC_BN_add(ECC_BN* c, ECC_BN* a, ECC_BN* b)
{
  int          i = 0;
  unsigned int carry = 0;
  ECC_BN out;

  // i<b->len
  for (i = 0; i < b->len; i++) {
	out.dat[i] = a->dat[i] + b->dat[i] + carry;
	if (carry) {
	  carry = a->dat[i] >= (~b->dat[i]);
	}
	else {
	  carry = out.dat[i] < a->dat[i];
	}	
  }

  // i<a->len
  for (; i < a->len; i++) {
	out.dat[i] = a->dat[i] + carry;
	carry      = out.dat[i] < carry;
  }
  out.dat[i] = carry;
  out.len    = a->len + carry;

  ECC_BN_copy(c, &out);
}

// c = a-b, a>=b
void __ECC_BN_sub(ECC_BN* c, ECC_BN* a, ECC_BN* b)
{
  int i = 0;
  unsigned int borrow = 0;

  ECC_BN out;

  for (i = 0; i < b -> len; i++) {
    out.dat[i] = a->dat[i] - borrow - b->dat[i];

    if (borrow) {
      borrow = a->dat[i] <= b->dat[i];
    }
    else {
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
  for(i = a->len - 1; i >= 0; i--) {
    if (out.dat[i] != 0)
      break;
  }
  out.len = i + 1;

  ECC_BN_copy(c, &out);
}

//c= a+b mod p
int ECC_BN_add_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b, ECC_BN* p)
{
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

//c= a-b mod p
int ECC_BN_sub_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b, ECC_BN* p)
{
  ECC_BN out;

  if (ECC_BN_cmp(a, b) >= 0)
    __ECC_BN_sub(&out, a, b);
  else  {
    __ECC_BN_add(&out, a, p);
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
void __ECC_BN_mul(ECC_BN * c, ECC_BN * a, ECC_BN * b)
{
  int i, j;
  ECC_BN out;
  unsigned long long int carry = 0;
  if ((a->len == 0) || ( b->len==0)) {
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
  for (i = 0; i < ECC_WORD_LEN; i++)
    out.dat[i] = 0;
  for (i = 0; i < b->len; i++)
  {
    carry = 0;
    for (j = 0; j < a->len; j++)
    {
      carry = (unsigned long long int)carry
            + (unsigned long long int)a->dat[j] * b->dat[i]
            + (unsigned long long int)out.dat[i + j];
      out.dat[i+j] = (unsigned int)carry;
      carry = (unsigned long long int)carry >> 32;
    }
    out.dat[i + j] = carry;
  }
  out.len = a->len + b->len;
  if (out.dat[out.len - 1] == 0)
    out.len--;
  ECC_BN_copy(c, &out);
}

int ECC_BN_mod_p256(ECC_BN* c, ECC_BN* a)
{
  ECC_BN t1, t2, t3;
    // s2+s3    7    6    5    4    3  2   1   0 
      //s2 = (c15, c14, c13, c12, c11, 0, 0, 0), t1
      //s3 = (0,   c15, c14, c13, c12, 0, 0, 0), t2
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

    
    //s1 = (c7, c6, c5, c4, c3, c2, c1, c0),
    //s4 = (c15, c14, 0, 0, 0, c10, c9, c8),
    //s5 = (c8, c13, c15, c14, c13, c11, c10, c9),
    //s6 = (c10, c8, 0, 0, 0, c13, c12, c11),
    //s7 = (c11, c9, 0, 0, c15, c14, c13, c12),
    //s8 = (c12, 0, c10, c9, c8, c15, c14, c13),
    //s9 = (c13, 0, c11, c10, c9, 0, c15, c14).
    // Return(s1 + 2s2 + 2s3 + s4 + s5 −s6 −s7 −s8 −s9 mod p256).
  return ECC_PASS;
}
// c = a+b mod p256
int ECC_BN_mul_mod_p256(ECC_BN* c, ECC_BN* a, ECC_BN* b)
{
  __ECC_BN_mul(c, a, b);
  ECC_BN_mod_p256(c, c);
}
int ECC_BN_mul_mod(ECC_BN* c, ECC_BN* a, ECC_BN* b, ECC_BN* p)
{
  return ECC_PASS;
}