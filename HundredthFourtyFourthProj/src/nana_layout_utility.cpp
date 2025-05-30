/*
 *	Utility Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/layout_utility.hpp
 *
 *
 */
#include <nana_layout_utility.hpp>

namespace nana
{
namespace gui
{
	//overlap test if overlaped between r1 and r2
	bool overlap(const rectangle& r1, const rectangle& r2)
	{
		if(r1.y + int(r1.height) <= r2.y) return false;
		if(r1.y >= int(r2.y + r2.height)) return false;

		if(int(r1.x + r1.width) <= r2.x) return false;
		if(r1.x >= int(r2.x + r2.width)) return false;

		return true;
	}

	//overlap, compute the overlap area between r1 and r2. the rect is for root
	bool overlap(const rectangle& r1,
						const rectangle& r2,
						rectangle& r)
	{
		if(overlap(r1, r2))
		{
			r.x = r1.x < r2.x ? r2.x : r1.x;
			r.y = r1.y < r2.y ? r2.y : r1.y;

			long_long_t li1 = long_long_t(r1.x) + r1.width;
			long_long_t li2 = long_long_t(r2.x) + r2.width;
			r.width = static_cast<unsigned>(li1 < li2 ? li1 - r.x: li2 - r.x);

			li1 = long_long_t(r1.y) + r1.height;
			li2 = long_long_t(r2.y) + r2.height;
			r.height = static_cast<unsigned>(li1 < li2 ? li1 - r.y: li2 - r.y);
			return true;
		}
		return false;
	}

	bool overlap(const rectangle& ir, const size& valid_input_area, const rectangle & dr, const size& valid_dst_area, rectangle& op_ir, rectangle& op_dr)
	{
		if(overlap(ir, valid_input_area, op_ir) == false)
			return false;

		rectangle good_dr;
		if(overlap(dr, valid_dst_area, good_dr) == false)
			return false;

		zoom(ir, op_ir, dr, op_dr);

		if(false == covered(op_dr, good_dr))
		{
			op_dr = good_dr;
			zoom(dr, good_dr, ir, op_ir);
		}
		return true;
	}

