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

#include "xasm.h"




//	Internal structures

#define	MAXINCLUDEPATHS	16




//	Internal variables

SFileStack*	g_pFileContext;

static char* g_aIncludePaths[MAXINCLUDEPATHS];
static ULONG g_nTotalIncludePaths = 0;
static SFileStack* g_pMacroContext;
static char* g_pszCurrentRunId;

static char** g_ppszNewMacroArgs;
static char* g_pszNewMacroArg0;
static ULONG g_nTotalNewMacroArgs;




//	Private routines

static char* fstk_CreateNewRunID(void)
{
	char s[MAXSYMNAMELENGTH + 1];
	char* r;
	static ULONG runid = 0;

	sprintf(s, "_%lu", runid++);
	if((r=strdup(s)) != NULL)
		return r;

	internalerror("Out of memory");
	return NULL;
}

static void fstk_SetNewContext(SFileStack* newcontext)
{
	list_Insert(g_pFileContext, newcontext);
	g_pFileContext = newcontext;
	g_pszCurrentRunId = g_pFileContext->RunID;
}

//	Public routines

char* fstk_GetMacroArgValue(char ch)
{
	ULONG ct;

	if(ch == '0')
		return g_pMacroContext->BlockInfo.Macro.Arg0;

	ct = ch - '1';
	if(g_pMacroContext == NULL
	|| g_pMacroContext->Type != CONTEXT_MACRO
	|| ct >= g_pMacroContext->BlockInfo.Macro.ArgCount)
	{
		return NULL;
	}

	return g_pMacroContext->BlockInfo.Macro.Args[ct];
}

char* fstk_GetMacroRunID(void)
{
	return g_pszCurrentRunId;
}

SLONG fstk_GetMacroArgCount(void)
{
	if(g_pMacroContext == NULL
	|| g_pMacroContext->Type != CONTEXT_MACRO)
	{
		return 0;
	}

	return g_pMacroContext->BlockInfo.Macro.ArgCount;
}

void fstk_AddMacroArg(char* s)
{
	if((g_ppszNewMacroArgs = (char**)realloc(g_ppszNewMacroArgs, sizeof(char*) * (g_nTotalNewMacroArgs+1))) != NULL)
	{
		if((g_ppszNewMacroArgs[g_nTotalNewMacroArgs++] = strdup(s)) != NULL)
		{
			return;
		}
	}

	internalerror("Out of memory");
}

void fstk_SetMacroArg0(char* s)
{
	if((g_pszNewMacroArg0 = strdup(s)) != NULL)
		return;

	internalerror("Out of memory");
}

void fstk_ShiftMacroArgs(SLONG count)
{
	if(g_pMacroContext)
	{
		while(count >= 1)
		{
			ULONG i;

			free(g_pMacroContext->BlockInfo.Macro.Args[0]);
			i = 1;
			while(i < g_pMacroContext->BlockInfo.Macro.ArgCount)
			{
				g_pMacroContext->BlockInfo.Macro.Args[i - 1] = g_pMacroContext->BlockInfo.Macro.Args[i];
				i += 1;
			}
			g_pMacroContext->BlockInfo.Macro.ArgCount -= 1;
			g_pMacroContext->BlockInfo.Macro.Args = realloc(g_pMacroContext->BlockInfo.Macro.Args, g_pMacroContext->BlockInfo.Macro.ArgCount * sizeof(char*));
			count -= 1;
		}
	}
	else
	{
		prj_Warn(WARN_SHIFT_MACRO);
	}
}

char* fstk_BuildPath(char* path, char* file)
{
	char* s = path + strlen(path) - 1;
	char* r;

	while(s > path && *s != '/' && *s != '\\')
		--s;

	if(s == path)
		return NULL;

	r = malloc(s + 1 - path + strlen(file) + 1);
	memcpy(r, path, s + 1 - path);
	strcpy(r + (s + 1 - path), file);

	return r;
}

BOOL fstk_FileExists(char* pszFile)
{
	FILE* f;
	if((f = fopen(pszFile, "rb")) != NULL)
	{
		fclose(f);
		return TRUE;
	}

	return FALSE;
}

