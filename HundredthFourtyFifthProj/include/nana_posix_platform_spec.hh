/*
 *	Platform Specification Implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2015 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://nanapro.org/LICENSE_1_0.txt)
 *
 *	@file: nana/detail/linux_X11/platform_spec.cpp
 *
 *	This file provides basis class and data structrue that required by nana
 *
 *	http://standards.freedesktop.org/clipboards-spec/clipboards-0.1.txt
 */
#include <nana_config.hpp>

#include PLATFORM_SPEC_HPP
#include <X11/Xlocale.h>
#include <locale>
#include <map>
#include <set>
#include <algorithm>
#include <nana_graphics.hpp>
#include <nana_bedrock.hpp>
#include <nana_basic_window.hpp>
#include <nana_platform.hpp>
#include <errno.h>
#include <sstream>

#include "msg_dispatcher.hpp"

namespace nana
{
namespace detail
{
	typedef native_window_type native_window_type;
#if defined(NANA_UNICODE)
	//class conf
		conf::conf(const char * file)
		{
			ifs_.open(file);
		}

		bool conf::open(const char* file)
		{
			ifs_.open(file);
			return static_cast<bool>(ifs_);
		}

		std::string conf::value(const char* key)
		{
			if((0 == key) || !ifs_) return "";
			size_t len = ::strlen(key);
			ifs_.seekg(0, std::ios::beg);
			ifs_.clear();
			std::string str;

			while(ifs_.good())
			{
				std::getline(ifs_, str);
				if(str.size() <= len + 1)
					continue;

				size_t kpos = str.find(key);
				if((kpos != str.npos) && ((kpos == 0) || (str.substr(0, kpos) == std::string(kpos, ' '))))
				{
					size_t aspos = str.find("=", kpos + len);
					if(aspos != str.npos)
					{
						if((aspos == kpos + len) || (str.substr(kpos + len, aspos) == std::string(aspos - kpos - len, ' ')))
						{
							std::string res = str.substr(aspos + 1);
							size_t beg = res.find_first_not_of(" ");
							if(beg && (beg != res.npos))
								res = res.substr(beg);
							beg = res.find("\"");
							if(beg == 0)
							{
								size_t end = res.find_last_of("\"");
								if(beg != end)
									return res.substr(beg + 1, (end == res.npos ? res.npos : (end - 1)));
							}
							return res;
						}
					}
				}
			}
			return std::string();
		}
	//end class conf

	//class charset_conv
		charset_conv::charset_conv(const char* tocode, const char* fromcode)
		{
			handle_ = ::iconv_open(tocode, fromcode);
		}

		charset_conv::~charset_conv()
		{
			::iconv_close(handle_);
		}

		std::string charset_conv::charset(const std::string& str) const
		{
			if(reinterpret_cast<iconv_t>(-1) == handle_)
				return std::string();

			char * inbuf = const_cast<char*>(str.c_str());
			std::size_t inleft = str.size();
			std::size_t outlen = (inleft * 4 + 4);
			char * strbuf = new char[outlen + 4];
			char * outbuf = strbuf;
			std::size_t outleft = outlen;
			::iconv(handle_, &inbuf, &inleft, &outbuf, &outleft);
			std::string rstr(strbuf, outbuf);
			delete [] strbuf;
			return rstr;
		}

		std::string charset_conv::charset(const char* buf, std::size_t len) const
		{
			if(reinterpret_cast<iconv_t>(-1) == handle_)
				return std::string();

			char * inbuf = const_cast<char*>(buf);
			std::size_t outlen = (len * 4 + 4);
			char * strbuf = new char[outlen + 4];
			char * outbuf = strbuf;
			std::size_t outleft = outlen;
			::iconv(handle_, &inbuf, &len, &outbuf, &outleft);
			std::string rstr(strbuf, outbuf);
			delete [] strbuf;
			return rstr;
		}
	//end class charset_conv
#endif

	struct caret_tag
	{
		native_window_type window;
		bool has_input_method_focus;
		bool visible;
		nana::point pos;
		nana::size	size;
		nana::rectangle rev;
		nana::paint::graphics graph;
		nana::paint::graphics rev_graph;
		XIM	input_method;
		XIC	input_context;
		XFontSet input_font;
		XRectangle input_spot;
		XRectangle input_status_area;
		long input_context_event_mask;

		caret_tag(native_window_type wd)
			: window(wd), has_input_method_focus(false), visible(false), input_method(0), input_context(0), input_font(0), input_context_event_mask(0)
		{}
	};
	
	class timer_runner
	{
		typedef void (*timer_proc_t)(std::size_t id);

		struct timer_tag
		{
			std::size_t id;
			unsigned tid;
			std::size_t interval;
			std::size_t timestamp;
			timer_proc_t proc;
		};

		//timer_group
		//It owns a set of timers' identifier, and a container for the delay deletion
		//The delay delection is used for storing a timer id when the timer is deleted in a timer's
		//event handler function. If the timer is deleted directly in timer's event handler function,
		//it will cause a crash because the deletion operation invalidates iterator.
		//According to ISO C++ 2011, 23.2.4 9 the erase members shall invalidate only iterators and
		//references to the erased elements(timer_group::timers is an associative container),
		//although the iterator can be moved to next before calling the timer handler function, the delay
		//deletion is still required. Becuase a timer which is erased in another timer's handler function
		//happens to be refereneced by the "next" iterator.
		struct timer_group
		{
			bool proc_entered{false};	//This flag indicates whether the timers are going to do event.
			std::set<std::size_t> timers;
			std::vector<std::size_t> delay_deleted;
		};
	public:
		timer_runner()
			: is_proc_handling_(false)
		{}

