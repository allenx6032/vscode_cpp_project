#ifndef WIDGET_ROTATEMENU_H_
#define WIDGET_ROTATEMENU_H_

#include "SLO_Misc.h"
#include "SLO_GLSprite3D.h"
#include <memory>
#include <vector>

namespace GL_ {

template<typename T>
class RotateMenu
{
public:
	UNCOPYABLE(RotateMenu);
	RotateMenu()
		: listMenuItem_()
		, radius_()
		, degrees_()
	{

	}
	virtual ~RotateMenu(){}

	void setRadius(double radius) { radius_ = radius; }
	double getRadius() const { return radius_; }

	void setDegrees(double degrees)
	{
		while(true) {
			if (degrees < 0.0) {
				degrees += MAX_DEGREES;
			}
			else if (MAX_DEGREES <= degrees) {
				degrees -= MAX_DEGREES;
			}
			else {
				break;
			}
		}
		degrees_ = degrees;
	}
	double getDegrees() const { return degrees_; }

	std::pair<double, uint32_t> computeDgrees(int oldPos, int movePos, uint32_t duration)
	{
		const int newPos = oldPos + movePos;
		const double delta = (MAX_DEGREES / listMenuItem_.size());
		double endDegrees = delta * newPos;
		if (movePos == 0) {
			return {0, 0};
		}

		const double beginDegrees = delta * oldPos;
		const double currDegrees = getDegrees();

		if (currDegrees != beginDegrees) {
			return {0, 0};
		}

		double subDegrees;
		// 左
		if (movePos < 0) {
			if (currDegrees < endDegrees) {
				endDegrees -= MAX_DEGREES;
			}
//			if (beginDegrees < currDegrees) {
//				return {endDegrees, 0};
//			}
			subDegrees = currDegrees - endDegrees;
		}
		// 右
		else {
			if (endDegrees < currDegrees) {
				endDegrees += MAX_DEGREES;
			}
			// current = c, begin = b, end = e とした場合
			// c = b < e 停止したカーソルを右に移動しようとした時
			// c < b < e 右に移動中のカーソルを右に移動しようとした時
			// b < c < e 左に移動中のカーソルを右に移動しようとした時
			subDegrees = endDegrees - currDegrees;
		}

		uint32_t newDuration;
		const double ratio = subDegrees/delta;
		if (ratio <= 1.0) {
			newDuration = static_cast<uint32_t>(duration * ratio);
		}
		else {
			newDuration = static_cast<uint32_t>(duration / ratio);
		}
		return {endDegrees, newDuration};
	}

	void addMenuItem(std::shared_ptr<Sprite3D> item) { listMenuItem_.push_back(item); }
	void clearMenuItem() { listMenuItem_.clear(); }
	const std::vector<std::shared_ptr<T>> &getMenuItemList() const { return listMenuItem_; }
	const auto getMenuItemListSize() const { return listMenuItem_.size(); }

	std::shared_ptr<T> getMenuItem(int index) {
//		const auto size = listMenuItem_.size();
//		if (index < 0) {
//			index += size;
//		}
//		else if (size <= index){
//			index -= size;
//		}
		return listMenuItem_[index];
	}

	virtual void draw() = 0;

	static constexpr double MAX_DEGREES = 360.0;

private:

	std::vector<std::shared_ptr<T>> listMenuItem_;
	double radius_;
	double degrees_;
};

} // namespace GL_

#endif // WIDGET_ROTATEMENU_H_
