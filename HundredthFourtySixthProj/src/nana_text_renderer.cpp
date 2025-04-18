
#include "nana_platform_spec_selector.hpp"
#include <nana_text_renderer.hpp>
#include <nana_unicode_bidi.hpp>
#include <nana_native_paint_interface.hpp>

namespace nana
{
	namespace paint
	{
		namespace helper
		{
			template<typename F>
			void for_each_line(const wchar_t * str, std::size_t len, int top, F & f)
			{
				auto const end = str + len;
				for(auto i = str; i != end; ++i)
				{
					if(*i == '\n')
					{
						top += static_cast<int>(f(top, str, i - str));
						str = i + 1;
					}
				}
				if(str != end)
					f(top, str, end - str);
			}

			struct draw_string
			{
				drawable_type dw;
				const int x, endpos;
				align text_align;

				draw_string(drawable_type dw, int x, int endpos, align ta)
					: dw(dw), x(x), endpos(endpos), text_align(ta)
				{}

				unsigned operator()(const int top, const wchar_t * buf, std::size_t bufsize)
				{
					auto const reordered = unicode_reorder(buf, bufsize);
					if (reordered.empty())
						return 0;

					nana::point pos{ x, top };
					unsigned pixels = 0;

					switch(text_align)
					{
					case align::left:
						for(auto & ent : reordered)
						{
							std::size_t len = ent.end - ent.begin;
							nana::size ts = detail::text_extent_size(dw, ent.begin, len);
							if(ts.height > pixels)	pixels = ts.height;
						
							if(pos.x + static_cast<int>(ts.width) > 0)
								detail::draw_string(dw, pos, ent.begin, len);

							pos.x += static_cast<int>(ts.width);
							if(pos.x >= endpos)
								break;
						}
						break;
					case align::center:
						{
							unsigned lenpx = 0;
							std::unique_ptr<unsigned[]> entity_pxs(new unsigned[reordered.size()]);

							auto ent_px = entity_pxs.get();

							for(auto & ent : reordered)
							{
								auto ts = detail::text_extent_size(dw, ent.begin, ent.end - ent.begin);
								if(ts.height > pixels) pixels = ts.height;
								lenpx += ts.width;
								*ent_px++ = ts.width;
							}

							pos.x += (endpos - pos.x - static_cast<int>(lenpx))/2;
							ent_px = entity_pxs.get();

							for(auto & ent : reordered)
							{
								if (pos.x + static_cast<int>(*ent_px) > 0)
									detail::draw_string(dw, pos, ent.begin, ent.end - ent.begin);

								pos.x += static_cast<int>(*ent_px++);

								if(pos.x >= endpos)
									break;
							}
						}
						break;
					case align::right:
						{
							int xend = endpos;
							std::swap(pos.x, xend);
							for(auto i = reordered.crbegin(); i != reordered.crend(); ++i)
							{
								auto & ent = *i;
								std::size_t len = ent.end - ent.begin;
								nana::size ts = detail::text_extent_size(dw, ent.begin, len);
								if(ts.height > pixels)	pixels = ts.height;

								if(pos.x > xend)
								{
									pos.x -= static_cast<int>(ts.width);
									detail::draw_string(dw, pos, i->begin, len);
								}
							
								if(pos.x <= xend || pos.x <= 0)
									break;
							}
						}
						break;
					}
					return pixels;
				}
			};

			struct draw_string_omitted
			{
				graphics & graph;
				int x, endpos;
				unsigned omitted_pixels;

				draw_string_omitted(graphics& graph, int x, int endpos, bool omitted)
					: graph(graph), x(x), endpos(endpos)
				{
					omitted_pixels = (omitted ? graph.text_extent_size("...", 3).width : 0);
					if (endpos - x > static_cast<int>(omitted_pixels))
						this->endpos -= omitted_pixels;
					else
						this->endpos = x;
				}

