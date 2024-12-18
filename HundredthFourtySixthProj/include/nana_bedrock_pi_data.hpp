#ifndef NANA_DETAIL_BEDROCK_PI_DATA_HPP
#define NANA_DETAIL_BEDROCK_PI_DATA_HPP

#include <nana_push_ignore_diagnostic>

#include <nana_bedrock.hpp>
#include "nana_color_schemes.hpp"
#include "nana_events_operation.hpp"
#include "nana_window_manager.hpp"
#include <set>

namespace nana
{
	namespace detail
	{
		struct bedrock::pi_data
		{
			color_schemes				scheme;
			events_operation			evt_operation;
			window_manager				wd_manager;
			std::set<core_window_t*>	auto_form_set;
			bool shortkey_occurred{ false };

			struct menu_rep
			{
				core_window_t*	taken_window{ nullptr };
				bool			delay_restore{ false };
				native_window_type window{ nullptr };
				native_window_type owner{ nullptr };
				bool	has_keyboard{ false };
			}menu;
		};
	}
}

#include <nana_pop_ignore_diagnostic>

#endif
