/*
 *	Text Token Stream
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/skeletons/text_token_stream.hpp
 */

#ifndef NANA_GUI_WIDGETS_SKELETONS_TEXT_TOKEN_STREAM
#define NANA_GUI_WIDGETS_SKELETONS_TEXT_TOKEN_STREAM

#include <sstream>
#include <deque>
#include <vector>
#include <list>
#include <stack>
#include <stdexcept>

namespace nana{	namespace gui{	namespace widgets{	namespace skeletons
{
	//The tokens are defined for representing a text, the tokens are divided
	//into two parts.
	//Formatted tokens: The tokens present in a format block, they form the format-syntax.
	//Data tokens: The tokens present a text displaying on the screen.
	struct token
	{
		enum t
		{
			tag_begin, tag_end, format_end,
			font, bold, size, color, url, target, image, top, center, bottom, baseline,
			number, string, _true, _false, red, green, blue, white, black, binary, min_limited, max_limited,
			
			equal, comma, backslash,
			data, endl,
			eof
		};
	};
	
	class tokenizer
	{
	public:
		tokenizer(const nana::string& s, bool format_enabled)
			:	iptr_(s.data()),
				endptr_(s.data() + s.size()),
				format_enabled_(format_enabled),
				format_state_(false),
				revert_token_(token::eof)
		{
		}
		
		void push(token::t tk)
		{
			revert_token_ = tk;
		}
		
		//Read the token.
		token::t read()
		{
			if(revert_token_ != token::eof)
			{
				token::t tk = revert_token_;
				revert_token_ = token::eof;
				return tk;
			}
			
			if(iptr_ == endptr_)
				return token::eof;
			
			//Check whether it is a format token.
			if(format_enabled_ && format_state_)
				return _m_format_token();
			
			return _m_token();
		}
		
		const nana::string& idstr() const
		{
			return idstr_;
		}


		const std::pair<nana::string, nana::string>& binary() const
		{
			return binary_;
		}

		std::pair<unsigned, unsigned> binary_number() const
		{
			std::stringstream ss;
			ss<<static_cast<std::string>(nana::charset(binary_.first))<<' '<<static_cast<std::string>(nana::charset(binary_.second));

			std::pair<unsigned, unsigned> r;
			ss>>r.first>>r.second;
			return r;
		}
		
		int number() const
		{
			std::stringstream ss;
			ss<<static_cast<std::string>(nana::charset(idstr_));
			
			//It's a hex number.
			if(idstr_.size() > 2 && idstr_[0] == '0' && (idstr_[1] == 'x' || idstr_[1] == 'X'))
				ss>>std::hex;

			int n;
			ss>>n;
			return n;
		}
	private:
		static bool _m_unicode_word_breakable(nana::char_t ch)
		{
			return ((0x4E00 <= ch) && (ch <= 0x9FFF));
		}

