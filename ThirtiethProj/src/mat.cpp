
#include "mat.hpp"

_MCV_START_

Mat::Mat() :flags_(0), data_(NULL), rows_(0), cols_(0), p_ref_(NULL){

}


Mat::Mat(int rows, int cols, int type) {
	creat(rows, cols, type);
}
Mat::Mat(const Mat& img) {
	normalCopy(img);
	addRef();
}


Mat::~Mat(){
	release();
}


Mat& Mat::operator=(const Mat& img) {
	if (p_ref_ == img.p_ref_) return *this;

	release();

	normalCopy(img);
	
	addRef();
	return *this;
}

void Mat::addRef() {
	if (p_ref_ != NULL)
		++p_ref_->ref_count_;
}

void Mat::release() {
	if (p_ref_ == NULL || p_ref_->ref_count_ <= 0) return;
	if (--p_ref_->ref_count_ == 0) {
		delete p_ref_;
		p_ref_ = NULL;
		data_ = NULL;
	}
}


bool Mat::empty() const{
	return data_ == NULL;
}


size_t Mat::elemSize1() const {
	return MCV_ELEM_SIZE1(flags_);
}


size_t Mat::elemSize() const {
	return MCV_ELEM_SIZE(flags_);
}


int Mat::type() const {
	return MCV_MAT_TYPE(flags_);
}


int Mat::depth() const {
	return MCV_MAT_DEPTH(flags_);
}


int Mat::channels() const{
	return MCV_MAT_CN(flags_);
}

size_t Mat::total() const {
	if (channels() <= 2)
		return rows_*cols_;
	else {
		return rows_*cols_*MCV_ELEM_SIZE(type());
	}
}

inline
void Mat::normalCopy(const Mat& img) {
	rows_ = img.rows_;
	cols_ = img.cols_;
	flags_ = img.flags_;
	p_ref_ = img.p_ref_;
	data_ = img.data_;
}

void Mat::creat(int rows, int cols,  int type) {
	
	release();

	rows_ = rows;
	cols_ = cols;
	flags_ = type; //need to discuss 
	p_ref_ = new RefCount(rows*cols*MCV_ELEM_SIZE(type));

	if (p_ref_ == NULL ||  p_ref_->data_in_ref_ == NULL){
		std::cerr << "out of memory in Mat::creat" << std::endl;
		exit(0);
	}
	
	data_ = p_ref_->data_in_ref_;
}


Mat Mat::clone() const {
	Mat tmp;

	copyTo(tmp);

	return tmp;
}

void Mat::copyTo(Mat& img) const{

	img.creat(rows_, cols_, flags_);
	
	memcpy(img.data_, data_, total());
}



void Mat::convertTo(Mat& img, int _type) const{
	if (type() == _type)
		copyTo(img);
	
	if (img.rows_ == rows_ && img.cols_ == cols_ && img.p_ref_ != NULL && img.p_ref_->ref_count_ == 1) {

	}
	else {
		img.creat(rows_, cols_, _type);
	}

	unsigned int ave = 0;
	if (type() == MCV_8UC3 && _type == MCV_8UC1) {
		for (int i = 0, j = 0;  i < total() && j < img.total(); i += 3, ++j) {
			ave = (unsigned int)data_[i] + (unsigned int)data_[i + 1] + (unsigned int)data_[i + 2];
			ave /= 3;
			img.data_[j] = (uchar)ave;
		}
	}

}


uchar* Mat::ptr(int i) {
	return data_ + i*cols_*channels();
}

uchar* Mat::ptr(int i) const{
	return data_ + i*cols_*channels();
}


template <class T>
T* Mat::ptr(int i = 0) {
	return (T*)data_ + i*cols_*channels();
}




/*Mat RefCount*/
Mat::RefCount::RefCount(int size = 0){
	if (size <= 0) {
		data_in_ref_ = NULL;
		ref_count_ = 0;
		return;
	}
	data_in_ref_ = new uchar[size];
	ref_count_ = 1;

}

Mat::RefCount::~RefCount() {
	delete[] data_in_ref_;

}

_MCV_END_