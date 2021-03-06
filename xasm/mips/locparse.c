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
#include "expr.h"
#include "parse.h"
#include "project.h"
#include "localasm.h"
#include "lexer.h"
#include "section.h"


SExpression* parse_ExpressionS16(void)
{
	SExpression* pExpr = parse_Expression(2);
	if(pExpr == NULL)
		return NULL;
		
	pExpr = expr_CheckRange(pExpr, -32768, 32767);
	if(pExpr == NULL)
		prj_Error(ERROR_OPERAND_RANGE);
	
	return expr_And(pExpr, expr_Const(0xFFFF));
}


SExpression* parse_ExpressionU16(void)
{
	SExpression* pExpr = parse_Expression(2);
	if(pExpr == NULL)
		return NULL;
		
	pExpr = expr_CheckRange(pExpr, 0, 65535);
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

bool_t parse_TargetSpecific(void)
{
	if(parse_IntegerInstruction())
		return true;

	return false;
}
