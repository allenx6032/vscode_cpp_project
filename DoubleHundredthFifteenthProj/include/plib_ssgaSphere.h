/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 1998,2002  Steve Baker
 
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
 
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
 
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 
     For further information visit http://plib.sourceforge.net

     $Id: ssgaSphere.h 2000 2005-01-09 13:13:12Z bram $
*/


#include "plib_ssg.h"


// return a sphere object as an ssgBranch (connected to the specified ssgSimpleState)
ssgBranch *ssgaMakeSphere(
  ssgSimpleState *state, ssgColourArray *cl, 
  float radius, int slices, int stacks,
  ssgCallback predraw, ssgCallback postdraw );