				unsigned operator()(const int top, const wchar_t * buf, std::size_t bufsize)
				{
					drawable_type dw = graph.handle();
					::nana::point pos{ x, top };
					unsigned pixels = 0;

					auto const reordered = unicode_reorder(buf, bufsize);

					for(auto & i : reordered)
					{
						std::size_t len = i.end - i.begin;
						nana::size ts = detail::text_extent_size(dw, i.begin, len);
						if(ts.height > pixels)	pixels = ts.height;
						
						if(pos.x + static_cast<int>(ts.width) <= endpos)
						{
							detail::draw_string(dw, pos, i.begin, len);
							pos.x += static_cast<int>(ts.width);
						}
						else
						{
							nana::rectangle r;
							r.width = endpos - pos.x;
							r.height = ts.height;

							nana::paint::graphics dum_graph({ r.width, r.height });

							dum_graph.bitblt(r, graph, pos);

							dum_graph.palette(true, graph.palette(true));
							dum_graph.string({}, i.begin, len);

							r.x = pos.x;
							r.y = top;
							graph.bitblt(r, dum_graph);
							if(omitted_pixels)
								detail::draw_string(dw, point{ endpos, top }, L"...", 3);
							break;
						}
					}
					return pixels;	
				}
			};


			struct draw_string_auto_changing_lines
			{
				graphics & graph;
				int x, endpos;
				align text_align;

				draw_string_auto_changing_lines(graphics& graph, int x, int endpos, align ta)
					: graph(graph), x(x), endpos(endpos), text_align(ta)
				{}

