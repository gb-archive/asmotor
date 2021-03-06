/*  Copyright 2008-2017 Carsten Elton Sorensen

    This file is part of ASMotor.

    ASMotor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ASMotor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ASMotor.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "xasm.h"
#include "lexer.h"
#include "localasm.h"

static SLexInitString localstrings[]=
{
	{ "abcd",	T_68K_ABCD	},
	{ "add",	T_68K_ADD	},
	{ "adda",	T_68K_ADD	},
	{ "addi",	T_68K_ADDI	},
	{ "addq",	T_68K_ADDQ	},
	{ "addx",	T_68K_ADDX	},
	{ "and",	T_68K_AND	},
	{ "andi",	T_68K_ANDI	},
	{ "asl",	T_68K_ASL	},
	{ "asr",	T_68K_ASR	},

	// Bcc start
	{ "bcc",	T_68K_BCC	},
	{ "bhs",	T_68K_BCC	},
	{ "bcs",	T_68K_BCS	},
	{ "blo",	T_68K_BCS	},
	{ "beq",	T_68K_BEQ	},
	{ "bge",	T_68K_BGE	},
	{ "bgt",	T_68K_BGT	},
	{ "bhi",	T_68K_BHI	},
	{ "ble",	T_68K_BLE	},
	{ "bls",	T_68K_BLS	},
	{ "blt",	T_68K_BLT	},
	{ "bmi",	T_68K_BMI	},
	{ "bne",	T_68K_BNE	},
	{ "bpl",	T_68K_BPL	},
	{ "bvc",	T_68K_BVC	},
	{ "bvs",	T_68K_BVS	},
	// Bcc end

	{ "bchg",	T_68K_BCHG		},
	{ "bclr",	T_68K_BCLR		},
	{ "bfchg",	T_68K_BFCHG		},
	{ "bfclr",	T_68K_BFCLR		},
	{ "bfexts",	T_68K_BFEXTS	},
	{ "bfextu",	T_68K_BFEXTU	},
	{ "bfffo",	T_68K_BFFFO		},
	{ "bfins",	T_68K_BFINS		},
	{ "bfset",	T_68K_BFSET		},
	{ "bftst",	T_68K_BFTST		},
	{ "bkpt",	T_68K_BKPT		},
	{ "bra",	T_68K_BRA		},
	{ "bset",	T_68K_BSET		},
	{ "bsr",	T_68K_BSR		},
	{ "btst",	T_68K_BTST		},
	{ "callm",	T_68K_CALLM		},
	{ "cas",	T_68K_CAS		},
	{ "cas2",	T_68K_CAS2		},
	{ "chk",	T_68K_CHK		},
	{ "chk2",	T_68K_CHK2		},
	{ "cinva",	T_68K_CINVA		},
	{ "cinvl",	T_68K_CINVL		},
	{ "cinvp",	T_68K_CINVP		},
	{ "clr",	T_68K_CLR		},
	{ "cmp",	T_68K_CMP		},
	{ "cmpa",	T_68K_CMPA		},
	{ "cmpi",	T_68K_CMPI		},
	{ "cmpm",	T_68K_CMPM		},
	{ "cmp2",	T_68K_CMP2		},
	{ "cpusha",	T_68K_CPUSHA	},
	{ "cpushl",	T_68K_CPUSHL	},
	{ "cpushp",	T_68K_CPUSHP	},

	{ "dbcc",	T_68K_DBCC	},
	{ "dbhs",	T_68K_DBCC	},
	{ "dbcs",	T_68K_DBCS	},
	{ "dblo",	T_68K_DBCS	},
	{ "dbeq",	T_68K_DBEQ	},
	{ "dbf",	T_68K_DBF	},
	{ "dbra",	T_68K_DBF	},
	{ "dbge",	T_68K_DBGE	},
	{ "dbgt",	T_68K_DBGT	},
	{ "dbhi",	T_68K_DBHI	},
	{ "dble",	T_68K_DBLE	},
	{ "dbls",	T_68K_DBLS	},
	{ "dblt",	T_68K_DBLT	},
	{ "dbmi",	T_68K_DBMI	},
	{ "dbne",	T_68K_DBNE	},
	{ "dbpl",	T_68K_DBPL	},
	{ "dbt",	T_68K_DBT	},
	{ "dbvc",	T_68K_DBVC	},
	{ "dbvc",	T_68K_DBVS	},

	{ "divs",	T_68K_DIVS	},
	{ "divsl",	T_68K_DIVSL	},
	{ "divu",	T_68K_DIVU	},
	{ "divul",	T_68K_DIVUL	},
	{ "eor",	T_68K_EOR	},
	{ "eori",	T_68K_EORI	},
	{ "exg",	T_68K_EXG	},
	{ "ext",	T_68K_EXT	},
	{ "extb",	T_68K_EXTB	},
	{ "illegal",	T_68K_ILLEGAL	},
	{ "jmp",	T_68K_JMP	},
	{ "jsr",	T_68K_JSR	},
	{ "lea",	T_68K_LEA	},
	{ "link",	T_68K_LINK	},
	{ "lsl",	T_68K_LSL	},
	{ "lsr",	T_68K_LSR	},
	{ "move",	T_68K_MOVE	},
	{ "movea",	T_68K_MOVEA	},
	{ "movec",	T_68K_MOVEC	},
	{ "move16",	T_68K_MOVE16	},
	{ "movem",	T_68K_MOVEM	},
	{ "movep",	T_68K_MOVEP	},
	{ "moveq",	T_68K_MOVEQ	},
	{ "moves",	T_68K_MOVES	},
	{ "muls",	T_68K_MULS	},
	{ "mulu",	T_68K_MULU	},
	{ "nbcd",	T_68K_NBCD	},
	{ "neg",	T_68K_NEG	},
	{ "negx",	T_68K_NEGX	},
	{ "nop",	T_68K_NOP	},
	{ "not",	T_68K_NOT	},
	{ "or",		T_68K_OR	},
	{ "ori",	T_68K_ORI	},
	{ "pack",	T_68K_PACK	},
	{ "pea",	T_68K_PEA	},
	{ "reset",	T_68K_RESET	},
	{ "rol",	T_68K_ROL	},
	{ "ror",	T_68K_ROR	},
	{ "roxl",	T_68K_ROXL	},
	{ "roxr",	T_68K_ROXR	},
	{ "rtd",	T_68K_RTD	},
	{ "rte",	T_68K_RTE	},
	{ "rtm",	T_68K_RTM	},
	{ "rtr",	T_68K_RTR	},
	{ "rts",	T_68K_RTS	},
	{ "sbcd",	T_68K_SBCD	},

	{ "scc",	T_68K_SCC	},
	{ "shs",	T_68K_SCC	},
	{ "scs",	T_68K_SCS	},
	{ "slo",	T_68K_SCS	},
	{ "seq",	T_68K_SEQ	},
	{ "sf",		T_68K_SF	},
	{ "sge",	T_68K_SGE	},
	{ "sgt",	T_68K_SGT	},
	{ "shi",	T_68K_SHI	},
	{ "sle",	T_68K_SLE	},
	{ "sls",	T_68K_SLS	},
	{ "slt",	T_68K_SLT	},
	{ "smi",	T_68K_SMI	},
	{ "sne",	T_68K_SNE	},
	{ "spl",	T_68K_SPL	},
	{ "st",		T_68K_ST	},
	{ "svc",	T_68K_SVC	},
	{ "svc",	T_68K_SVS	},

	{ "stop",	T_68K_STOP	},
	{ "sub",	T_68K_SUB	},
	{ "suba",	T_68K_SUBA	},
	{ "subi",	T_68K_SUBI	},
	{ "subq",	T_68K_SUBQ	},
	{ "subx",	T_68K_SUBX	},
	{ "swap",	T_68K_SWAP	},
	{ "tas",	T_68K_TAS	},
	{ "trap",	T_68K_TRAP	},

	{ "trapcc",	T_68K_TRAPCC	},
	{ "traphs",	T_68K_TRAPCC	},
	{ "trapcs",	T_68K_TRAPCS	},
	{ "traplo",	T_68K_TRAPCS	},
	{ "trapeq",	T_68K_TRAPEQ	},
	{ "trapf",	T_68K_TRAPF		},
	{ "trapge",	T_68K_TRAPGE	},
	{ "trapgt",	T_68K_TRAPGT	},
	{ "traphi",	T_68K_TRAPHI	},
	{ "traple",	T_68K_TRAPLE	},
	{ "trapls",	T_68K_TRAPLS	},
	{ "traplt",	T_68K_TRAPLT	},
	{ "trapmi",	T_68K_TRAPMI	},
	{ "trapne",	T_68K_TRAPNE	},
	{ "trappl",	T_68K_TRAPPL	},
	{ "trapt",	T_68K_TRAPT		},
	{ "trapvc",	T_68K_TRAPVC	},
	{ "trapvc",	T_68K_TRAPVS	},

	{ "trapv",	T_68K_TRAPV	},
	{ "tst",	T_68K_TST	},
	{ "unlk",	T_68K_UNLK	},
	{ "unpk",	T_68K_UNPK	},
	{ "unpack",	T_68K_UNPK	},

	{ "d0",	T_68K_REG_D0	},
	{ "d1",	T_68K_REG_D1	},
	{ "d2",	T_68K_REG_D2	},
	{ "d3",	T_68K_REG_D3	},
	{ "d4",	T_68K_REG_D4	},
	{ "d5",	T_68K_REG_D5	},
	{ "d6",	T_68K_REG_D6	},
	{ "d7",	T_68K_REG_D7	},

	{ "a0",	T_68K_REG_A0	},
	{ "a1",	T_68K_REG_A1	},
	{ "a2",	T_68K_REG_A2	},
	{ "a3",	T_68K_REG_A3	},
	{ "a4",	T_68K_REG_A4	},
	{ "a5",	T_68K_REG_A5	},
	{ "a6",	T_68K_REG_A6	},
	{ "a7",	T_68K_REG_A7	},
	{ "sp",	T_68K_REG_A7	},

	{ "ccr",	T_68K_REG_CCR	},
	{ "sr",		T_68K_REG_SR	},
	{ "bc",		T_68K_REG_BC	},
	{ "dc",		T_68K_REG_DC	},
	{ "ic",		T_68K_REG_IC	},
	{ "usp",	T_68K_REG_USP	},
	{ "sfc",	T_68K_REG_SFC	},
	{ "dfc",	T_68K_REG_DFC	},
	{ "usp",	T_68K_REG_USP	},
	{ "vbr",	T_68K_REG_VBR	},
	{ "cacr",	T_68K_REG_CACR	},
	{ "caar",	T_68K_REG_CAAR	},
	{ "msp",	T_68K_REG_MSP	},
	{ "isp",	T_68K_REG_ISP	},
	{ "tc",		T_68K_REG_TC	},
	{ "itt0",	T_68K_REG_ITT0	},
	{ "itt1",	T_68K_REG_ITT1	},
	{ "dtt0",	T_68K_REG_DTT0	},
	{ "dtt1",	T_68K_REG_DTT1	},
	{ "mmusr",	T_68K_REG_MMUSR	},
	{ "urp",	T_68K_REG_URP	},
	{ "srp",	T_68K_REG_SRP	},
	{ "iacr0",	T_68K_REG_IACR0	},
	{ "iacr1",	T_68K_REG_IACR1	},
	{ "dacr0",	T_68K_REG_DACR0	},
	{ "dacr1",	T_68K_REG_DACR1	},

	{ "pc",		T_68K_REG_PC		},
	{ "(pc)",	T_68K_REG_PC_IND	},

	{ "(a0)",	T_68K_REG_A0_IND	},
	{ "(a1)",	T_68K_REG_A1_IND	},
	{ "(a2)",	T_68K_REG_A2_IND	},
	{ "(a3)",	T_68K_REG_A3_IND	},
	{ "(a4)",	T_68K_REG_A4_IND	},
	{ "(a5)",	T_68K_REG_A5_IND	},
	{ "(a6)",	T_68K_REG_A6_IND	},
	{ "(a7)",	T_68K_REG_A7_IND	},
	{ "(sp)",	T_68K_REG_A7_IND	},

	{ "-(a0)",	T_68K_REG_A0_DEC	},
	{ "-(a1)",	T_68K_REG_A1_DEC	},
	{ "-(a2)",	T_68K_REG_A2_DEC	},
	{ "-(a3)",	T_68K_REG_A3_DEC	},
	{ "-(a4)",	T_68K_REG_A4_DEC	},
	{ "-(a5)",	T_68K_REG_A5_DEC	},
	{ "-(a6)",	T_68K_REG_A6_DEC	},
	{ "-(a7)",	T_68K_REG_A7_DEC	},
	{ "-(sp)",	T_68K_REG_A7_DEC	},

	{ "(a0)+",	T_68K_REG_A0_INC	},
	{ "(a1)+",	T_68K_REG_A1_INC	},
	{ "(a2)+",	T_68K_REG_A2_INC	},
	{ "(a3)+",	T_68K_REG_A3_INC	},
	{ "(a4)+",	T_68K_REG_A4_INC	},
	{ "(a5)+",	T_68K_REG_A5_INC	},
	{ "(a6)+",	T_68K_REG_A6_INC	},
	{ "(a7)+",	T_68K_REG_A7_INC	},
	{ "(sp)+",	T_68K_REG_A7_INC	},

	{ "mc68000",	T_68K_MC68000	},
	{ "mc68010",	T_68K_MC68010	},
	{ "mc68020",	T_68K_MC68020	},
	{ "mc68030",	T_68K_MC68030	},
	{ "mc68040",	T_68K_MC68040	},
	{ "mc68060",	T_68K_MC68060	},

	{ "fpu68881",	T_68K_FPU6888X	},
	{ "fpu68882",	T_68K_FPU6888X	},
	{ "fpu6888x",	T_68K_FPU6888X	},
	{ "fpu68040",	T_68K_FPU68040	},
	{ "fpu68060",	T_68K_FPU68060	},

	{ "regmask",	T_68K_REGMASK	},
	
	{ "fabs",	T_FPU_FABS		},
	{ "fsabs",	T_FPU_FSABS		},
	{ "fdabs",	T_FPU_FDABS		},
	{ "facos",	T_FPU_FACOS		},
	{ "fadd",	T_FPU_FADD		},
	{ "fasin",	T_FPU_FASIN		},
	{ "fatan",	T_FPU_FATAN		},
	{ "fatanh",	T_FPU_FATANH	},

	{ "fbf",	T_FPU_FBF	},
	{ "fbeq",	T_FPU_FBEQ	},
	{ "fbogt",	T_FPU_FBOGT	},
	{ "fboge",	T_FPU_FBOGE	},
	{ "fbolt",	T_FPU_FBOLT	},
	{ "fbole",	T_FPU_FBOLE	},
	{ "fbogl",	T_FPU_FBOGL	},
	{ "fbor",	T_FPU_FBOR	},

	{ "fbun",	T_FPU_FBUN	},
	{ "fbueq",	T_FPU_FBUEQ	},
	{ "fbugt",	T_FPU_FBUGT	},
	{ "fbuge",	T_FPU_FBUGE	},
	{ "fbult",	T_FPU_FBULT	},
	{ "fbule",	T_FPU_FBULE	},
	{ "fbne",	T_FPU_FBNE	},
	{ "fbt",	T_FPU_FBT	},
	
	{ "fbsf",	T_FPU_FBSF	},
	{ "fbseq",	T_FPU_FBSEQ	},
	{ "fbgt",	T_FPU_FBGT	},
	{ "fbge",	T_FPU_FBGE	},
	{ "fblt",	T_FPU_FBLT	},
	{ "fble",	T_FPU_FBLE	},
	{ "fbgl",	T_FPU_FBGL	},
	{ "fbgle",	T_FPU_FBGLE	},

	{ "fbngle",	T_FPU_FBNGLE	},
	{ "fbngl",	T_FPU_FBNGL		},
	{ "fbnle",	T_FPU_FBNLE		},
	{ "fbnlt",	T_FPU_FBNLT		},
	{ "fbnge",	T_FPU_FBNGE		},
	{ "fbngt",	T_FPU_FBNGT		},
	{ "fbsne",	T_FPU_FBSNE		},
	{ "fbst",	T_FPU_FBST		},

	{ "fcmp",	T_FPU_FCMP	},
	{ "fcos",	T_FPU_FCOS	},
	{ "fcosh",	T_FPU_FCOSH	},
	
	{ "fdbf",	T_FPU_FDBF		},
	{ "fdbeq",	T_FPU_FDBEQ		},
	{ "fdbogt",	T_FPU_FDBOGT	},
	{ "fdboge",	T_FPU_FDBOGE	},
	{ "fdbolt",	T_FPU_FDBOLT	},
	{ "fdbole",	T_FPU_FDBOLE	},
	{ "fdbogl",	T_FPU_FDBOGL	},
	{ "fdbor",	T_FPU_FDBOR		},

	{ "fdbun",	T_FPU_FDBUN		},
	{ "fdbueq",	T_FPU_FDBUEQ	},
	{ "fdbugt",	T_FPU_FDBUGT	},
	{ "fdbuge",	T_FPU_FDBUGE	},
	{ "fdbult",	T_FPU_FDBULT	},
	{ "fdbule",	T_FPU_FDBULE	},
	{ "fdbne",	T_FPU_FDBNE		},
	{ "fdbt",	T_FPU_FDBT		},
	
	{ "fdbsf",	T_FPU_FDBSF		},
	{ "fdbseq",	T_FPU_FDBSEQ	},
	{ "fdbgt",	T_FPU_FDBGT		},
	{ "fdbge",	T_FPU_FDBGE		},
	{ "fdblt",	T_FPU_FDBLT		},
	{ "fdble",	T_FPU_FDBLE		},
	{ "fdbgl",	T_FPU_FDBGL		},
	{ "fdbgle",	T_FPU_FDBGLE	},

	{ "fdbngle",	T_FPU_FDBNGLE	},
	{ "fdbngl",		T_FPU_FDBNGL	},
	{ "fdbnle",		T_FPU_FDBNLE	},
	{ "fdbnlt",		T_FPU_FDBNLT	},
	{ "fdbnge",		T_FPU_FDBNGE	},
	{ "fdbngt",		T_FPU_FDBNGT	},
	{ "fdbsne",		T_FPU_FDBSNE	},
	{ "fdbst",		T_FPU_FDBST		},
	
	{ "fdiv",		T_FPU_FDIV		},
	{ "fetox",		T_FPU_FETOX		},
	{ "fetoxm1",	T_FPU_FETOXM1	},
	{ "fgetexp",	T_FPU_FGETEXP	},
	{ "fgetman",	T_FPU_FGETMAN	},
	{ "fint",		T_FPU_FINT		},
	{ "fintrz",		T_FPU_FINTRZ	},
	{ "flog10",		T_FPU_FLOG10	},
	{ "flog2",		T_FPU_FLOG2		},
	{ "flogn",		T_FPU_FLOGN		},
	{ "flognp1",	T_FPU_FLOGNP1	},
	{ "fmod",		T_FPU_FMOD		},
	{ "fmove",		T_FPU_FMOVE		},
	{ "fmovecr",	T_FPU_FMOVECR	},
	{ "fmovem",		T_FPU_FMOVEM	},
	{ "fmul",		T_FPU_FMUL		},
	{ "fneg",		T_FPU_FNEG		},
	{ "fnop",		T_FPU_FNOP		},
	{ "frem",		T_FPU_FREM		},
	{ "frestore",	T_FPU_FRESTORE	},
	{ "fsave",		T_FPU_FSAVE		},
	{ "fscale",		T_FPU_FSCALE	},

	{ "fsf",	T_FPU_FSF	},
	{ "fseq",	T_FPU_FSEQ	},
	{ "fsogt",	T_FPU_FSOGT	},
	{ "fsoge",	T_FPU_FSOGE	},
	{ "fsolt",	T_FPU_FSOLT	},
	{ "fsole",	T_FPU_FSOLE	},
	{ "fsogl",	T_FPU_FSOGL	},
	{ "fsor",	T_FPU_FSOR	},

	{ "fsun",	T_FPU_FSUN	},
	{ "fsueq",	T_FPU_FSUEQ	},
	{ "fsugt",	T_FPU_FSUGT	},
	{ "fsuge",	T_FPU_FSUGE	},
	{ "fsult",	T_FPU_FSULT	},
	{ "fsule",	T_FPU_FSULE	},
	{ "fsne",	T_FPU_FSNE	},
	{ "fst",	T_FPU_FST	},
	
	{ "fssf",	T_FPU_FSSF	},
	{ "fsseq",	T_FPU_FSSEQ	},
	{ "fsgt",	T_FPU_FSGT	},
	{ "fsge",	T_FPU_FSGE	},
	{ "fslt",	T_FPU_FSLT	},
	{ "fsle",	T_FPU_FSLE	},
	{ "fsgl",	T_FPU_FSGL	},
	{ "fsgle",	T_FPU_FSGLE	},

	{ "fsngle",	T_FPU_FSNGLE	},
	{ "fsngl",	T_FPU_FSNGL		},
	{ "fsnle",	T_FPU_FSNLE		},
	{ "fsnlt",	T_FPU_FSNLT		},
	{ "fsnge",	T_FPU_FSNGE		},
	{ "fsngt",	T_FPU_FSNGT		},
	{ "fssne",	T_FPU_FSSNE		},
	{ "fsst",	T_FPU_FSST		},

	{ "fsgldiv",	T_FPU_FSGLDIV	},
	{ "fsglmul",	T_FPU_FSGLMUL	},
	{ "fsin",		T_FPU_FSIN		},
	{ "fsincos",	T_FPU_FSINCOS	},
	{ "fsinh",		T_FPU_FSINH		},
	{ "fsqrt",		T_FPU_FSQRT		},
	{ "fsub",		T_FPU_FSUB		},
	{ "ftan",		T_FPU_FTAN		},
	{ "ftanh",		T_FPU_FTANH		},
	{ "ftentox",	T_FPU_FTENTOX	},
	
	{ "ftrapf",		T_FPU_FTRAPF	},
	{ "ftrapeq",	T_FPU_FTRAPEQ	},
	{ "ftrapogt",	T_FPU_FTRAPOGT	},
	{ "ftrapoge",	T_FPU_FTRAPOGE	},
	{ "ftrapolt",	T_FPU_FTRAPOLT	},
	{ "ftrapole",	T_FPU_FTRAPOLE	},
	{ "ftrapogl",	T_FPU_FTRAPOGL	},
	{ "ftrapor",	T_FPU_FTRAPOR	},

	{ "ftrapun",	T_FPU_FTRAPUN	},
	{ "ftrapueq",	T_FPU_FTRAPUEQ	},
	{ "ftrapugt",	T_FPU_FTRAPUGT	},
	{ "ftrapuge",	T_FPU_FTRAPUGE	},
	{ "ftrapult",	T_FPU_FTRAPULT	},
	{ "ftrapule",	T_FPU_FTRAPULE	},
	{ "ftrapne",	T_FPU_FTRAPNE	},
	{ "ftrapt",		T_FPU_FTRAPT	},
	
	{ "ftrapsf",	T_FPU_FTRAPSF	},
	{ "ftrapseq",	T_FPU_FTRAPSEQ	},
	{ "ftrapgt",	T_FPU_FTRAPGT	},
	{ "ftrapge",	T_FPU_FTRAPGE	},
	{ "ftraplt",	T_FPU_FTRAPLT	},
	{ "ftraple",	T_FPU_FTRAPLE	},
	{ "ftrapgl",	T_FPU_FTRAPGL	},
	{ "ftrapgle",	T_FPU_FTRAPGLE	},

	{ "ftrapngle",	T_FPU_FTRAPNGLE	},
	{ "ftrapngl",	T_FPU_FTRAPNGL	},
	{ "ftrapnle",	T_FPU_FTRAPNLE	},
	{ "ftrapnlt",	T_FPU_FTRAPNLT	},
	{ "ftrapnge",	T_FPU_FTRAPNGE	},
	{ "ftrapngt",	T_FPU_FTRAPNGT	},
	{ "ftrapsne",	T_FPU_FTRAPSNE	},
	{ "ftrapst",	T_FPU_FTRAPST	},

	{ "ftst",		T_FPU_FTST		},
	{ "ftwotox",	T_FPU_FTWOTOX	},
	
	{ "fp0",	T_FPUREG_0 },
	{ "fp1",	T_FPUREG_1 },
	{ "fp2",	T_FPUREG_2 },
	{ "fp3",	T_FPUREG_3 },
	{ "fp4",	T_FPUREG_4 },
	{ "fp5",	T_FPUREG_5 },
	{ "fp6",	T_FPUREG_6 },
	{ "fp7",	T_FPUREG_7 },

	{ NULL, 0 }
};

void	loclexer_Init(void)
{
	lex_AddStrings(localstrings);
}
