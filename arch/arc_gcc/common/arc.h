/* Copyright (C) 
 * 2014 - Wayne Ren
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/*
 *  The common resource definitions of ARC architecture processors
 *  (ARCv2)
 */

#ifndef TOPPERS_ARC_H
#define TOPPERS_ARC_H

#define ARC_REG_STATUS32	0xa


/* Build Configuration Registers */
#define ARC_REG_DCCMBASE_BCR	0x61	/* DCCM Base Addr */
#define ARC_REG_CRC_BCR		0x62
#define ARC_REG_DVFB_BCR	0x64
#define ARC_REG_EXTARITH_BCR	0x65
#define ARC_REG_VECBASE_BCR	0x68
#define ARC_REG_PERIBASE_BCR	0x69
#define ARC_REG_FP_BCR		0x6b	/* Single-Precision FPU */
#define ARC_REG_DPFP_BCR	0x6c	/* Dbl Precision FPU */
#define ARC_REG_D_CACHE_BUILD	0x72
#define ARC_REG_DCCM_BCR	0x74	/* DCCM Present + SZ */
#define ARC_REG_TIMERS_BCR	0x75
#define ARC_REG_I_CACHE_BUILD	0x77
#define ARC_REG_ICCM_BCR	0x78
#define ARC_REG_XY_MEM_BCR	0x79
#define ARC_REG_MAC_BCR		0x7a
#define ARC_REG_MUL_BCR		0x7b
#define ARC_REG_SWAP_BCR	0x7c
#define ARC_REG_NORM_BCR	0x7d
#define ARC_REG_MIXMAX_BCR	0x7e
#define ARC_REG_BARREL_BCR	0x7f
#define ARC_REG_D_UNCACH_BCR	0x6a


#define ARC_REG_LP_START	0x2	/* loop related aux regs */
#define ARC_REG_LP_END		0x3

/* Timer related aux registers */
#define ARC_REG_TIMER0_LIMIT	0x23	/* timer 0 limit */
#define ARC_REG_TIMER0_CTRL	0x22	/* timer 0 control */
#define ARC_REG_TIMER0_CNT	0x21	/* timer 0 count */
#define ARC_REG_TIMER1_LIMIT	0x102	/* timer 1 limit */
#define ARC_REG_TIMER1_CTRL	0x101	/* timer 1 control */
#define ARC_REG_TIMER1_CNT	0x100	/* timer 1 count */


/* code density related regs */
#define ARC_REG_JLI_BASE	0x290
#define ARC_REG_LDI_BASE	0x291
#define ARC_REG_EI_BASE		0x292

/* exception related aux registers */
#define ARC_REG_ERRET		0x400
#define ARC_REG_ERBTA		0x401
#define ARC_REG_ERSTATUS	0x402
#define ARC_REG_ECR		0x403
/* Interrupt related aux registers */
#define ARC_REG_IRQ_CTRL	0xe
#define ARC_REG_INT_VECT_BASE	0x25	/* interrupt vector base */
#define ARC_REG_IRQ_ACT		0x43	/* the priority active status */
#define ARC_REG_IRQ_CAUSE	0x40a	/* interrupt cause */
#define ARC_REG_IRQ_SELECT	0x40b	/* IRQ select */
#define ARC_REG_IRQ_PRIORITY	0x206	/* IRQ priority */
#define ARC_REG_IRQ_ENABLE	0x40c	/* enable selected IRQ */
#define ARC_REG_IRQ_TRIGGLE	0x40d	/* set irq triggle: level or pluse */
#define ARC_REG_IRQ_PENDING	0x416	/* the pending status of selected IRQ */
#define ARC_REG_IRQ_PLUSE_CLR	0x415	/* clear the pluse req of selected IRQ */
#define ARC_REG_IRQ_STATUS	0x40f	/* the status of selected IRQ */
#define ARC_REG_IRQ_PRI_PENDING 0x200	/* the priority pending status */
#define ARC_REG_IRQ_HINT	0x201	/* software trigger interrupt */


/* cache related aux regs */

#define ARC_REG_IC_IVIC		0x10	/* invalidate icache */
#define ARC_REG_IC_CTRL		0x11	/* icache ctrl reg */
#define ARC_REG_IC_LIL		0x13	/* lock icache line */
#define ARC_REG_IC_IVIL		0x19	/* invalidate icache line */
#define ARC_REG_IC_RAM_ADDR	0x1a	/* external access address */
#define ARC_REG_IC_TAG		0x1b	/* icache tag access */
#define ARC_REG_IC_DATA		0x1d	/* icache data access */

#define ARC_REG_DC_IVDC		0x47	/* invalidate dcache */
#define ARC_REG_DC_CTRL		0x48	/* dcache ctrl reg */
#define ARC_REG_DC_LDL		0x49	/* lock dcache line */
#define ARC_REG_DC_IVDL		0x4a	/* invalidate dcache line */
#define ARC_REG_DC_FLSH		0x4b	/* flush data cache */
#define ARC_REG_DC_FLDL		0x4c	/* flush data line */
#define ARC_REG_DC_RAM_ADDR	0x58	/* external access address */
#define ARC_REG_DC_TAG		0x59	/* dcache tag access */
#define ARC_REG_DC_DATA		0x5b	/* dcache data access */



