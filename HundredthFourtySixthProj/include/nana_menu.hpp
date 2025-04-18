/**
 *	A Menu implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2009-2017 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/menu.hpp
 *
 */

#ifndef NANA_GUI_WIDGETS_MENU_HPP
#define NANA_GUI_WIDGETS_MENU_HPP
#include "nana_widget.hpp"
#include <nana_cloneable.hpp>
#include <nana_push_ignore_diagnostic>

namespace nana
{
	class menu;

	namespace drawerbase
	{
		namespace menu
		{
			struct menu_type; //declaration

			using native_string_type = ::nana::detail::native_string_type;

			enum class checks
			{
				none,
				option,
				highlight
			};

			struct menu_item_type
			{
				/// This class is used as parameter of menu event function.
				class item_proxy
				{
				public:
					item_proxy(std::size_t pos, ::nana::menu*);
					item_proxy& enabled(bool);
					bool		enabled() const;

					item_proxy&	check_style(checks);
					item_proxy&	checked(bool);
					bool		checked() const;

					item_proxy& text(std::string title_utf8);
					std::string text() const;

					std::size_t index() const;
				private:
					std::size_t const	pos_;
					::nana::menu* const	menu_;
				};
				    /// A callback functor type.  
				typedef std::function<void(item_proxy&)> event_fn_t;

				//Default constructor initializes the item as a splitter
				menu_item_type();
				menu_item_type(std::string, const event_fn_t&);

				struct
				{
					bool enabled:1;
					bool splitter:1;
					bool checked:1;
				}flags;

				struct
				{
					bool			own_creation;	//Indicates the menu_ptr is created by create_sub_menu
					menu_type*		menu_ptr;
				}linked;

				std::string	text;
				event_fn_t	event_handler;
				checks			style{checks::none};
				paint::image	image;
				mutable wchar_t	hotkey{0};
			};

			class renderer_interface
			{
			public:
				using graph_reference = nana::paint::graphics &;

				enum class state
				{
					normal, active
				};

				struct attr
				{
					state item_state;
					bool enabled;
					bool checked;
					checks check_style;
				};

				virtual ~renderer_interface() = default;

				virtual void background(graph_reference, window) = 0;
				virtual void item(graph_reference, const nana::rectangle&, const attr&) = 0;
				virtual void item_image(graph_reference, const nana::point&, unsigned image_px, const paint::image&) = 0;
				virtual void item_text(graph_reference, const nana::point&, const std::string&, unsigned text_pixels, const attr&) = 0;
				virtual void sub_arrow(graph_reference, const nana::point&, unsigned item_pixels, const attr&) = 0;
			};
		}//end namespace menu
	}//end namespace drawerbase

	class menu
		: private noncopyable
	{
		struct implement;

		//let menubar access the private _m_popup() method.
		friend class menu_accessor;
	public:
		typedef drawerbase::menu::checks checks;

		typedef drawerbase::menu::renderer_interface renderer_interface;
		typedef drawerbase::menu::menu_item_type::item_proxy item_proxy;
		typedef drawerbase::menu::menu_item_type::event_fn_t event_fn_t;	///< A callback functor type. Prototype: `void(item_proxy&)`

		menu();										///< The default constructor. NO OTHER CONSTRUCTOR.
		~menu();

			/// Appends an item to the menu.
		item_proxy	append(std::string text_utf8, const event_fn_t& handler = {});
		void		append_splitter();

		/// Inserts new item at specified position
		/**
		 * It will invalidate the existing item proxies from the specified position.
		 * @param pos The position where new item to be inserted
		 * @param text_utf8 The title of item
		 * @param handler The event handler for the item.
		 * @return the item proxy to the new inserted item.
		 */
		item_proxy	insert(std::size_t pos, std::string text_utf8, const event_fn_t& handler = {});

		void clear();								///< Erases all of the items.
		/// Closes the menu. It does not destroy the menu; just close the window for the menu.
		void close();
		void image(std::size_t pos, const paint::image& icon);
		void text(std::size_t pos, std::string text_utf8);
		std::string text(std::size_t pos) const;
		void check_style(std::size_t pos, checks);
		void checked(std::size_t pos, bool);
		bool checked(std::size_t pos) const;
		void enabled(std::size_t pos, bool);///< Enables or disables the mouse or keyboard input for the item.
		bool enabled(std::size_t pos) const;
		void erase(std::size_t pos);			 	 ///< Removes the item
		bool link(std::size_t pos, menu& menu_obj);///< Link a menu to the item as a sub menu.
		menu * link(std::size_t pos) const;		 	     ///< Retrieves a linked sub menu of the item.
		menu *create_sub_menu(std::size_t pos);
		void popup(window owner, int x, int y);     ///< Popup the menu at the owner window. 
		void popup_await(window owner, int x, int y);
		void answerer(std::size_t index, const event_fn_t&);  ///< Modify answerer of the specified item.
		void destroy_answer(std::function<void()>);  ///< Sets an answerer for the callback while the menu window is closing.
		void gaps(const nana::point&);				///< Sets the gap between a menu and its sub menus.(\See Note4)
		void goto_next(bool forward);				///< Moves the focus to the next or previous item.
		bool goto_submen();///< Popup the submenu of the current item if it has a sub menu. Returns true if succeeds.
		bool exit_submenu();						///< Closes the current window of the sub menu.
		std::size_t size() const;					///< Return the number of items.
		int send_shortkey(wchar_t key);
		void pick();

		menu& max_pixels(unsigned);				    ///< Sets the max width in pixels of the item.
		unsigned max_pixels() const;

		menu& item_pixels(unsigned);				///< Sets the height in pixel for the items.
		unsigned item_pixels() const;

		void renderer(const pat::cloneable<renderer_interface>&);	///< Sets a user-defined renderer. 
		const pat::cloneable<renderer_interface>& renderer() const;

		/// Returns the handle of menu window
		/**
		 * @return handle of menu window, nullptr if the menu hasn't been popped up.
		 */
		window handle() const;
	private:
		void _m_popup(window, const point& position, bool called_by_menubar);
	private:
		implement * impl_;
	};

	namespace detail
	{
		class popuper
		{
		public:
			popuper(menu&, mouse);
			popuper(menu&, window owner, const point&, mouse);
			void operator()(const arg_mouse&);
		private:
			menu & mobj_;
			window owner_;
			bool take_mouse_pos_;
			nana::point pos_;
			mouse mouse_;
		};
	}

	detail::popuper menu_popuper(menu&, mouse = mouse::right_button);
	detail::popuper menu_popuper(menu&, window owner, const point&, mouse = mouse::right_button);
}//end namespace nana
#include <nana_pop_ignore_diagnostic>

#endif
