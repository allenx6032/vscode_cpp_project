/*
 *	A Treebox Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/treebox.cpp
 */
#include <nana_treebox.hpp>
#include <nana_treebox_ext.hpp>
#include <nana_scroll.hpp>
#include <nana_element.hpp>
#include <nana_platform.hpp>
#include <stdexcept>

namespace nana
{
namespace gui
{
	namespace drawerbase
	{
		namespace treebox
		{
			typedef trigger::node_type node_type;

			bool no_sensitive_compare(const nana::string& text, const nana::char_t *pattern, std::size_t len)
			{
				if(len <= text.length())
				{
					const nana::char_t * s = text.c_str();
					for(std::size_t i = 0; i < len; ++i)
					{
						if('a' <= s[i] && s[i] <= 'z')
						{
							if(pattern[i] != s[i] - ('a' - 'A'))
								return false;
						}
						else
							if(pattern[i] != s[i]) return false;
					}
					return true;
				}
				return false;
			}

			const node_type* find_track_child_node(const node_type* node, const node_type * end, const nana::char_t* pattern, std::size_t len, bool &finish)
			{
				if(node->value.second.expanded)
				{
					node = node->child;

					while(node)
					{
						if(no_sensitive_compare(node->value.second.text, pattern, len)) return node;

						if(node == end) break;

						if(node->value.second.expanded)
						{
							const node_type* t = find_track_child_node(node, end, pattern, len, finish);
							if(t || finish)
								return t;
						}
						node = node->next;
					}
				}

				finish = (node && (node == end));
				return 0;
			}

			//class tooltip_window;

			struct pred_allow_child
			{
				bool operator()(const trigger::tree_cont_type::node_type& node)
				{
					return node.value.second.expanded;
				}
			};

			//item_locator should be defined before the definition of basic_implement
			class trigger::item_locator
			{
			public:
				typedef tree_cont_type::node_type node_type;

				item_locator(implement * impl, int item_pos, int x, int y);

				int operator()(node_type &node, int affect);
				node_type * node() const;
				component::t what() const;
				bool item_body() const;
				nana::rectangle text_pos() const;
			private:
				trigger::implement * impl_;
				nana::point item_pos_;
				const nana::point pos_;		//Mouse pointer position
				component::t	what_;
				node_type  *	node_;
				node_attribute node_attr_;
				nana::rectangle node_r_;
				nana::rectangle node_text_r_;
			};	//end class item_locator

			//struct implement
			//@brief:	some data for treebox trigger
			template<typename Renderer>
			struct trigger::basic_implement
			{
				typedef trigger::node_type node_type;

				struct rep_tag
				{
					nana::paint::graphics * graph;
					widget * widget_ptr;
					trigger * trigger_ptr;

					pat::cloneable<compset_placer_interface> comp_placer;
					pat::cloneable<renderer_interface> renderer;
					bool stop_drawing;
				}data;

				struct shape_tag
				{
					nana::upoint border;
					gui::scroll<true> scroll;
					std::size_t	prev_first_value;

					mutable std::map<nana::string, node_image_tag> image_table;

					tree_cont_type::node_type * first;
					int indent_pixels;
					int offset_x;
				}shape;

				struct attribute_tag
				{
					bool auto_draw;
					mutable ext_event_type ext_event;
					tree_cont_type tree_cont;
				}attr;

				struct node_state_tag
				{
					::nana::drawerbase::treebox::tooltip_window * tooltip;
					component::t	comp_pointed;
					tree_cont_type::node_type * pointed;
					tree_cont_type::node_type * selected;
					tree_cont_type::node_type * event_node;
				}node_state;

				struct track_node_tag
				{
					nana::string key_buf;
					std::size_t key_time;
				}track_node;

				struct adjust_tag
				{
					int offset_x_adjust;	//It is a new value of offset_x, and offset_x will be djusted to the new value
					tree_cont_type::node_type * node;
					std::size_t scroll_timestamp;
					gui::timer timer;
				}adjust;
			public:
				basic_implement()
				{
					data.graph			= 0;
					data.widget_ptr		= 0;
					data.stop_drawing	= false;

					shape.prev_first_value = 0;
					shape.first = 0;
					shape.indent_pixels = 10;
					shape.offset_x = 0;

					attr.auto_draw = true;

					node_state.tooltip = 0;
					node_state.comp_pointed = component::end;
					node_state.pointed = 0;
					node_state.selected = 0;
					node_state.event_node = 0;

					track_node.key_time = 0;

					adjust.offset_x_adjust = 0;
					adjust.node = 0;
					adjust.scroll_timestamp = 0;
				}

				void assign_node_attr(node_attribute& ndattr, const node_type* node) const
				{
					ndattr.has_children = (0 != node->child);
					ndattr.expended = node->value.second.expanded;
					ndattr.text = node->value.second.text;
					ndattr.checked = node->value.second.checked;
					ndattr.mouse_pointed = (node_state.pointed == node);
					ndattr.selected = (node_state.selected == node);

					ndattr.icon_hover.close();
					ndattr.icon_normal.close();
					ndattr.icon_expanded.close();
					if(data.comp_placer->enabled(component::icon))
					{
						std::map<nana::string, node_image_tag>::iterator i = shape.image_table.find(node->value.second.img_idstr);
						if(i != shape.image_table.end())
						{
							ndattr.icon_normal = i->second.normal;
							ndattr.icon_expanded = i->second.expanded;
							ndattr.icon_hover = i->second.hovered;
						}
					}
				}

				bool draw(bool scrollbar_react)
				{
					if(data.graph && (false == data.stop_drawing))
					{
						if(scrollbar_react)
							show_scroll();

						//Draw background
						data.graph->rectangle(data.widget_ptr->background(), true);

						//Draw tree
						attr.tree_cont.for_each(shape.first, Renderer(this, nana::point(static_cast<int>(attr.tree_cont.indent_size(shape.first) * shape.indent_pixels) - shape.offset_x, 1)));
						return true;
					}
					return false;
				}

				const trigger::node_type* find_track_node(nana::char_t key)
				{
					nana::string pattern;

					if('a' <= key && key <= 'z') key -= 'a' - 'A';

					unsigned long now = nana::system::timestamp();

					if(now - track_node.key_time > 1000)
						track_node.key_buf.clear();

					if(track_node.key_buf.length())
					{
						if(track_node.key_buf[track_node.key_buf.length() - 1] != key)
							pattern = track_node.key_buf;
					}

					track_node.key_time = now;
					pattern += key;
					track_node.key_buf += key;

					const node_type *begin = node_state.selected ? node_state.selected : attr.tree_cont.get_root()->child;

					if(begin)
					{
						const node_type *node = begin;
						const node_type *end = 0;
						if(pattern.length() == 1)
						{
							if(node->value.second.expanded && node->child)
							{
								node = node->child;
							}
							else if(0 == node->next)
							{
								if(0 == node->owner->next)
								{
									end = begin;
									node = attr.tree_cont.get_root()->child;
								}
								else
									node = node->owner->next;
							}
							else
								node = node->next;
						}

						while(node)
						{
							if(no_sensitive_compare(node->value.second.text, pattern.c_str(), pattern.length())) return node;

							bool finish;
							const node_type *child = find_track_child_node(node, end, pattern.c_str(), pattern.length(), finish);
							if(child)
								return child;

							if(finish || (node == end))
								return 0;

							if(0 == node->next)
							{
								node = (node->owner ? node->owner->next : 0);
								if(0 == node)
								{
									node = attr.tree_cont.get_root()->child;
									end = begin;
								}
							}
							else
								node = node->next;
						}
					}
					return 0;
				}

