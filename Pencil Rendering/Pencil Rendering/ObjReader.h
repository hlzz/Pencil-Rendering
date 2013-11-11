#ifndef OBJREADER_H
#define OBJREADER_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
class ObjReader
{
    public:
        ObjReader(char *FILE_NAME);
        virtual ~ObjReader();
        float Normal[10001][3];
        float Principle[10001][3];
        float ExternPrinciple[10001][3];
        float Point[10001][3];
        int FacesPoint[30001][3];
        int FacesNormal[30001][3];
        int NumOfFaces;
    protected:
    private:
        int pointerdian;
        int pointerface;
        int pointerfaxiang;
};

#endif // OBJREADER_H