				unsigned operator()(const int top, const wchar_t * buf, std::size_t bufsize)
				{
					unsigned pixels = 0;

					auto const dw = graph.handle();
					unsigned str_w = 0;

					std::vector<nana::size> ts_keeper;

					auto const reordered = unicode_reorder(buf, bufsize);

					for(auto & i : reordered)
					{
						nana::size ts = detail::text_extent_size(dw, i.begin, i.end - i.begin);
						if(ts.height > pixels) pixels = ts.height;
						ts_keeper.emplace_back(ts);
						str_w += ts.width;
					}

					//Test whether the text needs the new line.
					if(x + static_cast<int>(str_w) > endpos)
					{
						pixels = 0;
						unsigned line_pixels = 0;
						nana::point pos{ x, top };
						int orig_top = top;
						auto i_ts_keeper = ts_keeper.cbegin();
						for(auto & i : reordered)
						{
							if(line_pixels < i_ts_keeper->height)	
								line_pixels = i_ts_keeper->height;

							bool beyond_edge = (pos.x + static_cast<int>(i_ts_keeper->width) > endpos);
							if(beyond_edge)
							{
								const std::size_t len = i.end - i.begin;
								if(len > 1)
								{
									std::unique_ptr<unsigned[]> pixel_buf(new unsigned[len]);

									//Find the char that should be splitted
									graph.glyph_pixels(i.begin, len, pixel_buf.get());
									std::size_t idx_head = 0, idx_splitted;

									do
									{
										auto pxbuf = pixel_buf.get();

										idx_splitted = find_splitted(idx_head, len, pos.x, endpos, pxbuf);
										if(idx_splitted == len)
										{
											detail::draw_string(dw, pos, i.begin + idx_head, idx_splitted - idx_head);

											for(std::size_t i = idx_head; i < len; ++i)
												pos.x += static_cast<int>(pxbuf[i]);

											break;
										}
										//Check the word whether it is splittable.
										if(splittable(i.begin, idx_splitted))
										{
											detail::draw_string(dw, pos, i.begin + idx_head, idx_splitted - idx_head);
											idx_head = idx_splitted;
											pos.x = x;
											pos.y += static_cast<int>(line_pixels);
											line_pixels = i_ts_keeper->height;
										}
										else
										{
											//Search the splittable character from idx_head to idx_splitted
											const wchar_t * u = i.begin + idx_splitted;
											const wchar_t * head = i.begin + idx_head;

											for(; head < u; --u)
											{
												if(splittable(head, u - head))
													break;
											}

											if(u != head)
											{
												detail::draw_string(dw, pos, head, u - head);
												idx_head += u - head;
												pos.x = x;
												pos.y += static_cast<int>(line_pixels);
												line_pixels = i_ts_keeper->height;
											}
											else
											{
												u = i.begin + idx_splitted;
												const wchar_t * end = i.begin + len;
												for(; u < end; ++u)
												{
													if(splittable(head, u - head))
														break;
												}
												std::size_t splen = u - head;
												pos.y += static_cast<int>(line_pixels);
												pos.x = x;
												detail::draw_string(dw, pos, head, splen);
												line_pixels = i_ts_keeper->height;

												for(std::size_t k = idx_head; k < idx_head + splen; ++k)
													pos.x += static_cast<int>(pxbuf[k]);

												if (pos.x >= endpos)
												{
													pos.x = x;
													pos.y += static_cast<int>(line_pixels);
												}
												idx_head += splen;
											}
										}
									}while(idx_head < len);
								}
								else
								{
									pos.x = x;
									pos.y += static_cast<int>(line_pixels);
									detail::draw_string(dw, pos, i.begin, 1);
									pos.x += static_cast<int>(i_ts_keeper->width);
								}
								line_pixels = 0;
							}
							else
							{
								detail::draw_string(dw, pos, i.begin, i.end - i.begin);
								pos.x += static_cast<int>(i_ts_keeper->width);
							}

							++i_ts_keeper;
						}

						pixels = (top - orig_top) + line_pixels;
					}
					else
					{
						//The text could be drawn in a line.
						if((align::left == text_align) || (align::center == text_align))
						{
							point pos{ x, top };
							if(align::center == text_align)
								pos.x += (endpos - x - static_cast<int>(str_w)) / 2;
							auto i_ts_keeper = ts_keeper.cbegin();
							for(auto & ent : reordered)
							{
								const nana::size & ts = *i_ts_keeper;

								if (pos.x + static_cast<int>(ts.width) > 0)
									detail::draw_string(dw, pos, ent.begin, ent.end - ent.begin);

								pos.x += static_cast<int>(ts.width);
								++i_ts_keeper;
							}					
						}
						else if(align::right == text_align)
						{
							point pos{ endpos, top };
							auto i_ts_keeper = ts_keeper.crbegin();
							for(auto i = reordered.crbegin(); i != reordered.crend(); ++i)
							{
								auto & ent = *i;
								std::size_t len = ent.end - ent.begin;
								const nana::size & ts = *i_ts_keeper;

								pos.x -= static_cast<int>(ts.width);
								if (pos.x >= 0)
									detail::draw_string(dw, pos, ent.begin, len);
								++i_ts_keeper;
							}							
						}
					}
					return pixels;
				}

				static std::size_t find_splitted(std::size_t begin, std::size_t end, int x, int endpos, unsigned * pxbuf)
				{
					unsigned acc_width = 0;
					for(std::size_t i = begin; i < end; ++i)
					{
						if(x + static_cast<int>(acc_width + pxbuf[i]) > endpos)
						{
							if(i == begin)
								++i;
							return i;
						}
						acc_width += pxbuf[i];
					}
					return end;
				}

