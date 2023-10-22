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
