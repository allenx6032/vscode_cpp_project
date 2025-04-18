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

     $Id: ssgList.cxx 1949 2004-09-13 11:16:43Z wolfram_kuss $
*/


#include "plib_ssgLocal.h"

ssgList::ssgList ( int init )
{
  total = 0 ;
  next = 0 ;
  entity_list = new ssgEntity * [ limit = (init <= 0) ? 1 : init ] ;
}


ssgKidList::ssgKidList ( int init ) : ssgList ( init )
{
}


ssgList::~ssgList (void)
{
  removeAllEntities () ;
  delete [] entity_list ;
}


ssgKidList::~ssgKidList (void)
{
}


void ssgList::addEntity ( ssgEntity *entity )
{
entity->deadBeefCheck() ;
  sizeChk () ;
  entity_list [ total++ ] = entity ;
}

void ssgKidList::addEntity ( ssgEntity *entity )
{
entity->deadBeefCheck() ;
  entity -> ref () ;
  ssgList::addEntity ( entity ) ;
}

void ssgList::sizeChk (void)
{
  /* Room for one more Entity? */

  if ( total >= limit )
  {
    limit += limit ;
    ssgEntity **nlist = new ssgEntity * [ limit ] ;
    memmove ( nlist, entity_list, sizeof(ssgEntity *) * total ) ;
    delete [] entity_list ;
    entity_list = nlist ;
  }
}


int ssgList::searchForEntity ( ssgEntity *entity )
{
  for ( unsigned int i = 0 ; i < total ; i++ )
    if ( entity_list [ i ] == entity )
      return (int) i ;

  return -1 ;
}


void ssgList::removeAllEntities ()
{
  while ( total > 0 )
    removeEntity ( (unsigned int) total-1 ) ;
}

void ssgList::removeEntity ( unsigned int n )
{
  memmove ( &(entity_list[n]), &(entity_list[n+1]), sizeof(ssgEntity *) * (total-n-1) ) ;
  total-- ;

  if ( next >= n )
    next-- ;
}


void ssgKidList::removeEntity ( unsigned int n )
{
entity_list[n]->deadBeefCheck();
  ssgEntity *e = entity_list [ n ] ;

  ssgList::removeEntity ( n ) ;

  e -> deadBeefCheck () ;
  // wk 20.4.2002 problems with mywalk... when merging hiearchy nodes:
	ssgDeRefDelete ( e ) ;
}


void ssgList::replaceEntity ( unsigned int n, ssgEntity *new_entity )
{
  new_entity -> deadBeefCheck () ;
  entity_list [ n ] -> deadBeefCheck () ;
  entity_list [ n ] = new_entity;
}

void ssgKidList::replaceEntity ( unsigned int n, ssgEntity *new_entity )
{
  ssgEntity *old_entity = entity_list [ n ] ;
  new_entity -> ref () ;
  ssgList::replaceEntity ( n, new_entity ) ;
  ssgDeRefDelete ( old_entity ) ;
}
