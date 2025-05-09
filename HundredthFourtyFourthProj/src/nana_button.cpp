/*
 *	A Button Implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/button.cpp
 */

#include <nana_button.hpp>
#include <nana_text_renderer.hpp>

namespace nana{namespace gui{
namespace drawerbase
{
	namespace button
	{
		//trigger
		//@brief: draw the button
		trigger::trigger()
			:	widget_(0),
				graph_(0),
				cite_("button")
		{
			attr_.e_state = element_state::normal;
			attr_.omitted = attr_.focused = attr_.pushed = attr_.enable_pushed = attr_.keep_pressed = false;
			attr_.focus_color = true;
			attr_.icon = 0;
		}

		trigger::~trigger()
		{
			delete attr_.icon;
		}

		void trigger::attached(widget_reference widget, graph_reference graph)
		{
			widget_ = &widget;
			window wd = widget_->handle();

			using namespace API::dev;
			make_drawer_event<events::mouse_enter>(wd);
			make_drawer_event<events::mouse_leave>(wd);
			make_drawer_event<events::mouse_down>(wd);
			make_drawer_event<events::mouse_up>(wd);
			make_drawer_event<events::key_char>(wd);
			make_drawer_event<events::key_down>(wd);
			make_drawer_event<events::focus>(wd);

			API::tabstop(wd);
			API::effects_edge_nimbus(wd, effects::edge_nimbus::active);
			API::effects_edge_nimbus(wd, effects::edge_nimbus::over);

			graph_ = &graph;
		}

		bool trigger::enable_pushed(bool eb)
		{
			attr_.enable_pushed = eb;
			return((eb == false) && pushed(false));
		}

		bool trigger::pushed(bool pshd)
		{
			if(pshd != attr_.pushed)
			{
				attr_.pushed = pshd;
				if(!pshd)
				{
					if(API::find_window(API::cursor_position()) == this->widget_->handle())
						attr_.e_state = element_state::hovered;
					else
						attr_.e_state = element_state::normal;
				}
				else
					attr_.e_state = element_state::pressed;

				return true;
			}
			return false;
		}

		bool trigger::pushed() const
		{
			return attr_.pushed;
		}

		void trigger::omitted(bool om)
		{
			attr_.omitted = om;
		}

		bool trigger::focus_color(bool eb)
		{
			if(eb != attr_.focus_color)
			{
				attr_.focus_color = eb;
				return true;
			}
			return false;
		}

		element::cite_bground& trigger::cite()
		{
			return cite_;
		}

		void trigger::refresh(graph_reference graph)
		{
			_m_draw(graph);
		}

		void trigger::mouse_enter(graph_reference graph, const eventinfo&)
		{
			attr_.e_state = (attr_.pushed || attr_.keep_pressed ? element_state::pressed : element_state::hovered);
			_m_draw(graph);
			API::lazy_refresh();
		}

		void trigger::mouse_leave(graph_reference graph, const eventinfo&)
		{
			if(attr_.enable_pushed && attr_.pushed)
				return;

			attr_.e_state = element_state::normal;
			_m_draw(graph);
			API::lazy_refresh();
		}

		void trigger::mouse_down(graph_reference graph, const eventinfo&)
		{
			attr_.e_state = element_state::pressed;
			attr_.keep_pressed = true;
			_m_draw(graph);
			API::capture_window(*widget_, true);
			API::lazy_refresh();
		}

		void trigger::mouse_up(graph_reference graph, const eventinfo&)
		{
			API::capture_window(*widget_, false);
			attr_.keep_pressed = false;
			if(attr_.enable_pushed && (false == attr_.pushed))
			{
				attr_.pushed = true;
				return;
			}

			if(element_state::pressed == attr_.e_state)
				attr_.e_state = element_state::hovered;
			else
				attr_.e_state = element_state::normal;

			attr_.pushed = false;
			_m_draw(graph);
			API::lazy_refresh();
		}

		void trigger::key_char(graph_reference, const eventinfo& ei)
		{
			if(ei.keyboard.key == gui::keyboard::enter)
			{
				eventinfo e;
				e.mouse.ctrl = false;
				e.mouse.mid_button = e.mouse.right_button = false;
				e.mouse.left_button = true;
				e.mouse.shift = false;
				e.mouse.x = e.mouse.y = 0;
				API::raise_event<events::click>(widget_->handle(), e);
			}
		}

		void trigger::key_down(graph_reference, const eventinfo& ei)
		{
			bool ch_tabstop_next;
			switch(ei.keyboard.key)
			{
			case keyboard::os_arrow_left: case keyboard::os_arrow_up:
				ch_tabstop_next = false;
				break;
			case keyboard::os_arrow_right: case keyboard::os_arrow_down:
				ch_tabstop_next = true;
				break;
			default:
				return;
			}
			API::move_tabstop(widget_->handle(), ch_tabstop_next);
		}

