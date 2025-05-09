/*
 *	A Textbox Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/textbox.hpp
 */

#ifndef NANA_GUI_WIDGET_TEXTBOX_HPP
#define NANA_GUI_WIDGET_TEXTBOX_HPP

#include "nana_widget.hpp"
#include "nana_textbase_extra_evtbase.hpp"

namespace nana{ namespace gui{
	namespace widgets
	{
		namespace skeletons
		{
			class text_editor;
		}
	}

	namespace drawerbase
	{
		namespace textbox
		{
			//class drawer
			class drawer
				: public nana::gui::drawer_trigger
			{
			public:
				typedef nana::gui::widgets::skeletons::text_editor text_editor;

				struct extra_evtbase_t
					: widgets::skeletons::textbase_extra_evtbase<nana::char_t>
				{};

				mutable extra_evtbase_t	extra_evtbase;

				drawer();
				text_editor * editor();
				const text_editor * editor() const;
				void set_accept(nana::functor<bool(nana::char_t)>&);
			private:
				void attached(widget_reference, graph_reference);
				void detached();
				void refresh(graph_reference);
				void focus(graph_reference, const nana::gui::eventinfo&);
				void mouse_down(graph_reference, const nana::gui::eventinfo&);
				void mouse_move(graph_reference, const nana::gui::eventinfo&);
				void mouse_up(graph_reference, const nana::gui::eventinfo&);
				void mouse_enter(graph_reference, const nana::gui::eventinfo&);
				void mouse_leave(graph_reference, const nana::gui::eventinfo&);
				void key_down(graph_reference, const nana::gui::eventinfo&);
				void key_char(graph_reference, const nana::gui::eventinfo&);
				void mouse_wheel(graph_reference, const nana::gui::eventinfo&);
				void resize(graph_reference, const nana::gui::eventinfo&);
				void typeface_changed(graph_reference);
			private:
				void _m_text_area(unsigned width, unsigned height);
				void _m_draw_border(graph_reference, nana::color_t bgcolor);
			private:
				nana::gui::widget*	widget_;
				struct status_type
				{
					bool has_focus;		//Indicates whether it has the keyboard focus
				}status_;

				nana::functor<bool(nana::char_t)> pred_acceptive_;
				nana::gui::widgets::skeletons::text_editor * editor_;
			};
		}//end namespace textbox
	}//end namespace drawerbase

	class textbox
		:public widget_object<category::widget_tag, drawerbase::textbox::drawer>
	{
		typedef drawer_trigger_t::extra_evtbase_t ext_event_type;
	public:
		textbox();
		textbox(window, bool visible);
		textbox(window, const nana::string& text, bool visible = true);
		textbox(window, const nana::char_t* text, bool visible = true);
		textbox(window, const rectangle& = rectangle(), bool visible = true);

		ext_event_type & ext_event() const;

		void load(const nana::char_t* file);
		void store(const nana::char_t* file) const;
		void store(const nana::char_t* file, nana::unicode::t encoding) const;
		textbox& reset(const nana::string& newtext = STR(""));

		///@brief	The file of last store operation.
		///@return	The filename
		nana::string filename() const;

		/// Determine whether the text was edited.
		bool edited() const;

		/// Reset the edited flag to false manually
		textbox& edited_reset();

		/// Determine whether the changed text has been saved into the file.
		bool saved() const;

		/// Read the text from a specified line. It returns true for success.
		bool getline(std::size_t n, nana::string&) const;
		textbox& append(const nana::string&, bool at_caret);

		/// Determine whether the text mode is line wrapped
		bool line_wrapped() const;
		textbox& line_wrapped(bool);

		bool multi_lines() const;
		textbox& multi_lines(bool);
		bool editable() const;
		textbox& editable(bool);
		void set_accept(nana::functor<bool(nana::char_t)>);
		textbox& tip_string(const nana::string&);
		textbox& mask(nana::char_t);
		bool selected() const;
		void select(bool);

		void copy() const;
		void paste();
		void del();

		int to_int() const;
		double to_double() const;
		textbox& from(int);
		textbox& from(double);
	protected:
		//Override _m_caption for caption()
		nana::string _m_caption() const;
		void _m_caption(const nana::string&);
		//Override _m_typeface for changing the caret
		void _m_typeface(const nana::paint::font&);
	};

}//end namespace gui
}//end namespace nana
#endif
