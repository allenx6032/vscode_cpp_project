/*
 *	The Store for the Storage Of Elements
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/element_store.cpp
 */

#include <nana_element_store.hpp>

namespace nana
{
namespace gui
{
namespace detail
{
	//class element_store
	element_store::data::data()
		: fast_ptr(0)
	{}

	nana::gui::element::element_interface * const * element_store::bground(const std::string& name)
	{
		element_interface * const * addr = &(bground_.table[name].fast_ptr);
		return addr;
	}

	void element_store::bground(const std::string& name, const pat::cloneable<element_interface>& rhs)
	{
		data & store = bground_.table[name];

		store.object = rhs;
		store.fast_ptr = store.object.get();
	}
}//end namespace detail
}//end namespace gui
}