		void trigger::focus(graph_reference graph, const eventinfo& ei)
		{
			attr_.focused = ei.focus.getting;
			_m_draw(graph);
			API::lazy_refresh();
		}

		void trigger::_m_draw_title(graph_reference graph, bool enabled)
		{
			nana::string text = widget_->caption();

			nana::string::value_type shortkey;
			nana::string::size_type shortkey_pos;
			nana::string str = API::transform_shortkey_text(text, shortkey, &shortkey_pos);

			nana::size ts = graph.text_extent_size(str);

			nana::size icon_sz;
			if(attr_.icon)
			{
				icon_sz = attr_.icon->size();
				icon_sz.width += 5;
			}

			int x = (static_cast<int>(graph.width()  - 1  - ts.width) >> 1);
			int y = (static_cast<int>(graph.height() - 1 - ts.height) >> 1);

			if(x < static_cast<int>(icon_sz.width))
				x = static_cast<int>(icon_sz.width);

			unsigned omitted_pixels = graph.width() - icon_sz.width;

			std::size_t txtlen = str.size();
			const nana::char_t* txtptr = str.c_str();

			if(ts.width)
			{
				nana::paint::text_renderer tr(graph);
				if(enabled)
				{
					if(element_state::pressed == attr_.e_state)
					{
						++x;
						++y;
					}
					color_t fgcolor = (attr_.focus_color ? (attr_.focused ? 0xFF : attr_.fgcolor) : attr_.fgcolor);
					if(attr_.omitted)
						tr.render(x, y, fgcolor, txtptr, txtlen, omitted_pixels, true);
					else
						graph.bidi_string(x, y, fgcolor, txtptr, txtlen);

					if(shortkey)
					{
						unsigned off_w = (shortkey_pos ? graph.text_extent_size(str, static_cast<unsigned>(shortkey_pos)).width : 0);
						nana::size shortkey_size = graph.text_extent_size(txtptr + shortkey_pos, 1);
						x += off_w;
						y += shortkey_size.height;
						graph.line(x, y, x + shortkey_size.width - 1, y, 0x0);
					}
				}
				else
				{
					if(attr_.omitted)
					{
						tr.render(x + 1, y + 1, 0xFFFFFF, txtptr, txtlen, omitted_pixels, true);
						tr.render(x, y, 0x808080, txtptr, txtlen, omitted_pixels, true);
					}
					else
					{
						graph.bidi_string(x + 1, y + 1, 0xFFFFFF, txtptr, txtlen);
						graph.bidi_string(x, y, 0x808080, txtptr, txtlen);
					}
				}
			}

			if(attr_.icon)
				attr_.icon->paste(graph, 3, (graph.height() - icon_sz.height) / 2);
		}

		void trigger::_m_draw(graph_reference graph)
		{
			window wd = widget_->handle();
			bool eb = API::window_enabled(wd);

			attr_.bgcolor = API::background(wd);
			attr_.fgcolor = API::foreground(wd);

			element_state::t e_state = attr_.e_state;
			if(eb)
			{
				if(attr_.focused)
				{
					switch(e_state)
					{
					case element_state::normal:
						e_state = element_state::focus_normal;
						break;
					case element_state::hovered:
						e_state = element_state::focus_hovered;
						break;
					default:
						break;
					}
				}
			}
			else
				e_state = element_state::disabled;

			if(false == cite_.draw(graph, attr_.bgcolor, attr_.fgcolor, graph.size(), e_state))
			{
				if (bground_mode::basic != API::effects_bground_mode(wd))
				{
					_m_draw_background(graph);
					_m_draw_border(graph);
				}
			}
			_m_draw_title(graph, eb);
		}

		void trigger::_m_draw_background(graph_reference graph)
		{
			nana::rectangle r(graph.size());
			r.pare_off(1);
			nana::color_t color_start = nana::paint::graphics::mix(attr_.bgcolor, 0xFFFFFF, 0.2);
			nana::color_t color_end = nana::paint::graphics::mix(attr_.bgcolor, 0x0, 0.95);

			if(element_state::pressed == attr_.e_state)
			{
				r.x = r.y = 2;
				std::swap(color_start, color_end);
			}

			graph.shadow_rectangle(r, color_start, color_end, true);
		}