void fstk_FindFile(char** s)
{
	char* pszFile;
	ULONG count;

	if(fstk_FileExists(*s))
	{
		*s = strdup(*s);
		return;
	}

	pszFile = fstk_BuildPath(g_pFileContext->pName, *s);
	if(pszFile != NULL)
	{
		if(fstk_FileExists(pszFile))
		{
			*s = pszFile;
			return;
		}
		free(pszFile);
	}

	for(count = 0; count < g_nTotalIncludePaths; count += 1)
	{
		char* fname;

		if((fname = malloc(strlen(g_aIncludePaths[count]) + strlen(*s) + 1)) != NULL)
		{
			FILE* f;
			strcpy(fname, g_aIncludePaths[count]);
			strcat(fname, *s);
			if((f = fopen(fname,"rt")) != NULL)
			{
				fclose(f);
				free(*s);
				*s=fname;
				return;
			}
		}
		else
		{
			internalerror("Out of memory!");
		}
		free(fname);
	}
	free(*s);
	*s=NULL;
}

void fstk_Dump(void)
{
	SFileStack* stack;

	if(g_pFileContext == NULL)
	{
		printf("(From commandline) ");
		return;
	}

	stack = g_pFileContext;
	while(list_GetNext(stack))
	{
		stack = list_GetNext(stack);
	}

	while(stack)
	{
		printf("%s(%ld)", stack->pName, stack->LineNumber);
		stack = list_GetPrev(stack);
		if(stack)
		{
			printf("->");
		}
		else
		{
			printf(": ");
		}
	}
}

BOOL fstk_RunNextBuffer(void)
{
	if(list_isLast(g_pFileContext))
	{
		return FALSE;
	}
	else
	{
		SFileStack* newcontext;
		SFileStack* oldcontext;

		oldcontext = g_pFileContext;
		newcontext = list_GetNext(g_pFileContext);

		if(newcontext->Type == CONTEXT_MACRO)
			g_pMacroContext = newcontext;
		else
			g_pMacroContext = NULL;

		if(newcontext->Type == CONTEXT_MACRO
		|| newcontext->Type == CONTEXT_REPT)
		{
			g_pszCurrentRunId = newcontext->RunID;
		}

		list_Remove(g_pFileContext, g_pFileContext);
		lex_FreeBuffer(oldcontext->pLexBuffer);
		free(oldcontext->pName);

		switch(oldcontext->Type)
		{
			case CONTEXT_REPT:
			{
				if(oldcontext->BlockInfo.Rept.RemainingRuns > 0)
				{
					fstk_RunRept(oldcontext->BlockInfo.Rept.pOriginalBuffer, oldcontext->BlockInfo.Rept.OriginalSize, oldcontext->BlockInfo.Rept.RemainingRuns);
				}
				else
				{
					free(oldcontext->BlockInfo.Rept.pOriginalBuffer);
					lex_SetBuffer(g_pFileContext->pLexBuffer);
				}
				free(oldcontext->RunID);
				free(oldcontext);
				break;
			}
			case CONTEXT_MACRO:
			{
				ULONG i;

				for(i = 0; i < oldcontext->BlockInfo.Macro.ArgCount; i += 1)
				{
					free(oldcontext->BlockInfo.Macro.Args[i]);
				}
				free(oldcontext->RunID);
				free(oldcontext->BlockInfo.Macro.Args);
				free(oldcontext);
				lex_SetBuffer(g_pFileContext->pLexBuffer);
				break;
			}
			default:
			{
				free(oldcontext);
				lex_SetBuffer(g_pFileContext->pLexBuffer);
				break;
			}
		}
		return TRUE;
	}
}

void fstk_AddIncludePath(char* s)
{
	if(g_nTotalIncludePaths == MAXINCLUDEPATHS)
	{
		prj_Fail(ERROR_INCLUDE_LIMIT);
	}
	else
	{
		int len = (int)strlen(s);
		BOOL addslash = FALSE;
		if(s[len - 1] != '\\' && s[len - 1] != '/')
		{
			addslash = TRUE;
			++len;
		}

		g_aIncludePaths[g_nTotalIncludePaths] = malloc(len + 1);
		memcpy(g_aIncludePaths[g_nTotalIncludePaths], s, len);
		if(addslash)
			g_aIncludePaths[g_nTotalIncludePaths][len - 1] = '/';
		g_aIncludePaths[g_nTotalIncludePaths][len] = 0;
		++g_nTotalIncludePaths;
	}
}

