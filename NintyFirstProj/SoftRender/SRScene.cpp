#include "SRScene.h"

#include "SRMath.h"
#include "SRShader.h"
#include "SRMesh.h"
#include "SRObject.h"
#include "SRLight.h"
#include "SRCamera.h"
#include "SRFrameBuffer.h"
#include "SRMaterial.h"
#include "SRLight.h"

#include <omp.h>

Scene::Scene(const int& w, const int& h, RenderMode m) {
	width = w;
	height = h;
	mode = m;
	frameBuffer = nullptr;
}

Scene::~Scene() {
	if (frameBuffer) {
		delete frameBuffer;
	}
	lights.clear();
	std::vector <Light*>().swap(lights); // 清空内存
	objects.clear();
	std::vector <Object*>().swap(objects); // 清空内存
	frameBuffer = nullptr;
}

void Scene::init() {
	camera = nullptr;
	currentMaterial = nullptr;
	glm::vec4 backgroundColor = glm::vec4(0, 0, 0, 255);
	viewFrustumPlanes.resize(6, glm::vec4(0.0f));

	lights.clear();
	std::vector <Light*>().swap(lights); // 清空内存
	objects.clear();
	std::vector <Object*>().swap(objects); // 清空内存

	if (frameBuffer) {
		delete frameBuffer;
	}
	frameBuffer = new FrameBuffer(width, height);

	viewPortMatrix = getViewPortMatrix(0, 0, width, height);
	viewMatrix = glm::mat4(1.0f);
	projectMatrix = glm::mat4(1.0f);
}

void Scene::add(Camera* cam) {
	if (camera) {
		delete camera;
	}
	camera = cam;
	updateCamera();
	glm::vec4 temp = projectMatrix * viewMatrix * viewFrustumPlanes[4];
	nearZ = (temp.z / temp.w + 1.0) * 0.5;
	temp = projectMatrix * viewMatrix * viewFrustumPlanes[5];
	farZ = (temp.z / temp.w + 1.0) * 0.5;
}

void Scene::add(Light* light) {
	lights.push_back(light);
}

void Scene::add(Object* object) {
	objects.push_back(object);
	object->material->shader->setScene(this);
}

void Scene::updateCamera() {
	camera->updateAspact(width, height);
	viewMatrix = camera->viewMatrix(); // 更新观察空间矩阵
	projectMatrix = camera->perspectiveMatrix(); // 更新投影矩阵
	updateViewFrustumPlanes(viewFrustumPlanes, projectMatrix * viewMatrix); // 更新视锥平面
}

void Scene::setBackgroundColor(const glm::vec4& color) {
	backgroundColor = color;
}

void Scene::resize(const int& w, const int& h) {
	width = w;
	height = h;
	frameBuffer->resize(w, h);
	viewPortMatrix = getViewPortMatrix(0, 0, w, h);

	updateCamera();
}

void Scene::fillColorBuffer(const glm::vec4& color) {
	frameBuffer->fillColorBuffer(color);
}

void Scene::display() {
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, frameBuffer->colorBuffer.data());
}

void Scene::drawVertex(int x, int y, Vertex& v, Shader* shader) {
	float depth = frameBuffer->getDepth(x, y);
	if (v.windowPos.z < depth) {
		frameBuffer->setDepth(x, y, v.windowPos.z);

		// 透视映射
		v.worldPos /= v.z;
		v.texCoord /= v.z;
		v.normal /= v.z;
		v.color /= v.z;

		frameBuffer->drawPixel(x, y, shader->fragmentShader(v));
	}
}

/*
* 光栅化三角形
* 使用重心坐标系, 遍历三角形包围盒填充
*/
void Scene::drawTriangleBoundary(const Vertex& v1, const Vertex& v2, const Vertex& v3, Shader* shader) {

	// 计算包围盒
	int min_x = triMin(v1.windowPos.x, v2.windowPos.x, v3.windowPos.x);
	int min_y = triMin(v1.windowPos.y, v2.windowPos.y, v3.windowPos.y);
	int max_x = triMax(v1.windowPos.x, v2.windowPos.x, v3.windowPos.x) + 1;
	int max_y = triMax(v1.windowPos.y, v2.windowPos.y, v3.windowPos.y) + 1;

	// 遍历包围盒中的点
#pragma omp parallel for collapse(2)
	for (int x = min_x; x < max_x; x++) {
		for (int y = min_y; y < max_y; y++) {
			glm::vec3 barycentric = getBarycentric(v1, v2, v3, glm::vec2(x, y)); // 计算重心坐标
			if (barycentric[0] >= 0 && barycentric[1] >= 0 && barycentric[2] >= 0) { // 点在三角形内部
				Vertex v = Lerp(v1, v2, v3, barycentric);
				drawVertex(x, y, v, shader);
			}
		}
	}
}