				static bool check_kinship(const node_type* parent, const node_type * child)
				{
					if(0 == parent || 0 == child)
						return false;

					while(child && (child != parent))
						child = child->owner;

					return (0 != child);
				}

				bool make_adjust(node_type * node, int reason)
				{
					if(0 == node) return false;

					tree_cont_type & tree_container = attr.tree_cont;

					switch(reason)
					{
					case 0:
						//adjust if the node expanded and the number of its children are over the max number allowed
						if(shape.first != node)
						{
							unsigned child_size = tree_container.child_size_if(*node, pred_allow_child());
							const std::size_t max_allow = max_allowed();

							if(child_size < max_allow)
							{
								unsigned off1 = tree_container.distance_if(shape.first, pred_allow_child());
								unsigned off2 = tree_container.distance_if(node, pred_allow_child());
								const unsigned size = off2 - off1 + child_size + 1;
								if(size > max_allow)
									shape.first = tree_container.advance_if(shape.first, size - max_allow, pred_allow_child());
							}
							else
								shape.first = node;
						}
						break;
					case 1:
					case 2:
					case 3:
						//param is the begin pos of an item in absolute.
						{
							int beg = static_cast<int>(tree_container.indent_size(node) * shape.indent_pixels) - shape.offset_x;
							int end = beg + static_cast<int>(node_w_pixels(node));

							bool take_adjust = false;
							if(reason == 1)
								take_adjust = (beg < 0 || (beg > 0 && end > static_cast<int>(visible_w_pixels())));
							else if(reason == 2)
								take_adjust = (beg < 0);
							else if(reason == 3)
								return (beg > 0 && end > static_cast<int>(visible_w_pixels()));

							if(take_adjust)
							{
								adjust.offset_x_adjust = shape.offset_x + beg;
								adjust.node = node;
								adjust.timer.enable(true);
								return true;
							}
						}
						break;
					case 4:
						if(shape.first != node)
						{
							unsigned off_first = tree_container.distance_if(shape.first, pred_allow_child());
							unsigned off_node = tree_container.distance_if(node, pred_allow_child());
							if(off_node < off_first)
							{
								shape.first = node;
								return true;
							}
							else if(off_node - off_first > max_allowed())
							{
								shape.first = tree_container.advance_if(0, off_node - max_allowed() + 1, pred_allow_child());
								return true;
							}
						}
						break;
					}
					return false;
				}

				bool set_checked(node_type * node, checkstate::t cs)
				{
					if(node && node->value.second.checked != cs)
					{
						node->value.second.checked = cs;

						//Don't call the extevent when node is the root node.
						if(node->owner)
						{
							data.stop_drawing = true;
							attr.ext_event.checked(data.widget_ptr->handle(), item_proxy(data.trigger_ptr, node), (checkstate::unchecked != cs));
							data.stop_drawing = false;
						}
						return true;
					}
					return false;
				}

				bool set_selected(node_type * node)
				{
					if(node_state.selected != node)
					{
						data.stop_drawing = true;
						if(node_state.selected)
							attr.ext_event.selected(data.widget_ptr->handle(), item_proxy(data.trigger_ptr, node_state.selected), false);

						node_state.selected = node;
						if(node)
							attr.ext_event.selected(data.widget_ptr->handle(), item_proxy(data.trigger_ptr, node), true);
						data.stop_drawing = false;
						return true;
					}
					return false;
				}

				bool set_expanded(node_type* node, bool value)
				{
					if(node && node->value.second.expanded != value)
					{
						if(value == false)
						{
							//if contracting a parent of the selected node, select the contracted node.
							if(check_kinship(node, node_state.selected))
								set_selected(node);
						}

						node->value.second.expanded = value;
						if(node->child)
						{
							data.stop_drawing = true;
							attr.ext_event.expand(data.widget_ptr->handle(), item_proxy(data.trigger_ptr, node), value);
							data.stop_drawing = false;
						}
						return true;
					}
					return false;
				}

				void show_scroll()
				{
					if(0 == data.graph) return;

					std::size_t max_allow = max_allowed();
					std::size_t visual_items = visual_item_size();

					nana::gui::scroll<true> & scroll = shape.scroll;
					if(visual_items <= max_allow)
					{
						if(false == scroll.empty())
						{
							scroll.close();
							shape.first = 0;
						}
					}
					else
					{
						if(scroll.empty())
						{
							shape.prev_first_value = 0;
							scroll.create(*data.widget_ptr, nana::rectangle(data.graph->width() - 16, 0, 16, data.graph->height()));
							nana::functor<void(const eventinfo&)> scroll_fn = nana::make_fun(*this, &basic_implement::event_scrollbar);
							scroll.make_event<events::mouse_down>(scroll_fn);
							scroll.make_event<events::mouse_move>(scroll_fn);
							scroll.make_event<events::mouse_wheel>(scroll_fn);
						}

						scroll.amount(visual_items);
						scroll.range(max_allow);
					}

					scroll.value(attr.tree_cont.distance_if(shape.first, pred_allow_child()));
				}

				void event_scrollbar(const eventinfo& ei)
				{
					if(ei.identifier == events::mouse_wheel::identifier || ei.mouse.left_button)
					{
						if(shape.prev_first_value != shape.scroll.value())
						{
							shape.prev_first_value = shape.scroll.value();
							adjust.scroll_timestamp = nana::system::timestamp();
							adjust.timer.enable(true);

							shape.first = attr.tree_cont.advance_if(0, shape.prev_first_value, pred_allow_child());

							if(ei.window == shape.scroll.handle())
							{
								draw(false);
								API::update_window(data.widget_ptr->handle());
							}
						}
					}
				}

				std::size_t visual_item_size() const
				{
					return attr.tree_cont.child_size_if(STR(""), pred_allow_child());
				}

				unsigned visible_w_pixels() const
				{
					if(0 == data.graph)
						return 0;

					return data.graph->width() - (shape.scroll.empty() ? 0 : shape.scroll.size().width);
				}

				unsigned node_w_pixels(const node_type *node) const
				{
					node_attribute node_attr;
					assign_node_attr(node_attr, node);
					return data.comp_placer->item_width(*data.graph, node_attr);
				}

				std::size_t max_allowed() const
				{
					return (data.graph->height() / data.comp_placer->item_height(*data.graph));
				}

