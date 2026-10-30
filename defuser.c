#include "defuser.h"
#include <inttypes.h>
#include <stdio.h>

static int answer(char *out, size_t cap, const uint32_t *v, size_t n) {
    size_t used = 0;
    for (size_t i = 0; i < n; ++i) {
        int k = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", v[i]);
        if (k < 0 || (size_t)k >= cap - used) return 1;
        used += (size_t)k;
    }
    return 0;
}
int solve_phase_1(uint64_t c, char *out, size_t cap) {
    uint32_t hi = (uint32_t)(c >> 32), lo = (uint32_t)c;
    uint32_t a = (hi - (uint32_t)(c >> 16)) * 3u + 0xdc2cu;
    uint32_t v[] = { ((hi + 0xb4dbu) ^ lo) * 2u + (a ^ 0x4797u) };
    return answer(out, cap, v, 1);
}
int solve_phase_2(uint64_t c, char *out, size_t cap) {
    uint32_t v[6]; v[0] = (uint32_t)((c >> 33) & 31u) * 3u;
    for (unsigned i = 1; i < 6; ++i)
        v[i] = (v[i-1] * 3u - i) ^ ((uint32_t)((c >> (4 + 3*i)) & 63u) + 11u);
    return answer(out, cap, v, 6);
}
int solve_phase_3(uint64_t c, char *out, size_t cap) {
    uint32_t v[2], x = (uint32_t)(c >> 3) ^ (uint32_t)(c >> 32);
    v[0] = (uint32_t)c & 7u;
    static const unsigned cases[] = {6, 2, 1, 0, 5, 3, 7, 4};
    switch (cases[v[0]]) {
    case 0: v[1] = (x*2u)^0x599cu; break;
    case 1: v[1] = (x^0x9af9u)+(x>>1); break;
    case 2: v[1] = x*5u+0x5d57u; break;
    case 3: v[1] = x*5u+0x2b917u; break;
    case 4: v[1] = (x^0x9ec4u)+(x>>2); break;
    case 5: v[1] = x*5u+0x2e883u; break;
    case 6: v[1] = (x*2u)^0xf9fu; break;
    default: v[1] = x*5u+0xd7f0u; break;
    }
    return answer(out, cap, v, 2);
}
static uint32_t walk(uint32_t x, uint32_t lo, uint32_t hi, uint32_t salt) {
    uint32_t mid = lo + (hi-lo)/2u;
    if (mid == x) return (mid+0x31u)^salt;
    if (x < mid) return (walk(x, lo, mid-1u, salt)*5u) ^ (mid+salt) ^ 0x13u;
    return (walk(x, mid+1u, hi, salt)*5u) ^ (mid+salt) ^ 0x17u;
}
int solve_phase_4(uint64_t c, char *out, size_t cap) {
    uint32_t v[] = {walk((uint32_t)c & 127u, 0, 127, (uint32_t)(c>>7)^(uint32_t)(c>>32))};
    return answer(out, cap, v, 1);
}
int solve_phase_5(uint64_t c, char *out, size_t cap) {
    static const uint32_t val[] = {0x664e,0x2d99,0x2b33,0x5b8,0x7d31,0xedc0,0x9539,0x23c8,0x3fae,0x4dee,0x9de1,0xf446};
    static const uint32_t tag[] = {0x179,0x668,0x31,0xb4d,0x835,0x414,0xa41,0x256,0x316,0x776,0x57e,0x948};
    static const unsigned next[] = {0,2,6,0,3,9,8,4,7,1,11,5};
    static const unsigned head[] = {10,11,5,9,1,2,6,8};
    unsigned node = head[c & 7u], steps = (unsigned)((c>>3)&3u);
    uint32_t a = ((uint32_t)(c>>6) ^ (uint32_t)(c>>32) ^ 0xae5au);
    for (unsigned i=0; i<=steps; ++i) { a = (a*3u + val[node]) ^ tag[node]; if (i<steps) node=next[node]; }
    uint32_t v[] = {a,tag[node]}; return answer(out,cap,v,2);
}
static uint32_t op(unsigned n, uint32_t x) {
    switch(n) {
    case 0: return (x>>2)|(x<<30);
    case 1: return ((x<<7)|(x>>25)) ^ 0xea9u;
    case 2: return (x^0x6591u)+x*8u;
    case 3: return (x>>2)|(x<<30);
    case 4: return ((x<<5)+0x95e6u)^(x*5u);
    case 5: return (x+0x3a06u)^(x>>1);
    case 6: return (x^(x>>7))+0x2b7au;
    default: return (x^0x6591u)+x*8u;
    }
}
int solve_phase_6(uint64_t c, char *out, size_t cap) {
    uint32_t x=(uint32_t)(c>>6)^(uint32_t)(c>>32);
    uint32_t v[] = {op((unsigned)((c>>3)&7u),op((unsigned)(c&7u),x)^0xb38u)+0x8b25u};
    return answer(out,cap,v,1);
}
int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t a=(uint32_t)challenge^(uint32_t)(challenge>>32);
    uint32_t b=(uint32_t)context^(uint32_t)(context>>32);
    uint32_t idx=(uint32_t)context&3u, x=a^b;
    uint32_t y=((x^0x691cu)+((x>>3)^0x1719u)) ^ (idx*0x691cu) ^ (uint32_t)(context>>16);
    uint32_t z=y ^ (b+idx+0x1719u+(a>>3));
    int n=snprintf(out,cap,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOPEN%" PRIu32 " %" PRIu32 " %" PRIu32,idx,y,z);
    return n<0 || (size_t)n>=cap;
}
