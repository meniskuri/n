#pragma once
#define CC_(a, b) a##b
#define CC(a, b) CC_(a, b)
#define CCC(a, b, c) CC(CC(a, b), c)
#define CCCC(a, b, c, d) CCC(CC(a, b), c, d)
#define N(n) static void n(void ο(void *, void *), void *β)
#define Nand(n, nara, narb) Nandps(n, nara) narb(ο, β); }
#define Nor(n, nara, narb) Norps(n, nara) narb(ο, β); }
#define Nandps(n, nara)                                                        \
  N(CC(n, _pith));                                                             \
  N(n) { nara((void *)CC(n, _pith), (void *[]){ο, β}); }                       \
  N(CC(n, _pith)) {                                                            \
    ο = ((void **)β)[0];                                                       \
    β = ((void **)β)[1];
#define Norps(n, nara)                                                         \
  N(CC(n, _pith)) {                                                            \
    *((int **)β)[2] = 0;                                                       \
    ο = ((void **)β)[0];                                                       \
    β = ((void **)β)[1];                                                       \
    ο(ο, β);                                                                   \
  }                                                                            \
  N(n) {                                                                       \
    int r = 1;                                                                 \
    nara((void *)CC(n, _pith), (void *[]){ο, β, &r});                          \
    if (r == 0)                                                                \
      return;
