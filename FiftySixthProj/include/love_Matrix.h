/**
* Copyright (c) 2006-2010 LOVE Development Team
* 
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
**/

#ifndef LOVE_MATRIX_H
#define LOVE_MATRIX_H

// LOVE
#include "love_math.h"

namespace love
{
	/**
	* This class is the basis for all transformations in LOVE. Althought not
	* really needed for 2D, it contains 4x4 elements to be compatible with 
	* OpenGL without conversions. 
	**/
	class Matrix
	{
	private:

		/**
		* | e0 e4 e8  e12 |
		* | e1 e5 e9  e13 |
		* | e2 e6 e10 e14 |
		* | e3 e7 e11 e15 |
		**/
		float e[16];

	public:

		/**
		* Creates a new identity matrix.
		**/
		Matrix();

		/**
		* Destructor.
		**/
		~Matrix();

		/**
		* Multiplies this Matrix with another Matrix, changing neither.
		* @param m The Matrix to multiply with this Matrix.
		* @return The combined matrix.
		**/
		Matrix operator * (const Matrix & m) const;

		/**
		* Multiplies a Matrix into this Matrix.
		* @param m The Matrix to combine into this Matrix.
		**/
		void operator *= (const Matrix & m);

		/**
		* Gets a pointer to the 16 array elements.
		* @return The array elements.
		**/
		const float * getElements() const;

		/**
		* Resets this Matrix to the identity matrix.
		**/
		void setIdentity();

		/**
		* Resets this Matrix to a translation.
		* @param x Translation along x-axis.
		* @param y Translation along y-axis.
		**/
		void setTranslation(float x, float y);

		/**
		* Resets this Matrix to a rotaion.
		* @param r The angle in radians.
		**/
		void setRotation(float r);

		/**
		* Resets this Matrix to a scale transformation.
		* @param sx Scale factor along the x-axis.
		* @param sy Scale factor along the y-axis.
		**/
		void setScale(float sx, float sy);

		/**
		* Creates a transformation with a certain position, orientation, scale
		* and offset. Perfect for Drawables -- what a coincidence!
		* 
		* @param x The translation along the x-axis.
		* @param y The translation along the y-axis.
		* @param angle The rotation (rad) around the center with offset (ox,oy).
		* @param sx Scale along x-axis.
		* @param sy Scale along y-axis.
		* @param ox The offset for rotation along the x-axis.
		* @param oy The offset for rotation along the y-axis.
		**/
		void setTransformation(float x, float y, float angle, float sx, float sy, float ox, float oy);

		/**
		* Multiplies this Matrix with a translation.
		* @param x Translation along x-axis.
		* @param y Translation along y-axis.
		**/
		void translate(float x, float y);

		/**
		* Multiplies this Matrix with a rotation.
		* @param r Angle in radians.
		**/
		void rotate(float r);

		/**
		* Multiplies this Matrix with a scale transformation.
		* @param sx Scale factor along the x-axis.
		* @param sy Scale factor along the y-axis.
		**/
		void scale(float sx, float sy);
		
		/**
		* Transforms an array of vertices by this Matrix. The sources and
		* destination arrays may be the same. 
		* 
		* @param dst Storage for the transformed vertices.
		* @param src The source vertices.
		* @param size The number of vertices.
		**/
		void transform(vertex * dst, const vertex * src, int size) const;

	}; // Matrix

} //love

#endif// LOVE_MATRIX_H
