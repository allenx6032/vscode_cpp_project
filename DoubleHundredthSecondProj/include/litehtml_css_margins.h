#ifndef LH_CSS_MARGINS_H
#define LH_CSS_MARGINS_H

#include "litehtml_css_length.h"

namespace litehtml
{
	struct css_margins
	{
		css_length	left;
		css_length	right;
		css_length	top;
		css_length	bottom;

		css_margins() = default;

		css_margins(const css_margins& val)
		{
			left	= val.left;
			right	= val.right;
			top		= val.top;
			bottom	= val.bottom;
		}

		css_margins& operator=(const css_margins& val) = default;

		string to_string() const
		{
			return	"left: " + left.to_string() +
					", right: " + right.to_string() +
					", top: " + top.to_string() +
					", bottom: " + bottom.to_string();
		}
	};
}

#endif  // LH_CSS_MARGINS_H
