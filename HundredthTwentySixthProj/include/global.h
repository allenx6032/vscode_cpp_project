#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include <GLFW/glfw3.h>

#define uint unsigned int
// I know this is probably some shitty design but idk

#pragma pack(push, 1)
typedef struct vec3
{
	float x, y, z;
	// char c ;
} vec3;
typedef struct vec3i
{
	int x, y, z;
	// char c;
} vec3i;
typedef struct vec2
{
	float x, y;
	// char c;
} vec2;
typedef struct vec4
{
	float x, y, z, w;
} vec4;
typedef struct mat4
{
	// vec4 m0, m1, m2, m3;
	float
		m00,
		m01, m02, m03,
		m10, m11, m12, m13,
		m20, m21, m22, m23,
		m30, m31, m32, m33;
} mat4;
#pragma pack(pop)

#define PI 3.14159265f
#define deg2rad PI / 180.0f

#pragma region _INPUT_H_
typedef struct S_Mouse
{
	char Up[GLFW_MOUSE_BUTTON_LAST + 1];
	char Down[GLFW_MOUSE_BUTTON_LAST + 1];
	char cursorLocked;
	int toReset[GLFW_MOUSE_BUTTON_LAST + 1];
	int resetKeys;
	vec2 prevPos;
	vec2 Pos;
	vec2 Delta;
	vec2 ScrollDelta;

	int XPos, YPos;
	double wheelX, wheelY;
} S_Mouse;

typedef struct S_Input
{
	char KeyDown[GLFW_KEY_LAST + 1];
	char KeyPressed[GLFW_KEY_LAST + 1];
	char KeyUp[GLFW_KEY_LAST + 1];
	int toReset[GLFW_KEY_LAST + 1];
	int resetKeys;
	S_Mouse Mouse;
} S_Input;
extern S_Input Input;
#pragma endregion
#pragma region _GRAPHICS_H_
typedef struct Transform
{
	vec3 position;
	vec3 rotation;
	vec3 scale;
}Transform;
typedef struct Camera
{
	Transform transform;
	float FOV;
	float ScreenNear;
	float ScreenFar;
	float speed;
} Camera;
typedef struct Material
{
	uint ShaderProg;
	char *vFileName,* fFileName;
	void (*LoadData)(struct Material*);
	// Arrays
	int *intArr;
	size_t intArrSize;

	float *floatArr;
	size_t floatArrSize;

	vec2 *float2Arr;
	size_t float2ArrSize;

	vec3 *float3Arr;
	size_t float3ArrSize;

	vec4 *float4Arr;
	size_t float4ArrSize;

	mat4 *mat4Arr;
	size_t mat4ArrSize;

	uint *samplerArr;
	size_t samplerArrSize;

} Material;

typedef struct Model
{
	Transform transform;
	char state;
	uint VBO, VAO, EBO, ind_size;
	Material* material;
	

} Model;

typedef struct DirLight
{
	vec3 direction;
	float f3;
	vec3 ambient;
	float f2;
	vec3 diffuse;
	float f1;
	vec3 specular;
	float f;
}DirLight;
#pragma pack(push,1)
typedef struct Light
{
	vec3 position;
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} Light;
#pragma pack(pop)
extern DirLight dirLight;

extern Material* materials;
extern size_t materialsSize;

extern Camera FreeCam;
extern GLFWwindow *wnd;

// extern uint texture;
// extern Model mdl;

extern long long prevTime;
extern uint ShaderProg;

extern double dT;
extern long long startT;
//extern mat4 proj, camera;
extern vec2 res;
#pragma endregion

#endif