				nana::paint::image* image(const node_type* node)
				{
					const nana::string& idstr = node->value.second.img_idstr;
					if(idstr.size())
					{
						std::map<std::string, node_image_tag>::iterator i = shape.image_table.find(idstr);
						if(i == shape.image_table.end())
							return 0;

						unsigned long state = static_cast<unsigned long>(-1);
						if(node_state.pointed == node && (node_state.comp_pointed == component::text || node_state.comp_pointed == component::crook || node_state.comp_pointed == component::icon))
							state = (node_state.pointed != node_state.selected ? 0: 1);
						else if(node_state.selected == node)
							state = 2;

						node_image_tag & nodeimg = i->second;
						if(node->value.second.expanded || (state == 1 || state == 2))
							if(nodeimg.expanded.empty() == false)	return &nodeimg.expanded;

						if(node->value.second.expanded == false && state == 0)
							if(nodeimg.hovered.empty() == false)	return &nodeimg.hovered;

						return &nodeimg.normal;
					}
					return 0;
				}

				bool track_mouse(int x, int y)
				{
					int xpos = attr.tree_cont.indent_size(shape.first) * shape.indent_pixels - shape.offset_x;
					item_locator nl(this, xpos, x, y);
					attr.tree_cont.template for_each<item_locator&>(shape.first, nl);

					bool redraw = false;
					node_state.event_node = nl.node();

					if(nl.node() && (nl.what() != component::end))
					{
						if((nl.what() != node_state.comp_pointed || nl.node() != node_state.pointed))
						{
							node_state.comp_pointed = nl.what();
							node_state.pointed = nl.node();
							redraw = (node_state.comp_pointed != component::end);
						}
					}
					else if(node_state.pointed)
					{
						redraw = true;
						node_state.comp_pointed = component::end;
						node_state.pointed = 0;
						close_tooltip_window();
					}

					if(redraw && (node_state.comp_pointed == component::text))
					{
						make_adjust(node_state.pointed, 2);
						adjust.scroll_timestamp = 1;

						show_tooltip_window(node_state.pointed, nl.text_pos());
					}

					return redraw;
				}

				void show_tooltip_window(node_type* node, const rectangle& text_r)
				{
					close_tooltip_window();

					if((0 == node_state.tooltip) && (text_r.x + text_r.width > visible_w_pixels()))
					{
						node_state.tooltip = new ::nana::drawerbase::treebox::tooltip_window(data.widget_ptr->handle(), text_r);
						node_attribute node_attr;
						assign_node_attr(node_attr, node_state.pointed);
						node_state.tooltip->impl().assign(node_attr, &data.renderer, &data.comp_placer);
						node_state.tooltip->show();
						node_state.tooltip->template make_event<events::mouse_leave>(*this, &basic_implement::close_tooltip_window);
						node_state.tooltip->template make_event<events::mouse_move>(*this, &basic_implement::mouse_move_tooltip_window);
						nana::functor<void(const eventinfo&)> click_fn = nana::make_fun(*this, &basic_implement::click_tooltip_window);
						node_state.tooltip->template make_event<events::mouse_down>(click_fn);
						node_state.tooltip->template make_event<events::mouse_up>(click_fn);
						node_state.tooltip->template make_event<events::dbl_click>(click_fn);

						// tooltip_window tw = new tooltip_window(data.widget_ptr->handle(), text_r);
						// node_attribute node_attr;
						// assign_node_attr(node_attr, node_state.pointed);
						// tw->impl().assign(node_attr, &data.renderer, &data.comp_placer);
						// tw->show();
						// tw->template make_event<events::mouse_leave>(*this, &basic_implement::close_tooltip_window);
						// tw->template make_event<events::mouse_move>(*this, &basic_implement::mouse_move_tooltip_window);
						// nana::functor<void(const eventinfo&)> click_fn = nana::make_fun(*this, &basic_implement::click_tooltip_window);
						// tw->template make_event<events::mouse_down>(click_fn);
						// tw->template make_event<events::mouse_up>(click_fn);
						// tw->template make_event<events::dbl_click>(click_fn);
						// node_state.tooltip = tw;
					}
				}

				void close_tooltip_window()
				{
					if(node_state.tooltip)
					{
						::nana::drawerbase::treebox::tooltip_window * x = node_state.tooltip;
						node_state.tooltip = 0;
						delete x;
					}
				}

				void mouse_move_tooltip_window()
				{
					nana::point pos = API::cursor_position();
					API::calc_window_point(data.widget_ptr->handle(), pos);

					if(pos.x >= static_cast<int>(visible_w_pixels()))
						close_tooltip_window();
				}

				void click_tooltip_window(const eventinfo& ei)
				{
					switch(ei.identifier)
					{
					case events::mouse_down::identifier:
						if(make_adjust(node_state.pointed, 1))
							adjust.scroll_timestamp = 1;
						return;
					case events::mouse_up::identifier:
						if(node_state.selected == node_state.pointed)
							return;

						set_selected(node_state.pointed);
						break;
					default:
						set_expanded(node_state.selected, !node_state.selected->value.second.expanded);
					}

					draw(false);
					API::update_window(data.widget_ptr->handle());
				}

				void check_child(node_type * node, bool checked)
				{
					set_checked(node, (checked ? checkstate::checked : checkstate::unchecked));
					node = node->child;
					while(node)
					{
						check_child(node, checked);
						node = node->next;
					}
				}
			}; //end struct trigger::implement;


			//class trigger::item_locator
			trigger::item_locator::item_locator(implement * impl, int item_pos, int x, int y)
				:	impl_(impl),
					item_pos_(item_pos, 1),
					pos_(x, y),
					what_(component::end),
					node_(0)
			{}

			int trigger::item_locator::operator()(node_type &node, int affect)
			{
				implement::shape_tag & node_desc = impl_->shape;

				switch(affect)
				{
				case 0: break;
				case 1: item_pos_.x += static_cast<int>(node_desc.indent_pixels); break;
				default:
					if(affect >= 2)
						item_pos_.x -= static_cast<int>(node_desc.indent_pixels) * (affect - 1);
				}

				impl_->assign_node_attr(node_attr_, &node);
				nana::rectangle node_r;
				pat::cloneable<compset_placer_interface> & comp_placer = impl_->data.comp_placer;

				node_r.width = comp_placer->item_width(*impl_->data.graph, node_attr_);
				node_r.height = comp_placer->item_height(*impl_->data.graph);

				if(pos_.y < item_pos_.y + static_cast<int>(node_r.height))
				{
					int logic_x = pos_.x - item_pos_.x;
					int logic_y = pos_.y - item_pos_.y;

					node_ = &node;

					for(int comp = static_cast<int>(component::begin); comp != static_cast<int>(component::end); ++comp)
					{
						nana::rectangle r = node_r;
						if(comp_placer->locate(static_cast<component::t>(comp), node_attr_, &r))
						{
							if(r.is_hit(logic_x, logic_y))
							{
								what_ = static_cast<component::t>(comp);
								if(component::expender == what_ && (false == node_attr_.has_children))
									what_ = component::end;

								if(component::text == what_)
									node_text_r_ = r;

								return 0;
							}
						}
					}
				}

				item_pos_.y += node_r.height;

				if(node.value.second.expanded && node.child)
					return 1;

				return 2;
			}

