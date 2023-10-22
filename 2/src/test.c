// gcc -o test test.c -g -m32 -lgmp

#include <stdio.h>
#include <gmp.h>

int main() {
    mpz_t           p, q, n, e, d, m, c, out, phi;
    gmp_randstate_t state;

    mpz_init(p); mpz_init(q); mpz_init(n); mpz_init(e); mpz_init(d);
    mpz_init(m); mpz_init(c); mpz_init(out); mpz_init(phi);
    gmp_randinit_default(state);

    // 키 생성 : |n|=2048bit
    // e= 2^16+1
    mpz_set_ui(e, 0x10001); // mpz_set_ui(e, 65537);

    // p생성 : 난수생성 -> 소수 판정, gcd(p-1,e)=1
    mpz_urandomb(p, state, 1024);  // 1024bit 홀수
    p->_mp_d[0]  = p->_mp_d[0]  | 1;
    p->_mp_d[31] = p->_mp_d[31] | 0x80000000;

    while (1) {
        if (mpz_probab_prime_p(p, 56)) { // 소수 만족
        p->_mp_d[0] = p->_mp_d[0] &0xfffffffe;
        mpz_gcd(n, p, e);
        p->_mp_d[0] = p->_mp_d[0] | 1;
        if (mpz_cmp_ui(n, 1) == 0) { // 서로소 만족
            break;
        }
        }
        mpz_add_ui(p, p, 2);
    }


    // q생성 : 난수생성 -> 소수 판정, gcd(q-1,e)=1
    mpz_urandomb(q, state, 1024);  // 1024bit 홀수
    q->_mp_d[0]  = q->_mp_d[0]  | 1;
    q->_mp_d[31] = q->_mp_d[31] | 0x80000000;

    while (1) {
        if (mpz_probab_prime_p(q, 56)) { // 소수 만족
        q->_mp_d[0] = q->_mp_d[0] &0xfffffffe;
        mpz_gcd(n, q, e);
        q->_mp_d[0] = q->_mp_d[0] | 1;
        if (mpz_cmp_ui(n, 1) == 0) { // 서로소 만족
            break;
        }
        }
        mpz_add_ui(q, q, 2);
    }

    
    // d = e^(-1) mod phi(n)=(p-1)(q-1) : mpz_invert()
    mpz_mul(phi, p, q);
    mpz_invert(d, e, phi);

    // n = pq  : mpz_mul() 
    p->_mp_d[0]  = p->_mp_d[0] | 1;
    q->_mp_d[0]  = q->_mp_d[0] | 1;
    mpz_mul(n, p, q);

    // 메시지 선택 : "여러분이 선택"

    char *msg = "hello world";
    mpz_init_set_str(m, msg, sizeof(msg));

    // 암호화 
    // m^e mod n = c : mpz_powm

    mpz_powm(c, m, e, n);

    // 복호화
    // c^d mod n = out : mpz_powm

    mpz_powm(out, c, d, n);
    
    // 메시지 비교 및 화면 출력

    if (mpz_cmp(m, out) == 0){
        puts("Correct :)");
    }
    else {
        puts("Wrong :(");
    }


    puts("haha");


    mpz_clear(p); mpz_clear(q); mpz_clear(n); mpz_clear(e); mpz_clear(d);
    mpz_clear(m); mpz_clear(c); mpz_clear(out); mpz_clear(phi);
    gmp_randclear(state);
 
    return 0;
}