				static bool splittable(const wchar_t * str, std::size_t index)
				{
					wchar_t ch = str[index];
					if(('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
					{
						wchar_t prch;
						if(index)
						{
							prch = str[index - 1];
							if('0' <= ch && ch <= '9')
								return !(('0' <= prch && prch <= '9') || (str[index - 1] == '-'));

							return (('z' < prch || prch < 'a') && ('Z' < prch || prch < 'A'));
						}
						else
							return false;
					}
					return true;
				}
			};

			struct extent_auto_changing_lines
			{
				graphics & graph;
				int x, endpos;
				unsigned extents;

				extent_auto_changing_lines(graphics& graph, int x, int endpos)
					: graph(graph), x(x), endpos(endpos), extents(0)
				{}

				unsigned operator()(int top, const wchar_t * buf, std::size_t bufsize)
				{
					unsigned pixels = 0;

					drawable_type dw = graph.handle();
					unsigned str_w = 0;
					std::vector<nana::size> ts_keeper;

					auto const reordered = unicode_reorder(buf, bufsize);

					for(auto & i : reordered)
					{
						nana::size ts = detail::text_extent_size(dw, i.begin, i.end - i.begin);
						ts_keeper.emplace_back(ts);
						str_w += ts.width;
					}

					auto i_ts_keeper = ts_keeper.cbegin();
					//Test whether the text needs the new line.
					if(x + static_cast<int>(str_w) > endpos)
					{
						unsigned line_pixels = 0;
						int xpos = x;
						int orig_top = top;

						for(auto & i : reordered)
						{
							if(line_pixels < i_ts_keeper->height)
								line_pixels = i_ts_keeper->height;

							bool beyond_edge = (xpos + static_cast<int>(i_ts_keeper->width) > endpos);
							if(beyond_edge)
							{
								std::size_t len = i.end - i.begin;
								if(len > 1)
								{
									std::unique_ptr<unsigned[]> scope_res(new unsigned[len]);
									auto pxbuf = scope_res.get();
									//Find the char that should be splitted
									graph.glyph_pixels(i.begin, len, pxbuf);
									std::size_t idx_head = 0, idx_splitted;

									do
									{
										idx_splitted = draw_string_auto_changing_lines::find_splitted(idx_head, len, xpos, endpos, pxbuf);

										if(idx_splitted == len)
										{
											for(std::size_t i = idx_head; i < len; ++i)
												xpos += static_cast<int>(pxbuf[i]);
											break;
										}
										//Check the word whether it is splittable.
										if(draw_string_auto_changing_lines::splittable(i.begin, idx_splitted))
										{
											idx_head = idx_splitted;
											xpos = x;
											top += line_pixels;
											line_pixels = i_ts_keeper->height;
										}
										else
										{
											//Search the splittable character from idx_head to idx_splitted
											const wchar_t * u = i.begin + idx_splitted;
											const wchar_t * head = i.begin + idx_head;

											for(; head < u; --u)
											{
												if(draw_string_auto_changing_lines::splittable(head, u - head))
													break;
											}

											if(u != head)
											{
												idx_head += u - head;
												xpos = x;
												top += line_pixels;
												line_pixels = i_ts_keeper->height;
											}
											else
											{
												u = i.begin + idx_splitted;
												const wchar_t * end = i.begin + len;
												for(; u < end; ++u)
												{
													if(draw_string_auto_changing_lines::splittable(head, u - head))
														break;
												}
												std::size_t splen = u - head;
												top += line_pixels;
												xpos = x;
												line_pixels = i_ts_keeper->height;

												for(std::size_t k = idx_head; k < idx_head + splen; ++k)
													xpos += static_cast<int>(pxbuf[k]);
												if(xpos >= endpos)
												{
													xpos = x;
													top += line_pixels;
												}
												idx_head += splen;
											}
										}
									}while(idx_head < len);
								}
								else
									xpos = x + static_cast<int>(i_ts_keeper->width);

								line_pixels = 0;
							}
							else
								xpos += static_cast<int>(i_ts_keeper->width);

							++i_ts_keeper;
						}

						pixels = (top - orig_top) + line_pixels;
					}
					else
					{
						while(i_ts_keeper != ts_keeper.cend())
						{
							const nana::size & ts = *(i_ts_keeper++);
							if(ts.height > pixels) pixels = ts.height;
						}
					}
					extents += pixels;
					return pixels;
				}
			};
		}//end namespace helper

		//class text_renderer
		text_renderer::text_renderer(graph_reference graph, align ta)
			: graph_(graph), text_align_(ta)
		{}

