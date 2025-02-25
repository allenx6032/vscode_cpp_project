// stack_layout.hpp
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
#include "layout.hpp"

namespace neogfx
{
	class stack_layout : public layout
	{
	public:
		stack_layout(i_widget& aParent);
		stack_layout(i_layout& aParent);
	public:
		using layout::add_spacer;
		virtual i_spacer& add_spacer();
		virtual i_spacer& add_spacer_at(item_index aPosition);
	public:
		virtual size minimum_size(const optional_size& aAvailableSpace = optional_size()) const;
		virtual size maximum_size(const optional_size& aAvailableSpace = optional_size()) const;
	public:
		virtual void layout_items(const point& aPosition, const size& aSize);
	};
}