		void set(std::size_t id, std::size_t interval, timer_proc_t proc)
		{
			auto i = holder_.find(id);
			if(i != holder_.end())
			{
				i->second.interval = interval;
				i->second.proc = proc;
				return;
			}
			unsigned tid = nana::system::this_thread_id();
			threadmap_[tid].timers.insert(id);

			timer_tag & tag = holder_[id];
			tag.id = id;
			tag.tid = tid;
			tag.interval = interval;
			tag.timestamp = 0;
			tag.proc = proc;
		}

		bool is_proc_handling() const
		{
			return is_proc_handling_;
		}

		void kill(std::size_t id)
		{
			auto i = holder_.find(id);
			if(i != holder_.end())
			{
				auto tid = i->second.tid;
				
				auto ig = threadmap_.find(tid);
				if(ig != threadmap_.end())	//Generally, the ig should not be the end of threadmap_
				{
					auto & group = ig->second;
					if(!group.proc_entered)
					{
						group.timers.erase(id);
						if(group.timers.empty())
							threadmap_.erase(ig);
					}
					else
						group.delay_deleted.push_back(id);
				}
				holder_.erase(i);
			}
		}

		bool empty() const
		{
			return (holder_.empty());
		}

		void timer_proc(unsigned tid)
		{
			is_proc_handling_ = true;
			auto i = threadmap_.find(tid);
			if(i != threadmap_.end())
			{
				auto & group = i->second;
				group.proc_entered = true;
				unsigned ticks = nana::system::timestamp();
				for(auto timer_id : group.timers)
				{
					auto & tag = holder_[timer_id];
					if(tag.timestamp)
					{
						if(ticks >= tag.timestamp + tag.interval)
						{
							tag.timestamp = ticks;
							try
							{
								tag.proc(tag.id);
							}catch(...){}	//nothrow
						}
					}
					else
						tag.timestamp = ticks;
				}
				group.proc_entered = false;
				for(auto tmr: group.delay_deleted)
					group.timers.erase(tmr);
			}
			is_proc_handling_ = false;
		}
	private:
		bool is_proc_handling_;
		std::map<unsigned, timer_group> threadmap_;
		std::map<std::size_t, timer_tag> holder_;
	};

	drawable_impl_type::drawable_impl_type()
	{
		string.tab_length = 4;
		string.tab_pixels = 0;
		string.whitespace_pixels = 0;
#if defined(NANA_UNICODE)
		conv_.handle = ::iconv_open("UTF-8", "UTF-32");
		conv_.code = "UTF-32";
#endif
	}

	drawable_impl_type::~drawable_impl_type()
	{
#if defined(NANA_UNICODE)
		::iconv_close(conv_.handle);
#endif
	}

	void drawable_impl_type::set_color(const ::nana::color& clr)
	{
		color_ = clr.px_color().value;
	}

	void drawable_impl_type::set_text_color(const ::nana::color& clr)
	{
		text_color_ = clr.px_color().value;
		update_text_color();
	}

	void drawable_impl_type::update_color()
	{
		if (color_ != current_color_)
		{
			auto & spec = nana::detail::platform_spec::instance();
			platform_scope_guard lock;

			current_color_ = color_;
			auto col = color_;
			switch (spec.screen_depth())
			{
			case 16:
				col = ((((col >> 16) & 0xFF) * 31 / 255) << 11) |
					((((col >> 8) & 0xFF) * 63 / 255) << 5) |
					(col & 0xFF) * 31 / 255;
				break;
			}
			::XSetForeground(spec.open_display(), context, col);
			::XSetBackground(spec.open_display(), context, col);
		}
	}

	void drawable_impl_type::update_text_color()
	{
		if (text_color_ != current_color_)
		{
			auto & spec = nana::detail::platform_spec::instance();
			platform_scope_guard lock;

			current_color_ = text_color_;
			auto col = text_color_;
			switch (spec.screen_depth())
			{
			case 16:
				col = ((((col >> 16) & 0xFF) * 31 / 255) << 11) |
					((((col >> 8) & 0xFF) * 63 / 255) << 5) |
					(col & 0xFF) * 31 / 255;
				break;
			}
			::XSetForeground(spec.open_display(), context, col);
			::XSetBackground(spec.open_display(), context, col);

#if defined(NANA_UNICODE)
			xft_fgcolor.color.red = ((0xFF0000 & col) >> 16) * 0x101;
			xft_fgcolor.color.green = ((0xFF00 & col) >> 8) * 0x101;
			xft_fgcolor.color.blue = (0xFF & col) * 0x101;
			xft_fgcolor.color.alpha = 0xFFFF;
#endif
		}
	}

	void drawable_impl_type::fgcolor(const ::nana::color& clr)
	{
		auto rgb = clr.px_color().value;

		if (rgb != current_color_)
		{
			auto & spec = nana::detail::platform_spec::instance();
			platform_scope_guard psg;

			current_color_ = rgb;
			switch(spec.screen_depth())
			{
			case 16:
				rgb = ((((rgb >> 16) & 0xFF) * 31 / 255) << 11) |
					((((rgb >> 8) & 0xFF) * 63 / 255) << 5)	|
					(rgb & 0xFF) * 31 / 255;
				break;
			}
			::XSetForeground(spec.open_display(), context, rgb);
			::XSetBackground(spec.open_display(), context, rgb);
#if defined(NANA_UNICODE)
			xft_fgcolor.color.red = ((0xFF0000 & rgb) >> 16) * 0x101;
			xft_fgcolor.color.green = ((0xFF00 & rgb) >> 8) * 0x101;
			xft_fgcolor.color.blue = (0xFF & rgb) * 0x101;
			xft_fgcolor.color.alpha = 0xFFFF;
#endif
		}
	}