			trigger::item_locator::node_type * trigger::item_locator::node() const
			{
				return node_;
			}

			component::t trigger::item_locator::what() const
			{
				return what_;
			}

			bool trigger::item_locator::item_body() const
			{
				return (component::text == what_ || component::icon == what_ || component::bground == what_);
			}

			nana::rectangle trigger::item_locator::text_pos() const
			{
				rectangle r = node_text_r_;
				r.x += item_pos_.x;
				r.y += item_pos_.y;
				return r;
			}
			//end class item_locator

			class trigger::item_renderer
				: public compset_interface
			{
			public:
				typedef tree_cont_type::node_type node_type;

				item_renderer(implement * impl, const nana::point& pos)
					:impl_(impl), pos_(pos)
				{
					bgcolor_ = impl_->data.widget_ptr->background();
					fgcolor_ = impl_->data.widget_ptr->foreground();
				}

				//affect
				//0 = Sibling, the last is a sibling of node
				//1 = Owner, the last is the owner of node
				//>=2 = Children, the last is a child of a node that before this node.
				int operator()(const node_type& node, int affect)
				{
					implement * draw_impl = impl_;

					iterated_node_ = &node;
					switch(affect)
					{
					case 1:
						pos_.x += draw_impl->shape.indent_pixels;
						break;
					default:
						if(affect >= 2)
							pos_.x -= draw_impl->shape.indent_pixels * (affect - 1);
					}

					pat::cloneable<compset_placer_interface> & comp_placer = impl_->data.comp_placer;

					impl_->assign_node_attr(node_attr_, iterated_node_);
					node_r_.x = node_r_.y = 0;
					node_r_.width = comp_placer->item_width(*impl_->data.graph, node_attr_);
					node_r_.height = comp_placer->item_height(*impl_->data.graph);

					draw_impl->data.renderer->render(*draw_impl->data.graph, bgcolor_, fgcolor_, this);

					pos_.y += node_r_.height;

					if(pos_.y > static_cast<int>(draw_impl->data.graph->height()))
						return 0;

					return (node.child && node.value.second.expanded ? 1 : 2);
				}
			private:
				//Overrides compset_interface
				virtual const item_attribute_t& item_attribute() const
				{
					return node_attr_;
				}

				virtual bool comp_attribute(component::t comp, comp_attribute_t& attr) const
				{
					attr.area = node_r_;
					if(impl_->data.comp_placer->locate(comp, node_attr_, &attr.area))
					{
						attr.area.x += pos_.x;
						attr.area.y += pos_.y;
						return true;
					}
					return false;
				}
			private:
				trigger::implement * impl_;
				nana::color_t bgcolor_;
				nana::color_t fgcolor_;
				nana::point pos_;
				const node_type * iterated_node_;
				item_attribute_t node_attr_;
				nana::rectangle node_r_;
			};//end class item_renderer

			//class item_proxy
			item_proxy::item_proxy()
				: trigger_(0), node_(0)
			{}

			item_proxy::item_proxy(trigger* trg, trigger::node_type* node)
				: trigger_(trg), node_(node)
			{
				//Make it an end itertor if one of them is a nullptr
				if(0 == trg || 0 == node)
				{
					trigger_ = 0;
					node_ = 0;
				}
			}

			item_proxy item_proxy::append(const nana::string& key, const nana::string& name)
			{
				if(0 == trigger_ || 0 == node_)
					return item_proxy();
				return item_proxy(trigger_, trigger_->insert(node_, key, name));
			}

			bool item_proxy::empty() const
			{
				return !(trigger_ && node_);
			}

			std::size_t item_proxy::level() const
			{
				if (0 == trigger_ || 0 == node_)
					return 0;

				std::size_t n = 0;
				trigger::node_type* owner = node_->owner;
				while (owner)
				{
					++n;
					owner = owner->owner;
				}
				return n;
			}

			bool item_proxy::checked() const
			{
				return (node_ && (checkstate::checked == node_->value.second.checked));
			}

			item_proxy& item_proxy::check(bool ck)
			{
				trigger_->check(node_, ck ? checkstate::checked : checkstate::unchecked);
				if(trigger_->draw())
					API::update_window(trigger_->impl()->data.widget_ptr->handle());
				return *this;
			}

			bool item_proxy::expended() const
			{
				return (node_ && node_->value.second.expanded);
			}

			item_proxy& item_proxy::expend(bool exp)
			{
				trigger::implement * impl = trigger_->impl();
				if(impl->set_expanded(node_, exp))
				{
					impl->draw(true);
					API::update_window(impl->data.widget_ptr->handle());
				}
				return *this;
			}

			bool item_proxy::selected() const
			{
				return (trigger_->impl()->node_state.selected == node_);
			}

			item_proxy& item_proxy::select(bool s)
			{
				trigger::implement * impl = trigger_->impl();
				if(impl->set_selected(s ? node_ : 0))
				{
					impl->draw(true);
					API::update_window(*impl->data.widget_ptr);
				}
				return *this;
			}

			const nana::string& item_proxy::icon() const
			{
				return node_->value.second.img_idstr;
			}

			item_proxy& item_proxy::icon(const nana::string& id)
			{
				node_->value.second.img_idstr = id;
				return *this;
			}

			item_proxy& item_proxy::key(const nana::string& kstr)
			{
				trigger_->rename(node_, kstr.data(), 0);
				return *this;
			}

			const nana::string& item_proxy::key() const
			{
				return node_->value.first;
			}

			const nana::string& item_proxy::text() const
			{
				return node_->value.second.text;
			}

			item_proxy& item_proxy::text(const nana::string& id)
			{
				trigger_->rename(node_, 0, id.data());
				return *this;
			}

			std::size_t item_proxy::size() const
			{
				std::size_t n = 0;
				for(trigger::node_type * child = node_->child; child; child = child->child)
					++n;

				return n;
			}

			item_proxy item_proxy::child() const
			{
				return item_proxy(trigger_, node_->child);
			}

			item_proxy item_proxy::owner() const
			{
				return item_proxy(trigger_, node_->owner);
			}

			item_proxy item_proxy::sibling() const
			{
				return item_proxy(trigger_, node_->next);
			}

			item_proxy item_proxy::begin() const
			{
				return item_proxy(trigger_, node_->child);
			}

			item_proxy item_proxy::end() const
			{
				return item_proxy(0, 0);
			}

			bool item_proxy::operator==(const nana::string& s) const
			{
				return (node_ && (node_->value.second.text == s));
			}

			bool item_proxy::operator==(const char* s) const
			{
				return (node_ && (node_->value.second.text == nana::string(nana::charset(s))));
			}

			bool item_proxy::operator==(const wchar_t* s) const
			{
				return (node_ && (node_->value.second.text == nana::string(nana::charset(s))));
			}

			// Behavior of Iterator
			item_proxy& item_proxy::operator=(const item_proxy& r)
			{
				if(this != &r)
				{
					trigger_ = r.trigger_;
					node_ = r.node_;
				}
				return *this;
			}

			item_proxy & item_proxy::operator++()
			{
				if(trigger_ && node_)
					node_ = node_->child;

				return *this;
			}

