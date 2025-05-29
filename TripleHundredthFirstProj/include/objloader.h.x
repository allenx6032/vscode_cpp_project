#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>

namespace sp // simple and to the point
{

struct TexCoord
{
    float u;
    float v;
    TexCoord(): u(0), v(0) {}
    TexCoord(float _u, float _v): u(_u), v(_v) {}
    float& operator[](int idx);
};

struct Face
{
    int a;
    int b;
    int c;
};

class ObjLoader
{
public:
    ObjLoader();

    void load(char* filename);

    int getIndexCount();
    int getVertCount();

    const unsigned int* getFaces();
    const float* getPositions();
    const float* getNormals();

    int getTexCoordLayers();
    const float* getTexCoords(int multiTexCoordLayer);

private:

    std::vector<unsigned int> Faces;
    std::vector<float> Positions;
    std::vector<float> Normals;

    // obj's only have 1 layer ever
    std::vector<float> TexCoords;
    unsigned int TexCoordLayers;
};
}
#endif // OBJLOADER_H
