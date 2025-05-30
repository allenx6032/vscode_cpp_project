#include <nana_effects.hpp>
#include <nana_programming_interface.hpp>

namespace nana{	namespace gui{
	namespace effects
	{
		bground_interface::~bground_interface()
		{}

		bground_factory_interface::~bground_factory_interface()
		{}

		//Here defines some effect implementations.
		namespace impl
		{
			class transparent
				: public bground_interface
			{
			public:
				transparent(std::size_t percent)
					: fade_rate_( percent <= 100 ? double(percent) / 100.0 : 0)
				{}

				void take_effect(window wd, graph_reference graph) const
				{
					if(fade_rate_ < 0.001)
						return;

					nana::color_t color = API::background(wd);
					graph.blend(graph.size(), color, fade_rate_);
				}
			private:
				const double fade_rate_;
			};

			class blur
				: public bground_interface
			{
			public:
				blur(std::size_t radius)
					:radius_(radius)
				{}

				void take_effect(window, graph_reference graph) const
				{
					graph.blur(graph.size(), radius_);
				}
			private:
				const std::size_t radius_;
			};
		}//end namespace impl
		


		//class bground_transparent
			bground_transparent::bground_transparent(std::size_t percent)
				: percent_(percent)
			{}

			bground_interface* bground_transparent::create() const
			{
				return new impl::transparent(percent_);
			}
		//end class bgroun_transparent

		//class bground_blur
			bground_blur::bground_blur(std::size_t radius)
				: radius_(radius)
			{}

			bground_interface * bground_blur::create() const
			{
				return new impl::blur(radius_);
			}
		//end class bground_blur
	}
}//end namespace gui
}//end namespace nana
