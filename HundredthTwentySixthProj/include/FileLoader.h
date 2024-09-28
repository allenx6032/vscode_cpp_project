#ifndef _FILELOADER_H_
#define _FILELOADER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Math.h"
enum Attribute
{
	POSITION = 1,
	UV = 2,
	NORMAL = 4,
	ALL = 7
};
typedef char Attributes;
#pragma pack(push,1)

typedef struct Vertex
{
	vec3 Position;
	vec2 UV;
	vec3 Normal;
}Vertex;
typedef struct Mesh
{
	Vertex* vertices;
	unsigned int* Indices;
	size_t numVertices;
	size_t numIndices;
	Attributes atr;
}Mesh;
typedef struct MDLHeader
{
	char signature[21];
	unsigned char ATR;
	unsigned int vCnt;
	unsigned int iCnt;
}MDLHeader;

typedef struct BMPheader
{
	unsigned char data1[10];
	unsigned int offset;
	unsigned int DIBsize;
}BMPheader;


typedef struct BITMAPINFOHEADER
{
	unsigned int width;
	unsigned int height;
	unsigned short colorPlanes;
	unsigned short bpp;
	unsigned int compression;
	unsigned int imgSize;
	unsigned char data[8];
	unsigned int palette;
	unsigned int importantColors;

}bmpInfo;
#pragma pack(pop)


void DelMesh(Mesh* m);

int equals(const vec3i a,const vec3i b);
int LoadOBJModel(const char* file, Mesh* mesh);
void SaveMDL(const Mesh m,const char* path);
int LoadMDL(const char* path, Mesh* mesh);

unsigned char* LoadBMP(const char* file, int* h, int* w, unsigned char* bpp, int switchRB);


#endif