	bool intersection(const rectangle & r, point pos_beg, point pos_end, point& good_pos_beg, point& good_pos_end)
	{
		const int right = r.x + static_cast<int>(r.width);
		const int bottom = r.y + static_cast<int>(r.height);

		if(pos_beg.x > pos_end.x)
		{
			int t = pos_end.x;
			pos_end.x = pos_beg.x;
			pos_beg.x = t;

			t = pos_end.y;
			pos_end.y = pos_beg.y;
			pos_beg.y = t;
		}

		bool good_beg = (0 <= pos_beg.x && pos_beg.x < right && 0 <= pos_beg.y && pos_beg.y < bottom);
		bool good_end = (0 <= pos_end.x && pos_end.x < right && 0 <= pos_end.y && pos_end.y < bottom);


		if(good_beg && good_end)
		{
			good_pos_beg = pos_beg;
			good_pos_end = pos_end;
			return true;
		}
		else if(pos_beg.x == pos_end.x)
		{
			if(r.x <= pos_beg.x && pos_beg.x < right)
			{
				if(pos_beg.y < r.y)
				{
					if(pos_end.y < r.y)
						return false;
					good_pos_beg.y = r.y;
					good_pos_end.y = (pos_end.y < bottom ? pos_end.y  : bottom - 1);
				}
				else if(pos_beg.y >= bottom)
				{
					if(pos_end.y >= bottom)
						return false;
					good_pos_beg.y = bottom - 1;
					good_pos_end.y = (pos_end.y < r.y ? r.y : pos_end.y);
				}

				good_pos_beg.x = good_pos_end.x = r.x;
				return true;
			}
			return false;
		}
		else if(pos_beg.y == pos_end.y)
		{
			if(r.y <= pos_beg.y && pos_beg.y < bottom)
			{
				if(pos_beg.x < r.x)
				{
					if(pos_end.x < r.x)
						return false;
					good_pos_beg.x = r.x;
					good_pos_end.x = (pos_end.x < right ? pos_end.x : right - 1);
				}
				else if(pos_beg.x >= right)
				{
					if(pos_end.x >= right)
						return false;

					good_pos_beg.x = right - 1;
					good_pos_end.x = (pos_end.x < r.x ? r.x : pos_end.x);
				}
				good_pos_beg.y = good_pos_end.y = r.y;
				return true;
			}
			return false;
		}

		double m = (pos_end.y - pos_beg.y ) / double(pos_end.x - pos_beg.x);
		bool is_nw_to_se = (m >= 0.0);
		//The formulas for the line.
		//y = m * (x - pos_beg.x) + pos_beg.y
		//x = (y - pos_beg.y) / m + pos_beg.x
		if(!good_beg)
		{
			good_pos_beg.y = static_cast<int>(m * (r.x - pos_beg.x)) + pos_beg.y;
			if(r.y <= good_pos_beg.y && good_pos_beg.y < bottom)
			{
				good_pos_beg.x = r.x;
			}
			else
			{
				bool cond;
				int y;
				if(is_nw_to_se)
				{
					y = r.y;
					cond = good_pos_beg.y < y;
				}
				else
				{
					y = bottom - 1;
					cond = good_pos_beg.y > y;
				}

				if(cond)
				{
					good_pos_beg.x = static_cast<int>((y - pos_beg.y) / m) + pos_beg.x;
					if(r.x <= good_pos_beg.x && good_pos_beg.x < right)
						good_pos_beg.y = y;
					else
						return false;
				}
				else
					return false;
			}

			if(good_pos_beg.x < pos_beg.x)
				return false;
		}
		else
			good_pos_beg = pos_beg;

		if(!good_end)
		{
			good_pos_end.y = static_cast<int>(m * (right - 1 - pos_beg.x)) + pos_beg.y;
			if(r.y <= good_pos_end.y && good_pos_end.y < bottom)
			{
				good_pos_end.x = right - 1;
			}
			else
			{
				bool cond;
				int y;
				if(is_nw_to_se)
				{
					y = bottom - 1;
					cond = good_pos_end.y > y;
				}
				else
				{
					y = r.y;
					cond = good_pos_end.y < y;
				}

				if(cond)
				{
					good_pos_end.x = static_cast<int>((y - pos_beg.y) / m) + pos_beg.x;
					if(r.x <= good_pos_end.x && good_pos_end.x < right)
						good_pos_end.y = y;
					else
						return false;
				}
				else
					return false;
			}
			if(good_pos_end.x > pos_end.x)
				return false;
		}
		else
			good_pos_end = pos_end;

		return true;
	}

	void fit_zoom(const size& input_s, const size& ref_s, size& result_s)
	{
		double rate_input = double(input_s.width) / double(input_s.height);
		double rate_ref = double(ref_s.width) / double(ref_s.height);

		if(rate_input < rate_ref)
		{
			result_s.height = ref_s.height;
			result_s.width = static_cast<unsigned>(ref_s.height * rate_input);
		}
		else if(rate_input > rate_ref)
		{
			result_s.width = ref_s.width;
			result_s.height = static_cast<unsigned>(ref_s.width / rate_input);
		}
		else
			result_s = ref_s;
	}

	void zoom(const rectangle& ref, const rectangle& scaled, const rectangle& ref_dst, rectangle& r)
	{
		double rate_x = (scaled.x - ref.x) / double(ref.width);
		double rate_y = (scaled.y - ref.y) / double(ref.height);

		r.x = static_cast<int>(rate_x * ref_dst.width) + ref_dst.x;
		r.y = static_cast<int>(rate_y * ref_dst.height) + ref_dst.y;

		r.width = static_cast<unsigned>(scaled.width / double(ref.width) * ref_dst.width);
		r.height = static_cast<unsigned>(scaled.height / double(ref.height) * ref_dst.height);
	}

	//covered test if r2 covers r1.
	bool covered(const rectangle& r1, //Rectangle 1 is must under rectangle 2
						const rectangle& r2) //Rectangle 2
	{
		if(r1.x < r2.x || r1.x + r1.width > r2.x + r2.width) return false;
		if(r1.y < r2.y || r1.y + r1.height > r2.y + r2.height) return false;
		return true;
	}

	bool is_hit_the_rectangle(const rectangle& r, int x, int y)
	{
		return	(r.x <= x) && (x < r.x + static_cast<int>(r.width))
				&&
				(r.y <= y) && (y < r.y + static_cast<int>(r.height));
	}
}
};
