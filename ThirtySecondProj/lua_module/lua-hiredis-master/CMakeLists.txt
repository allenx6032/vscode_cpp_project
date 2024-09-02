# Copyright (C) 2012 LuaDist
# Submitted by David Manura.
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( lua-hiredis C )
cmake_minimum_required ( VERSION 2.6 )
include ( cmake/dist.cmake )
include ( lua )

include_directories ( lib/hiredis )

set ( SRC
	src/lua-hiredis.c
	# bundled hiredis code
	lib/hiredis/net.c
	lib/hiredis/async.c
	lib/hiredis/dict.c
	lib/hiredis/hiredis.c
	lib/hiredis/sds.c
)

install_lua_module ( hiredis ${SRC} )

# TODO: launch redis-server for tests?
add_lua_test ( test/test.lua )

install_data ( AUTHORS COPYRIGHT HISTORY README.md TODO )
install_data (
  lib/hiredis/COPYING lib/hiredis/README.md
  lib/hiredis/CHANGELOG.md lib/hiredis/VERSION
  INTO hiredis )
install_test ( test/test.lua )