			item_proxy	item_proxy::operator++(int)
			{
				return sibling();
			}

			item_proxy& item_proxy::operator*()
			{
				return *this;
			}

			const item_proxy& item_proxy::operator*() const
			{
				return *this;
			}

			item_proxy* item_proxy::operator->()
			{
				return this;
			}

			const item_proxy* item_proxy::operator->() const
			{
				return this;
			}

			bool item_proxy::operator==(const item_proxy& rhs) const
			{
				if(empty() != rhs.empty())
					return false;

				//Not empty
				if(node_ && trigger_)
					return ((node_ == rhs.node_) && (trigger_ == rhs.trigger_));

				//Both are empty
				return true;
			}

			bool item_proxy::operator!=(const item_proxy& rhs) const
			{
				return !(this->operator==(rhs));
			}

			nana::any& item_proxy::_m_value()
			{
				return node_->value.second.value;
			}

			const nana::any& item_proxy::_m_value() const
			{
				return node_->value.second.value;
			}

			//Undocumentated methods for internal use.
			trigger::node_type * item_proxy::_m_node() const
			{
				return node_;
			}
			//end class item_proxy

			class internal_placer
				: public compset_placer_interface
			{
				static const unsigned item_offset = 16;
				static const unsigned text_offset = 4;
			public:
				internal_placer()
					:	pixels_crook_(0),
						pixels_icon_(0)
				{}
			private:
				//Implement the compset_locator_interface

				virtual void enable(component::t comp, bool enabled)
				{
					switch(comp)
					{
					case component::crook:
						pixels_crook_ = (enabled ? 16 : 0);
						break;
					case component::icon:
						pixels_icon_ = (enabled ? 16 : 0);
						break;
					default:
						break;
					}
				}

				virtual bool enabled(component::t comp) const
				{
					switch(comp)
					{
					case component::crook:
						return (0 != pixels_crook_);
					case component::icon:
						return (0 != pixels_icon_);
					default:
						break;
					}
					return true;
				}

				virtual unsigned item_height(graph_reference graph) const
				{
					return graph.text_extent_size(STR("jH{"), 3).height + 8;
				}

				virtual unsigned item_width(graph_reference graph, const item_attribute_t& attr) const
				{
					return graph.text_extent_size(attr.text).width + pixels_crook_ + pixels_icon_ + (text_offset << 1) + item_offset;
				}

				// Locate a component through the specified coordinate.
				// @param comp the component of the item.
				// @param attr the attribute of the item.
				// @param r the pointer refers to a rectangle for receiving the position and size of the component.
				// @returns the true when the component is located by the locator.
				virtual bool locate(component::t comp, const item_attribute_t& attr, rectangle * r) const
				{
					switch(comp)
					{
					case component::expender:
						if(attr.has_children)
						{
							r->width = item_offset;
							return true;
						}
						return false;
					case component::bground:
						return true;
					case component::crook:
						if(pixels_crook_)
						{
							r->x += item_offset;
							r->width = pixels_crook_;
							return true;
						}
						return false;
					case component::icon:
						if(pixels_icon_)
						{
							r->x += item_offset + pixels_crook_;
							r->y = 2;
							r->width = pixels_icon_;
							r->height -= 2;
							return true;
						}
						return false;
					case component::text:
						{
							unsigned text_pos = item_offset + pixels_crook_ + pixels_icon_ + text_offset;
							r->x += static_cast<int>(text_pos);
							r->width -= (text_pos + text_offset);
						};
						return true;
					default:
						break;
					}
					return false;
				}
			private:
				unsigned pixels_crook_;
				unsigned pixels_icon_;
			};

			class internal_renderer
				: public renderer_interface
			{
				void render(graph_reference graph, nana::color_t bgcolor, nana::color_t fgcolor, const compset_interface * compset) const
				{
					comp_attribute_t attr;

					if(compset->comp_attribute(component::bground, attr))
					{
						const nana::color_t color_table[][2] = {	{0xE8F5FD, 0xD8F0FA}, //highlighted
															{0xC4E8FA, 0xB6E6FB}, //Selected and highlighted
															{0xD5EFFC, 0x99DEFD}  //Selected but not highlighted
														};

						const nana::color_t *colptr = 0;
						if(compset->item_attribute().mouse_pointed)
						{
							if(compset->item_attribute().selected)
								colptr = color_table[1];
							else
								colptr = color_table[0];
						}
						else if(compset->item_attribute().selected)
							colptr = color_table[2];

						if(colptr)
						{
							graph.rectangle(attr.area, colptr[1], false);
							graph.rectangle(attr.area.pare_off(1), *colptr, true);
						}
					}

					if(compset->comp_attribute(component::expender, attr))
					{
						using namespace nana::paint;

						uint32_t style = 1;
						gadget::directions::t dir = gadget::directions::to_southeast;
						if(! compset->item_attribute().expended)
						{
							style = 0;
							dir = gadget::directions::to_east;
						}
						gadget::arrow_16_pixels(graph, attr.area.x, attr.area.y + (attr.area.height - 16) / 2, (attr.mouse_pointed ? 0x1CC4F7 : 0x0), style, dir);
					}

					if(compset->comp_attribute(component::crook, attr))
					{
						attr.area.y += (attr.area.height - 16) / 2;
						crook_.check(compset->item_attribute().checked);
						crook_.draw(graph, bgcolor, fgcolor, attr.area, attr.mouse_pointed ? element_state::hovered : element_state::normal);
					}

					if(compset->comp_attribute(component::icon, attr))
					{
						const nana::paint::image * img = 0;
						if(compset->item_attribute().mouse_pointed)
							img = &(compset->item_attribute().icon_hover);
						else if(compset->item_attribute().expended)
							img = &(compset->item_attribute().icon_expanded);

						if((0 == img) || img->empty())
							img = &(compset->item_attribute().icon_normal);

						if(! img->empty())
						{
							nana::size size = img->size();
							if(size.width > attr.area.width || size.height > attr.area.height)
							{
								nana::size fit_size;
								gui::fit_zoom(size, attr.area, fit_size);

								attr.area.x += (attr.area.width - fit_size.width) / 2;
								attr.area.y += (attr.area.height - fit_size.height) / 2;
								attr.area = fit_size;
								img->stretch(size, graph, attr.area);
							}
							else
								img->paste(graph, attr.area.x + static_cast<int>(attr.area.width - size.width) / 2, attr.area.y + static_cast<int>(attr.area.height - size.height) / 2);
						}
					}

					if(compset->comp_attribute(component::text, attr))
						graph.string(attr.area.x, attr.area.y + 3, fgcolor, compset->item_attribute().text);

				}

			private:
				mutable facade<element::crook> crook_;
			};

		}

		//Treebox Implementation
		namespace treebox
		{


			//class trigger
				//struct treebox_node_type
					trigger::treebox_node_type::treebox_node_type()
						:expanded(false), checked(checkstate::unchecked)
					{}

					trigger::treebox_node_type::treebox_node_type(const nana::string& text)
						:text(text), expanded(false), checked(checkstate::unchecked)
					{}