/*
* 光栅化三角形
* 使用普通平面坐标系, 将三角形拆分成上下两部分分别使用扫描线算法进行填充与插值
*/
void Scene::drawTriangleScanLine(const Vertex& v1, const Vertex& v2, const Vertex& v3, Shader* shader) {

	// 将v1,v2,v3按照纵坐标由小到大排序
	Vertex arr[3] = { v1, v2, v3 };
	if (arr[0].windowPos.y > arr[1].windowPos.y) std::swap(arr[0], arr[1]);
	if (arr[1].windowPos.y > arr[2].windowPos.y) std::swap(arr[1], arr[2]);
	if (arr[0].windowPos.y > arr[1].windowPos.y) std::swap(arr[0], arr[1]);

	// 填充前初始化
	int minY = arr[0].windowPos.y, midY = arr[1].windowPos.y, maxY = arr[2].windowPos.y;
	int bottomHeight = midY - minY, upperHeight = maxY - midY, totalHeight = maxY - minY;

	Vertex leftPoint = Lerp(arr[0], arr[2], float(bottomHeight) / totalHeight);
	Vertex rightPoint = arr[1];
	if (leftPoint.windowPos.x > rightPoint.windowPos.x) std::swap(leftPoint, rightPoint);

	// 填充下半部分
#pragma omp parallel for
	for (int y = minY; y < midY; y++) {
		float weight = float(y - minY) / bottomHeight;
		Vertex leftpos = Lerp(arr[0], leftPoint, weight);
		Vertex rightpos = Lerp(arr[0], rightPoint, weight);
		scanLineFilling(leftpos, rightpos, shader);
	}

	// 填充上半部分
#pragma omp parallel for
	for (int y = midY; y <= maxY; y++) {
		float weight = float(y - midY) / upperHeight;
		Vertex leftpos = Lerp(leftPoint, arr[2], weight);
		Vertex rightpos = Lerp(rightPoint, arr[2], weight);
		scanLineFilling(leftpos, rightpos, shader);
	}
}

/*
* 扫描线填充
* 填充并插值从left到right的所有点
*/
void Scene::scanLineFilling(const Vertex& left, const Vertex& right, Shader* shader) {

	if (left.windowPos.x == right.windowPos.x) {
		float depth = frameBuffer->getDepth(left.windowPos.x, left.windowPos.y);
		if (left.windowPos.z < depth) {// 深度测试
			frameBuffer->setDepth(left.windowPos.x, left.windowPos.y, left.windowPos.z);
			frameBuffer->drawPixel(left.windowPos.x, left.windowPos.y, shader->fragmentShader(left));
		}
		return;
	}

	int xmin = left.windowPos.x, xmax = right.windowPos.x; // 
	int lenght = xmax - xmin;
	int y = left.windowPos.y;

#pragma omp parallel for
	for (int x = xmin; x <= xmax; x++) {
		Vertex v = Lerp(left, right, float(x - xmin) / lenght);
		drawVertex(x, y, v, shader);
	}
}

/*
* 绘制Mesh
* 一个Mesh由多个三角面构成
*/
void Scene::drawMesh(const Mesh* mesh, Shader* shader) {
	int size = mesh->EBO.size();
	if (size == 0) {
		return;
	}

#pragma omp parallel for
	for (int i = 0; i < size; i += 3) {
		RawVertex A = *(mesh->VBO.data() + *(mesh->EBO.data() + i));
		RawVertex B = *(mesh->VBO.data() + *(mesh->EBO.data() + i + 1));
		RawVertex C = *(mesh->VBO.data() + *(mesh->EBO.data() + i + 2));

		// 顶点着色器
		Vertex v1 = shader->vertexShader(A);
		Vertex v2 = shader->vertexShader(B);
		Vertex v3 = shader->vertexShader(C);

		// 视锥剔除
		if (viewFrustumCutting(v1, v2, v3, viewFrustumPlanes)) continue;

		// 透视除法
		perspectiveDivision(v1);
		perspectiveDivision(v2);
		perspectiveDivision(v3);

		// 背面剔除
		if (backFaceCutting(v1, v2, v3)) continue;

		// NDC坐标转换为窗口屏幕坐标
		v1.windowPos = viewPortMatrix * v1.windowPos;
		v2.windowPos = viewPortMatrix * v2.windowPos;
		v3.windowPos = viewPortMatrix * v3.windowPos;

		if (mode == RenderMode::MESH_SCANLINE) {
			drawTriangleScanLine(v1, v2, v3, shader);
		}
		else if (mode == RenderMode::MESH_BOUNDARY) {
			drawTriangleBoundary(v1, v2, v3, shader);
		}
		else if (mode == RenderMode::LINE) {
			drawLine(v1, v2);
			drawLine(v2, v3);
			drawLine(v1, v3);
		}
	}
}

/*
* Bresenham画线算法
*/
void Scene::drawLine(const Vertex& v1, const Vertex& v2) {
	int x0 = v1.windowPos.x, x1 = v2.windowPos.x;
	int y0 = v1.windowPos.y, y1 = v2.windowPos.y;
	bool steep = false; // 是否交换x,y轴
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) { // 如果斜率大于1, 则交换x,y轴
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	int x;
	for (x = x0; x <= x1; x++) {
		if (steep) {
			frameBuffer->drawPixel(y, x, glm::vec4(0, 0, 0, 255));
		}
		else {
			frameBuffer->drawPixel(x, y, glm::vec4(0, 0, 0, 255));
		}
		error2 += derror2;
		if (error2 > dx) {
			y += y1 > y0 ? 1 : -1;
			error2 -= dx * 2;
		}
	}
}

void Scene::drawObject(const Object* obj) {
	if (obj->mesh->EBO.empty()) {
		return;
	}
	drawMesh(obj->mesh, obj->material->shader);
}

void Scene::render() {
	updateCamera();
	fillColorBuffer(backgroundColor);
	for (auto object : objects) {
		currentMaterial = object->material;
		drawObject(object);
	}
	display();
}