/***************************************************************

Date: September 8th, 2006.
Copyright (c) 2006 Cyan Technology Limited. All Rights Reserved.

Cyan Technology Limited, hereby grants a non-exclusive license
under Cyan Technology Limited's copyright to copy, modify and
distribute this software for any purpose and without fee,
provided that the above copyright notice and the following
paragraphs appear on all copies.

Cyan Technology Limited makes no representation that this
source code is correct or is an accurate representation of any
standard.

In no event shall Cyan Technology Limited have any liability
for any direct, indirect, or speculative damages, (including,
without limiting the forgoing, consequential, incidental and
special damages) including, but not limited to infringement,
loss of use, business interruptions, and loss of profits,
respective of whether Cyan Technology Limited has advance
notice of the possibility of any such damages.

Cyan Technology Limited specifically disclaims any warranties 
including, but not limited to, the implied warranties of
merchantability, fitness for a particular purpose, and non-
infringement. The software provided hereunder is on an "as is"
basis and Cyan Technology Limited has no obligation to provide
maintenance, support, updates, enhancements, or modifications.

****************************************************************/

/******************************************************************************
NAME
    putchar

SYNOPSIS
    int putchar(int c)

FUNCTION
    Implements the ANSI standard character output function.
    This version outputs a character through the DUART channels A & B and the LCD.

RETURNS
    The character.
******************************************************************************/

#include <registers.h>
#include <core_lib.h>

int putchar(int c)
{
	duart_a_write(c);
	duart_b_write(c);

    return (c);
}