void	fstk_RunInclude(char* s)
{
	SFileStack* newcontext;

	if((newcontext = malloc(sizeof(SFileStack))) != NULL)
	{
		memset(newcontext, 0, sizeof(SFileStack));
		newcontext->Type=CONTEXT_FILE;

		if((newcontext->pName = strdup(s)) != NULL)
		{
			FILE* f;

		    fstk_FindFile(&newcontext->pName);

		    if(newcontext->pName != NULL
			&&(f=fopen(newcontext->pName, "rt")) != NULL)
		    {
				if((newcontext->pLexBuffer = lex_CreateFileBuffer(f)) != NULL)
				{
					lex_SetBuffer(newcontext->pLexBuffer);
					lex_SetState(LEX_STATE_NORMAL);
					newcontext->LineNumber = 0;
					fstk_SetNewContext(newcontext);
					return;
				}
				fclose(f);
		    }
			else
			{
				prj_Fail(ERROR_NO_FILE);
			}
		}
	}

	internalerror("Out of memory");
}

void fstk_RunRept(char* buffer, ULONG size, ULONG count)
{
	SFileStack* newcontext;

	if((newcontext = malloc(sizeof(SFileStack))) != NULL)
	{
		memset(newcontext, 0, sizeof(SFileStack));
		newcontext->Type = CONTEXT_REPT;

		if((newcontext->pName = strdup("REPT")) != NULL)
		{
			if((newcontext->pLexBuffer = lex_CreateMemoryBuffer(buffer,size)) != NULL)
			{
				lex_SetBuffer(newcontext->pLexBuffer);
				lex_SetState(LEX_STATE_NORMAL);
				newcontext->LineNumber = 0;
				newcontext->BlockInfo.Rept.pOriginalBuffer = buffer;
				newcontext->BlockInfo.Rept.OriginalSize = size;
				newcontext->BlockInfo.Rept.RemainingRuns = count-1;
				newcontext->RunID = fstk_CreateNewRunID();
				fstk_SetNewContext(newcontext);
				return;
			}
		}
	}

	internalerror("Out of memory");
}

void	fstk_RunMacro(char* symname)
{
	SSymbol	*sym;

	if((sym = sym_FindSymbol(symname)) != NULL)
	{
		SFileStack* newcontext;
		if((newcontext = malloc(sizeof(SFileStack))) != NULL)
		{
			memset(newcontext, 0, sizeof(SFileStack));
			newcontext->Type = CONTEXT_MACRO;

			if((newcontext->pName = strdup(symname)) != NULL)
			{
				if((newcontext->pLexBuffer = lex_CreateMemoryBuffer(sym->Value.Macro.pData, sym->Value.Macro.Size)) != NULL)
				{
					lex_SetBuffer(newcontext->pLexBuffer);
					lex_SetState(LEX_STATE_NORMAL);
					newcontext->LineNumber = -1;
					newcontext->BlockInfo.Macro.Arg0 = g_pszNewMacroArg0;
					newcontext->BlockInfo.Macro.Args = g_ppszNewMacroArgs;
					newcontext->BlockInfo.Macro.ArgCount = g_nTotalNewMacroArgs;
					newcontext->RunID = fstk_CreateNewRunID();
					g_nTotalNewMacroArgs = 0;
					g_ppszNewMacroArgs = NULL;
					g_pszNewMacroArg0 = NULL;
					g_pMacroContext = newcontext;
					fstk_SetNewContext(newcontext);
					return;
				}
			}
		}
		internalerror("Out of memory");
	}
	else
	{
		prj_Fail(ERROR_NO_MACRO);
	}
}

BOOL fstk_Init(char* s)
{
	g_nTotalNewMacroArgs = 0;
	g_ppszNewMacroArgs = NULL;
	g_pMacroContext = NULL;
	g_pszCurrentRunId = NULL;

    sym_AddEQUS("__FILE", s);

	if((g_pFileContext = malloc(sizeof(SFileStack))) != NULL)
	{
		memset(g_pFileContext, 0, sizeof(SFileStack));
		g_pFileContext->Type = CONTEXT_FILE;

		if((g_pFileContext->pName = strdup(s)) != NULL)
		{
			FILE* f;

		    fstk_FindFile(&g_pFileContext->pName);

		    if(g_pFileContext->pName != NULL
			&&(f = fopen(g_pFileContext->pName, "rt")) != NULL)
		    {
				g_pFileContext->pLexBuffer = lex_CreateFileBuffer(f);
				fclose(f);
				lex_SetBuffer(g_pFileContext->pLexBuffer);
				lex_SetState(LEX_STATE_NORMAL);
				g_pFileContext->LineNumber = 1;
				return TRUE;
		    }
			else
			{
				prj_Fail(ERROR_NO_FILE);
			}
		}
	}

	internalerror("Out of memory");
	return FALSE;
}

void fstk_Cleanup(void)
{
}