					trigger::treebox_node_type& trigger::treebox_node_type::operator=(const treebox_node_type& rhs)
					{
						if(this != &rhs)
						{
							text = rhs.text;
							value = rhs.value;
							checked = rhs.checked;
							img_idstr = rhs.img_idstr;
						}
						return *this;
					}
				//end struct treebox_node_type

				trigger::trigger()
					:	impl_(new implement)
				{
					impl_->data.trigger_ptr = this;
					impl_->data.renderer = nana::pat::cloneable<renderer_interface>(internal_renderer());
					impl_->data.comp_placer = nana::pat::cloneable<compset_placer_interface>(internal_placer());
					impl_->adjust.timer.enable(false);
					impl_->adjust.timer.make_tick(nana::make_fun(*this, &trigger::_m_deal_adjust));
					impl_->adjust.timer.interval(10);
				}

				trigger::~trigger()
				{
					delete impl_;
				}

				trigger::implement * trigger::impl() const
				{
					return impl_;
				}

				void trigger::auto_draw(bool ad)
				{
					if(impl_->attr.auto_draw != ad)
					{
						impl_->attr.auto_draw = ad;
						if(ad)
							API::update_window(impl_->data.widget_ptr->handle());
					}
				}

				void trigger::checkable(bool enable)
				{
					pat::cloneable<compset_placer_interface> & comp_placer = impl_->data.comp_placer;

					if(comp_placer->enabled(component::crook) != enable)
					{
						comp_placer->enable(component::crook, enable);
						if(impl_->attr.auto_draw)
						{
							impl_->draw(false);
							API::update_window(impl_->data.widget_ptr->handle());
						}
					}
				}

				bool trigger::checkable() const
				{
					return impl_->data.comp_placer->enabled(component::crook);
				}


				void trigger::check(node_type* node, checkstate::t cs)
				{
					//The ROOT node is not operational and leave the user-node independent.
					if(0 == node->owner)
						return;

					if(checkstate::unchecked != cs)
						cs = checkstate::checked;

					//Return if they are same.
					if(node->value.second.checked == cs)
						return;
					
					//First, check the children of node, it prevents the use of
					//unactualized child nodes during "on_checked".
					node_type * child = node->child;
					while(child)
					{
						impl_->check_child(child, cs != checkstate::unchecked);
						child = child->next;
					}

					//After that, check self.
					impl_->set_checked(node, cs);

					//Then, change the parent node check state
					node_type * owner = node->owner;

					//Make sure that the owner is not the ROOT node.
					while(owner)
					{
						std::size_t len_checked = 0;
						std::size_t size = 0;
						checkstate::t cs = checkstate::unchecked;
						child = owner->child;
						while(child)
						{
							++size;
							if(checkstate::checked == child->value.second.checked)
							{
								++len_checked;
								if(size != len_checked)
								{
									cs = checkstate::partial;
									break;
								}
							}
							else if((checkstate::partial == child->value.second.checked) || (len_checked && (len_checked < size)))
							{
								cs = checkstate::partial;
								break;
							}
							child = child->next;
						}

						if(size && (size == len_checked))
							cs = checkstate::checked;

						if(cs == owner->value.second.checked)
							break;

						impl_->set_checked(owner, cs);
						owner = owner->owner;
					}
				}

				bool trigger::draw()
				{
					if(impl_->attr.auto_draw)
					{
						impl_->draw(false);
						return true;
					}
					return false;
				}

				trigger::tree_cont_type& trigger::tree()
				{
					return impl_->attr.tree_cont;
				}

				const trigger::tree_cont_type& trigger::tree() const
				{
					return impl_->attr.tree_cont;
				}

				const ::nana::pat::cloneable<renderer_interface>& trigger::renderer() const
				{
					return impl_->data.renderer;
				}

				void trigger::renderer(const ::nana::pat::cloneable<renderer_interface>& rd) const
				{
					impl_->data.renderer = rd;
				}

				const ::nana::pat::cloneable<compset_placer_interface>& trigger::placer() const
				{
					return impl_->data.comp_placer;
				}

				void trigger::placer(const ::nana::pat::cloneable<compset_placer_interface>& pd) const
				{
					impl_->data.comp_placer = pd;
				}

				nana::any & trigger::value(node_type* node) const
				{
					if(impl_->attr.tree_cont.verify(node) == false)
						throw std::invalid_argument("Nana.GUI.treebox.value() invalid node");

					return node->value.second.value;
				}

				trigger::node_type* trigger::insert(node_type* node, const nana::string& key, const nana::string& title)
				{
					node_type * p = impl_->attr.tree_cont.node(node, key);
					if(p)
						p->value.second.text = title;
					else
						p = impl_->attr.tree_cont.insert(node, key, treebox_node_type(title));

					if(p && impl_->attr.auto_draw && impl_->draw(true))
						API::update_window(impl_->data.widget_ptr->handle());
					return p;
				}

				trigger::node_type* trigger::insert(const nana::string& path, const nana::string& title)
				{
					node_type* x = impl_->attr.tree_cont.insert(path, treebox_node_type(title));
					if(x && impl_->attr.auto_draw && impl_->draw(true))
						API::update_window(impl_->data.widget_ptr->handle());
					return x;
				}

				bool trigger::verify(const void* node) const
				{
					return impl_->attr.tree_cont.verify(reinterpret_cast<const node_type*>(node));
				}

				bool trigger::verify_kinship(node_type* parent, node_type* child) const
				{
					if(false == (parent && child)) return false;

					while(child && (child != parent))
						child = child->owner;

					return (0 != child);
				}

				void trigger::remove(node_type* node)
				{
					if(false == verify(node))
						return;

					implement::shape_tag & shape = impl_->shape;
					implement::node_state_tag & node_state = impl_->node_state;

					if(verify_kinship(node, node_state.event_node))
						node_state.event_node = 0;

					if(verify_kinship(node, shape.first))
						shape.first = 0;

					if(verify_kinship(node, node_state.selected))
						node_state.selected = 0;

					impl_->attr.tree_cont.remove(node);
				}

				trigger::node_type* trigger::selected() const
				{
					return impl_->node_state.selected;
				}

				void trigger::selected(node_type* node)
				{
					if(impl_->attr.tree_cont.verify(node) && impl_->set_selected(node))
					{
						impl_->draw(true);
						API::update_window(impl_->data.widget_ptr->handle());
					}
				}

				void trigger::set_expand(node_type* node, bool exp)
				{
					if((impl_->data.widget_ptr) && impl_->set_expanded(node, exp))
					{
						impl_->draw(true);
						API::update_window(impl_->data.widget_ptr->handle());
					}
				}

				void trigger::set_expand(const nana::string& path, bool exp)
				{
					if(impl_->set_expanded(impl_->attr.tree_cont.find(path), exp))
					{
						impl_->draw(true);
						API::update_window(impl_->data.widget_ptr->handle());
					}
				}

				node_image_tag& trigger::icon(const nana::string& id) const
				{
					std::map<nana::string, node_image_tag>::iterator i = impl_->shape.image_table.find(id);
					if(i != impl_->shape.image_table.end())
						return i->second;

					impl_->data.comp_placer->enable(component::icon, true);

					return impl_->shape.image_table[id];
				}