		void trigger::_m_draw_border(graph_reference graph)
		{
			nana::rectangle r(graph.size());
			int right = r.width - 1;
			int bottom = r.height - 1;
			graph.line(1, 0, right - 1, 0, 0x7F7F7F);
			graph.line(1, bottom, right - 1, bottom, 0x707070);
			graph.line(0, 1, 0, bottom - 1, 0x7F7F7F);
			graph.line(right, 1, right, bottom - 1, 0x707070);

			graph.set_pixel(1, 1, 0x919191);
			graph.set_pixel(right - 1, 1, 0x919191);
			graph.set_pixel(right - 1, bottom - 1, 0x919191);
			graph.set_pixel(1, bottom - 1, 0x919191);

			graph.set_pixel(0, 0, gui::color::button_face);
			graph.set_pixel(right, 0, gui::color::button_face);
			graph.set_pixel(0, bottom, gui::color::button_face);
			graph.set_pixel(right, bottom, gui::color::button_face);


			if(element_state::pressed == attr_.e_state)
			{
				r.pare_off(1);
				graph.rectangle(r, 0xC3C3C3, false);
			}
		}

		void trigger::icon(const nana::paint::image& img)
		{
			if(img.empty())	return;

			if(0 == attr_.icon)
				attr_.icon = new paint::image;
			*attr_.icon = img;
		}
		//end class trigger
	}//end namespace button
}//end namespace drawerbase

		//button
		//@brief:Defaine a button widget and it provides the interfaces to be operational
			button::button(){}

			button::button(window wd, bool visible)
			{
				create(wd, rectangle(), visible);
			}

			button::button(window wd, const nana::string& text, bool visible)
			{
				create(wd, rectangle(), visible);
				caption(text);
			}

			button::button(window wd, const nana::char_t* text, bool visible)
			{
				create(wd, rectangle(), visible);
				caption(text);
			}

			button::button(window wd, const rectangle& r, bool visible)
			{
				create(wd, r, visible);
			}

			button& button::icon(const nana::paint::image& img)
			{
				internal_scope_guard isg;
				get_drawer_trigger().icon(img);
				API::refresh_window(handle());
				return *this;
			}

			button& button::enable_pushed(bool eb)
			{
				internal_scope_guard isg;
				if(get_drawer_trigger().enable_pushed(eb))
					API::refresh_window(handle());
				return *this;
			}

			bool button::pushed() const
			{
				return get_drawer_trigger().pushed();
			}

			button& button::pushed(bool psd)
			{
				internal_scope_guard isg;
				if(get_drawer_trigger().pushed(psd))
					API::refresh_window(handle());
				return *this;
			}

			button& button::omitted(bool om)
			{
				internal_scope_guard isg;
				get_drawer_trigger().omitted(om);
				API::refresh_window(handle());
				return *this;
			}

			button& button::enable_focus_color(bool eb)
			{
				internal_scope_guard lock;
				if(get_drawer_trigger().focus_color(eb))
					API::refresh_window(handle());
				return *this;
			}

			button& button::set_bground(const pat::cloneable<element::element_interface>& rv)
			{
				internal_scope_guard lock;
				get_drawer_trigger().cite().set(rv);
				return *this;
			}

			button& button::set_bground(const std::string& name)
			{
				internal_scope_guard lock;
				get_drawer_trigger().cite().set(name.data());
				return *this;
			}

			button& button::transparent(bool enabled)
			{
				if (enabled)
					API::effects_bground(*this, effects::bground_transparent(0), 0.0);
				else
					API::effects_bground_remove(*this);
				return *this;
			}

			bool button::transparent() const
			{
				return (bground_mode::basic == API::effects_bground_mode(*this));
			}

			button& button::edge_effects(bool enable)
			{
				if (enable)
				{
					API::effects_edge_nimbus(*this, effects::edge_nimbus::active);
					API::effects_edge_nimbus(*this, effects::edge_nimbus::over);
				}
				else
					API::effects_edge_nimbus(*this, effects::edge_nimbus::none);

				return *this;
			}

			void button::_m_shortkey()
			{
				eventinfo ei;
				ei.mouse.x= 0, ei.mouse.y = 0;
				ei.mouse.left_button = true;
				ei.mouse.ctrl = ei.mouse.shift = false;

				API::raise_event<events::click>(handle(), ei);
			}

			void button::_m_complete_creation()
			{
				make_event<events::shortkey>(*this, &self_type::_m_shortkey);
			}

			void button::_m_caption(const nana::string& text)
			{
				API::unregister_shortkey(handle());

				nana::string::value_type shortkey;
				API::transform_shortkey_text(text, shortkey, 0);
				if(shortkey)
					API::register_shortkey(handle(), shortkey);

				base_type::_m_caption(text);
			}
		//end class button
}//end namespace gui
}//end namespace nana

