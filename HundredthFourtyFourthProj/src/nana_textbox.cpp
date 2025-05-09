/*
 *	A Textbox Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/textbox.cpp
 */

#include <nana_textbox.hpp>
#include <nana_text_editor.hpp>
#include <stdexcept>
#include <sstream>

namespace nana{ namespace gui{ namespace drawerbase {
	namespace textbox
	{
	//class draweer
		drawer::drawer()
			: widget_(0), editor_(0)
		{
			status_.has_focus = false;
		}

		drawer::text_editor* drawer::editor()
		{
			return editor_;
		}

		const drawer::text_editor* drawer::editor() const
		{
			return editor_;
		}

		void drawer::set_accept(nana::functor<bool(nana::char_t)>& pred)
		{
			pred_acceptive_ = pred;
		}

		void drawer::attached(widget_reference widget, graph_reference graph)
		{
			widget_ = & widget;
			window wd = widget_->handle();

			editor_ = new text_editor(wd, graph);
			editor_->textbase().bind_ext_evtbase(extra_evtbase);
			editor_->border_renderer(nana::make_fun(*this, &drawer::_m_draw_border));

			_m_text_area(graph.width(), graph.height());

			using namespace API::dev;
			make_drawer_event<events::focus>(wd);
			make_drawer_event<events::key_char>(wd);
			make_drawer_event<events::key_down>(wd);
			make_drawer_event<events::mouse_down>(wd);
			make_drawer_event<events::mouse_up>(wd);
			make_drawer_event<events::mouse_move>(wd);
			make_drawer_event<events::mouse_wheel>(wd);
			make_drawer_event<events::mouse_enter>(wd);
			make_drawer_event<events::mouse_leave>(wd);

			API::tabstop(wd);
			API::eat_tabstop(wd, true);
			API::effects_edge_nimbus(wd, effects::edge_nimbus::active);
			API::effects_edge_nimbus(wd, effects::edge_nimbus::over);
		}

		void drawer::detached()
		{
			delete editor_;
			editor_ = 0;
		}

		void drawer::refresh(graph_reference graph)
		{
			editor_->render(status_.has_focus);
		}

		void drawer::focus(graph_reference graph, const eventinfo& ei)
		{
			status_.has_focus = ei.focus.getting;
			refresh(graph);

			editor_->show_caret(status_.has_focus);
			editor_->reset_caret();
			API::lazy_refresh();
		}

		void drawer::mouse_down(graph_reference graph, const eventinfo& ei)
		{
			if(editor_->mouse_down(ei.mouse.left_button, ei.mouse.x, ei.mouse.y))
				API::lazy_refresh();
		}

		void drawer::mouse_move(graph_reference graph, const eventinfo& ei)
		{
			if(editor_->mouse_move(ei.mouse.left_button, ei.mouse.x, ei.mouse.y))
				API::lazy_refresh();
		}

		void drawer::mouse_up(graph_reference graph, const eventinfo& ei)
		{
			if(editor_->mouse_up(ei.mouse.left_button, ei.mouse.x, ei.mouse.y))
				API::lazy_refresh();
		}

		void drawer::mouse_wheel(graph_reference graph, const eventinfo& ei)
		{
			if(editor_->scroll(ei.wheel.upwards, true))
			{
				editor_->reset_caret();
				API::lazy_refresh();
			}
		}

		void drawer::mouse_enter(graph_reference graph, const eventinfo&)
		{
			if(editor_->mouse_enter(true))
				API::lazy_refresh();
		}

		void drawer::mouse_leave(graph_reference graph, const eventinfo&)
		{
			if(editor_->mouse_enter(false))
				API::lazy_refresh();
		}

		void drawer::key_down(graph_reference graph, const eventinfo& ei)
		{
			if(editor_->move(ei.keyboard.key))
			{
				editor_->reset_caret();
				API::lazy_refresh();
			}
		}

		void drawer::key_char(graph_reference graph, const eventinfo& ei)
		{
			if(editor_->attr().editable)
			{
				if (pred_acceptive_ && !pred_acceptive_(ei.keyboard.key))
					return;

				switch(ei.keyboard.key)
				{
				case '\b':
					editor_->backspace();	break;
				case '\n': case '\r':
					editor_->enter();	break;
				case keyboard::copy:
					editor_->copy();	break;
				case keyboard::paste:
					editor_->paste();	break;
				case keyboard::tab:
					editor_->put(keyboard::tab); break;
				case keyboard::cut:
					editor_->copy();
					editor_->del();
					break;
				default:
					if(ei.keyboard.key >= 0xFF || (32 <= ei.keyboard.key && ei.keyboard.key <= 126))
						editor_->put(ei.keyboard.key);
					else if(sizeof(nana::char_t) == sizeof(char))
					{	//Non-Unicode Version for Non-English characters
						if(ei.keyboard.key & (1<<(sizeof(nana::char_t)*8 - 1)))
							editor_->put(ei.keyboard.key);
					}
				}
				editor_->reset_caret();
				API::lazy_refresh();
			}
			else if(ei.keyboard.key == keyboard::copy)
				editor_->copy();
		}

