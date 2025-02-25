/**
 *	A Bedrock Implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2017 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file nana/gui/detail/bedrock.hpp
 *
 *  @brief A Bedrock Implementation
 */

#ifndef NANA_GUI_DETAIL_BEDROCK_HPP
#define NANA_GUI_DETAIL_BEDROCK_HPP
#include "nana_general_events.hpp"
#include "nana_color_schemes.hpp"

namespace nana
{
namespace detail
{
	class	element_store;

	class	events_operation;
	struct	basic_window;
	class	window_manager;

	
	/// @brief	fundamental core component, it provides an abstraction to the OS platform and some basic functions.
	class bedrock
	{
		bedrock();

		bedrock(const bedrock&) = delete;
		bedrock& operator=(const bedrock&) = delete;
	public:
		using core_window_t = basic_window;

		struct thread_context;

		class flag_guard;

		~bedrock();
		void pump_event(window, bool is_modal);
		void flush_surface(core_window_t*, bool forced, const rectangle* update_area = nullptr);
		static int inc_window(unsigned tid = 0);
		thread_context* open_thread_context(unsigned tid = 0);
		thread_context* get_thread_context(unsigned tid = 0);
		void remove_thread_context(unsigned tid = 0);
		static bedrock& instance();

		core_window_t* focus();

		void set_menubar_taken(core_window_t*);

		//Delay Restores focus when a menu which attached to menubar is closed
		void delay_restore(int);
		bool close_menu_if_focus_other_window(native_window_type focus);
		void set_menu(native_window_type menu_window, bool is_keyboard_condition);
		native_window_type get_menu(native_window_type owner, bool is_keyboard_condition);
		native_window_type get_menu();
		void erase_menu(bool try_destroy);

		void get_key_state(arg_keyboard&);

		bool shortkey_occurred(bool status);
		bool shortkey_occurred() const;

		element_store& get_element_store() const;
		void map_through_widgets(core_window_t*, native_drawable_type);

		//Closes the windows which are associated with the specified thread. If the given thread_id is 0, it closes all windows
		void close_thread_window(unsigned thread_id);
	public:
		void event_expose(core_window_t *, bool exposed);
		void event_move(core_window_t*, int x, int y);
		bool event_msleave(core_window_t*);
		void event_focus_changed(core_window_t* root_wd, native_window_type receiver, bool getting);
		void thread_context_destroy(core_window_t*);
		void thread_context_lazy_refresh();
		void update_cursor(core_window_t*);
		void set_cursor(core_window_t*, nana::cursor, thread_context*);
		void define_state_cursor(core_window_t*, nana::cursor, thread_context*);
		void undefine_state_cursor(core_window_t*, thread_context*);

		color_schemes& scheme();
		events_operation&	evt_operation();
		window_manager&		wd_manager();

		void manage_form_loader(core_window_t*, bool insert_or_remove);
	public:
		// if 'bForce__EmitInternal', then ONLY internal (widget's) events are processed (even through explicit filtering)
		bool emit(event_code, core_window_t*, const event_arg&, bool ask_update, thread_context*, const bool bForce__EmitInternal = false);
	private:
		void _m_emit_core(event_code, core_window_t*, bool draw_only, const event_arg&, const bool bForce__EmitInternal);
		void _m_event_filter(event_code, core_window_t*, thread_context*);
	private:
		static bedrock bedrock_object;

		struct pi_data;
		pi_data*	pi_data_;
		struct private_impl;
		private_impl *impl_;
	};//end class bedrock
}//end namespace detail
}//end namespace nana

#endif

