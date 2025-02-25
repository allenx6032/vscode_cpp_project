#include <nana_filebox.hpp>
#include <nana_fs_utility.hpp>

#if defined(NANA_WINDOWS)
	#include <windows.h>
#elif defined(NANA_LINUX)
	#include <nana_label.hpp>
	#include <nana_button.hpp>
	#include <nana_listbox.hpp>
	#include <nana_categorize.hpp>
	#include <nana_textbox.hpp>
	#include <nana_treebox.hpp>
	#include <nana_combox.hpp>
	#include <nana_file_iterator.hpp>
	#include <nana_place.hpp>
	#include <nana_functional.hpp>
	#include <stdexcept>
#endif

namespace nana{	namespace gui
{
#if defined(NANA_LINUX)
	class filebox_implement
		: public form
	{
		struct item_fs
		{
			nana::string name;
			::tm modified_time;
			bool directory;
			nana::long_long_t bytes;
		};

		class fs_resolver: public listbox::resolver_interface<item_fs>
		{
			static std::ostream& _m_stream2(std::stringstream& ss, unsigned v)
			{
				if(v < 10)	ss<<'0';
				return (ss<<v);
			}

			nana::string decode(std::size_t i, const item_fs& item) const
			{
				switch(i)
				{
				case 0:
					return item.name;
				case 1:
					{
						std::stringstream ss;
						ss<<(item.modified_time.tm_year + 1900)<<'-';
						_m_stream2(ss, item.modified_time.tm_mon + 1)<<'-';
						_m_stream2(ss, item.modified_time.tm_mday)<<' ';

						_m_stream2(ss, item.modified_time.tm_hour)<<':';
						_m_stream2(ss, item.modified_time.tm_min)<<':';
						_m_stream2(ss, item.modified_time.tm_sec);
						return nana::charset(ss.str());
					}
				case 2:
					if(false == item.directory)
					{
						std::size_t pos = item.name.find_last_of(STR('.'));
						if(pos != item.name.npos && (pos + 1 < item.name.size()))
							return item.name.substr(pos + 1);
						return STR("File");
					}
					return STR("Directory");
				case 3:
					if(false == item.directory)
					{
						return _m_trans(item.bytes);
					}
					return nana::string();
				}
				return nana::string();
			}

			void encode(item_fs& item, std::size_t i, const nana::string& s) const
			{
				switch(i)
				{
				case 0:
					item.name = s;
					break;
				case 2:
					item.directory = (s == STR("Directory"));
					break;
				}
			}

			static nana::string _m_trans(std::size_t bytes)
			{
				const nana::char_t * ustr[] = {STR(" KB"), STR(" MB"), STR(" GB"), STR(" TB")};
				std::stringstream ss;
				if(bytes >= 1024)
				{
					double cap = bytes / 1024.0;
					std::size_t uid = 0;
					while((cap >= 1024.0) && (uid < (sizeof(ustr) / sizeof(nana::char_t*))))
					{
						cap /= 1024.0;
						++uid;
					}
					ss<<cap;
					std::string s = ss.str();
					std::size_t pos = s.find('.');
					if(pos != s.npos)
					{
						if(pos + 2 < s.size())
							s.erase(pos + 2);
					}
					return static_cast<nana::string>(nana::charset(s)) + ustr[uid];
				}
				
				ss<<bytes<<" Bytes";
				return nana::charset(ss.str());
			}
		};
		
		struct pred_sort_fs
		{
			bool operator()(const item_fs& a, const item_fs& b) const
			{
				return ((a.directory != b.directory) && a.directory);
			}
		};

		struct comp_sort_by_name
		{
			bool operator()(const nana::string& a, nana::any* fs_a, const nana::string& b, nana::any* fs_b, bool reverse) const
			{
				int dira = fs_a->get<item_fs>()->directory ? 1 : 0;
				int dirb = fs_b->get<item_fs>()->directory ? 1 : 0;
				if(dira != dirb)
					return (reverse ? dira < dirb : dira > dirb);

				std::size_t seek_a = 0;
				std::size_t seek_b = 0;

				while(true)
				{
					std::size_t pos_a = a.find_first_of(STR("0123456789"), seek_a);
					std::size_t pos_b = b.find_first_of(STR("0123456789"), seek_b);

					if((pos_a != a.npos) && (pos_a == pos_b))
					{
						nana::cistring text_a = a.substr(seek_a, pos_a - seek_a).data();
						nana::cistring text_b = b.substr(seek_b, pos_b - seek_b).data();

						if(text_a != text_b)
							return (reverse ? text_a > text_b : text_a < text_b);

						std::size_t end_a = a.find_first_not_of(STR("0123456789"), pos_a + 1);
						std::size_t end_b = b.find_first_not_of(STR("0123456789"), pos_b + 1);
		
						nana::string num_a = a.substr(pos_a, end_a != a.npos ? end_a - pos_a : a.npos);
						nana::string num_b = b.substr(pos_b, end_b != b.npos ? end_b - pos_b : b.npos);
		
						if(num_a != num_b)
						{
							double ai = nana::strtod(num_a.data(), 0);
							double bi = nana::strtod(num_b.data(), 0);
							if(ai != bi)
								return (reverse ? ai > bi : ai < bi);
						}

						seek_a = end_a;
						seek_b = end_b;
					}
					else
						break;
				}
				if(seek_a == a.npos)
					seek_a = 0;
				if(seek_b == b.npos)
					seek_b = 0;

				nana::cistring cia = a.data();
				nana::cistring cib = b.data();
				if(seek_a == seek_b && seek_a == 0)
					return (reverse ? cia > cib : cia < cib);
				return (reverse ? cia.substr(seek_a) > cib.substr(seek_b) : cia.substr(seek_a) < cib.substr(seek_b));		
			}
		};

		struct comp_sort_by_type
		{
			bool operator()(const nana::string& a, nana::any* anyptr_a, const nana::string& b, nana::any* anyptr_b, bool reverse) const
			{
				int dir1 = anyptr_a->get<item_fs>()->directory ? 1 : 0;
				int dir2 = anyptr_b->get<item_fs>()->directory ? 1 : 0;
				if(dir1 != dir2)
					return (reverse ? dir1 < dir2 : dir1 > dir2);

				return (reverse ? a > b : a < b);
			}
		};

		struct comp_sort_by_size
		{
			bool operator()(const nana::string& a, nana::any* anyptr_a, const nana::string& b, nana::any* anyptr_b, bool reverse) const
			{
				item_fs * fsa = anyptr_a->get<item_fs>();
				item_fs * fsb = anyptr_b->get<item_fs>();
				return (reverse ? fsa->bytes > fsb->bytes : fsa->bytes < fsb->bytes);
			}
		};
	public:
		struct kind
		{
			enum t{none, filesystem};
		};

		typedef treebox::item_proxy item_proxy;
	public:

		filebox_implement(window owner, bool io_read, const nana::string& title)
			: form(owner, API::make_center(owner, 630, 440)), io_read_(io_read)
		{
			path_.create(*this);
			path_.splitstr(STR("/"));
			path_.ext_event().selected = nana::make_fun(*this, &filebox_implement::_m_cat_selected);

			filter_.create(*this);
			filter_.multi_lines(false);
			filter_.tip_string(STR("Filter"));
			filter_.make_event<events::key_up>(*this, &filebox_implement::_m_filter_changed);

			btn_folder_.create(*this);
			btn_folder_.caption(STR("&New Folder"));
			btn_folder_.make_event<events::click>(*this, &filebox_implement::_m_new_folder);

			tree_.create(*this);

			ls_file_.create(*this);
			ls_file_.append_header(STR("Name"), 190);
			ls_file_.append_header(STR("Modified"), 145);
			ls_file_.append_header(STR("Type"), 80);
			ls_file_.append_header(STR("Size"), 70);
			ls_file_.make_event<events::dbl_click>(*this, &filebox_implement::_m_sel_file);
			ls_file_.make_event<events::mouse_down>(*this, &filebox_implement::_m_sel_file);
			ls_file_.set_sort_compare(0, comp_sort_by_name());
			ls_file_.set_sort_compare(2, comp_sort_by_type());
			ls_file_.set_sort_compare(3, comp_sort_by_size());

			lb_file_.create(*this);
			lb_file_.caption(STR("File:"));
			
			tb_file_.create(*this);
			tb_file_.multi_lines(false);
			tb_file_.make_event<events::key_char>(*this, &filebox_implement::_m_keychar);

			cb_types_.create(*this);
			cb_types_.editable(false);
			cb_types_.ext_event().selected = nana::make_fun(*this, &filebox_implement::_m_cb_type_selected);

			btn_ok_.create(*this);
			btn_ok_.caption(STR("&OK"));
			btn_ok_.make_event<events::click>(*this, &filebox_implement::_m_ok);
			btn_cancel_.create(*this);
			btn_cancel_.caption(STR("&Cancel"));
			btn_cancel_.make_event<events::click>(destroy(*this));

			selection_.type = kind::none;
			_m_layout();
			_m_init_tree();

			if(0 == title.size())
				caption(io_read ? STR("Open") : STR("Save As"));
			else
				caption(title);
		}

		void def_extension(const nana::string& ext)
		{
			def_ext_ = ext;
		}

		void load_fs(const nana::string& init_path, const nana::string& init_file)
		{
			//Simulate the behavior like Windows7's lpstrInitialDir(http://msdn.microsoft.com/en-us/library/windows/desktop/ms646839%28v=vs.85%29.aspx)

			//Phase 1
			nana::string dir;

			std::size_t pos = init_file.find_last_of(STR("\\/"));
			nana::string file_with_path_removed = (pos != init_file.npos ? init_file.substr(pos + 1) : init_file);

			if(saved_init_path != init_path)
			{
				if(saved_init_path.size() == 0)
					saved_init_path = init_path;

				//Phase 2: Check whether init_file contains a path
				if(file_with_path_removed == init_file)
				{
					//Phase 3: Check whether init_path is empty
					if(init_path.size())
						dir = init_path;
				}
				else
					dir = init_file.substr(0, pos);
			}
			else
				dir = saved_selected_path;

			_m_load_cat_path(dir.size() ? dir : nana::filesystem::path_user());

			tb_file_.caption(file_with_path_removed);
		}

		void add_filter(const nana::string& desc, const nana::string& type)
		{
			std::size_t i = cb_types_.the_number_of_options();
			cb_types_.push_back(desc);
			if(0 == i)
				cb_types_.option(0);

			std::vector<nana::string> v;
			std::size_t beg = 0;
			while(true)
			{
				std::size_t pos = type.find(STR(';'), beg);
				nana::string ext = type.substr(beg, pos == type.npos ? type.npos : pos - beg);
				std::size_t dot = ext.find(STR('.'));
				if((dot != ext.npos) && (dot + 1 < ext.size()))
				{
					ext.erase(0, dot + 1);
					if(ext == STR("*"))
					{
						v.clear();
						break;
					}
					else
						v.push_back(ext);
				}
				if(pos == type.npos)
					break;
				beg = pos + 1;
			}
			if(v.size())
				cb_types_.anyobj(i, v);
		}

		bool file(nana::string& fs) const
		{
			if(selection_.type == kind::none)
				return false;

			std::size_t pos = selection_.target.find_last_of(STR("\\/"));
			if(pos != selection_.target.npos)
				saved_selected_path = selection_.target.substr(0, pos);
			else
				saved_selected_path.clear();

			fs = selection_.target;
			return true;
		}
	private:
		void _m_layout()
		{
			place_.bind(*this);
			place_.div(	"vertical<weight=5>"
					"<weight=24 path><weight=5>"
					"<weight=25 new_folder><weight=5>"
					"<content><weight=8>"
					"<weight=26<weight=100><vertical weight=60 label><file>><weight=8>"
					"<weight=26<><buttons weight=208>><weight=14>");

			place_.field("path")<<5<<path_<<5<<place_.fixed(filter_, 200)<<5;
			place_.field("new_folder")<<10<<place_.fixed(btn_folder_, 100);
			place_.field("content")<<place_.fixed(tree_, 180)<<5<<ls_file_;
			place_.field("label")<<5<<lb_file_;
			place_.field("file")<<5<<tb_file_<<10<<place_.fixed(cb_types_, 190)<<18;
			place_.field("buttons")<<btn_ok_<<14<<btn_cancel_<<18;
			place_.collocate();
		}

		void _m_init_tree()
		{
			using namespace nana::filesystem;

			//The path in linux is starting with the character '/', the name of root key should be
			//"FS.HOME", "FS.ROOT". Because a key of the tree widget should not be '/'
			nodes_.home = tree_.insert(STR("FS.HOME"), STR("Home"));
			nodes_.home.value(kind::filesystem);

			nodes_.filesystem = tree_.insert(STR("FS.ROOT"), STR("Filesystem"));
			nodes_.filesystem.value(kind::filesystem);

			file_iterator end;
			for(nana::filesystem::file_iterator i(path_user()); i != end; ++i)
			{
				if((false == i->directory) || (i->name.size() && i->name[0] == '.')) continue;

				item_proxy node = tree_.insert(nodes_.home, i->name, i->name);
				if(false == node.empty())
				{
					node.value(kind::filesystem);
					break;
				}
			}

			for(file_iterator i(STR("/")); i != end; ++i)
			{
				if((false == i->directory) || (i->name.size() && i->name[0] == '.')) continue;

				item_proxy node = tree_.insert(nodes_.filesystem, i->name, i->name);
				if(false == node.empty())
				{
					node.value(kind::filesystem);
					break;
				}
			}

			tree_.ext_event().expand = nana::make_fun(*this, &filebox_implement::_m_tr_expand);
			tree_.ext_event().selected = nana::make_fun(*this, &filebox_implement::_m_tr_selected);
		}

		nana::string _m_resolute_path(nana::string& path)
		{
			std::size_t pos = path.find(STR('/'));
			if(pos != path.npos)
			{
				nana::string begstr = path.substr(0, pos);
				if(begstr == STR("FS.HOME"))
					path.replace(0, 7, nana::filesystem::path_user());
				else
					path.erase(0, pos);
				return begstr;
			}
			return nana::string();
		}

		void _m_load_path(const nana::string& path)
		{
			addr_.filesystem = path;
			if(addr_.filesystem.size() && addr_.filesystem[addr_.filesystem.size() - 1] != STR('/'))
				addr_.filesystem += STR('/');

			file_container_.clear();

			using namespace nana::filesystem;
			attribute fattr;
			file_iterator end;
			for(file_iterator i(path); i != end; ++i)
			{
				if((i->name.size() == 0) || (i->name[0] == STR('.')))
					continue;
				item_fs m;
				m.name = i->name;
				if(file_attrib(path + m.name, fattr))
				{
					m.bytes = fattr.bytes;
					m.directory = fattr.is_directory;
					m.modified_time = fattr.modified;
				}
				else
				{
					m.bytes = 0;
					m.directory = i->directory;
					modified_file_time(path + i->name, m.modified_time);
				}

				file_container_.push_back(m);

				if(m.directory)
					path_.childset(m.name, 0);
			}
			std::sort(file_container_.begin(), file_container_.end(), pred_sort_fs());
		}

		void _m_load_cat_path(nana::string path)
		{
			if((path.size() == 0) || (path[path.size() - 1] != STR('/')))
				path += STR('/');

			item_proxy beg_node = tree_.selected();
			while(!beg_node.empty() && (beg_node != nodes_.home) && (beg_node != nodes_.filesystem))
				beg_node = beg_node.owner();
			
			nana::string head = nana::filesystem::path_user();
			if(path.size() >= head.size() && (path.substr(0, head.size()) == head))
			{//This is HOME
				path_.caption(STR("HOME"));
				if(beg_node != nodes_.home)
					nodes_.home->select(true);
			}
			else
			{	//Redirect to '/'
				path_.caption(STR("FILESYSTEM"));
				if(beg_node != nodes_.filesystem)
					nodes_.filesystem->select(true);
				head.clear();
			}

			if(head.size() == 0 || head[head.size() - 1] != STR('/'))
				head += STR('/');

			nana::filesystem::file_iterator end;
			for(nana::filesystem::file_iterator i(head); i != end; ++i)
			{
				if(i->directory)
					path_.childset(i->name, 0);
			}
			nana::string cat_path = path_.caption();
			if(cat_path.size() && cat_path[cat_path.size() - 1] != STR('/'))
				cat_path += STR('/');

			std::size_t beg = head.size();
			while(true)
			{
				std::size_t pos = path.find(STR('/'), beg);
				nana::string folder = path.substr(beg, pos != path.npos ? pos - beg: path.npos);
				if(folder.size() == 0) break;
				(cat_path += folder) += STR('/');
				(head += folder) += STR('/');
				path_.caption(cat_path);
				
				for(nana::filesystem::file_iterator i(head); i != end; ++i)
				{
					if(i->directory)
						path_.childset(i->name, 0);
				}

				if(pos == path.npos)
					break;
				beg = pos + 1;
			}
			_m_load_path(path);
			_m_list_fs();
		}

		bool _m_filter_allowed(const nana::string& name, bool is_dir, const nana::string& filter, const std::vector<nana::string>* extension) const
		{
			if(filter.size() && (name.find(filter) == filter.npos))
				return false;

			if((is_dir || 0 == extension) || (0 == extension->size())) return true;

			for(std::vector<nana::string>::const_iterator i = extension->begin(); i != extension->end(); ++i)
			{
				std::size_t pos = name.rfind(*i);
				if((pos != name.npos) && (name.size() == pos + i->size()))
					return true;
			}
			return false;
		}

		void _m_list_fs()
		{
			nana::string filter = filter_.caption();
			ls_file_.auto_draw(false);

			ls_file_.clear();
			ls_file_.resolver(fs_resolver());

			std::vector<nana::string>* ext_types = cb_types_.anyobj<std::vector<nana::string> >(cb_types_.option());

			listbox::cat_proxy cat = ls_file_.at(0);
			for(std::vector<item_fs>::iterator i = file_container_.begin(); i != file_container_.end(); ++i)
			{
				item_fs & fs = *i;
				if(_m_filter_allowed(fs.name, fs.directory, filter, ext_types))
				{
					cat.append(fs).value(fs);
				}
			}
			ls_file_.auto_draw(true);
		}

		void _m_finish(kind::t type, const nana::string& tar)
		{
			selection_.target = tar;
			selection_.type = type;
			close();
		}

		void _m_keychar(const eventinfo& ei)
		{
			if(ei.keyboard.key == nana::gui::keyboard::enter)
				_m_ok();
		}

		struct folder_creator
		{
			folder_creator(filebox_implement& fb, form & fm, textbox& tx)
				:	fb_(fb), fm_(fm), tx_path_(tx)
			{}

			void operator()()
			{
				nana::string path = tx_path_.caption();

				msgbox mb(fm_, STR("Create Folder"));
				mb.icon(msgbox::icon_warning);
				if(0 == path.size() || path[0] == STR('.') || path[0] == STR('/'))
				{
					mb<<STR("Please input a valid name for the new folder.");
					mb();
					return;
				}
				
				bool if_exist;
				if(false == nana::filesystem::mkdir(fb_.addr_.filesystem + path, if_exist))
				{
					if(if_exist)
						mb<<STR("The folder is existing, please rename it.");
					else
						mb<<STR("Failed to create the folder, please rename it.");
					mb();
					return;
				}
				fb_._m_load_cat_path(fb_.addr_.filesystem);
				fm_.close();
			}

			filebox_implement& fb_;
			form& fm_;
			textbox & tx_path_;
		};

		void _m_new_folder()
		{
			form fm(*this, API::make_center(*this, 300, 35));
			fm.caption(STR("Name the new folder"));

			textbox folder(fm, nana::rectangle(5, 5, 160, 25));
			folder.multi_lines(false);

			button btn(fm, nana::rectangle(170, 5, 60, 25));
			btn.caption(STR("Create"));
			btn.make_event<events::click>(folder_creator(*this, fm, folder));

			button btn_cancel(fm, nana::rectangle(235, 5, 60, 25));
			btn_cancel.caption(STR("Cancel"));
			btn_cancel.make_event<events::click>(destroy(fm));
			API::modal_window(fm);
		}

		bool _m_append_def_extension(nana::string& tar) const
		{
			std::size_t dotpos = tar.find_last_of(STR('.'));
			if(dotpos != tar.npos)
			{
				std::size_t pos = tar.find_last_of(STR("/\\"));
				if(pos == tar.npos || pos < dotpos)
					return false;
			}

			if(def_ext_.size())
			{
				if(def_ext_[0] != STR('.'))
					tar += STR('.');
				tar += def_ext_;
			}

			std::vector<nana::string> * exts = cb_types_.anyobj<std::vector<nana::string> >(cb_types_.option());
			if(0 == exts || exts->size() == 0)	return false;
			
			const nana::string & ext = exts->at(0);
			if(def_ext_[0] != STR('.'))
				tar += STR('.');
			tar += ext;
			return true;
		}
	private:
		void _m_sel_file(const eventinfo& ei)
		{
			listbox::selection sel = ls_file_.selected();
			if(sel.size() == 0)
				return;
			listbox::index_pair index = sel[0];

			
			item_fs m;
			ls_file_.at(index).resolve_to(m);

			if(events::dbl_click::identifier == ei.identifier)
			{
				if(m.directory)
					_m_load_cat_path(addr_.filesystem + m.name + STR("/"));
				else
					_m_finish(kind::filesystem, addr_.filesystem + m.name);
			}
			else
			{
				if(false == m.directory)
				{
					selection_.target = addr_.filesystem + m.name;
					tb_file_.caption(m.name);
				}
			}
		}

		void _m_filter_changed()
		{
			_m_list_fs();
		}

		void _m_ok()
		{
			if(0 == selection_.target.size())
			{
				nana::string file = tb_file_.caption();
				if(file.size())
				{
					if(file[0] == STR('.'))
					{
						msgbox mb(*this, caption());
						mb.icon(msgbox::icon_warning);
						mb<<file<<std::endl<<STR("The filename is invalid.");
						mb();
						return;
					}
					nana::string tar;
					if(file[0] == STR('/'))
						tar = file;
					else
						tar = addr_.filesystem + file;

					bool good = true;
					nana::filesystem::attribute attr;
					if(nana::filesystem::file_attrib(tar, attr) == false)
					{
						if(_m_append_def_extension(tar))
							good = nana::filesystem::file_attrib(tar, attr);
						else
							good = false;
					}
					
					if(good && attr.is_directory)
					{
						_m_load_cat_path(tar);
						tb_file_.caption(STR(""));
						return;
					}
					
					if(io_read_)
					{
						if(false == good)
						{
							msgbox mb(*this, caption());
							mb.icon(msgbox::icon_information);
							mb<<STR("The file \"")<<tar<<STR("\"\n is not existing. Please check and retry.");
							mb();	
						}
						else
							_m_finish(kind::filesystem, tar);
					}
					else
					{
						if(good)
						{
							msgbox mb(*this, caption(), msgbox::yes_no);
							mb.icon(msgbox::icon_question);
							mb<<STR("The input file is existing, do you want to overwrite it?");
							if(msgbox::pick_no == mb())
								return;
						}
						_m_finish(kind::filesystem, tar);
					}
				}
			}
			else
				_m_finish(kind::filesystem, selection_.target);
		}

		void _m_tr_expand(window, item_proxy node, bool exp)
		{
			if(false == exp) return;

			if(kind::filesystem == node.value<kind::t>())
			{
				nana::string path = tree_.make_key_path(node, STR("/")) + STR("/");
				_m_resolute_path(path);

				nana::filesystem::file_iterator end;
				for(nana::filesystem::file_iterator i(path); i != end; ++i)
				{
					if((false == i->directory) || (i->name.size() && i->name[0] == '.')) continue;
					item_proxy child = node.append(i->name, i->name, kind::filesystem);
					if(!child.empty())
					{
						for(nana::filesystem::file_iterator u(path + i->name); u != end; ++u)
						{
							if(false == u->directory || (u->name.size() && u->name[0] == '.')) continue;
							
							child.append(u->name, u->name, kind::filesystem);
							break;
						}
					}
				}
			}
		}

		void _m_tr_selected(window, item_proxy node, bool set)
		{
			if(set && (node.value<kind::t>() == kind::filesystem))
			{
				nana::string path = tree_.make_key_path(node, STR("/")) + STR("/");
				_m_resolute_path(path);
				_m_load_cat_path(path);
			}
		}

		void _m_cat_selected(categorize<int>& cat, int&)
		{
			nana::string path = cat.caption();
			std::size_t pos = path.find(STR('/'));
			nana::string root = path.substr(0, pos);
			if(root == STR("HOME"))
				path.replace(0, 4, nana::filesystem::path_user());
			else if(root == STR("FILESYSTEM"))
				path.erase(0, 10);
			else
				throw std::runtime_error("Nana.GUI.Filebox: Wrong categorize path");

			if(path.size() == 0) path = STR("/");
			_m_load_cat_path(path);
		}

		void _m_cb_type_selected(combox& cb)
		{
			_m_list_fs();
		}
	private:
		bool io_read_;
		nana::string def_ext_;

		place	place_;
		categorize<int> path_;
		textbox		filter_;
		button	btn_folder_;
		treebox tree_;
		listbox	ls_file_;

		label lb_file_;
		textbox	tb_file_;
		combox	cb_types_;
		button btn_ok_, btn_cancel_;

		struct tree_node_tag
		{
			item_proxy home;
			item_proxy filesystem;
		}nodes_;

		std::vector<item_fs> file_container_;
		struct path_tag
		{
			nana::string filesystem;
		}addr_;

		struct selection_tag
		{
			kind::t type;
			nana::string target;
		}selection_;
		
		static nana::string saved_init_path;
		static nana::string saved_selected_path;
	};//end class filebox_implement
	nana::string filebox_implement::saved_init_path;
	nana::string filebox_implement::saved_selected_path;

#endif
	//class filebox
		struct filebox::implement
		{
			struct filter
			{
				nana::string des;
				nana::string type;
			};

