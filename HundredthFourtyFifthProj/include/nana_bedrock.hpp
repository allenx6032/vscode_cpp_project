/*
 *	A Bedrock Implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2015 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/bedrock.hpp
 */

#ifndef NANA_GUI_DETAIL_BEDROCK_HPP
#define NANA_GUI_DETAIL_BEDROCK_HPP
#include "nana_window_manager.hpp"
#include "nana_events_operation.hpp"
#include "nana_runtime_manager.hpp"
#include "nana_general_events.hpp"
#include "nana_color_schemes.hpp"
#include "nana_internal_scope_guard.hpp"

namespace nana
{
namespace detail
{
	struct native_interface;

	class element_store;

	//class bedrock
	//@brief:	bedrock is a fundamental core component, it provides a abstract to the OS platform
	//			and some basic functions.
	class bedrock
	{
		bedrock();
	public:
		typedef native_interface	interface_type;
		typedef window_manager window_manager_t;
		typedef window_manager_t::core_window_t core_window_t;

		struct thread_context;

		class flag_guard;

		~bedrock();
		void pump_event(window, bool is_modal);
		void map_thread_root_buffer(core_window_t*, bool forced, const rectangle* update_area = nullptr);
		static int inc_window(unsigned tid = 0);
		thread_context* open_thread_context(unsigned tid = 0);
		thread_context* get_thread_context(unsigned tid = 0);
		void remove_thread_context(unsigned tid = 0);
		static bedrock& instance();

		::nana::category::flags category(core_window_t*);
		core_window_t* focus();
		native_window_type root(core_window_t*);

		void set_menubar_taken(core_window_t*);

		//Delay Restores focus when a menu which attached to menubar is closed
		void delay_restore(int);
		bool close_menu_if_focus_other_window(native_window_type focus);
		void set_menu(native_window_type menu_window, bool is_keyboard_condition);
		native_window_type get_menu(native_window_type owner, bool is_keyboard_condition);
		native_window_type get_menu();
		void erase_menu(bool try_destroy);

		void get_key_state(arg_keyboard&);
		bool set_keyboard_shortkey(bool yes);
		bool whether_keyboard_shortkey() const;

		element_store& get_element_store() const;
		void map_through_widgets(core_window_t*, native_drawable_type);
	public:
		void event_expose(core_window_t *, bool exposed);
		void event_move(core_window_t*, int x, int y);
		bool event_msleave(core_window_t*);
		void thread_context_destroy(core_window_t*);
		void thread_context_lazy_refresh();
		void update_cursor(core_window_t*);
		void set_cursor(core_window_t*, nana::cursor, thread_context*);
		void define_state_cursor(core_window_t*, nana::cursor, thread_context*);
		void undefine_state_cursor(core_window_t*, thread_context*);

		widget_colors& get_scheme_template(scheme_factory_base&&);
		std::unique_ptr<widget_colors> make_scheme(scheme_factory_base&&);
	public:
		events_operation	evt_operation;
		window_manager_t	wd_manager;

		runtime_manager<core_window_t*, bedrock>	rt_manager;

		bool emit(event_code, core_window_t*, const arg_mouse&, bool ask_update, thread_context*);
		bool emit(event_code, core_window_t*, const event_arg&, bool ask_update, thread_context*);
		bool emit_drawer(event_code, core_window_t*, const event_arg&, thread_context*);
	private:
		void _m_emit_core(event_code, core_window_t*, bool draw_only, const event_arg&);
		void _m_event_filter(event_code, core_window_t*, thread_context*);
		void _m_except_handler();
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