				void trigger::icon_erase(const nana::string& id)
				{
					impl_->shape.image_table.erase(id);
					if(0 == impl_->shape.image_table.size())
						impl_->data.comp_placer->enable(component::icon, false);
				}

				void trigger::node_icon(node_type* node, const nana::string& id)
				{
					if(tree().verify(node))
					{
						node->value.second.img_idstr = id;
						std::map<nana::string, node_image_tag>::iterator i = impl_->shape.image_table.find(id);
						if((i != impl_->shape.image_table.end()) && impl_->draw(true))
							API::update_window(impl_->data.widget_ptr->handle());
					}
				}

				unsigned trigger::node_width(const node_type *node) const
				{
					node_attribute node_attr;
					impl_->assign_node_attr(node_attr, node);
					return impl_->data.comp_placer->item_width(*impl_->data.graph, node_attr);
				}

				bool trigger::rename(node_type *node, const nana::char_t* key, const nana::char_t* name)
				{
					if((key || name ) && tree().verify(node))
					{
						if(key && (key != node->value.first))
						{
							node_type * element = node->owner->child;
							for(; element; element = element->next)
							{
								if((element->value.first == key) && (node != element))
									return false;
							}
							node->value.first = key;
						}

						if(name)
							node->value.second.text = name;

						return (key || name);
					}
					return false;

				}

				trigger::ext_event_type& trigger::ext_event() const
				{
					return impl_->attr.ext_event;
				}

				void trigger::attached(widget_reference widget, graph_reference graph)
				{
					impl_->data.widget_ptr = & widget;
					widget.background(0xFFFFFF);
					widget.caption(STR("Nana Treebox"));

					impl_->data.graph = &graph;
					window wd = impl_->data.widget_ptr->handle();
					using namespace API::dev;
					make_drawer_event<events::dbl_click>(wd);
					make_drawer_event<events::mouse_move>(wd);
					make_drawer_event<events::mouse_down>(wd);
					make_drawer_event<events::mouse_up>(wd);
					make_drawer_event<events::mouse_wheel>(wd);
					make_drawer_event<events::size>(wd);
					make_drawer_event<events::key_down>(wd);
					make_drawer_event<events::key_char>(wd);
				}

				void trigger::refresh(graph_reference)
				{
					impl_->draw(false);
				}

