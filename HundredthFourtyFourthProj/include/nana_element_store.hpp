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

#ifndef NANA_GUI_DETAIL_ELEMENT_STORE_HPP
#define NANA_GUI_DETAIL_ELEMENT_STORE_HPP

#include <nana_element.hpp>
#include <nana_cloneable.hpp>
#include <map>
#include <string>

namespace nana
{
namespace gui
{
namespace detail
{
	class element_store
	{
		typedef nana::gui::element::element_interface element_interface;
		typedef pat::cloneable<nana::gui::element::element_interface> cloneable_element;

		struct data
		{
			cloneable_element object;
			nana::gui::element::element_interface * fast_ptr;

			data();
		};

		struct store
		{
			std::map<std::string, data> table;
		};
	public:
		element_interface * const * bground(const std::string&);
		void bground(const std::string&, const pat::cloneable<element_interface>&);
	private:
		store bground_;
	};
}//end namespace detail
}//end namespace gui
}
#endif

