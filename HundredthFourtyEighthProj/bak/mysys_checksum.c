/* Copyright (C) 2000 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */


#include <libmysql_my_global.h>
#include <libmysql_my_sys.h>
#include <zlib.h>

ha_checksum my_crc_dbug_check= 1;               /* Unlikely number */

/*
  Calculate a long checksum for a memoryblock.

  SYNOPSIS
    my_checksum()
      crc       start value for crc
      pos       pointer to memory block
      length    length of the block
*/

ha_checksum my_checksum(ha_checksum crc, const uchar *pos, size_t length)
{
  crc= (ha_checksum) crc32((uint)crc, pos, length);
  DBUG_PRINT("info", ("crc: %lu", (ulong) crc));
  if (crc == my_crc_dbug_check)
    my_debug_put_break_here();
  return crc;
}