		nana::size text_renderer::extent_size(int x, int y, const wchar_t* str, std::size_t len, unsigned restricted_pixels) const
		{
			nana::size extents;
			if(graph_)
			{
				helper::extent_auto_changing_lines eacl(graph_, x, x + static_cast<int>(restricted_pixels));
				helper::for_each_line(str, len, y, eacl);
				extents.width = restricted_pixels;
				extents.height = eacl.extents;
			}
			return extents;
		}

		void text_renderer::render(const point& pos, const wchar_t * str, std::size_t len)
		{
			if (graph_)
			{
				helper::draw_string ds(graph_.handle(), pos.x, static_cast<int>(graph_.width()), text_align_);
				helper::for_each_line(str, len, pos.y, ds);
			}
		}

		void text_renderer::render(const point& pos, const wchar_t* str, std::size_t len, unsigned restricted_pixels, bool omitted)
		{
			if (graph_)
			{
				helper::draw_string_omitted dso(graph_, pos.x, pos.x + static_cast<int>(restricted_pixels), omitted);
				helper::for_each_line(str, len, pos.y, dso);
			}
		}

		void text_renderer::render(const point& pos, const wchar_t * str, std::size_t len, unsigned restricted_pixels)
		{
			if (graph_)
			{
				helper::draw_string_auto_changing_lines dsacl(graph_, pos.x, pos.x + static_cast<int>(restricted_pixels), text_align_);
				helper::for_each_line(str, len, pos.y, dsacl);
			}
		}
		//end class text_renderer

		//class aligner

		//Constructor
		aligner::aligner(graph_reference graph, align text_align)
			: aligner{ graph, text_align, text_align }
		{}

		aligner::aligner(graph_reference graph, align text_align, align text_align_ex) :
			graph_(graph),
			text_align_(text_align), 
			text_align_ex_(text_align_ex)
		{}

		// Draws a text with specified text alignment.
		void aligner::draw(const std::string& text, point pos, unsigned width)
		{
			draw(to_wstring(text), pos, width);
		}

		void aligner::draw(const std::wstring& text, point pos, unsigned width)
		{
			auto text_px = graph_.text_extent_size(text).width;
			if (text_px <= width)
			{
				switch (text_align_)
				{
				case align::center:
					pos.x += static_cast<int>(width - text_px) / 2;
					break;
				case align::right:
					pos.x += static_cast<int>(width - text_px);
				default:
					break;
				}

				graph_.bidi_string(pos, text.c_str(), text.size());
				return;
			}

			const auto ellipsis = graph_.text_extent_size("...", 3).width;

			std::unique_ptr<unsigned[]> pixels(new unsigned[text.size()]);
			graph_.glyph_pixels(text.c_str(), text.size(), pixels.get());

			std::size_t substr_len = 0;
			unsigned substr_px = 0;

			if (align::right == text_align_ex_)
			{
				auto end = pixels.get();
				auto p = end + text.size();
				do
				{
					--p;
					if (substr_px + *p + ellipsis > width)
					{
						substr_len = p - pixels.get() + 1;
						break;
					}
					substr_px += *p;
				} while (p != end);

				pos.x += static_cast<int>(width - ellipsis - substr_px) + ellipsis;
				graph_.bidi_string(pos, text.c_str() + substr_len, text.size() - substr_len);
				pos.x -= ellipsis;
			}
			else
			{
				for (auto p = pixels.get(), end = pixels.get() + text.size(); p != end; ++p)
				{
					if (substr_px + *p + ellipsis > width)
					{
						substr_len = p - pixels.get();
						break;
					}
					substr_px += *p;
				}

				if (align::center == text_align_ex_)
					pos.x += (width - substr_px - ellipsis) / 2;

				graph_.bidi_string(pos, text.c_str(), substr_len);

				pos.x += substr_px;
			}

			graph_.string(pos, "...");
		}

		//end class string
	}
}