		void drawer::resize(graph_reference graph, const eventinfo& ei)
		{
			_m_text_area(ei.size.width, ei.size.height);
			refresh(graph);
			API::lazy_refresh();
		}

		void drawer::typeface_changed(graph_reference graph)
		{
			editor_->typeface_changed();
			refresh(graph);
			API::update_window(widget_->handle());
		}

		void drawer::_m_text_area(unsigned width, unsigned height)
		{
			if(editor_)
			{
				nana::rectangle r(0, 0, width, height);
				if(!API::widget_borderless(widget_->handle()))
				{
					r.x = r.y = 2;
					r.width = (width > 4 ? width - 4 : 0);
					r.height = (height > 4 ? height - 4 : 0);
				}
				editor_->text_area(r);
			}
		}

		void drawer::_m_draw_border(graph_reference graph, nana::color_t bgcolor)
		{
			if (!API::widget_borderless(widget_->handle()))
			{
				nana::rectangle r(graph.size());
				graph.rectangle(r, (status_.has_focus ? 0x0595E2 : 0x999A9E), false);
				graph.rectangle(r.pare_off(1), bgcolor, false);
			}
		}
	//end class drawer
}//end namespace textbox
}//end namespace drawerbase

	//class textbox
		textbox::textbox()
		{
		}

		textbox::textbox(window wd, bool visible)
		{
			create(wd, rectangle(), visible);
		}

		textbox::textbox(window wd, const nana::string& text, bool visible)
		{
			create(wd, rectangle(), visible);
			caption(text);
		}

		textbox::textbox(window wd, const nana::char_t* text, bool visible)
		{
			create(wd, rectangle(), visible);
			caption(text);		
		}

		textbox::textbox(window wd, const rectangle& r, bool visible)
		{
			create(wd, r, visible);
		}

		textbox::ext_event_type& textbox::ext_event() const
		{
			return get_drawer_trigger().extra_evtbase;
		}

		void textbox::load(const nana::char_t* file)
		{
			if(0 == file)
				return;

			internal_scope_guard lock;
			drawerbase::textbox::drawer::text_editor* editor = get_drawer_trigger().editor();
			if(editor && editor->load(file))
				API::update_window(handle());
		}

		void textbox::store(const nana::char_t* file) const
		{
			internal_scope_guard lock;
			const drawerbase::textbox::drawer::text_editor* editor = get_drawer_trigger().editor();
			if(editor && file)
				editor->textbase().store(file);
		}

		void textbox::store(const nana::char_t* file, nana::unicode::t encoding) const
		{
			internal_scope_guard lock;
			const drawerbase::textbox::drawer::text_editor* editor = get_drawer_trigger().editor();
			if(editor && file)
				editor->textbase().store(file, encoding);
		}

		textbox& textbox::reset(const nana::string& str)
		{
			internal_scope_guard lock;
			drawerbase::textbox::drawer::text_editor* editor = get_drawer_trigger().editor();
			if (editor)
			{
				editor->text(str);
				editor->textbase().reset();
				API::update_window(this->handle());
			}
			return *this;
		}

		nana::string textbox::filename() const
		{
			internal_scope_guard lock;
			const drawerbase::textbox::drawer::text_editor* editor = get_drawer_trigger().editor();
			if(editor)
				return editor->textbase().filename();

			return nana::string();
		}

		bool textbox::edited() const
		{
			internal_scope_guard lock;
			const drawerbase::textbox::drawer::text_editor* editor = get_drawer_trigger().editor();
			return (editor ? editor->textbase().edited() : false);
		}

		textbox& textbox::edited_reset()
		{
			drawerbase::textbox::drawer::text_editor* editor = get_drawer_trigger().editor();
			if (editor)
				editor->textbase().edited_reset();

			return *this;
		}

		bool textbox::saved() const
		{
			internal_scope_guard lock;
			const drawerbase::textbox::drawer::text_editor* editor = get_drawer_trigger().editor();
			return (editor ? editor->textbase().saved() : false);
		}

		bool textbox::getline(std::size_t n, nana::string& text) const
		{
			const drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			return (editor ? editor->getline(n, text) : false);
		}

		textbox& textbox::append(const nana::string& text, bool at_caret)
		{
			drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			if(editor)
			{
				if(at_caret == false)
					editor->move_caret_end();

				editor->put(text);
				API::update_window(this->handle());
			}
			return *this;
		}