/* status32 Bits Positions */
#define STATUS_AE_BIT		5	/* Exception active */
#define STATUS_DE_BIT		6	/* PC is in delay slot */
#define STATUS_U_BIT		7	/* User/Kernel mode */
#define STATUS_L_BIT		12	/* Loop inhibit */
#define STATUS_IE_BIT		31	/* interrupt enable bit */

/* These masks correspond to the status word(STATUS_32) bits */
#define STATUS_AE_MASK		(1<<STATUS_AE_BIT)
#define STATUS_DE_MASK		(1<<STATUS_DE_BIT)
#define STATUS_U_MASK		(1<<STATUS_U_BIT)
#define STATUS_L_MASK		(1<<STATUS_L_BIT)
#define STATUS_IE_MASK		(1<<STATUS_IE_BIT)

/*
 * ECR: Exception Cause Reg bits-n-pieces
 * [23:16] = Exception Vector
 * [15: 8] = Exception Cause Code
 * [ 7: 0] = Exception Parameters (for certain types only)
 */
#define ECR_VEC_MASK			0xff0000
#define ECR_CODE_MASK			0x00ff00
#define ECR_PARAM_MASK			0x0000ff

/* Exception Cause Vector Values */
#define ECR_V_INSN_ERR			0x02
#define ECR_V_MACH_CHK			0x20
#define ECR_V_ITLB_MISS			0x21
#define ECR_V_DTLB_MISS			0x22
#define ECR_V_PROTV			0x23
#define ECR_V_TRAP			0x25

/* Protection Violation Exception Cause Code Values */
#define ECR_C_PROTV_INST_FETCH		0x00
#define ECR_C_PROTV_LOAD		0x01
#define ECR_C_PROTV_STORE		0x02
#define ECR_C_PROTV_XCHG		0x03
#define ECR_C_PROTV_MISALIG_DATA	0x04

#define ECR_C_BIT_PROTV_MISALIG_DATA	10

/* Machine Check Cause Code Values */
#define ECR_C_MCHK_DUP_TLB		0x01

/* DTLB Miss Exception Cause Code Values */
#define ECR_C_BIT_DTLB_LD_MISS		8
#define ECR_C_BIT_DTLB_ST_MISS		9

/* Dummy ECR values for Interrupts */
#define event_IRQ1		0x0031abcd
#define event_IRQ2		0x0032abcd

/* Auxiliary registers */
#define AUX_IDENTITY		4
#define AUX_INTR_VEC_BASE	0x25


/*
 * Floating Pt Registers
 * Status regs are read-only (build-time) so need not be saved/restored
 */
#define ARC_AUX_FP_STAT         0x300
#define ARC_AUX_DPFP_1L         0x301
#define ARC_AUX_DPFP_1H         0x302
#define ARC_AUX_DPFP_2L         0x303
#define ARC_AUX_DPFP_2H         0x304
#define ARC_AUX_DPFP_STAT       0x305

/*
 *  ARC exception vector
 */
#define EXC_VECTOR_RESET	UINT_C(0x00)
#define EXC_VECTOR_MEM_ERR	UINT_C(0x04)
#define EXC_VECTOR_INS_ERR	UINT_C(0x08)
#define EXC_VECTOR_MAC_CHK	UINT_C(0x0C)
#define EXC_VECTOR_TLB_MISS_I	UINT_C(0x10)
#define EXC_VECTOR_TLB_MISS_D	UINT_C(0x14)
#define EXC_VECTOR_PRO_VIO	UINT_C(0x18)
#define EXC_VECTOR_PRI_VIO	UINT_C(0x1C)
#define EXC_VECTOR_SWI		UINT_C(0x20)
#define EXC_VECTOR_TRAP		UINT_C(0x24)
#define EXC_VECTOR_EXT		UINT_C(0x28)
#define EXC_VECTOR_DIV_ZER0	UINT_C(0x2C)
#define EXC_VECTOR_DC_ERR	UINT_C(0x30)
#define EXC_VECTOR_MAL_ALIGN	UINT_C(0x34)
#define EXC_VECTOR_RESERVE0	UINT_C(0x38)
#define EXC_VECTOR_RESERVE1	UINT_C(0x3C)

/*
 * ARC exception number
 */
#define EXC_NO_RESET		UINT_C(0)
#define EXC_NO_MEM_ERR		UINT_C(1)
#define EXC_NO_INS_ERR		UINT_C(2)
#define EXC_NO_MAC_CHK		UINT_C(3)
#define EXC_NO_TLB_MISS_I	UINT_C(4)
#define EXC_NO_TLB_MISS_D	UINT_C(5)
#define EXC_NO_PRO_VIO		UINT_C(6)
#define EXC_NO_PRI_VIO		UINT_C(7)
#define EXC_NO_SWI		UINT_C(8)
#define EXC_NO_TRAP		UINT_C(9)
#define EXC_NO_EXT		UINT_C(10)
#define EXC_NO_DIV_ZER0		UINT_C(11)
#define EXC_NO_DC_ERR		UINT_C(12)
#define EXC_NO_MAL_ALIGN	UINT_C(13)
#define EXC_NO_RESERVE0		UINT_C(14)
#define EXC_NO_RESERVE1		UINT_C(15)


/* exception frame offset */
#define P_EXCINF_OFFSET_ERBTA		0x0
#define P_EXCINF_OFFSET_ERET		0x1
#define P_EXCINF_OFFSET_ERSTATUS	0x2

#endif /* TOPPERS_ARC_H */