		//Read the data token
		token::t _m_token()
		{
			nana::char_t ch = *iptr_;

			if(ch > 0xFF)
			{
				//This is the Unicode.

				idstr_.clear();
				idstr_.append(1, ch);

				if(_m_unicode_word_breakable(ch))
				{
					++iptr_;
					return token::data;
				}

				ch = *++iptr_;
				while((iptr_ != endptr_) && (ch > 0xFF) && (false == _m_unicode_word_breakable(ch)))
				{
					idstr_.append(1, ch);

					ch = *++iptr_;
				}

				return token::data;
			}

			if('\n' == ch)
			{
				++iptr_;
				return token::endl;
			}
			
			if(('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
			{
				const nana::char_t * idstr = iptr_;
				do
				{
					ch = *(++iptr_);
				}
				while(('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'));

				idstr_.assign(idstr, iptr_);

				return token::data;
			}
			
			if('0' <= ch && ch <= '9')
			{
				_m_read_number();
				return token::data;
			}
			
			if(('<' == ch) && format_enabled_)
			{
				//pos keeps the current position, and it used for restring
				//iptr_ when the search is failed.
				const nana::char_t * pos = ++iptr_;
				_m_eat_whitespace();
				if(*iptr_ == '/')
				{
					++iptr_;
					_m_eat_whitespace();
					if(*iptr_ == '>')
					{
						++iptr_;
						return token::format_end;
					}
				}
				
				//Restore the iptr_;
				iptr_ = pos;
				
				format_state_ = true;
				return token::tag_begin;
			}
			
			//Escape
			if(ch == '\\')
			{
				if(iptr_ + 1 < endptr_)
				{
					ch = *(iptr_ + 1);
					iptr_ += 2;
				}
				else
				{
					iptr_ = endptr_;
					return token::eof;
				}
			}

			++iptr_;
			
			idstr_.clear();
			idstr_.append(1, ch);
			return token::data;
		}
		
		//Read the format token
		token::t _m_format_token()
		{
			_m_eat_whitespace();
			
			nana::char_t ch = *iptr_++;
			
			switch(ch)
			{
			case ',':	return token::comma;
			case '/':	return token::backslash;
			case '=':	return token::equal;
			case '>':
				format_state_ = false;
				return token::tag_end;
			case '"':
				//Here is a string and all the meta characters will be ignored except "
				{
					const nana::char_t * str = iptr_;
					
					while((iptr_ != endptr_) && (*iptr_ != '"'))
						++iptr_;

					idstr_.assign(str, iptr_++);
				}
				return token::string;
			case '(':
				_m_eat_whitespace();
				if((iptr_ < endptr_) && _m_is_idstr_element(*iptr_))
				{
					const nana::char_t * pbegin = iptr_;
					while((iptr_ < endptr_) && _m_is_idstr_element(*iptr_))
						++iptr_;

					binary_.first.assign(pbegin, iptr_);

					_m_eat_whitespace();
					if((iptr_ < endptr_) && (',' == *iptr_))
					{
						++iptr_;
						_m_eat_whitespace();
						if((iptr_ < endptr_) && _m_is_idstr_element(*iptr_))
						{
							pbegin = iptr_;
							while((iptr_ < endptr_) && _m_is_idstr_element(*iptr_))
								++iptr_;

							binary_.second.assign(pbegin, iptr_);

							_m_eat_whitespace();
							if((iptr_ < endptr_) && (')' == *iptr_))
							{
								++iptr_;
								return token::binary;
							}
						}
					}
				}
				return token::eof;
			}
			
			if(('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || '_' == ch)
			{
				--iptr_;
				//Here is a identifier
				_m_read_idstr();
				
				if(STR("font") == idstr_)
					return token::font;
				else if(STR("bold") == idstr_)
					return token::bold;
				else if(STR("size") == idstr_)
					return token::size;
				else if(STR("baseline") == idstr_)
					return token::baseline;
				else if(STR("top") == idstr_)
					return token::top;
				else if(STR("center") == idstr_)
					return token::center;
				else if(STR("bottom") == idstr_)
					return token::bottom;
				else if(STR("color") == idstr_)
					return token::color;
				else if(STR("image") == idstr_)
					return token::image;
				else if(STR("true") == idstr_)
					return token::_true;
				else if(STR("url") == idstr_)
					return token::url;
				else if(STR("target") == idstr_)
					return token::target;
				else if(STR("false") == idstr_)
					return token::_false;
				else if(STR("red") == idstr_)
					return token::red;
				else if(STR("green") == idstr_)
					return token::green;
				else if(STR("blue") == idstr_)
					return token::blue;
				else if(STR("white") == idstr_)
					return token::white;
				else if(STR("black") == idstr_)
					return token::black;
				else if(STR("min_limited") == idstr_)
					return token::min_limited;
				else if(STR("max_limited") == idstr_)
					return token::max_limited;
				
				return token::string;
			}
			
			if('0' <= ch && ch <= '9')
			{
				--iptr_;
				_m_read_number();
				return token::number;
			}

			return token::eof;
		}

		static bool _m_is_idstr_element(nana::char_t ch)
		{
			return (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('_' == ch) || ('0' <= ch && ch <= '9'));
		}
		
		//Read the identifier.
		void _m_read_idstr()
		{
			const nana::char_t * idstr = iptr_;
	
			nana::char_t ch;
			do
			{
				ch = *(++iptr_);
			}
			while(('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('_' == ch) || ('0' <= ch && ch <= '9'));

			idstr_.assign(idstr, iptr_);
		}
		
		//Read the number
		void _m_read_number()
		{
			idstr_.clear();
			
			nana::char_t ch = *iptr_;
			
			idstr_ += ch;
				
			//First check the number whether will be a hex number.
			if('0' == ch)
			{
				ch = *++iptr_;
				if((!('0' <= ch && ch <= '9')) && (ch != 'x' && ch != 'X'))
					return;

				if(ch == 'x' || ch == 'X')
				{
					//Here is a hex number
					idstr_ += 'x';
					ch = *++iptr_;
					while(('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F'))
					{
						idstr_ += ch;
						ch = *++iptr_;
					}
					return;
				}
	
				//Here is not a hex number
				idstr_ += ch;
			}
			
			ch = *++iptr_;
			while('0' <= ch && ch <= '9')
			{
				idstr_ += ch;
				ch = *++iptr_;
			}
		}
		
		void _m_eat_whitespace()
		{
			while(true)
			{
				switch(*iptr_)
				{
				case ' ':
				case '\t':
					++iptr_;
					break;
				default:
					return;
				}
			}
		}
	private:
		const nana::char_t * iptr_;
		const nana::char_t * endptr_;
		const bool	format_enabled_;
		bool	format_state_;
		
		nana::string idstr_;
		std::pair<nana::string, nana::string> binary_;

		std::size_t	whspace_size_;
		
		token::t revert_token_;
	};
	
	//The fblock states a format, and a format from which it is inherted
	struct fblock
	{
		struct aligns
		{
			enum t
			{
				top, center, bottom,
				baseline
			};
		};

		nana::string	font;
		std::size_t		font_size;
		bool	bold;
		bool	bold_empty;	//bold should be ignored if bold_empty is true
		aligns::t	text_align;
		nana::color_t	bgcolor;	//If the color is not specified, it will be ignored, and the system will search for its parent.
		nana::color_t	fgcolor;	//ditto
		
		nana::string	target;
		nana::string	url;
		
		fblock * parent;
	};
	
	//The abstruct data class states a data.
	class data
	{
	public:
		typedef nana::paint::graphics& graph_reference;
		
		virtual ~data(){}
		
		virtual bool	is_text() const = 0;
		virtual bool	is_whitespace() const = 0;
		virtual const nana::string& text() const = 0;
		virtual void measure(graph_reference) = 0;
		virtual void nontext_render(graph_reference, int x, int y) = 0;
		virtual const nana::size & size() const = 0;
		virtual std::size_t ascent() const = 0;
	};
	
	
	class data_text
		: public data
	{
	public:
		data_text(const nana::string& s)
			: str_(s)
		{}
	private:
		virtual bool is_text() const
		{
			return true;
		}

		virtual bool is_whitespace() const
		{
			return false;
		}
		
		virtual const nana::string& text() const
		{
			return str_;
		}
		
		virtual void measure(graph_reference graph)
		{
			size_ = graph.text_extent_size(str_);
			unsigned ascent;
			unsigned descent;
			unsigned internal_leading;
			graph.text_metrics(ascent, descent, internal_leading);
			ascent_ = ascent;
		}

		virtual void nontext_render(graph_reference, int, int)
		{
		}
		
		virtual const nana::size & size() const
		{
			return size_;
		}

		virtual std::size_t ascent() const
		{
			return ascent_;
		}
	private:
		nana::string str_;
		nana::size	size_;
		std::size_t ascent_;
	};
	
	class data_image
		: public data
	{
	public:
		data_image(const nana::string& imgpath, const nana::size & sz, std::size_t limited)
			: image_(imgpath), limited_(limited)
		{
			size_ = image_.size();

			if(sz.width != 0 && sz.height != 0)
			{
				bool make_fit = false;
				switch(limited)
				{
				case 1:
					make_fit = (size_.width < sz.width || size_.height < sz.height);
					break;
				case 2:
					make_fit = (size_.width > sz.width || size_.height > sz.height);
					break;
				}

				if(make_fit)
				{
					nana::size  res;
					nana::gui::fit_zoom(size_, sz, res);
					size_ = res;				
				}
				else
					size_ = sz;
			}
		}
	private:
		//implement data interface
		virtual bool is_text() const
		{
			return false;
		}

		virtual bool is_whitespace() const
		{
			return false;
		}
		
		virtual const nana::string& text() const
		{
			return str_;
		}
		
		virtual void measure(graph_reference)
		{
		}

		virtual void nontext_render(graph_reference graph, int x, int y)
		{
			if(size_ != image_.size())
				image_.stretch(image_.size(), graph, nana::rectangle(x, y, size_.width, size_.height));
			else
				image_.paste(graph, x, y);
		}

		virtual const nana::size & size() const
		{
			return size_;
		}

		virtual std::size_t ascent() const
		{
			return size_.height;
		}
	private:
		nana::string str_;
		nana::paint::image image_;
		nana::size size_;
		std::size_t limited_;
	};
	
	class dstream
	{
		struct value
		{
			fblock * fblock_ptr;
			data * data_ptr;
		};
	public:
		typedef std::list<std::deque<value> >::iterator iterator;
		typedef std::deque<value> linecontainer;

		~dstream()
		{
			close();
		}
		
		void close()
		{
			for(std::list<std::deque<value> >::iterator i = lines_.begin(); i != lines_.end(); ++i)
			{
				for(std::deque<value>::iterator u = i->begin(); u != i->end(); ++u)
					delete u->data_ptr;
			}

			lines_.clear();

			for(std::vector<fblock*>::iterator i = fblocks_.begin(); i != fblocks_.end(); ++i)
				delete (*i);

			fblocks_.clear();
		}
		
		void parse(const nana::string& s, bool format_enabled)
		{
			close();
			
			tokenizer tknizer(s, format_enabled);
			std::stack<fblock*> fstack;
			
			fstack.push(_m_create_default_fblock());
			
			while(true)
			{
				token::t tk = tknizer.read();
				
				switch(tk)
				{
				case token::data:
					_m_data_factory(tk, tknizer.idstr(), fstack.top(), lines_.back());
					break;
				case token::endl:
					lines_.push_back(std::deque<value>());
					break;
				case token::tag_begin:
					_m_parse_format(tknizer, fstack);

					if(attr_image_.path.size())
					{
						_m_data_image(fstack.top(), lines_.back());
						//This fblock just serves the image. So we should restore the pervious fblock
						fstack.pop();	
					}

					break;
				case token::format_end:
					if(fstack.size() > 1)
						fstack.pop();
					break;
				case token::eof:
					return;
				default:
					int * debug = 0;	//for debug.
					*debug = 0;
				}
			}
		}
		
		std::size_t lines() const
		{
			return lines_.size();
		}
		
		iterator begin()
		{
			return lines_.begin();
		}
		
		iterator end()
		{
			return lines_.end();
		}
		
	private:
		void _m_parse_format(tokenizer & tknizer, std::stack<fblock*> & fbstack)
		{
			fblock * fp = _m_inhert_from(fbstack.top());

			attr_image_.reset();
			
			while(true)
			{
				switch(tknizer.read())
				{
				case token::comma:	//Eat the comma, now the comma can be omitted.
					break;
				case token::eof:
				case token::tag_end:
					fblocks_.push_back(fp);
					fbstack.push(fp);
					return;
				case token::font:
					if(token::equal != tknizer.read())
						throw std::runtime_error("");
						
					if(token::string != tknizer.read())
						throw std::runtime_error("");
					
					fp->font = tknizer.idstr();
					break;
				case token::size:
					if(token::equal != tknizer.read())
						throw std::runtime_error("");

					switch(tknizer.read())
					{
					case token::number:
						fp->font_size = tknizer.number();
						break;
					case token::binary:
						{
							std::pair<unsigned, unsigned> value = tknizer.binary_number();
							attr_image_.size.width = value.first;
							attr_image_.size.height = value.second;
						}
						break;
					default:
						throw std::runtime_error("");
					}
					break;
				case token::color:
					if(token::equal != tknizer.read())
						throw std::runtime_error("");
					
					switch(tknizer.read())
					{
					case token::number:
						fp->fgcolor = tknizer.number();
						break;
					case token::red:
						fp->fgcolor = 0xFF0000;
						break;
					case token::green:
						fp->fgcolor = 0xFF00;
						break;
					case token::blue:
						fp->fgcolor = 0xFF;
						break;
					case token::white:
						fp->fgcolor = 0xFFFFFF;
						break;
					case token::black:
						fp->fgcolor = 0x0;
						break;
					default:
						throw std::runtime_error("");
					}				
					break;
				case token::red:	//support the omitting of color. 
					fp->fgcolor = 0xFF0000;
					break;
				case token::green:	//support the omitting of color. 
					fp->fgcolor = 0xFF00;
					break;
				case token::blue:	//support the omitting of color. 
					fp->fgcolor = 0xFF;
					break;
				case token::white:	//support the omitting of color. 
					fp->fgcolor = 0xFFFFFF;
					break;
				case token::black:	//support the omitting of color. 
					fp->fgcolor = 0x0;
					break;
				case token::baseline:
					fp->text_align = fblock::aligns::baseline;
					break;
				case token::top:
					fp->text_align = fblock::aligns::top;
					break;
				case token::center:
					fp->text_align = fblock::aligns::center;
					break;
				case token::bottom:
					fp->text_align = fblock::aligns::bottom;
					break;
				case token::image:
					if(token::equal != tknizer.read())
						throw std::runtime_error("");

					if(token::string != tknizer.read())
						throw std::runtime_error("");

					attr_image_.path = tknizer.idstr();
					break;
				case token::min_limited:
					attr_image_.limited = 1;
					break;
				case token::max_limited:
					attr_image_.limited = 2;
					break;
				case token::target:
					if(token::equal != tknizer.read())
						throw std::runtime_error("error: a '=' is required behind 'target'");
					
					if(token::string != tknizer.read())
						throw std::runtime_error("error: the value of 'target' should be a string");
						
					fp->target = tknizer.idstr();
					break;
				case token::url:
					if(token::equal != tknizer.read())
						throw std::runtime_error("error: a '=' is required behind 'url'");
					
					if(token::string != tknizer.read())
						throw std::runtime_error("error: the value of 'url' should be a string");
						
					fp->url = tknizer.idstr();
					break;
				case token::bold:
					{
						token::t tk = tknizer.read();
						if(token::equal == tk)
						{
							switch(tknizer.read())
							{
							case token::_true:
								fp->bold = true;
								break;
							case token::_false:
								fp->bold = false;
								break;
							default:
								throw std::runtime_error("");
							}
						}
						else
						{
							tknizer.push(tk);
							fp->bold = true;
						}
						fp->bold_empty = false;
					}
					break;
				default:
					throw std::runtime_error("");
				}
			}
		}
		
		fblock* _m_create_default_fblock()
		{
			//Make sure that there is not a fblock is created.
			if(fblocks_.size())
				return fblocks_.front();

			//Create a default fblock.
			fblock * fbp = new fblock;
			
			fbp->font_size = 0xFFFFFFFF;
			fbp->bold = false;
			fbp->bold_empty = true;
			fbp->text_align = fblock::aligns::baseline;
			
			//Refer to the definition for the color specification.
			fbp->bgcolor = 0xFFFFFFFF;
			fbp->fgcolor = 0xFFFFFFFF;
			
			fbp->parent = 0;
					
			fblocks_.push_back(fbp);
			lines_.push_back(std::deque<value>());
			
			return fbp;
		}
		
		fblock * _m_inhert_from(fblock* fp)
		{
			fblock * fbp = new fblock;
			
			fbp->font = fp->font;
			fbp->font_size = fp->font_size;
			fbp->bold = fp->bold;
			fbp->bold_empty = fp->bold_empty;
			fbp->text_align = fp->text_align;

			fbp->bgcolor = fp->bgcolor;
			fbp->fgcolor = fp->fgcolor;
			
			fbp->target = fp->target;
			
			fbp->parent = fp;

			return fbp;
		}
		
		void _m_data_factory(token::t tk, const nana::string& idstr, fblock* fp, std::deque<value>& line)
		{
			value v;
			v.fblock_ptr = fp;
			
			switch(tk)
			{
			case token::data:
				v.data_ptr = new data_text(idstr);
				break;
			default:
				int * debug = 0;	//for debug
				*debug = 0;
			}
			
			line.push_back(v);
		}

		void _m_data_image(fblock* fp, std::deque<value>& line)
		{
			value v;
			v.fblock_ptr = fp;
			
			v.data_ptr = new data_image(attr_image_.path, attr_image_.size, attr_image_.limited);
			
			line.push_back(v);			
		}
		
	private:
		bool format_enabled_;
		std::vector<fblock*> fblocks_;
		std::list<std::deque<value> > lines_;

		struct attr_image_tag
		{
			nana::string	path;
			nana::size		size;
			std::size_t		limited;

			void reset()
			{
				path.clear();
				size.width = size.height = 0;
				limited = 0;
			}
		}attr_image_;
	};
}//end namespace skeletons
}//end namespace widgets
}//end naemspace gui
}//end namepsace nana
#endif	//NANA_GUI_WIDGETS_SKELETONS_TEXT_TOKEN_STREAM
