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

     $Id: ssgAux.cxx 1568 2002-09-02 06:05:49Z sjbaker $
*/


#include "plib_ssgAux.h"

static ssgBase *createCube ()      { return new ssgaCube ( 0 )     ; }
static ssgBase *createSphere ()    { return new ssgaSphere ( 0 )   ; }
static ssgBase *createCylinder ()  { return new ssgaCylinder ( 0 ) ; }

void ssgaInit ()
{
  ssgRegisterType ( ssgaTypeCube ()      , createCube       ) ;
  ssgRegisterType ( ssgaTypeSphere ()    , createSphere     ) ;
  ssgRegisterType ( ssgaTypeCylinder ()  , createCylinder   ) ;
}
