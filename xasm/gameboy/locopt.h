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

#ifndef GAMEBOY_LOCOPT_H_
#define GAMEBOY_LOCOPT_H_

#define CPUF_GB  0x01
#define CPUF_Z80 0x02

typedef struct MachineOptions
{
	char GameboyChar[4];
    char nCpu;
} SMachineOptions;

extern uint32_t GameboyConstID;

#endif
