/*****************************************************************************
**  Copyright (C) 1998-2001  Ljubomir Milanovic & Horst Wagner
**  This file is part of the g2 library
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public
**  License as published by the Free Software Foundation; either
**  version 2.1 of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License along with this library; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
******************************************************************************/
#include <stdio.h>
#include <g2.h>
#include <g2_win32.h>

int main()
{
    int d;
    double x, y;
    unsigned int button;
    
#include "penguin.c"

    d=g2_open_win32(150, 150,"simple_win32", 0);

    g2_set_coordinate_system(d, 50, 25, 50., 25.);

    g2_line(d, -1, 0, 1, 0);
    g2_line(d, 0, -1, 0, 1);


    g2_image(d, 5., 0., 53, 62, penguin);
    g2_image(d, 25., 25., 53, 62, penguin);

    printf("Press <Enter> to start\n");
    getchar();

    for(;;) {
        g2_query_pointer(d, &x, &y, &button);
        printf("%f %f  0x%04x\n", x, y, button);
    }
	
    return 0;
}