		/// Determine wheter the text is auto-line changed. 
		bool textbox::line_wrapped() const
		{
			return get_drawer_trigger().editor()->line_wrapped();
		}

		textbox& textbox::line_wrapped(bool autl)
		{
			drawerbase::textbox::drawer::text_editor* editor = get_drawer_trigger().editor();
			if (editor->line_wrapped(autl))
				editor->render(API::is_focus_window(handle()));

			return *this;
		}

		bool textbox::multi_lines() const
		{
			const drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			return (editor ? editor->attr().multi_lines : false);
		}

		textbox& textbox::multi_lines(bool ml)
		{
			drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			if(editor && editor->multi_lines(ml))
				API::update_window(handle());
			return *this;
		}

		bool textbox::editable() const
		{
			const drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			return (editor ? editor->attr().editable : false);
		}

		textbox& textbox::editable(bool able)
		{
			drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			if(editor)
				editor->editable(able);
			return *this;
		}

		void textbox::set_accept(nana::functor<bool(nana::char_t)> pred)
		{
			internal_scope_guard lock;
			get_drawer_trigger().set_accept(pred);
		}

		textbox& textbox::tip_string(const nana::string& str)
		{
			internal_scope_guard lock;
			drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			if(editor && editor->tip_string(str))
				API::refresh_window(this->handle());
			return *this;
		}

		textbox& textbox::mask(nana::char_t ch)
		{
			drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			if(editor && editor->mask(ch))
				API::refresh_window(this->handle());
			return *this;
		}

		bool textbox::selected() const
		{
			internal_scope_guard lock;
			const drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			return (editor ? editor->selected() : false);
		}

		void textbox::select(bool yes)
		{
			internal_scope_guard lock;
			widgets::skeletons::text_editor * editor = get_drawer_trigger().editor();
			if(editor && editor->select(yes))
				API::refresh_window(*this);		
		}

		void textbox::copy() const
		{
			internal_scope_guard lock;
			const drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			if(editor)
				editor->copy();
		}

		void textbox::paste()
		{
			internal_scope_guard lock;
			drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			if(editor)
			{
				editor->paste();
				API::refresh_window(*this);
			}
		}

		void textbox::del()
		{
			internal_scope_guard lock;
			drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			if(editor)
			{
				editor->del();
				API::refresh_window(*this);
			}
		}


		int textbox::to_int() const
		{
			nana::string s = _m_caption();
			if (s.empty()) return 0;

#ifdef NANA_MINGW
			std::stringstream ss;
			ss << static_cast<std::string>(nana::charset(s));
#elif defined(NANA_UNICODE)
			std::wstringstream ss;
			ss << s;
#else
			std::stringstream ss;
			ss << s;
#endif
			int value;
			ss >> value;
			return value;
		}

		double textbox::to_double() const
		{
			nana::string s = _m_caption();
			if (s.empty()) return 0;

#ifdef NANA_MINGW
			std::stringstream ss;
			ss << static_cast<std::string>(nana::charset(s));
#elif defined(NANA_UNICODE)
			std::wstringstream ss;
			ss << s;
#else
			std::stringstream ss;
			ss << s;
#endif
			double value;
			ss >> value;
			return value;
		}

		textbox& textbox::from(int n)
		{
#ifdef NANA_MINGW
			std::stringstream ss;
			ss<<n;
			_m_caption(nana::charset(ss.str()));
#else
	#ifdef NANA_UNICODE
			std::wstringstream ss;
	#else
			std::stringstream ss;
	#endif
			ss << n;
			_m_caption(ss.str());
#endif
			return *this;
		}

		textbox& textbox::from(double d)
		{
#ifdef NANA_MINGW
			std::stringstream ss;
			ss << d;
			_m_caption(nana::charset(ss.str()));
#else
	#ifdef NANA_UNICODE
			std::wstringstream ss;
	#else
			std::stringstream ss;
	#endif
			ss << d;
			_m_caption(ss.str());
#endif
			return *this;
		}

		//Override _m_caption for caption()
		nana::string textbox::_m_caption() const
		{
			internal_scope_guard lock;
			const drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			return (editor ? editor->text() : nana::string());
		}

		void textbox::_m_caption(const nana::string& str)
		{
			internal_scope_guard lock;
			drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			if(editor)
			{
				editor->text(str);
				API::update_window(this->handle());
			}
		}

		//Override _m_typeface for changing the caret
		void textbox::_m_typeface(const nana::paint::font& font)
		{
			widget::_m_typeface(font);
			drawerbase::textbox::drawer::text_editor * editor = get_drawer_trigger().editor();
			if(editor)
				editor->reset_caret_height();
		}
	//end class textbox
}//end namespace gui
}//end namespace nana

