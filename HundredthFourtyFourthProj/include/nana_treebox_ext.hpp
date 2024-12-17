/*
 *	A Tree Box Implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/treebox.hpp
 *	@brief:
 *		The treebox organizes the nodes by a key string. 
 *		The treebox would have a vertical scrollbar if the node
 *	is too many to display. And it does not have a horizontal scrollbar,
 *	the widget will adjust the node's displaying position for fitting.
 */

#ifndef NANA_GUI_WIDGETS_TREEBOX_EXT_HPP
#define NANA_GUI_WIDGETS_TREEBOX_EXT_HPP
#include "nana_treebox.hpp"
#include <nana_scroll.hpp>
#include <nana_element.hpp>
#include <nana_platform.hpp>
#include <stdexcept>

namespace nana
{
	class treebox;

	namespace drawerbase
	{
					class tlwnd_drawer
				: public gui::drawer_trigger, public  gui::drawerbase::treebox::compset_interface
			{
			public:
				typedef drawer_trigger::graph_reference graph_reference;

				tlwnd_drawer()
				{
				}

				void assign(const item_attribute_t & item_attr, const pat::cloneable< gui::treebox::renderer_interface>* renderer, const pat::cloneable< gui::drawerbase::treebox::compset_placer_interface> * compset_placer)
				{
					if(renderer && compset_placer)
					{
						renderer_ = *renderer;
						placer_ = *compset_placer;

						item_attr_ = item_attr;

						_m_draw();
					}
				}
			private:
				void _m_draw()
				{
					item_r_.x = item_r_.y = 0;
					item_r_.width = placer_->item_width(*this->graph_, item_attr_);
					item_r_.height = placer_->item_height(*this->graph_);

					comp_attribute_t attr;
					if(comp_attribute(component::text, attr))
					{
						nana::paint::graphics item_graph(item_r_.width, item_r_.height);
						item_graph.typeface(graph_->typeface());

						renderer_->render(item_graph, widget_->background(), widget_->foreground(), this);

						item_graph.paste(attr.area, *graph_, 1, 1);
						graph_->rectangle(0x0, false);
					}
				}
			private:
				// Implementation of drawer_trigger
				void attached(widget_reference wd, graph_reference graph)
				{
					widget_ = &wd;
					graph_ = &graph;
					graph.typeface(widget_->typeface());
				}
			private:
				// Implementation of compset_interface
				virtual const item_attribute_t& item_attribute() const
				{
					return item_attr_;
				}

				virtual bool comp_attribute(component::t comp, comp_attribute_t& comp_attr) const
				{
					comp_attr.area = item_r_;
					if(placer_->locate(comp, item_attr_, &comp_attr.area))
						return true;

					return false;
				}
			private:
				::nana::paint::graphics * graph_;
				::nana::pat::cloneable<gui::treebox::renderer_interface> renderer_;
				::nana::pat::cloneable<gui::treebox::compset_placer_interface> placer_;
				 gui::widget	*widget_;
				item_attribute_t item_attr_;
				nana::rectangle item_r_;
			};//end class tlwnd_drawer


		namespace treebox
		{
			
			class tooltip_window
				: public gui::widget_object<gui::category::root_tag, tlwnd_drawer>
			{
			public:
				tooltip_window( gui::window wd, const rectangle& r)
					: widget_object<gui::category::root_tag, tlwnd_drawer>(wd, false, rectangle(r).pare_off(-1),  gui::appear::bald< gui::appear::floating>())
				{
					 gui::API::take_active(handle(), false, 0);
				}

				drawer_trigger_t & impl()
				{
					return get_drawer_trigger();
				}
			};//end class tooltip_window

		}//end namespace treebox
	}//end namespace drawerbase
}//end namespace nana
#endif
