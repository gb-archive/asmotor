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

#include "xlink.h"


static void fputbl(uint32_t d, FILE* f)
{
	fputc((d >> 24) & 0xFF, f);
	fputc((d >> 16) & 0xFF, f);
	fputc((d >> 8) & 0xFF, f);
	fputc(d & 0xFF, f);
}


static void fputbw(uint16_t d, FILE* f)
{
	fputc((d >> 8) & 0xFF, f);
	fputc(d & 0xFF, f);
}


static void fputlw(uint16_t d, FILE* f)
{
	fputc(d & 0xFF, f);
	fputc((d >> 8) & 0xFF, f);
}


static uint16_t fgetmw(FILE* f)
{
    uint16_t hi = fgetc(f) << 8;
    return hi | fgetc(f);
}


static uint16_t sega_CalcMegaDriveChecksum(FILE* fileHandle, long length)
{
    uint16_t r = 0;
    long c = (length - 0x200) >> 1;

    fseek(fileHandle, 0x200, SEEK_SET);
    while (c--)
        r += fgetmw(fileHandle);

    return r;
}


static void sega_UpdateMegaDriveHeader(FILE* fileHandle)
{
    fseek(fileHandle, 0, SEEK_END);
    long length = ftell(fileHandle);

    fseek(fileHandle, 0x1A4, SEEK_SET);
    fputbl(length - 1, fileHandle);

    uint16_t checksum = sega_CalcMegaDriveChecksum(fileHandle, length);
    fseek(fileHandle, 0x18E, SEEK_SET);
    fputbw(checksum, fileHandle);
}


void sega_WriteMegaDriveImage(char* outputFilename)
{
	FILE* fileHandle = fopen(outputFilename, "w+b");
	if (fileHandle == NULL)
		Error("Unable to open \"%s\" for writing", outputFilename);

	image_WriteBinaryToFile(fileHandle, 0);

    sega_UpdateMegaDriveHeader(fileHandle);

	fclose(fileHandle);
}


static uint16_t sega_CalcMasterSystemCheckSumPart(FILE* fileHandle, int count, uint16_t checkSumIn)
{
    while (count--)
    {
        checkSumIn += (uint16_t)fgetc(fileHandle);
    }
    return checkSumIn;
}


static uint16_t sega_CalcMasterSystemCheckSum(FILE* fileHandle, int fileSize, int headerLocation)
{
    uint16_t checkSum = 0;

    fseek(fileHandle, 0, SEEK_SET);
    checkSum = sega_CalcMasterSystemCheckSumPart(fileHandle, headerLocation, checkSum);
    fseek(fileHandle, 16, SEEK_CUR);
    fileSize -= headerLocation + 16;

    if (fileSize > 0)
        checkSum = sega_CalcMasterSystemCheckSumPart(fileHandle, fileSize, checkSum);

    return checkSum;
}


uint8_t sega_CalcSizeCode(uint8_t code, int fileSize)
{
    uint8_t newCode = 0;
    
    switch (fileSize)
    {
        case 0x002000: newCode = 0x0A; break;
        case 0x004000: newCode = 0x0B; break;
        case 0x008000: newCode = 0x0C; break;
        case 0x00C000: newCode = 0x0D; break;
        case 0x010000: newCode = 0x0E; break;
        case 0x020000: newCode = 0x0F; break;
        case 0x040000: newCode = 0x00; break;
        case 0x080000: newCode = 0x01; break;
        default:
        case 0x100000: newCode = 0x02; break;
    }

    return (code & 0xF0) | newCode;
}


void sega_UpdateMasterSystemHeader(FILE* fileHandle, int headerLocation)
{
    uint16_t checkSum;
    int fileSize;
    uint8_t code;

    fseek(fileHandle, 0, SEEK_END);
    fileSize = ftell(fileHandle);

    checkSum = sega_CalcMasterSystemCheckSum(fileHandle, fileSize, headerLocation);

    fseek(fileHandle, headerLocation, SEEK_SET);
    fwrite("TMR SEGA  ", 1, 10, fileHandle);
    fputlw(checkSum, fileHandle);
    fseek(fileHandle, 3, SEEK_CUR);
    code = fgetc(fileHandle);
    fseek(fileHandle, -1, SEEK_CUR);
    fputc(sega_CalcSizeCode(code, fileSize), fileHandle);
}


void sega_WriteMasterSystemImage(char* outputFilename, int binaryPad)
{
    int headerLocation =  (binaryPad == 0) || (binaryPad >= 0x8000) ? 0x8000 : binaryPad;

	FILE* fileHandle = fopen(outputFilename, "w+b");
	if (fileHandle == NULL)
		Error("Unable to open \"%s\" for writing", outputFilename);

	image_WriteBinaryToFile(fileHandle, binaryPad);

    sega_UpdateMasterSystemHeader(fileHandle, headerLocation - 16);

	fclose(fileHandle);
}
