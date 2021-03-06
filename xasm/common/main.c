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

#if defined(_DEBUG)
#include <crtdbg.h>
#endif

#include "asmotor.h"
#include "amitime.h"
#include "amigaobj.h"
#include "section.h"
#include "symbol.h"
#include "globlex.h"
#include "options.h"
#include "project.h"
#include "fstack.h"
#include "parse.h"
#include "patch.h"
#include "object.h"
#include "binobj.h"
#include "xasm.h"




/*	Some global variables*/

uint32_t g_nTotalLines = 0;
uint32_t g_nTotalErrors = 0;
uint32_t g_nTotalWarnings = 0;




/*	Help text*/

void PrintUsage(void)
{
    printf(
		"%s v%s, ASMotor v" ASMOTOR_VERSION "\n\nUsage: %s [options] asmfile\n"
   		"Options:\n"
   		"    -b<AS>  Change the two characters used for binary constants\n"
		"            (default is 01)\n"
   		"    -e(l|b) Change endianness (CAUTION!)\n"
		"    -f<f>   Output format, one of\n"
		"                x - xobj (default)\n"
		"                b - binary file\n"
		"                v - verilog readmemh file\n",
		g_pConfiguration->pszExecutable, g_pConfiguration->pszBackendVersion, g_pConfiguration->pszExecutable);

	if(g_pConfiguration->bSupportAmiga)
	{
		printf(
		"                g - Amiga executable file\n"
		"                h - Amiga object file\n");
	}

	printf(
		"    -h      This text\n"
   		"    -i<dir> Extra include path (can appear more than once)\n"
   		"    -o<f>   Write assembly output to <file>\n"
		"    -v      Verbose text output\n"
		"    -w<d>   Disable warning <d> (four digits)\n"
   		"    -z<XX>  Set the byte value (hex format) used for uninitialised\n"
		"            data (default is ? for random)\n"
		"\n"
		"Machine specific options:\n");
	locopt_PrintOptions();
    exit(EXIT_SUCCESS);
}




/*	This thing runs the show*/

extern int xasm_Main(int argc, char* argv[])
{
	char format = 'x';
	int	argn = 1;
	int	rcode;
	clock_t	StartClock;
	clock_t EndClock;
	string* pOutname = NULL;
	bool_t debuginfo = false;
	bool_t verbose = false;

#if defined(_DEBUG)
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)| _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	atexit(getchar);
#endif

    StartClock = clock();

	argc -= 1;
	if(argc == 0)
		PrintUsage();

	sect_Init();
	sym_Init();
	globlex_Init();
	loclexer_Init();

	opt_Open();

	while(argc && argv[argn][0] == '-')
	{
		switch(argv[argn][1])
		{
			case '?':
			case 'h':
				PrintUsage();
				break;
			case 'g':
				debuginfo = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'f':
				if(strlen(argv[argn]) > 2)
				{
					switch(argv[argn][2])
					{
						case 'x':
						case 'b':
						case 'v':
							format = argv[argn][2];
							break;
						case 'g':
						case 'h':
							if(g_pConfiguration->bSupportAmiga)
							{
								format = argv[argn][2];
								break;
							}
						default:
							prj_Warn(WARN_OPTION, argv[argn]);
							break;
					}
				}
				break;
			case 'o':
				pOutname = str_Create(&argv[argn][2]);
				break;
			case 'i':
			case 'e':
			case 'm':
			case 'b':
			case 'w':
			case 'z':
				opt_Parse(&argv[argn][1]);
				break;
			default:
				prj_Warn(WARN_OPTION, argv[argn]);
				break;
		}
		++argn;
		--argc;
	}

	rcode = EXIT_SUCCESS;

	if(argc == 1)
	{
		string* source = str_Create(argv[argn]);
		if(fstk_Init(source))
		{
			bool_t b = parse_Do();

			if(b)
			{
				patch_OptimizeAll();
				patch_BackPatch();
			}

			if(b && g_nTotalErrors == 0)
			{
				float timespent;
				bool_t wr = false;

				if(verbose)
				{
					EndClock = clock();

					timespent = ((float)(EndClock - StartClock))/CLOCKS_PER_SEC;
					printf("Success! %u lines in %.02f seconds ", g_nTotalLines, timespent);
					if(timespent == 0)
					{
						printf("\n");
					}
					else
					{
						printf("(%d lines/minute)\n", (int)(60/timespent*g_nTotalLines));
					}
					if(g_nTotalWarnings != 0)
					{
						printf("Encountered %u warnings\n", g_nTotalWarnings);
					}
				}

				if(pOutname != NULL)
				{
					switch(format)
					{
						case 'x':
							wr = obj_Write(pOutname);
							break;
						case 'b':
							wr = bin_Write(pOutname);
							break;
						case 'v':
							wr = bin_WriteVerilog(pOutname);
							break;
						case 'g':
							wr = ami_WriteExecutable(pOutname, debuginfo);
							break;
						case 'h':
							wr = ami_WriteObject(pOutname, source, debuginfo);
							break;
					}
					if(!wr)
					{
						remove(str_String(pOutname));
					}
				}
			}
			else
			{
				if(verbose)
				{
					printf("Encountered %u error%s", g_nTotalErrors, g_nTotalErrors > 1 ? "s" : "");
					if(g_nTotalWarnings != 0)
						printf(" and %u warning%s\n", g_nTotalWarnings, g_nTotalWarnings > 1 ? "s" : "");
					else
						printf("\n");
				}
				rcode = EXIT_FAILURE;
			}
			fstk_Cleanup();
		}
		str_Free(source);
	}

	str_Free(pOutname);
	opt_Close();

	return rcode;
}
