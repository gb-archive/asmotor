/*  Copyright 2008 Carsten S�rensen

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

#ifndef	INCLUDE_SYMBOL_H
#define	INCLUDE_SYMBOL_H

#include <stdlib.h>

#include "xasm.h"
#include "lists.h"
#include "str.h"

#define	HASHSIZE	1024

typedef	enum
{
	SYM_LABEL = 0,
	SYM_EQU,
	SYM_SET,
	SYM_EQUS,
	SYM_MACRO,
	SYM_IMPORT,
	SYM_GROUP,
	SYM_GLOBAL,
	SYM_UNDEFINED
}	ESymbolType;

typedef	enum
{
	GROUP_TEXT = 0,
	GROUP_BSS = 1
}	EGroupType;

#define	SYMF_CONSTANT		0x001		/*	symbol has a constant value (the Value field is safe to use)	*/
#define	SYMF_RELOC			0x002		/*	symbol will change its value during linking						*/
#define	SYMF_REFERENCED		0x004		/*	symbol has been referenced										*/
#define	SYMF_EXPORT			0x008		/*	symbol should be exported										*/
#define	SYMF_EXPORTABLE		0x010		/*	symbol can be exported											*/
#define	SYMF_LOCAL			0x020		/*	symbol is a local symbol (the pScope field will be non-NULL)	*/
#define	SYMF_EXPR			0x040		/*	symbol can be used in expressions								*/
#define	SYMF_MODIFY			0x080		/*	symbol can be redefined											*/
#define	SYMF_HASDATA		0x100		/*	symbol has data attached (Macro.pData)							*/
#define	SYMF_LOCALEXPORT	0x200		/*	symbol should be exported to sections local to this file        */
#define SYMF_DATA 0x40000000
#define SYMF_CHIP 0x20000000

struct Symbol
{
	list_Data(struct Symbol);
	string*		pName;
	ESymbolType	Type;
	uint32_t	Flags;
	struct Symbol*	pScope;
	struct Section*	pSection;
	union
	{
		int32_t (*Integer)(struct Symbol*);
		char*	(*String)(struct Symbol*);
	} Callback;
	union
	{
		int32_t		Value;
		EGroupType	GroupType;
		struct
		{
			size_t	Size;
			char*	pData;
		} Macro;
	} Value;

	uint32_t ID;	/*	Used by object output routines */
};

typedef	struct	Symbol	SSymbol;


extern bool_t	sym_Init(void);

extern SSymbol* sym_CreateLabel(string* pName);
extern SSymbol* sym_CreateEQUS(string* pName, char* value);
extern SSymbol* sym_CreateEQU(string* pName, int32_t value);
extern SSymbol* sym_CreateSET(string* pName, int32_t value);
extern SSymbol* sym_CreateGROUP(string* pName, EGroupType value);
extern SSymbol* sym_CreateMACRO(string* pName, char* value, uint32_t size);

extern char*	sym_ConvertSymbolValueToString(char* dst, char* sym);
extern SSymbol*	sym_Export(char* name);

extern int32_t	sym_GetValueField(SSymbol* sym);
extern char*	sym_GetStringValue(char* name);
extern int32_t	sym_GetConstant(char* name);

extern bool_t	sym_isDefined(char* name);
extern bool_t	sym_isString(char* name);
extern bool_t	sym_isMacro(char* name);

extern SSymbol* sym_FindSymbol(char* name);
extern SSymbol* sym_Import(char* name);
extern SSymbol* sym_Global(char* name);
extern bool_t	sym_Purge(char* name);

extern	SSymbol* g_pHashedSymbols[HASHSIZE];

#endif	/*INCLUDE_SYMBOL_H*/