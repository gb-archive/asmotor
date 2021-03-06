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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xasm.h"
#include "mem.h"
#include "project.h"
#include "options.h"
#include "locopt.h"

#include "loccpu.h"

void locopt_Copy(SMachineOptions* pDest, SMachineOptions* pSrc)
{
	*pDest = *pSrc;
}

SMachineOptions* locopt_Alloc(void)
{
	return mem_Alloc(sizeof(SMachineOptions));
}

void locopt_Open(void)
{
	g_pOptions->pMachine->nCpu = CPUF_68000;
	g_pOptions->pMachine->nFpu = 0;
}

void locopt_Update(void)
{
}

bool_t locopt_Parse(char* s)
{
	if(s == NULL || strlen(s) == 0)
		return false;

	switch(s[0])
	{
		case 'c':
			if(strlen(&s[1]) == 1)
			{
				switch(s[1] - '0')
				{
					case 0:
						g_pOptions->pMachine->nCpu = CPUF_68000;
						return true;
					case 1:
						g_pOptions->pMachine->nCpu = CPUF_68010;
						return true;
					case 2:
						g_pOptions->pMachine->nCpu = CPUF_68020;
						return true;
					case 3:
						g_pOptions->pMachine->nCpu = CPUF_68030;
						return true;
					case 4:
						g_pOptions->pMachine->nCpu = CPUF_68040;
						return true;
					case 6:
						g_pOptions->pMachine->nCpu = CPUF_68060;
						return true;
					default:
						prj_Warn(WARN_MACHINE_UNKNOWN_OPTION, s);
						return false;
				}
			}
			else
			{
				prj_Warn(WARN_MACHINE_UNKNOWN_OPTION, s);
				return false;
			}
			break;
		case 'f':
			if(strlen(&s[1]) == 1)
			{
				switch(s[1])
				{
					case '1':
					case '2':
					case 'x':
						g_pOptions->pMachine->nCpu = FPUF_6888X;
						return true;
					case '4':
						g_pOptions->pMachine->nCpu = FPUF_68040;
						return true;
					case '6':
						g_pOptions->pMachine->nCpu = FPUF_68060;
						return true;
					default:
						prj_Warn(WARN_MACHINE_UNKNOWN_OPTION, s);
						return false;
				}
			}
			else
			{
				prj_Warn(WARN_MACHINE_UNKNOWN_OPTION, s);
				return false;
			}
			break;
		default:
			prj_Warn(WARN_MACHINE_UNKNOWN_OPTION, s);
			return false;
	}
}

void locopt_PrintOptions(void)
{
	printf("    -mc<X>  Enable CPU 680X0\n");
}
