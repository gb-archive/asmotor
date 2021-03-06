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

#include <string.h>

#include "xlink.h"

#define SYS_ASCII_ADDRESS 5

static char basicSys[] =
{
    0x0B, 0x08, 0x0A, 0x00, 0x9E, 0x37, 0x31, 0x38, 0x31, 0x00, 0x00, 0x00, 0x00
};


static int startAddressOfFirstCodeSection(void)
{
    Section* section;
    
    for (section = g_sections; section != NULL; section = section->nextSection)
    {
        if (section->used && section->group && (section->group->type == GROUP_TEXT))
        {
            return section->cpuLocation;
        }
    }

    Error("No start address found because no CODE sections found");
}


static void writeHeader(FILE* fileHandle, uint32_t baseAddress)
{
    int startAddress = startAddressOfFirstCodeSection();

    fputc(baseAddress & 0xFF, fileHandle);
    fputc((baseAddress >> 8) & 0xFF, fileHandle);

    sprintf(&basicSys[SYS_ASCII_ADDRESS], "%d", startAddress);

    fwrite(basicSys, 1, sizeof(basicSys), fileHandle);
}


extern void commodore_WritePrg(char* outputFilename, uint32_t baseAddress)
{
	FILE* fileHandle = fopen(outputFilename, "wb");
	if (fileHandle == NULL)
		Error("Unable to open \"%s\" for writing", outputFilename);

    writeHeader(fileHandle, baseAddress);

	image_WriteBinaryToFile(fileHandle, -1);

	fclose(fileHandle);
}
