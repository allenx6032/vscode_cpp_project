// check_box.hpp
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
#include <neogfx_button.hpp>
#include <neogfx_spacer.hpp>

namespace neogfx
{
	class check_box : public button
	{
	private:
		class box : public widget
		{
		public:
			box(check_box& aParent);
		public:
			size minimum_size(const optional_size& aAvailableSpace = optional_size()) const override;
			size maximum_size(const optional_size& aAvailableSpace = optional_size()) const override;
		public:
			void paint(graphics_context& aGraphicsContext) const override;
		};
	public:
		check_box(const std::string& aText = std::string(), checkable_e aCheckable = BiState);
		check_box(i_widget& aParent, const std::string& aText = std::string(), checkable_e aCheckable = BiState);
		check_box(i_layout& aLayout, const std::string& aText = std::string(), checkable_e aCheckable = BiState);
	public:
		neogfx::size_policy size_policy() const override;
	protected:
		void paint(graphics_context& aGraphicsContext) const override;
	protected:
		void mouse_entered() override;
		void mouse_left() override;
	private:
		box iBox;
	};
}