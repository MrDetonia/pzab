/*
 * PZAB Interpreter
 * ----------------
 *  Interprets a PZAB script file as specified by <file>:
 *      $ 'pzab <file>'
 *
 *  Example scripts are in the etc/examples directory.
 *  Please see the README to learn more about this program, and how to
 *  write your own PZAB scripts.
 *
 *  Copyright (C) 2015 Zac Herd <zacjherd@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>
#include <stdio.h>

#include "loops.h"

// define unsigned char as byte
#define byte unsigned char

// The PZAB registers
struct PZAB
{
    byte P0;
    byte P1:3;
    byte Z:1;
    byte A:1;
    byte B:1;
}reg;

int main (int argc, char* argv[])
{
    // check valid no. of cmd args
    if(argc != 2)
    {
        fprintf(stderr, "Invalid arguments.\nUsage: 'pzab <file>'\n");
        return 1;
    }

    // open file
    FILE* src = fopen(argv[1], "r");
    if(src == NULL)
    {
        fprintf(stderr, "Error opening '%s'\n", argv[1]);
        return 2;
    }

    // get filesize
    fseek(src, 0, SEEK_END);
    long src_siz = ftell(src);
    rewind(src);

    // allocate space in memory to store file
    byte* prog = (byte*)calloc(src_siz+1, sizeof(byte));
    if(!prog)
    {
        fclose(src);
        fprintf(stderr, "Could not allocate enough memory for '%s'\n", argv[1]);
        return 3;
    }

    // load file into memory
    if(fread(prog, src_siz, 1, src) != 1)
    {
        fclose(src);
        free(prog);
        fprintf(stderr, "Could not load '%s'\n", argv[1]);
        return 4;
    }

    // close file
    fclose(src);

    // pointer to current instruction
    long inst = 0;

    // initialise PZAB machine
    byte* mem = (byte*)calloc(256, sizeof(byte));
    if(!mem)
    {
        free(prog);
        fprintf(stderr, "Could not allocate enough memory for PZAB memory\n");
        return 5;
    }

    // track conditional blocks
    unsigned int cons = 0;

    // loop until inst reaches end of program
    do
    {
        // don't execute code while in conditional block
        if(cons == 0)
        {
            // process current instruction
            switch(prog[inst])
            {
                case '+': // increment P0
                    if(reg.P0 < 0xff) reg.P0++;
                    break;

                case '-': // decrement P0
                    if(reg.P0 > 0x00) reg.P0--;
                    break;

                case '<': // increment P1
                    if(reg.P1 < 0x07) reg.P1++;
                    break;

                case '>': // decrement P1
                    if(reg.P1 > 0x00) reg.P1--;
                    break;

                case 'A': // assign bit P1 of mem[P0] to A
                    reg.A = (mem[reg.P0] >> reg.P1) & 0x01;
                    break;

                case 'B': // assign bit P1 of mem[P0] to B
                    reg.B = (mem[reg.P0] >> reg.P1) & 0x01;
                    break;

                case 'a': // assign Z to A
                    reg.A = reg.Z;
                    break;

                case 'b': // assign Z to B
                    reg.B = reg.Z;
                    break;

                case 'Z': // assign Z to bit P1 of mem[P0]
                    if(reg.Z == 0x00) mem[reg.P0] = mem[reg.P0] & (~(0x01 << reg.P1));
                    else mem[reg.P0] = mem[reg.P0] | (reg.Z << reg.P1);
                    break;

                case '!': // logical NOT Z
                    reg.Z = (reg.Z + 0x01) % 0x02;
                    break;

                case '&': // logical AND A and B, store result in Z
                    reg.Z = reg.A & reg.B;
                    break;

                case '|': // logical OR A and B, store result in Z
                    reg.Z = reg.A | reg.B;
                    break;

                case '_': // logical XOR A and B, store result in Z
                    reg.Z = reg.A ^ reg.B;
                    break;

                case '.': // get input as integer, store in mem[P0]
                {
                    unsigned int in;
                    scanf("%u", &in);
                    mem[reg.P0] = (byte)(in % 0x0100);
                    break;
                }

                case ',': // get input as char, store in mem[P0]
                    scanf("%c", &(mem[reg.P0]));
                    break;

                case ':': // output mem[P0] as integer
                    printf("%d", mem[reg.P0]);
                    break;

                case ';': // output mem[P0] as ASCII character
                    printf("%c", mem[reg.P0]);
                    break;

                case '1': // infinte loop
                    push(10, inst);
                    break;

                case '2': // other loops
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    push(prog[inst]-48, inst);
                    break;

                case '0': // loop break
                    if(loops->count != 0)
                    {
                        loops->count--;
                        inst = loops->inst;
                    }
                    else if(loops->prev != NULL) pop();
                    break;

                default: // no error, just ignore
                    break;
            }
        }

        // manage conditional blocks
        if(mem[inst] == '?') cons++;
        else if(mem[inst] == '~' && cons > 0) cons--;

    } while(++inst <= src_siz);

    // clean up and exit
    free(prog);
    free(mem);
    return 0;
}