			window owner;
			bool open_or_save;

			nana::string file;
			nana::string title;
			nana::string path;
			std::vector<filter> filters;
		};

		filebox::filebox(window owner, bool open)
			: impl_(new implement)
		{
			impl_->owner = owner;
			impl_->open_or_save = open;
#if defined(NANA_WINDOWS)
			DWORD len = ::GetCurrentDirectory(0, 0);
			if(len)
			{
				impl_->path.resize(len + 1);
				::GetCurrentDirectory(len, &(impl_->path[0]));
				impl_->path.resize(len);
			}
#endif
		}

		filebox::~filebox()
		{
			delete impl_;
		}

		nana::string filebox::title(nana::string s)
		{
			impl_->title.swap(s);
			return s;
		}

		filebox& filebox::init_path(const nana::string& ipstr)
		{
			nana::filesystem::attribute attr;
			if(nana::filesystem::file_attrib(ipstr, attr))
				if(attr.is_directory)
					impl_->path = ipstr;
			return *this;
		}

		filebox& filebox::init_file(const nana::string& ifstr)
		{
			impl_->file = ifstr;
			return *this;
		}

		filebox& filebox::add_filter(const nana::string& description, const nana::string& filetype)
		{
			implement::filter flt = {description, filetype};
			impl_->filters.push_back(flt);
			return *this;
		}

