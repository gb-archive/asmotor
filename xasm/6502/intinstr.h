/*  Copyright 2008 Carsten Sørensen

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

#if !defined(INTEGER_INSTRUCTIONS_65XX_)
#define INTEGER_INSTRUCTIONS_65XX_

#define MODE_NONE	0x001
#define MODE_IMM	0x002
#define MODE_ZP		0x004
#define MODE_ZP_X	0x008
#define MODE_ZP_Y	0x010
#define MODE_ABS	0x020
#define MODE_ABS_X	0x040
#define MODE_ABS_Y	0x080
#define MODE_IND_X	0x100
#define MODE_IND_Y	0x200
#define MODE_A		0x400
#define MODE_IND	0x800

typedef struct
{
	int	nMode;
	SExpression* pExpr;
} SAddressingMode;

typedef struct Parser
{
	UBYTE	nBaseOpcode;
	int		nAddressingModes;
	BOOL	(*fpParser)(int nToken, UBYTE nBaseOpcode, SAddressingMode* pAddrMode);
} SParser;

static BOOL parse_Standard_All(int nToken, UBYTE nBaseOpcode, SAddressingMode* pAddrMode)
{
	switch(pAddrMode->nMode)
	{
		case MODE_IND_X:
			sect_OutputAbsByte(nBaseOpcode | (0 << 2));
			sect_OutputExprByte(pAddrMode->pExpr);
			return TRUE;
		case MODE_IMM:
			sect_OutputAbsByte(nBaseOpcode | (2 << 2));
			sect_OutputExprByte(pAddrMode->pExpr);
			return TRUE;
		case MODE_IND_Y:
			sect_OutputAbsByte(nBaseOpcode | (4 << 2));
			sect_OutputExprByte(pAddrMode->pExpr);
			return TRUE;
		case MODE_ABS_Y:
			sect_OutputAbsByte(nBaseOpcode | (6 << 2));
			sect_OutputExprWord(pAddrMode->pExpr);
			return TRUE;
		case MODE_ZP:
			sect_OutputAbsByte(nBaseOpcode | (1 << 2));
			sect_OutputExprByte(pAddrMode->pExpr);
			return TRUE;
		case MODE_ABS:
			sect_OutputAbsByte(nBaseOpcode | (3 << 2));
			sect_OutputExprWord(pAddrMode->pExpr);
			return TRUE;
		case MODE_ZP_X:
			sect_OutputAbsByte(nBaseOpcode | (5 << 2));
			sect_OutputExprByte(pAddrMode->pExpr);
			return TRUE;
		case MODE_ABS_X:
			sect_OutputAbsByte(nBaseOpcode | (7 << 2));
			sect_OutputExprWord(pAddrMode->pExpr);
			return TRUE;
	}

	prj_Fail(MERROR_ILLEGAL_ADDRMODE);
	return TRUE;
}

static BOOL parse_Standard_Imm0(int nToken, UBYTE nBaseOpcode, SAddressingMode* pAddrMode)
{
	switch(pAddrMode->nMode)
	{
		case MODE_IMM:
			sect_OutputAbsByte(nBaseOpcode | (0 << 2));
			sect_OutputExprByte(pAddrMode->pExpr);
			return TRUE;
		case MODE_ZP:
			sect_OutputAbsByte(nBaseOpcode | (1 << 2));
			sect_OutputExprByte(pAddrMode->pExpr);
			return TRUE;
		case MODE_ABS:
			sect_OutputAbsByte(nBaseOpcode | (3 << 2));
			sect_OutputExprWord(pAddrMode->pExpr);
			return TRUE;
		case MODE_ZP_X:
		case MODE_ZP_Y:
			sect_OutputAbsByte(nBaseOpcode | (5 << 2));
			sect_OutputExprByte(pAddrMode->pExpr);
			return TRUE;
		case MODE_ABS_X:
		case MODE_ABS_Y:
			sect_OutputAbsByte(nBaseOpcode | (7 << 2));
			sect_OutputExprWord(pAddrMode->pExpr);
			return TRUE;
	}

	prj_Fail(MERROR_ILLEGAL_ADDRMODE);
	return TRUE;
}

static BOOL parse_Standard_Rotate(int nToken, UBYTE nBaseOpcode, SAddressingMode* pAddrMode)
{
	switch(pAddrMode->nMode)
	{
		case MODE_A:
			sect_OutputAbsByte(nBaseOpcode | (2 << 2));
			return TRUE;
		case MODE_ZP:
			sect_OutputAbsByte(nBaseOpcode | (1 << 2));
			sect_OutputExprByte(pAddrMode->pExpr);
			return TRUE;
		case MODE_ABS:
			sect_OutputAbsByte(nBaseOpcode | (3 << 2));
			sect_OutputExprWord(pAddrMode->pExpr);
			return TRUE;
		case MODE_ZP_X:
			sect_OutputAbsByte(nBaseOpcode | (5 << 2));
			sect_OutputExprByte(pAddrMode->pExpr);
			return TRUE;
		case MODE_ABS_X:
			sect_OutputAbsByte(nBaseOpcode | (7 << 2));
			sect_OutputExprWord(pAddrMode->pExpr);
			return TRUE;
	}

	prj_Fail(MERROR_ILLEGAL_ADDRMODE);
	return TRUE;
}


static BOOL parse_Branch(int nToken, UBYTE nBaseOpcode, SAddressingMode* pAddrMode)
{
	SExpression* pExpr;

	sect_OutputAbsByte(nBaseOpcode);
	pExpr = parse_CreatePCRelExpr(pAddrMode->pExpr, -1);
	pExpr = parse_CheckRange(pExpr, -128, 127);
	if(pExpr == NULL)
	{
		prj_Error(ERROR_OPERAND_RANGE);
		return TRUE;
	}
	else
	{
		sect_OutputExprByte(pExpr);
	}

	return TRUE;
}


static BOOL parse_Implied(int nToken, UBYTE nBaseOpcode, SAddressingMode* pAddrMode)
{
	sect_OutputAbsByte(nBaseOpcode);
	return TRUE;
}


static BOOL parse_JMP(int nToken, UBYTE nBaseOpcode, SAddressingMode* pAddrMode)
{
	if(pAddrMode->nMode == MODE_IND)
		nBaseOpcode += 0x20;

	sect_OutputAbsByte(nBaseOpcode);
	sect_OutputExprWord(pAddrMode->pExpr);
	return TRUE;
}


static BOOL parse_BRK(int nToken, UBYTE nBaseOpcode, SAddressingMode* pAddrMode)
{
	sect_OutputAbsByte(nBaseOpcode);
	if(pAddrMode->nMode == MODE_IMM)
		sect_OutputExprByte(pAddrMode->pExpr);
	return TRUE;
}


static SParser g_Parsers[T_6502_STY - T_6502_ADC + 1] = 
{
	{ 0x61, MODE_IMM | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X | MODE_ABS_Y | MODE_IND_X | MODE_IND_Y, parse_Standard_All },	//	ADC
	{ 0x21, MODE_IMM | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X | MODE_ABS_Y | MODE_IND_X | MODE_IND_Y, parse_Standard_All },	//	AND
	{ 0x02, MODE_A | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X, parse_Standard_Rotate },	//	ASL
	{ 0x20, MODE_ZP | MODE_ABS, parse_Standard_All },	//	BIT
	{ 0x10, MODE_ABS, parse_Branch },	//	BPL
	{ 0x30, MODE_ABS, parse_Branch },	//	BMI
	{ 0x50, MODE_ABS, parse_Branch },	//	BVC
	{ 0x70, MODE_ABS, parse_Branch },	//	BVS
	{ 0x90, MODE_ABS, parse_Branch },	//	BCC
	{ 0xB0, MODE_ABS, parse_Branch },	//	BCS
	{ 0xD0, MODE_ABS, parse_Branch },	//	BNE
	{ 0xF0, MODE_ABS, parse_Branch },	//	BEQ
	{ 0x00, MODE_NONE | MODE_IMM, parse_BRK },	//	BRK
	{ 0xC1, MODE_IMM | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X | MODE_ABS_Y | MODE_IND_X | MODE_IND_Y, parse_Standard_All },	//	CMP
	{ 0xE0, MODE_IMM | MODE_ZP | MODE_ABS, parse_Standard_Imm0 },	//	CPX
	{ 0xC0, MODE_IMM | MODE_ZP | MODE_ABS, parse_Standard_Imm0 },	//	CPY
	{ 0xC2, MODE_ZP | MODE_ABS | MODE_ZP_X | MODE_ABS_X, parse_Standard_All },	//	DEC
	{ 0x41, MODE_IMM | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X | MODE_ABS_Y | MODE_IND_X | MODE_IND_Y, parse_Standard_All },	//	EOR
	{ 0x18, 0, parse_Implied },	//	CLC
	{ 0x38, 0, parse_Implied },	//	SEC
	{ 0x58, 0, parse_Implied },	//	CLI
	{ 0x78, 0, parse_Implied },	//	SEI
	{ 0xB8, 0, parse_Implied },	//	CLV
	{ 0xD8, 0, parse_Implied },	//	CLD
	{ 0xF8, 0, parse_Implied },	//	SED
	{ 0xE2, MODE_ZP | MODE_ABS | MODE_ZP_X | MODE_ABS_X, parse_Standard_All },	//	INC
	{ 0x4C, MODE_ABS | MODE_IND, parse_JMP },	//	JMP
	{ 0x20, MODE_ABS, parse_JMP },	//	JSR
	{ 0xA1, MODE_IMM | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X | MODE_ABS_Y | MODE_IND_X | MODE_IND_Y, parse_Standard_All },	//	LDA
	{ 0xA2, MODE_IMM | MODE_ZP | MODE_ABS | MODE_ZP_Y | MODE_ABS_Y, parse_Standard_Imm0 },	//	LDX
	{ 0xA0, MODE_IMM | MODE_ZP | MODE_ABS | MODE_ZP_X | MODE_ABS_X, parse_Standard_Imm0 },	//	LDY
	{ 0x42, MODE_A | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X, parse_Standard_Rotate },	//	LSR
	{ 0xEA, 0, parse_Implied },	//	NOP
	{ 0x01, MODE_IMM | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X | MODE_ABS_Y | MODE_IND_X | MODE_IND_Y, parse_Standard_All },	//	ORA
	{ 0xAA, 0, parse_Implied },	//	TAX
	{ 0x8A, 0, parse_Implied },	//	TXA
	{ 0xCA, 0, parse_Implied },	//	DEX
	{ 0xE8, 0, parse_Implied },	//	INX
	{ 0xA8, 0, parse_Implied },	//	TAY
	{ 0x98, 0, parse_Implied },	//	TYA
	{ 0x88, 0, parse_Implied },	//	DEY
	{ 0xC8, 0, parse_Implied },	//	INY
	{ 0x22, MODE_A | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X, parse_Standard_Rotate },	//	ROL
	{ 0x62, MODE_A | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X, parse_Standard_Rotate },	//	ROR
	{ 0x40, 0, parse_Implied },	//	RTI
	{ 0x60, 0, parse_Implied },	//	RTS
	{ 0xE1, MODE_IMM | MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X | MODE_ABS_Y | MODE_IND_X | MODE_IND_Y, parse_Standard_All },	//	SBC
	{ 0x81, MODE_ZP | MODE_ZP_X | MODE_ABS | MODE_ABS_X | MODE_ABS_Y | MODE_IND_X | MODE_IND_Y, parse_Standard_All },	//	LDA
	{ 0x9A, 0, parse_Implied },	//	TXS
	{ 0xBA, 0, parse_Implied },	//	TSX
	{ 0x48, 0, parse_Implied },	//	PHA
	{ 0x68, 0, parse_Implied },	//	PLA
	{ 0x08, 0, parse_Implied },	//	PHP
	{ 0x28, 0, parse_Implied },	//	PLP
	{ 0x82, MODE_ZP | MODE_ABS | MODE_ZP_Y, parse_Standard_Imm0 },	//	STX
	{ 0x80, MODE_ZP | MODE_ABS | MODE_ZP_X, parse_Standard_Imm0 },	//	STY
};


BOOL parse_AddressingMode(SAddressingMode* pAddrMode, int nAllowedModes)
{
	SLexBookmark bm;
	lex_Bookmark(&bm);

	if((nAllowedModes & MODE_A) && g_CurrentToken.ID.TargetToken == T_6502_REG_A)
	{
		parse_GetToken();
		pAddrMode->nMode = MODE_A;
		pAddrMode->pExpr = NULL;
		return TRUE;
	}
	else if((nAllowedModes & MODE_IMM) && g_CurrentToken.ID.Token == '#')
	{
		parse_GetToken();
		pAddrMode->nMode = MODE_IMM;
		pAddrMode->pExpr = parse_ExpressionSU8();
		return TRUE;
	}

	if((nAllowedModes & (MODE_IND_X | MODE_IND_Y)) && g_CurrentToken.ID.Token == '(')
	{
		parse_GetToken();
		pAddrMode->pExpr = parse_ExpressionSU8();

		if(pAddrMode->pExpr != NULL)
		{
			if(g_CurrentToken.ID.Token == ',')
			{
				parse_GetToken();
				if(g_CurrentToken.ID.TargetToken == T_6502_REG_X)
				{
					parse_GetToken();
					if(parse_ExpectChar(')'))
					{
						pAddrMode->nMode = MODE_IND_X;
						return TRUE;
					}
				}
			}
			else if(g_CurrentToken.ID.Token == ')')
			{
				parse_GetToken();
				if(g_CurrentToken.ID.Token == ',')
				{
					parse_GetToken();
					if(g_CurrentToken.ID.TargetToken == T_6502_REG_Y)
					{
						parse_GetToken();
						pAddrMode->nMode = MODE_IND_Y;
						return TRUE;
					}
				}
			}
		}

		lex_Goto(&bm);
	}

	if(nAllowedModes & MODE_IND)
	{
		if(g_CurrentToken.ID.Token == '(')
		{
			parse_GetToken();

			pAddrMode->pExpr = parse_Expression();
			if(pAddrMode->pExpr != NULL)
			{
				if(parse_ExpectChar(')'))
				{
					pAddrMode->nMode = MODE_IND;
					return TRUE;
				}
			}
		}
		lex_Goto(&bm);
	}

	if(nAllowedModes & (MODE_ZP | MODE_ZP_X | MODE_ZP_Y | MODE_ABS | MODE_ABS_X | MODE_ABS_Y))
	{
		pAddrMode->pExpr = parse_Expression();

		if(pAddrMode->pExpr != NULL)
		{
			if((pAddrMode->pExpr->Flags & EXPRF_isCONSTANT)
			&& 0 <= pAddrMode->pExpr->Value.Value && pAddrMode->pExpr->Value.Value <= 255)
			{
				if(g_CurrentToken.ID.Token == ',')
				{
					parse_GetToken();
					if(g_CurrentToken.ID.TargetToken == T_6502_REG_X)
					{
						parse_GetToken();
						pAddrMode->nMode = MODE_ZP_X;
						return TRUE;
					}
					else if(g_CurrentToken.ID.TargetToken == T_6502_REG_Y)
					{
						parse_GetToken();
						pAddrMode->nMode = MODE_ZP_Y;
						return TRUE;
					}
				}
				pAddrMode->nMode = MODE_ZP;
				return TRUE;
			}

			if(g_CurrentToken.ID.Token == ',')
			{
				parse_GetToken();

				if(g_CurrentToken.ID.TargetToken == T_6502_REG_X)
				{
					parse_GetToken();
					pAddrMode->nMode = MODE_ABS_X;
					return TRUE;
				}
				else if(g_CurrentToken.ID.TargetToken == T_6502_REG_Y)
				{
					parse_GetToken();
					pAddrMode->nMode = MODE_ABS_Y;
					return TRUE;
				}
			}

			pAddrMode->nMode = MODE_ABS;
			return TRUE;
		}

		lex_Goto(&bm);
	}

	if((nAllowedModes == 0) || (nAllowedModes & MODE_NONE))
	{
		pAddrMode->nMode = MODE_NONE;
		pAddrMode->pExpr = NULL;
		return TRUE;
	}

	if(nAllowedModes & MODE_A)
	{
		pAddrMode->nMode = MODE_A;
		pAddrMode->pExpr = NULL;
		return TRUE;
	}

	return FALSE;
}


BOOL parse_IntegerInstruction(void)
{
	if(T_6502_ADC <= g_CurrentToken.ID.TargetToken && g_CurrentToken.ID.TargetToken <= T_6502_STY)
	{
		SAddressingMode addrMode;
		eTargetToken nToken = g_CurrentToken.ID.TargetToken;
		SParser* pParser = &g_Parsers[nToken - T_6502_ADC];

		parse_GetToken();
		if(parse_AddressingMode(&addrMode, pParser->nAddressingModes))
			return pParser->fpParser(nToken, pParser->nBaseOpcode, &addrMode);
		else
			prj_Error(MERROR_ILLEGAL_ADDRMODE);
	}

	return FALSE;
}


#endif