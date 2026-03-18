#ifndef _CHIPYARD_ASM_H
#define _CHIPYARD_ASM_H

#ifdef __CHERI_PURE_CAPABILITY__
#if __riscv_xlen == 64
#define LREG cld
#define SREG csd
#define REGBYTES 8
#else
#define LREG clw
#define SREG csw
#define REGBYTES 4
#endif
#define LWORD clw
#define SWORD csw
#else /* __CHERI_PURE_CAPABILITY__ */
#if __riscv_xlen == 64
#define LREG ld
#define SREG sd
#define REGBYTES 8
#else
#define LREG lw
#define SREG sw
#define REGBYTES 4
#endif
#define LWORD lw
#define SWORD sw
#endif /* __CHERI_PURE_CAPABILITY__ */

#ifdef __ASSEMBLY__

    /* Signal barrier release */
    .macro BARRIER_PASS flag
    li t0, -1
    fence w, w
#ifdef __CHERI_PURE_CAPABILITY__
    lgc ct1,\flag
    csw t0, 0(ct1)
#else
    sw t0, \flag, t1
#endif
    .endm

    /* Wait at barrier */
    .macro BARRIER_WAIT flag
#ifdef __CHERI_PURE_CAPABILITY__
    llc ct1,\flag
457:
    clw t0, 0(ct1)
#else
456:
    auipc t1, %pcrel_hi(\flag)
457:
    lw t0, %pcrel_lo(456b)(t1)
#endif
    beqz t0, 457b
    fence r, r
    .endm

#endif /* __ASSEMBLY__ */

#endif /* _CHIPYARD_ASM_H */