	class font_deleter
	{
    public:
        void operator()(const font_tag* fp) const
        {
            if(fp && fp->handle)
            {
                platform_scope_guard psg;
#if defined(NANA_UNICODE)
                ::XftFontClose(nana::detail::platform_spec::instance().open_display(), fp->handle);
#else
                ::XFreeFontSet(nana::detail::platform_spec::instance().open_display(), fp->handle);
#endif
            }
            delete fp;
        }
	};//end class font_deleter

	platform_scope_guard::platform_scope_guard()
	{
		platform_spec::instance().lock_xlib();
	}

	platform_scope_guard::~platform_scope_guard()
	{
		platform_spec::instance().unlock_xlib();
	}

	int X11_error_handler(Display* disp, XErrorEvent* err)
	{
	    platform_spec::instance().error_code = err->error_code;
		return 0;
	}

	int X11_fatal_handler(Display* disp)
	{
		return 0;
	}

	platform_spec::timer_runner_tag::timer_runner_tag()
		: runner(0), delete_declared(false)
	{}

	platform_spec::platform_spec()
		:display_(0), colormap_(0), def_X11_error_handler_(0), grab_(0)
	{
		::XInitThreads();
		const char * langstr = getenv("LC_CTYPE");
		if(0 == langstr)
		{
			langstr = getenv("LC_ALL");
		}

		std::string langstr_dup;
		if(langstr)
		{
			langstr_dup = langstr;
			auto dotpos = langstr_dup.find(".");
			if(dotpos != langstr_dup.npos)
			{
				auto beg = langstr_dup.begin() + dotpos + 1;
				std::transform(beg, langstr_dup.end(), beg, toupper);
			}
		}
		else
			langstr_dup = "en_US.UTF-8";
		std::setlocale(LC_CTYPE, langstr_dup.c_str());
		if(::XSupportsLocale())
			::XSetLocaleModifiers(langstr_dup.c_str());


		display_ = ::XOpenDisplay(0);
		colormap_ = DefaultColormap(display_,  ::XDefaultScreen(display_));

		//Initialize the member data
		selection_.content.utf8_string = 0;
		xdnd_.good_type = None;

		atombase_.wm_protocols = ::XInternAtom(display_, "WM_PROTOCOLS", False);
		atombase_.wm_change_state = ::XInternAtom(display_, "WM_CHANGE_STATE", False);
		atombase_.wm_delete_window = ::XInternAtom(display_, "WM_DELETE_WINDOW", False);
		atombase_.net_wm_state = ::XInternAtom(display_, "_NET_WM_STATE", False);
		atombase_.net_wm_state_skip_taskbar = ::XInternAtom(display_, "_NET_WM_STATE_SKIP_TASKBAR", False);
		atombase_.net_wm_state_fullscreen = ::XInternAtom(display_, "_NET_WM_STATE_FULLSCREEN", False);
		atombase_.net_wm_state_maximized_horz = ::XInternAtom(display_, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
		atombase_.net_wm_state_maximized_vert = ::XInternAtom(display_, "_NET_WM_STATE_MAXIMIZED_VERT", False);
		atombase_.net_wm_state_modal = ::XInternAtom(display_, "_NET_WM_STATE_MODAL", False);
		atombase_.net_wm_name = ::XInternAtom(display_, "_NET_WM_NAME", False);
		atombase_.net_wm_window_type = ::XInternAtom(display_, "_NET_WM_WINDOW_TYPE", False);
		atombase_.net_wm_window_type_normal = ::XInternAtom(display_, "_NET_WM_WINDOW_TYPE_NORMAL", False);
		atombase_.net_wm_window_type_utility = ::XInternAtom(display_, "_NET_WM_WINDOW_TYPE_UTILITY", False);
		atombase_.net_wm_window_type_dialog = ::XInternAtom(display_, "_NET_WM_WINDOW_TYPE_DIALOG", False);
		atombase_.motif_wm_hints = ::XInternAtom(display_, "_MOTIF_WM_HINTS", False);

		atombase_.clipboard = ::XInternAtom(display_, "CLIPBOARD", True);
		atombase_.text = ::XInternAtom(display_, "TEXT", True);
		atombase_.text_uri_list = ::XInternAtom(display_, "text/uri-list", True);
		atombase_.utf8_string = ::XInternAtom(display_, "UTF8_STRING", True);
		atombase_.targets = ::XInternAtom(display_, "TARGETS", True);

		atombase_.xdnd_aware = ::XInternAtom(display_, "XdndAware", False);
		atombase_.xdnd_enter = ::XInternAtom(display_, "XdndEnter", False);
		atombase_.xdnd_position = ::XInternAtom(display_, "XdndPosition", False);
		atombase_.xdnd_status	= ::XInternAtom(display_, "XdndStatus", False);
		atombase_.xdnd_action_copy = ::XInternAtom(display_, "XdndActionCopy", False);
		atombase_.xdnd_drop = ::XInternAtom(display_, "XdndDrop", False);
		atombase_.xdnd_selection = ::XInternAtom(display_, "XdndSelection", False);
		atombase_.xdnd_typelist = ::XInternAtom(display_, "XdndTypeList", False);
		atombase_.xdnd_finished = ::XInternAtom(display_, "XdndFinished", False);

		//Create default font object.
		def_font_ptr_ = make_native_font(0, font_size_to_height(10), 400, false, false, false);
		msg_dispatcher_ = new msg_dispatcher(display_);
	}

	platform_spec::~platform_spec()
	{
		delete msg_dispatcher_;

		//The font should be destroyed before closing display,
		//otherwise it crashs
		def_font_ptr_.reset();

		close_display();
	}

	const platform_spec::font_ptr_t& platform_spec::default_native_font() const
	{
		return def_font_ptr_;
	}
	
	void platform_spec::default_native_font(const font_ptr_t& fp)
	{
		def_font_ptr_ = fp;
	}

	unsigned platform_spec::font_size_to_height(unsigned size) const
	{
		return size;
	}

	unsigned platform_spec::font_height_to_size(unsigned height) const
	{
		return height;
	}

	platform_spec::font_ptr_t platform_spec::make_native_font(const nana::char_t* name, unsigned height, unsigned weight, bool italic, bool underline, bool strike_out)
	{
		font_ptr_t ref;
#if defined(NANA_UNICODE)
		if(0 == name || *name == 0)
			name = STR("*");

		std::string nmstr = nana::charset(name);
		XftFont* handle = 0;
		std::stringstream ss;
		ss<<nmstr<<"-"<<(height ? height : 10);
		XftPattern * pat = ::XftNameParse(ss.str().c_str());
		XftResult res;
		XftPattern * match_pat = ::XftFontMatch(display_, ::XDefaultScreen(display_), pat, &res);
		if(match_pat)
			handle = ::XftFontOpenPattern(display_, match_pat);
#else
		std::string basestr;
		if(0 == name || *name == 0)
		{
			basestr = "-misc-fixed-*";
		}
		else
			basestr = "-misc-fixed-*";

		char ** missing_list;
		int missing_count;
		char * defstr;
		XFontSet handle = ::XCreateFontSet(display_, const_cast<char*>(basestr.c_str()), &missing_list, &missing_count, &defstr);
#endif
		if(handle)
		{
			font_tag * impl = new font_tag;
			impl->name = name;
			impl->height = height;
			impl->weight = weight;
			impl->italic = italic;
			impl->underline = underline;
			impl->strikeout = strike_out;
			impl->handle = handle;
			return font_ptr_t(impl, font_deleter());
		}
		return font_ptr_t();
	}

	Display* platform_spec::open_display()
	{
		return display_;
	}

	void platform_spec::close_display()
	{
		if(display_)
		{
		    ::XSync(reinterpret_cast<Display*>(display_), true);
			::XCloseDisplay(reinterpret_cast<Display*>(display_));
			display_ = 0;
		}
	}

	void platform_spec::lock_xlib()
	{
		xlib_locker_.lock();
	}

	void platform_spec::unlock_xlib()
	{
		xlib_locker_.unlock();
	}

	Window platform_spec::root_window()
	{
		return ::XDefaultRootWindow(reinterpret_cast<Display*>(display_));
	}

	int platform_spec::screen_depth()
	{
		return ::XDefaultDepth(display_, ::XDefaultScreen(display_));
	}

	Visual* platform_spec::screen_visual()
	{
		return ::XDefaultVisual(display_, ::XDefaultScreen(display_));
	}

	Colormap& platform_spec::colormap()
	{
		return colormap_;
	}

	platform_spec& platform_spec::instance()
	{
		static platform_spec object;
		return object;
	}

	const atombase_tag& platform_spec::atombase() const
	{
		return atombase_;
	}

	//There are three members make_owner(), get_owner() and remove(),
	//they are maintain a table to discribe the owner of windows because the feature in X11, the
	//owner of top level window must be RootWindow.
	void platform_spec::make_owner(native_window_type owner, native_window_type wd)
	{
		platform_scope_guard psg;
		wincontext_[wd].owner = owner;
		window_context_t & context = wincontext_[owner];
		if(context.owned == 0)
			context.owned = new std::vector<native_window_type>;
		context.owned->push_back(wd);
	}

	native_window_type platform_spec::get_owner(native_window_type wd) const
	{
		platform_scope_guard psg;
		auto i = wincontext_.find(wd);
		return (i != wincontext_.end() ? i->second.owner : nullptr);
	}

	void platform_spec::remove(native_window_type wd)
	{
		msg_dispatcher_->erase(reinterpret_cast<Window>(wd));
		platform_scope_guard psg;
		auto i = wincontext_.find(wd);
		if(i == wincontext_.end()) return;

		if(i->second.owner)
		{
			auto u = wincontext_.find(i->second.owner);
			if(u != wincontext_.end())
			{
				auto * vec = u->second.owned;
				if(vec)
				{
					auto j = std::find(vec->begin(), vec->end(), i->first);
					if(j != vec->end())
						vec->erase(j);
				}
			}
		}

		auto * vec = i->second.owned;
		if(vec)
		{
			set_error_handler();
			auto & wd_manager = detail::bedrock::instance().wd_manager;
			for(auto u = vec->rbegin(); u != vec->rend(); ++u)
				wd_manager.close(wd_manager.root(*u));

			rev_error_handler();
		}
		delete vec;
		wincontext_.erase(i);
		iconbase_.erase(wd);
	}


	void platform_spec::write_keystate(const XKeyEvent& xkey)
	{
		this->key_state_ = xkey;
	}

	void platform_spec::read_keystate(XKeyEvent& xkey)
	{
		xkey = this->key_state_;
	}

	XIC platform_spec::caret_input_context(native_window_type wd) const
	{
		platform_scope_guard psg;
		auto i = caret_holder_.carets.find(wd);
		if(i != caret_holder_.carets.end())
			return i->second->input_context;
		return 0;
	}

	void platform_spec::caret_open(native_window_type wd, const ::nana::size& caret_sz)
	{
		bool is_start_routine = false;
		platform_scope_guard psg;
		caret_tag * & addr = caret_holder_.carets[wd];
		if(0 == addr)
		{
			addr = new caret_tag(wd);
			is_start_routine = (caret_holder_.carets.size() == 1);
			addr->input_method = ::XOpenIM(display_, 0, 0, 0);
			if(addr->input_method)
			{
				XIMStyles* imstyle;
				::XGetIMValues(addr->input_method, XNQueryInputStyle, &imstyle, nullptr, nullptr);
				if(imstyle)
				{
					if(imstyle->count_styles)
					{
						addr->input_font = 0;
						XVaNestedList preedit_attr = ::XVaCreateNestedList(0, XNSpotLocation, &(addr->input_spot), nullptr);
						XVaNestedList status_attr = ::XVaCreateNestedList(0, XNAreaNeeded, &(addr->input_status_area), nullptr);
						XIMStyle * style_end = imstyle->supported_styles + imstyle->count_styles;
						bool has_status = false;
						bool has_preedit = false;
						for(XIMStyle * i = imstyle->supported_styles; i != style_end; ++i)
						{
							if(*i == (XIMPreeditPosition | XIMStatusArea))
							{
								has_status = has_preedit = true;
								break;
							}
							else if(*i == (XIMPreeditPosition | XIMStatusNothing))
								has_preedit = true;
						}

						if(has_status)
						{
							addr->input_context = ::XCreateIC(addr->input_method, XNInputStyle, (XIMPreeditPosition | XIMStatusArea),
														XNPreeditAttributes, preedit_attr, XNStatusAttributes, status_attr,
														XNClientWindow, reinterpret_cast<Window>(wd), nullptr);
						}
						else
							addr->input_context = 0;

						if((addr->input_context == 0) && has_preedit)
						{
							addr->input_context = ::XCreateIC(addr->input_method, XNInputStyle, (XIMPreeditPosition | XIMStatusNothing),
															XNPreeditAttributes, preedit_attr, XNClientWindow, reinterpret_cast<Window>(wd), nullptr);
						}

						if(addr->input_context)
						{
							XVaNestedList attr = ::XVaCreateNestedList(0, XNAreaNeeded, &(addr->input_status_area), XNClientWindow, reinterpret_cast<Window>(wd), nullptr);
							::XGetICValues(addr->input_context, XNStatusAttributes, attr, nullptr);
							::XFree(attr);
						}
						else
							addr->input_context = ::XCreateIC(addr->input_method, XNInputStyle, (XIMPreeditNothing | XIMStatusNothing),
															XNClientWindow, reinterpret_cast<Window>(wd), nullptr);

						if(addr->input_context)
						{
							//Make the IM event filter.
							::XGetICValues(addr->input_context, XNFilterEvents, &(addr->input_context_event_mask), nullptr);
							XWindowAttributes attr;
							::XGetWindowAttributes(display_, reinterpret_cast<Window>(wd), &attr);
							XSetWindowAttributes new_attr;
							new_attr.event_mask = (attr.your_event_mask | addr->input_context_event_mask);
							::XChangeWindowAttributes(display_, reinterpret_cast<Window>(wd), CWEventMask, &new_attr);
						}
						::XFree(preedit_attr);
						::XFree(status_attr);
					}
					::XFree(imstyle);
				}
			}
		}

		addr->visible = false;
		addr->graph.make(caret_sz);
		addr->graph.rectangle(true, colors::black);
		addr->rev_graph.make(caret_sz);

		addr->size = caret_sz;

		if(addr->input_context && (false == addr->has_input_method_focus))
		{
			::XSetICFocus(addr->input_context);
			addr->has_input_method_focus = true;
		}

		if(is_start_routine)
		{
			caret_holder_.exit_thread = false;
			auto fn = [this](){ this->_m_caret_routine(); };
			caret_holder_.thr.reset(new std::thread(fn));
		}
	}

	void platform_spec::caret_close(native_window_type wd)
	{
		bool is_end_routine = false;
		{
			platform_scope_guard psg;

			auto i = caret_holder_.carets.find(wd);
			if(i != caret_holder_.carets.end())
			{
				caret_tag * addr = i->second;
				if(addr->input_context)
				{
					if(addr->has_input_method_focus)
					{
						::XUnsetICFocus(addr->input_context);
						addr->has_input_method_focus = false;
					}

					//Remove the IM event filter.
					set_error_handler();
					XWindowAttributes attr;
					if(BadWindow != ::XGetWindowAttributes(display_, reinterpret_cast<Window>(wd), &attr))
					{
						if((attr.your_event_mask & addr->input_context_event_mask) == addr->input_context_event_mask)
						{
							XSetWindowAttributes new_attr;

							//Don't remove the KeyPress and KeyRelease mask(0x3), otherwise the window will not receive
							//Keyboard events after destroying caret 
							new_attr.event_mask = (attr.your_event_mask & ~(addr->input_context_event_mask & (~0x3)));
							::XChangeWindowAttributes(display_, reinterpret_cast<Window>(wd), CWEventMask, &new_attr);
						}
					}
					rev_error_handler();

					::XDestroyIC(addr->input_context);
				}

				if(addr->input_font)
					::XFreeFontSet(display_, addr->input_font);

				if(addr->input_method)
					::XCloseIM(addr->input_method);

				delete i->second;
				caret_holder_.carets.erase(i);

			}

			is_end_routine = (caret_holder_.carets.size() == 0);
		}

		if(is_end_routine && (caret_holder_.thr != nullptr) && (caret_holder_.thr->joinable()))
		{
			caret_holder_.exit_thread = true;
			caret_holder_.thr->join();
			caret_holder_.thr.reset();
		}
	}

	void platform_spec::caret_pos(native_window_type wd, const point& pos)
	{
		platform_scope_guard psg;
		auto i = caret_holder_.carets.find(wd);
		if(i != caret_holder_.carets.end())
		{
			caret_tag & crt = *i->second;
			caret_reinstate(crt);
			crt.pos = pos;
		}
	}

	void platform_spec::caret_visible(native_window_type wd, bool vis)
	{
		platform_scope_guard psg;
		auto i = caret_holder_.carets.find(wd);
		if(i != caret_holder_.carets.end())
		{
			caret_tag& crt = *i->second;
			if(crt.visible != vis)
			{
				if(vis == false)
				{
					caret_reinstate(crt);
					if(crt.input_context && crt.has_input_method_focus)
					{
						::XUnsetICFocus(crt.input_context);
						crt.has_input_method_focus = false;
					}
				}
				else
				{
					if(crt.input_context && (false == crt.has_input_method_focus))
					{
						::XSetICFocus(crt.input_context);
						crt.has_input_method_focus = true;
					}
				}
				crt.visible = vis;
			}
		}
	}

	void platform_spec::caret_flash(caret_tag & crt)
	{
		if(crt.visible && (false == caret_reinstate(crt)))
		{
			crt.rev_graph.bitblt(rectangle{crt.size}, crt.window, crt.pos);
			crt.rev.width = crt.size.width;
			crt.rev.height = crt.size.height;
			crt.rev.x = crt.pos.x;
			crt.rev.y = crt.pos.y;
			crt.graph.paste(crt.window, crt.rev, 0, 0);
		}
	}

	bool platform_spec::caret_update(native_window_type wd, nana::paint::graphics& root_graph, bool is_erase_caret_from_root_graph)
	{
		platform_scope_guard psg;
		auto i = caret_holder_.carets.find(wd);
		if(i != caret_holder_.carets.end())
		{
			caret_tag & crt = *i->second;
			if(is_erase_caret_from_root_graph)
			{
				root_graph.bitblt(crt.rev, crt.rev_graph);
			}
			else
			{
				bool owns_caret = false;
				nana::paint::graphics * crt_graph;
				if(crt.rev.width && crt.rev.height)
				{
					crt.rev_graph.bitblt(rectangle{crt.size}, root_graph, crt.pos);
					crt_graph = &crt.graph;
					owns_caret = true;
				}
				else
					crt_graph = &crt.rev_graph;

				root_graph.bitblt(crt.rev, *crt_graph);
				return owns_caret;
			}
		}
		return false;
	}

	//Copy the reversed graphics to the window
	bool platform_spec::caret_reinstate(caret_tag& crt)
	{
		if(crt.rev.width && crt.rev.height)
		{
			crt.rev_graph.paste(crt.window, crt.rev, 0, 0);
			//Drop the reversed graphics in order to draw the
			//caret in the next flash.
			crt.rev.width = crt.rev.height = 0;
			return true;
		}
		return false;
	}

	void platform_spec::set_error_handler()
	{
		platform_scope_guard psg;
		error_code = 0;
		def_X11_error_handler_ = ::XSetErrorHandler(X11_error_handler);
	}

	int platform_spec::rev_error_handler()
	{
		if(def_X11_error_handler_)
		{
			platform_scope_guard psg;
			::XSync(display_, False);
			::XSetErrorHandler(def_X11_error_handler_);
		}
		return error_code;
	}

	void platform_spec::_m_caret_routine()
	{
		while(false == caret_holder_.exit_thread)
		{
			if(xlib_locker_.try_lock())
			{
				for(auto i : caret_holder_.carets)
					caret_flash(*(i.second));

				xlib_locker_.unlock();
			}
			for(int i = 0; i < 5 && (false == caret_holder_.exit_thread); ++i)
				nana::system::sleep(100);
		}
	}

	Window platform_spec::grab(Window wd)
	{
		Window r = grab_;
		grab_ = wd;
		return r;
	}

	void platform_spec::set_timer(std::size_t id, std::size_t interval, void (*timer_proc)(std::size_t))
	{
		std::lock_guard<decltype(timer_.mutex)> lock(timer_.mutex);
		if(0 == timer_.runner)
			timer_.runner = new timer_runner;
		timer_.runner->set(id, interval, timer_proc);
		timer_.delete_declared = false;
	}

	void platform_spec::kill_timer(std::size_t id)
	{
		if(timer_.runner == 0) return;

		std::lock_guard<decltype(timer_.mutex)> lock(timer_.mutex);
		timer_.runner->kill(id);
		if(timer_.runner->empty())
		{
			if(timer_.runner->is_proc_handling() == false)
			{
				delete timer_.runner;
				timer_.runner = 0;
			}
			else
				timer_.delete_declared = true;
		}
	}

	void platform_spec::timer_proc(unsigned tid)
	{
		std::lock_guard<decltype(timer_.mutex)> lock(timer_.mutex);
		if(timer_.runner)
		{
			timer_.runner->timer_proc(tid);
			if(timer_.delete_declared)
			{
				delete timer_.runner;
				timer_.runner = 0;
				timer_.delete_declared = false;
			}
		}
	}

	void platform_spec::msg_insert(native_window_type wd)
	{
		msg_dispatcher_->insert(reinterpret_cast<Window>(wd));
	}

	void platform_spec::msg_set(timer_proc_type tp, event_proc_type ep)
	{
		msg_dispatcher_->set(tp, ep, &platform_spec::_m_msg_filter);
	}

	void platform_spec::msg_dispatch(native_window_type modal)
	{
		msg_dispatcher_->dispatch(reinterpret_cast<Window>(modal));
	}

	void* platform_spec::request_selection(native_window_type requestor, Atom type, size_t& size)
	{
		if(requestor)
		{
			Atom clipboard = atombase_.clipboard;
			xlib_locker_.lock();
			Window owner = ::XGetSelectionOwner(display_, clipboard);
			if(owner)
			{
				selection_tag::item_t * selim = new selection_tag::item_t;
				selim->type = type;
				selim->requestor = reinterpret_cast<Window>(requestor);
				selim->buffer = nullptr;
				selim->bufsize = 0;

				this->selection_.items.push_back(selim);
				::XConvertSelection(display_, clipboard, type, clipboard,
							reinterpret_cast<Window>(requestor), CurrentTime);
				::XFlush(display_);
				xlib_locker_.unlock();

				std::unique_lock<decltype(selim->cond_mutex)> lock(selim->cond_mutex);
				selim->cond.wait(lock);

				size = selim->bufsize;
				void * retbuf = selim->buffer;
				delete selim;
				return retbuf;
			}
			else
				xlib_locker_.unlock();
		}
		return nullptr;
	}

	void platform_spec::write_selection(native_window_type owner, Atom type, const void * buf, size_t bufsize)
	{
		platform_scope_guard psg;
		::XSetSelectionOwner(display_, atombase_.clipboard, reinterpret_cast<Window>(owner), CurrentTime);
		::XFlush(display_);
		if(XA_STRING == type || atombase_.utf8_string == type)
		{
			std::string * utf8str = selection_.content.utf8_string;
			if(utf8str)
				utf8str->clear();
			else
				utf8str = new std::string;

			utf8str->append(reinterpret_cast<const char*>(buf), reinterpret_cast<const char*>(buf) + bufsize);
			selection_.content.utf8_string = utf8str;
		}
	}

	//Icon Storage
	const nana::paint::graphics& platform_spec::keep_window_icon(native_window_type wd, const nana::paint::image& img)
	{
		nana::paint::graphics & graph = iconbase_[wd];
		graph.make(img.size());
		img.paste(graph, {});
		return graph;
	}

	//_m_msg_filter
	//@return:	_m_msg_filter returns three states
	//		0 = msg_dispatcher dispatches the XEvent
	//		1 = msg_dispatcher dispatches the msg_packet_tag that modified by _m_msg_filter
	//		2 = msg_dispatcher should ignore the msg, because the XEvent is processed by _m_msg_filter
	int platform_spec::_m_msg_filter(XEvent& evt, msg_packet_tag& msg)
	{
		platform_spec & self = instance();
		if(SelectionNotify == evt.type)
		{
			if(evt.xselection.property)
			{
				Atom type;
				int format;
				unsigned long len, bytes_left = 0;
				unsigned char *data;

				::XGetWindowProperty(self.display_, evt.xselection.requestor, evt.xselection.property, 0, 0, 0,
									 AnyPropertyType, &type, &format, &len, &bytes_left, &data);

				if(evt.xselection.property == self.atombase_.clipboard)
				{
					platform_scope_guard psg;

					if(self.selection_.items.size())
					{
						selection_tag::item_t * im = self.selection_.items.front();

						if(bytes_left > 0 && (type == im->type))
						{
							unsigned long dummy_bytes_left;
							if(Success == ::XGetWindowProperty(self.display_, evt.xselection.requestor,
															evt.xselection.property, 0, bytes_left,
															0, AnyPropertyType, &type, &format, &len,
															&dummy_bytes_left, &data))
							{
								im->buffer = data;
								im->bufsize = len;
							}
						}


						self.selection_.items.erase(self.selection_.items.begin());

						std::lock_guard<decltype(im->cond_mutex)> lock(im->cond_mutex);
						im->cond.notify_one();

					}
				}
				else if(evt.xselection.property == self.atombase_.xdnd_selection)
				{
					bool accepted = false;
					msg.kind = msg.kind_mouse_drop;
					msg.u.mouse_drop.window = 0;
					if(bytes_left > 0 && type == self.xdnd_.good_type)
					{
						unsigned long dummy_bytes_left;
						if(Success == ::XGetWindowProperty(self.display_, evt.xselection.requestor,
															evt.xselection.property, 0, bytes_left,
															0, AnyPropertyType, &type, &format, &len,
															&dummy_bytes_left, &data))
						{
							std::vector<nana::string> * files = new std::vector<nana::string>;
							std::stringstream ss(reinterpret_cast<char*>(data));
							while(true)
							{
								std::string file;
								std::getline(ss, file);
								if(false == ss.good()) break;
								if(0 == file.find("file://"))
									file = file.substr(7);

								while(file.size())
								{
									auto ch = file.back();
									if('\r' == ch || '\n' == ch)
										file.pop_back();
									else
										break;
								}

								files->push_back(nana::charset(file));
							}
							if(files->size())
							{
								msg.u.mouse_drop.window = evt.xselection.requestor;
								msg.u.mouse_drop.x = self.xdnd_.pos.x;
								msg.u.mouse_drop.y = self.xdnd_.pos.y;
								msg.u.mouse_drop.files = files;
							}

							accepted = true;
							::XFree(data);
						}
					}
					XEvent respond;
					::memset(respond.xclient.data.l, 0, sizeof(respond.xclient.data.l));
					respond.xclient.display = self.display_;
					respond.xclient.window = self.xdnd_.wd_src;
					respond.xclient.message_type = self.atombase_.xdnd_finished;
					respond.xclient.format = 32;
					respond.xclient.data.l[0] = evt.xselection.requestor;
					if(accepted)
					{
						respond.xclient.data.l[1] = 1;
						respond.xclient.data.l[2] = self.atombase_.xdnd_action_copy;
					}
					::XSendEvent(self.display_, self.xdnd_.wd_src, False, NoEventMask, &respond);

					if(msg.u.mouse_drop.window)
						return 1;	//Use the packet directly.
				}
			}
			return 2;
		}
		else if(SelectionRequest == evt.type)
		{
			auto disp = evt.xselectionrequest.display;
			XEvent respond;

			respond.xselection.property = evt.xselectionrequest.property;
			if(self.atombase_.targets == evt.xselectionrequest.target)
			{
				std::vector<Atom> atoms;
				if(self.selection_.content.utf8_string)
				{
					atoms.push_back(self.atombase_.utf8_string);
					atoms.push_back(XA_STRING);
				}

				::XChangeProperty(self.display_, evt.xselectionrequest.requestor,
						evt.xselectionrequest.property, XA_ATOM, sizeof(Atom) * 8, 0,
						reinterpret_cast<unsigned char*>(atoms.size() ? &atoms[0] : 0), static_cast<int>(atoms.size()));
			}
			else if(XA_STRING == evt.xselectionrequest.target || self.atombase_.utf8_string == evt.xselectionrequest.target)
			{
				std::string str;
				if(self.selection_.content.utf8_string)
					str = *self.selection_.content.utf8_string;

				::XChangeProperty(self.display_, evt.xselectionrequest.requestor, evt.xselectionrequest.property, evt.xselectionrequest.target, 8, 0,
									reinterpret_cast<unsigned char*>(str.size() ? const_cast<std::string::value_type*>(str.c_str()) : 0), static_cast<int>(str.size()));
			}
			else
				respond.xselection.property = None;

			respond.xselection.type = SelectionNotify;
			respond.xselection.display = disp;
			respond.xselection.requestor = evt.xselectionrequest.requestor;
			respond.xselection.selection = evt.xselectionrequest.selection;
			respond.xselection.target = evt.xselectionrequest.target;
			respond.xselection.time = evt.xselectionrequest.time;

			platform_scope_guard psg;
			::XSendEvent(disp, evt.xselectionrequest.requestor, 0, 0, &respond);
			::XFlush(disp);
			return 2;
		}
		else if(ClientMessage == evt.type)
		{
			if(self.atombase_.xdnd_enter == evt.xclient.message_type)
			{
				const Atom * atoms = reinterpret_cast<const Atom*>(&(evt.xclient.data.l[2]));
				unsigned long len = 3;
				unsigned char * data = 0;
				self.xdnd_.wd_src = evt.xclient.data.l[0];

				//Check whether there is more than three types.
				if(evt.xclient.data.l[1] & 1)
				{
					Atom type;
					int format;
					unsigned long bytes_left;
					::XGetWindowProperty(self.display_, self.xdnd_.wd_src, self.atombase_.xdnd_typelist, 0, 0, False,
										XA_ATOM, &type, &format, &len, &bytes_left, &data);

					if(bytes_left > 0)
					{
						::XGetWindowProperty(self.display_, self.xdnd_.wd_src, self.atombase_.xdnd_typelist,
											0, bytes_left, False, XA_ATOM,
											&type, &format, &len, &bytes_left, &data);
						if(XA_ATOM == type && len > 0)
							atoms = reinterpret_cast<const Atom*>(data);
					}
				}

				self.xdnd_.good_type = None;
				for(unsigned long i = 0; i < len; ++i)
				{
					if(atoms[i] == self.atombase_.text_uri_list)
					{
						self.xdnd_.good_type = self.atombase_.text_uri_list;
						break;
					}
				}

				if(data)
					::XFree(data);

				return 2;
			}
			else if(self.atombase_.xdnd_position == evt.xclient.message_type)
			{
				Window wd_src = evt.xclient.data.l[0];
				int x = (evt.xclient.data.l[2] >> 16);
				int y = (evt.xclient.data.l[2] & 0xFFFF);

				bool accepted = false;
				//We have got the type what we want.
				if(self.xdnd_.good_type != None)
				{
					Window child;
					::XTranslateCoordinates(self.display_, self.root_window(), evt.xclient.window, x, y, &self.xdnd_.pos.x, &self.xdnd_.pos.y, &child);
					typedef detail::bedrock bedrock;

					auto wd = bedrock::instance().wd_manager.find_window(reinterpret_cast<native_window_type>(evt.xclient.window),													self.xdnd_.pos.x, self.xdnd_.pos.y);
					if(wd && wd->flags.dropable)
					{
						accepted = true;
						self.xdnd_.timestamp = evt.xclient.data.l[3];
						self.xdnd_.pos = wd->pos_root;
					}
				}

				XEvent respond;
				memset(&respond, 0, sizeof respond);
				respond.xany.type = ClientMessage;
				respond.xany.display = self.display_;
				respond.xclient.window = wd_src;
				respond.xclient.message_type = self.atombase_.xdnd_status;
				respond.xclient.format = 32;

				//Target window
				respond.xclient.data.l[0] = evt.xclient.window;
				//Accept set
				respond.xclient.data.l[1] = (accepted ? 1 : 0);
				respond.xclient.data.l[2] = 0;
				respond.xclient.data.l[3] = 0;
				respond.xclient.data.l[4] = self.atombase_.xdnd_action_copy;

				::XSendEvent(self.display_, wd_src, True, NoEventMask, &respond);
				return 2;
			}
			else if(self.atombase_.xdnd_drop == evt.xclient.message_type)
			{
				::XConvertSelection(self.display_, self.atombase_.xdnd_selection, self.xdnd_.good_type, self.atombase_.xdnd_selection,
									evt.xclient.window, self.xdnd_.timestamp);
				//The XdndDrop should send a XdndFinished to source window.
				//This operation is implemented in SelectionNotify, because
				//XdndFinished should be sent after retrieving data.
				return 2;
			}
		}
		return 0;
	}
}//end namespace detail
}//end namespace nana
