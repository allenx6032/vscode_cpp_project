
#ifndef _MAT_HPP_
#define _MAT_HPP_

#include "mcv_types.h"

_MCV_START_

class Mat {

public:
	Mat();
	Mat(const Mat& img);
	Mat(int rows, int cols, int type);

	~Mat();

	Mat& operator= (const Mat& img);

	void addRef();
	void release();

	bool empty() const;

	void creat(int rows, int cols, int type);

	Mat clone() const;
	void copyTo(Mat& img) const;
	void convertTo(Mat& img, int _depth) const;


	uchar* ptr(int i = 0);
	uchar* ptr(int i = 0) const;

	template <class T>
	T* ptr(int i);

	size_t elemSize1() const;
	size_t elemSize() const;
	int type() const;
	int depth() const;
	int channels() const;
	size_t total() const;


private:
	void normalCopy(const Mat& img);


private:
	struct RefCount
	{
		int ref_count_;

		uchar* data_in_ref_;
		RefCount(int);
		~RefCount();

	};

	RefCount* p_ref_;

public:
	uchar*	data_;
	int		rows_;
	int		cols_;
	int		flags_;
};


_MCV_END_


#endif // !_MAT_HPP_