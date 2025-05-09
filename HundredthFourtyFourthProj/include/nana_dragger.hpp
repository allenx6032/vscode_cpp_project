#ifndef NANA_GUI_DRAGGER_HPP
#define NANA_GUI_DRAGGER_HPP
#include "nana_programming_interface.hpp"

namespace nana{ namespace gui
{
	class dragger
		: nana::noncopyable
	{
		class dragger_impl_t;
	public:
		dragger();
		~dragger();
		void target(window);
		void target(window, const rectangle& restrict_area, nana::arrange);
		void remove_target(window);
		void trigger(window);
	private:
		dragger_impl_t * impl_;
	};
}//end namespace gui
}//end namespace nana
#endif