				void trigger::dbl_click(graph_reference, const eventinfo& ei)
				{
					implement::shape_tag & shape = impl_->shape;

					int xpos = impl_->attr.tree_cont.indent_size(shape.first) * shape.indent_pixels - shape.offset_x;
					item_locator nl(impl_, xpos, ei.mouse.x, ei.mouse.y);
					impl_->attr.tree_cont.for_each<item_locator&>(shape.first, nl);

					if(nl.node() && (nl.what() == component::text || nl.what() == component::icon))
					{
						impl_->node_state.event_node = nl.node();
						impl_->set_expanded(impl_->node_state.event_node, !impl_->node_state.event_node->value.second.expanded);
						impl_->draw(true);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_down(graph_reference graph, const eventinfo& ei)
				{
					implement::shape_tag & shape = impl_->shape;

					int xpos = impl_->attr.tree_cont.indent_size(shape.first) * shape.indent_pixels - shape.offset_x;
					item_locator nl(impl_, xpos, ei.mouse.x, ei.mouse.y);
					impl_->attr.tree_cont.for_each<item_locator&>(shape.first, nl);

					bool has_redraw = false;

					implement::node_state_tag & node_state = impl_->node_state;
					node_state.event_node = 0;

					if(nl.node())
					{
						node_state.event_node = nl.node();
						if(nl.what() != component::end)
						{
							if(nl.what() ==  component::expender)
							{
								if(impl_->set_expanded(node_state.event_node, !node_state.event_node->value.second.expanded))
									impl_->make_adjust(node_state.event_node, 0);

								has_redraw = true;
							}
							else if(nl.item_body())
							{
								if(node_state.selected != node_state.event_node)
								{
									impl_->set_selected(node_state.event_node);
									has_redraw = true;
								}
							}
						}
						else if(node_state.selected != node_state.event_node)
						{
							impl_->set_selected(node_state.event_node);
							has_redraw = true;
						}
					}

					if(has_redraw)
					{
						impl_->draw(true);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_up(graph_reference, const eventinfo& ei)
				{
					implement::shape_tag & shape = impl_->shape;

					int xpos = impl_->attr.tree_cont.indent_size(shape.first) * shape.indent_pixels - shape.offset_x;
					item_locator nl(impl_, xpos, ei.mouse.x, ei.mouse.y);
					impl_->attr.tree_cont.for_each<item_locator&>(shape.first, nl);

					if(0 == nl.node())
						return;

					if((impl_->node_state.selected != nl.node()) && nl.item_body())
					{
						impl_->set_selected(nl.node());
						if(impl_->make_adjust(impl_->node_state.selected, 1))
							impl_->adjust.scroll_timestamp = 1;

						impl_->draw(true);
						API::lazy_refresh();
					}
					else if(nl.what() == component::crook)
					{
						checkstate::t cs = checkstate::unchecked;
						if(checkstate::unchecked == nl.node()->value.second.checked)
							cs = checkstate::checked;

						check(nl.node(), cs);
						impl_->draw(true);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_move(graph_reference graph, const eventinfo& ei)
				{
					if(impl_->track_mouse(ei.mouse.x, ei.mouse.y))
					{
						impl_->draw(false);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_wheel(graph_reference, const eventinfo& ei)
				{
					implement::shape_tag & shape = impl_->shape;
					std::size_t prev = shape.prev_first_value;

					shape.scroll.make_step(!ei.wheel.upwards);

					impl_->event_scrollbar(ei);

					if(prev != shape.prev_first_value)
					{
						impl_->track_mouse(ei.wheel.x, ei.wheel.y);

						impl_->draw(false);
						API::lazy_refresh();
					}
				}

				void trigger::resize(graph_reference, const eventinfo&)
				{
					impl_->draw(false);
					API::lazy_refresh();
					impl_->show_scroll();
					if(false == impl_->shape.scroll.empty())
					{
						nana::size s = impl_->data.graph->size();
						impl_->shape.scroll.move(s.width - 16, 0, 16, s.height);
					}
				}

				void trigger::key_down(graph_reference, const eventinfo& ei)
				{
					bool redraw = false;
					bool scroll = false; //Adjust the scrollbar

					implement::node_state_tag & node_state = impl_->node_state;

					switch(ei.keyboard.key)
					{
					case keyboard::os_arrow_up:
						if(node_state.selected && node_state.selected != impl_->attr.tree_cont.get_root()->child)
						{
							node_type * prev = node_state.selected->owner;
							if(prev->child != node_state.selected)
							{
								prev = prev->child;
								while(prev->next != node_state.selected)
									prev = prev->next;

								while(prev->child && prev->value.second.expanded)
								{
									prev = prev->child;
									while(prev->next)
										prev = prev->next;
								}
							}

							impl_->set_selected(prev);

							if(impl_->make_adjust(prev, 4))
								scroll = true;

							redraw = true;
						}
						break;
					case keyboard::os_arrow_down:
						if(node_state.selected)
						{
							node_type * node = node_state.selected;
							if(node->value.second.expanded)
							{
								node = node->child;
							}
							else if(node->next)
							{
								node = node->next;
							}
							else
							{
								node = node->owner;
								while(node && (0 == node->next))
									node = node->owner;

								if(node)
									node = node->next;
							}

							if(node)
							{
								impl_->set_selected(node);
								redraw = true;
								if(impl_->make_adjust(node_state.selected, 4))
									scroll = true;
							}
						}
						break;
					case keyboard::os_arrow_left:
						if(node_state.selected)
						{
							if(node_state.selected->value.second.expanded == false)
							{
								if(node_state.selected->owner != impl_->attr.tree_cont.get_root())
								{
									impl_->set_selected(node_state.selected->owner);
									impl_->make_adjust(node_state.selected, 4);
								}
							}
							else
								impl_->set_expanded(node_state.selected, false);

							redraw = true;
							scroll = true;
						}
						break;
					case keyboard::os_arrow_right:
						if(node_state.selected)
						{
							if(node_state.selected->value.second.expanded == false)
							{
								impl_->set_expanded(node_state.selected, true);
								redraw = true;
								scroll = true;
							}
							else if(node_state.selected->child)
							{
								impl_->set_selected(node_state.selected->child);
								impl_->make_adjust(node_state.selected, 4);
								redraw = true;
								scroll = true;
							}
						}
						break;
					}

					if(redraw)
					{
						impl_->draw(scroll);
						API::lazy_refresh();
					}
				}

				void trigger::key_char(graph_reference, const eventinfo& ei)
				{
					node_type* node = const_cast<node_type*>(impl_->find_track_node(ei.keyboard.key));

					if(node && (node != impl_->node_state.selected))
					{
						impl_->set_selected(node);
						impl_->make_adjust(node, 4);
						impl_->draw(false);
						API::lazy_refresh();
					}
				}

				void trigger::_m_deal_adjust()
				{
					implement::adjust_tag & adjust = impl_->adjust;
					if(adjust.scroll_timestamp && (nana::system::timestamp() - adjust.scroll_timestamp >= 500))
					{
						if(adjust.offset_x_adjust == 0)
						{
							if(false == impl_->make_adjust(adjust.node ? adjust.node : impl_->shape.first, 1))
							{
								adjust.offset_x_adjust = 0;
								adjust.node = 0;
								adjust.scroll_timestamp = 0;
								adjust.timer.enable(false);
								return;
							}
						}

						implement::shape_tag & shape = impl_->shape;
						const int delta = 5;
						int old = shape.offset_x;
						if(shape.offset_x < adjust.offset_x_adjust)
						{
							shape.offset_x += delta;
							if(shape.offset_x > adjust.offset_x_adjust)
								shape.offset_x = adjust.offset_x_adjust;
						}
						else if(shape.offset_x > adjust.offset_x_adjust)
						{
							shape.offset_x -= delta;
							if(shape.offset_x < adjust.offset_x_adjust)
								shape.offset_x = adjust.offset_x_adjust;
						}

						impl_->draw(false);
						API::update_window(impl_->data.widget_ptr->handle());

						if(impl_->node_state.tooltip)
						{
							nana::point pos = impl_->node_state.tooltip->pos();
							pos.x -= (shape.offset_x - old);
							impl_->node_state.tooltip->move(pos.x, pos.y);
						}

						if(shape.offset_x == adjust.offset_x_adjust)
						{
							adjust.offset_x_adjust = 0;
							adjust.node = 0;
							adjust.scroll_timestamp = 0;
							adjust.timer.enable(false);
						}
					}
				}
			//end class trigger
		}//end namespace treebox
	}//end namespace drawerbase

	//class treebox
		treebox::treebox(){}

		treebox::treebox(window wd, bool visible)
		{
			create(wd, rectangle(), visible);
		}

		treebox::treebox(window wd, const rectangle& r, bool visible)
		{
			create(wd, r, visible);
		}

		const nana::pat::cloneable<treebox::renderer_interface> & treebox::renderer() const
		{
			return get_drawer_trigger().impl()->data.renderer;
		}

		const nana::pat::cloneable<treebox::compset_placer_interface> & treebox::placer() const
		{
			return get_drawer_trigger().impl()->data.comp_placer;
		}

		void treebox::auto_draw(bool ad)
		{
			get_drawer_trigger().auto_draw(ad);
		}

		treebox & treebox::checkable(bool enable)
		{
			get_drawer_trigger().checkable(enable);
			return *this;
		}

		bool treebox::checkable() const
		{
			return get_drawer_trigger().checkable();
		}

		treebox::ext_event_type& treebox::ext_event() const
		{
			return get_drawer_trigger().ext_event();
		}

		treebox::node_image_type& treebox::icon(const nana::string& id) const
		{
			return get_drawer_trigger().icon(id);
		}

		void treebox::icon_erase(const nana::string& id)
		{
			get_drawer_trigger().icon_erase(id);
		}

		treebox::item_proxy treebox::find(const nana::string& keypath)
		{
			drawer_trigger_t * trg = &get_drawer_trigger();
			return item_proxy(trg, trg->tree().find(keypath));
		}

		treebox::item_proxy treebox::insert(const nana::string& path_key, const nana::string& title)
		{
			return item_proxy(&get_drawer_trigger(), get_drawer_trigger().insert(path_key, title));
		}

		treebox::item_proxy treebox::insert(item_proxy i, const nana::string& key, const nana::string& title)
		{
			return item_proxy(&get_drawer_trigger(), get_drawer_trigger().insert(i._m_node(), key, title));
		}

		treebox::item_proxy treebox::erase(item_proxy i)
		{
			item_proxy next = i.sibling();
			get_drawer_trigger().remove(i._m_node());
			return next;
		}

		void treebox::erase(const nana::string& keypath)
		{
			item_proxy i = find(keypath);
			if(false == i.empty())
				get_drawer_trigger().remove(i._m_node());
		}

		nana::string treebox::make_key_path(item_proxy i, const nana::string& splitter) const
		{
			typedef drawer_trigger_t::tree_cont_type container;
			const container & tree = get_drawer_trigger().tree();
			container::node_type* pnode = i._m_node();
			if(tree.verify(pnode))
			{
				container::node_type* root = tree.get_root();
				nana::string path;
				nana::string temp;
				while(pnode->owner != root)
				{
					temp = splitter;
					temp += pnode->value.first;
					path.insert(0, temp);
					pnode = pnode->owner;
				}

				path.insert(0, pnode->value.first);
				return path;
			}
			return nana::string();
		}

		treebox::item_proxy treebox::selected() const
		{
			return item_proxy(const_cast<drawer_trigger_t*>(&get_drawer_trigger()), get_drawer_trigger().selected());
		}
	//end class treebox
}//end namespace gui
}//end namespace nana