		nana::string filebox::path() const
		{
			return impl_->path;
		}

		nana::string filebox::file() const
		{
			return impl_->file;
		}

		bool filebox::show() const
		{
#if defined(NANA_WINDOWS)
			if(impl_->file.size() < 520)
				impl_->file.resize(520);

			OPENFILENAME ofn;
			memset(&ofn, 0, sizeof ofn);
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = reinterpret_cast<HWND>(API::root(impl_->owner));
			ofn.lpstrFile = &(impl_->file[0]);
			ofn.nMaxFile = static_cast<DWORD>(impl_->file.size() - 1);

			const nana::char_t * filter;
			nana::string filter_holder;
			if(impl_->filters.size())
			{
				for(std::vector<implement::filter>::iterator i = impl_->filters.begin(); i != impl_->filters.end(); ++i)
				{
					filter_holder += i->des;
					filter_holder += static_cast<nana::string::value_type>('\0');
					nana::string fs = i->type;
					std::size_t pos = 0;
					while(true)
					{
						pos = fs.find(STR(" "), pos);
						if(pos == fs.npos)
							break;
						fs.erase(pos);
					}
					filter_holder += fs;
					filter_holder += static_cast<nana::string::value_type>('\0');
				}
				filter = filter_holder.data();
			}
			else
				filter = STR("ALl Files\0*.*\0");

			ofn.lpstrFilter = filter;
			ofn.lpstrTitle = (impl_->title.size() ? impl_->title.c_str() : 0);
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = (impl_->path.size() ? impl_->path.c_str() : 0);
			if(FALSE == (impl_->open_or_save ? ::GetOpenFileName(&ofn) : ::GetSaveFileName(&ofn)))
				return false;

			impl_->file.resize(nana::strlen(impl_->file.data()));
#elif defined(NANA_LINUX)
			filebox_implement fb(impl_->owner, impl_->open_or_save, impl_->title);

			if(impl_->filters.size())
			{
				for(std::vector<implement::filter>::iterator i = impl_->filters.begin(); i != impl_->filters.end(); ++i)
				{
					nana::string fs = i->type;
					std::size_t pos = 0;
					while(true)
					{
						pos = fs.find(STR(" "), pos);
						if(pos == fs.npos)
							break;
						fs.erase(pos);
					}
					fb.add_filter(i->des, fs);
				}
			}
			else
				fb.add_filter(STR("All Files"), STR("*.*"));

			fb.load_fs(impl_->path, impl_->file);

			API::modal_window(fb);
			if(false == fb.file(impl_->file))
				return false;
#endif
			std::size_t tpos = impl_->file.find_last_of(STR("\\/"));
			if(tpos != impl_->file.npos)
				impl_->path = impl_->file.substr(0, tpos);
			else
				impl_->path.clear();

			return true;
		}//end class filebox
}//end namespace gui
}//end namespace nana

