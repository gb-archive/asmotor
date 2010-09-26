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

#include "../common/xasm.h"


SExpression* parse_ExpressionS16(void)
{
	SExpression* pExpr = parse_Expression();
	if(pExpr == NULL)
		return NULL;
		
	pExpr = parse_CheckRange(pExpr, -32768, 32767);
	if(pExpr == NULL)
		prj_Error(ERROR_OPERAND_RANGE);
	
	return parse_CreateANDExpr(pExpr, parse_CreateConstExpr(0xFFFF));
}


SExpression* parse_ExpressionU16(void)
{
	SExpression* pExpr = parse_Expression();
	if(pExpr == NULL)
		return NULL;
		
	pExpr = parse_CheckRange(pExpr, 0, 65535);
	if(pExpr == NULL)
		prj_Error(ERROR_OPERAND_RANGE);
	return pExpr;
}



#include "intinstr.h"

SExpression* parse_TargetFunction(void)
{
	switch(g_CurrentToken.ID.TargetToken)
	{
		default:
			return NULL;
	}
}

BOOL parse_TargetSpecific(void)
{
	if(parse_IntegerInstruction())
		return TRUE;

	return FALSE;
}