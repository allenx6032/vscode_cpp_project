// i_tab_container.hpp
/*
neogfx C++ GUI Library
Copyright(C) 2016 Leigh Johnston

This program is free software: you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <neogfx.hpp>
#include "i_tab_container.hpp"

namespace neogfx
{
	class i_tab_page_container : public i_tab_container
	{
	public:
		event<i_tab_page&> selected_tab_page_changed;
	public:
		typedef uint32_t tab_index;
	public:
		struct tab_page_not_found : std::logic_error { tab_page_not_found() : std::logic_error("neogfx::i_tab_page_container::tab_page_not_found") {} };
	public:
		virtual const i_tab_page& tab_page(tab_index aTabIndex) const = 0;
		virtual i_tab_page& tab_page(tab_index aTabIndex) = 0;
		virtual const i_tab_page& selected_tab_page() const = 0;
		virtual i_tab_page& selected_tab_page() = 0;
		virtual i_tab_page& add_tab_page(const std::string& aTabText) = 0;
		virtual i_tab_page& insert_tab_page(tab_index aTabIndex, const std::string& aTabText) = 0;
		virtual i_tab_page& add_tab_page(i_tab& aTab) = 0;
		virtual i_tab_page& add_tab_page(i_tab& aTab, i_tab_page& aWidget) = 0;
		virtual i_tab_page& add_tab_page(i_tab& aTab, std::shared_ptr<i_tab_page> aWidget) = 0;
	};
}