#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <stdlib.h>
using namespace std;
double Point[10000][3];
double Normal[10000][3];
double GetArea(double * const u,double * const v,double * const t)
{//传入三个点u,v,t，求他们围成的三角形面积
    double x1 = v[0]-u[0],y1 = v[1]-u[1],z1 = v[2]-u[2];
    double x2 = t[0]-u[0],y2 = t[1]-u[1],z2 = t[2]-u[2];
    double result[3];
    result[0] = y1*z2-y2*z1; result[1] = z1*x2-z2*x1; result[2] = x1*y2-y1*x2;
    double ans = result[0]*result[0]+result[1]*result[1]+result[2]*result[2];
    ans = sqrt(ans);
    ans /=2;
    return ans;
}
void GetPieceNormal(double * const u,double * const v,double * const t,double *res)
{//传入三个点坐标u[3],v[3]，t[3]求得他们所围成的三角形的法向量保存到res[3]
    //三点是逆时针方向为正面的，法向量是沿正面指向外的单位向量
    double x1 = v[0]-u[0],y1 = v[1]-u[1],z1 = v[2]-u[2];
    double x2 = t[0]-u[0],y2 = t[1]-u[1],z2 = t[2]-u[2];
    res[0] = y1*z2-y2*z1; res[1] = z1*x2-z2*x1; res[2] = x1*y2-y1*x2;
    double normalize = res[0]*res[0]+res[1]*res[1]+res[2]*res[2];
    normalize = sqrt(normalize);
    res[0] /= normalize;
    res[1] /= normalize;
    res[2] /= normalize;
}

double diancheng(double * const u,double * const v)
{//返回两个向量点乘的结果
    return (u[0]*v[0] + u[1]*v[1] + u[2] * v[2]);
}

void FunNormalize(double * u)
{
    double xishu = u[0] * u[0] + u[1] * u[1] + u[2] * u[2];
    if(xishu == 0)
        return;
    xishu = sqrt(xishu);
    u[0] /= xishu;
    u[1] /= xishu;
    u[2] /= xishu;
}
void JuZhenXiangCheng(double (* sour1)[3],double (* sour2)[3],double (* res)[3])
{
    for(int i = 0 ; i < 3 ; i ++)
    {
        for(int j = 0; j < 3; j ++)
        {
            res[i][j] = sour1[i][0] * sour2[0][j] + sour1[i][1] * sour2[1][j] + sour1[i][2] * sour2[2][j];
        }
    }
}
#define FILENAME "agatha.obj"
int main()
{
    FILE *fin = fopen(FILENAME,"r");
    FILE *ftriangle = fopen("triangle.temp","w");
    char temp[100];
    int pointer = 1;
    while(fgets(temp,99,fin) != NULL)
    {
        if(temp[1]!=' ')
            continue;
        if(temp[0]!='v'&&temp[0]!='f')
            continue;
        if(temp[0] == 'v')
        {//这是一个点
            sscanf(temp+1,"%lf%lf%lf",&Point[pointer][0],&Point[pointer][1],&Point[pointer][2]);
            pointer++;
        }
        else
        {
            int tempa,tempb,tempc;
            sscanf(temp+1,"%d//%*d %d//%*d %d//%*d",&tempa,&tempb,&tempc);
            double tempnormal[3];
            GetPieceNormal(Point[tempa],Point[tempb],Point[tempc],tempnormal);
            fprintf(ftriangle,"%d %d %d %lf %lf %lf %lf\n",tempa,tempb,tempc,
                     GetArea(Point[tempa],Point[tempb],Point[tempc]),
                    tempnormal[0],tempnormal[1],tempnormal[2]);//输出格式：3个点索引，面积，法向量

        }
    }
    fclose(fin);
    fclose(ftriangle);
    int PointTotal = pointer;
    //逐点计算法向和主方向
    for(pointer = 1; pointer <= PointTotal ; pointer++)
    {//计算法向
        ftriangle = fopen("triangle.temp","r");//3个点索引，面积，法向量
        double AreaInTotal = 0;
        printf("%d\n",pointer);
        while(fgets(temp,99,ftriangle) != NULL)
        {
            int pointa,pointb,pointc;
            double AreaNow;
            double NormalNowx,NormalNowy,NormalNowz;
            sscanf(temp,"%d %d %d %lf %lf %lf %lf",
                   &pointa,&pointb,&pointc,&AreaNow
                   ,&NormalNowx,&NormalNowy,&NormalNowz);
            if(pointa != pointer && pointb != pointer && pointc != pointer)
                continue;
            AreaInTotal += AreaNow;
            Normal[pointer][0] += NormalNowx * AreaNow;
            Normal[pointer][1] += NormalNowy * AreaNow;
            Normal[pointer][2] += NormalNowz * AreaNow;
        }
        Normal[pointer][0] /= AreaInTotal;
        Normal[pointer][1] /= AreaInTotal;
        Normal[pointer][2] /= AreaInTotal;
        fclose(ftriangle);
    }
    //逐点计算两个主方向向量，写入文件
    char filename[100];
    sscanf(FILENAME,"%[^.]",filename);
    strcat(filename,".txt");
    FILE * fout = fopen(filename,"w+");
    for(pointer = 1; pointer <= PointTotal ; pointer++)
    {
        //计算Mpi，3x3矩阵
        double M[3][3]={};
        double AreaInTotal = 0;
        ftriangle = fopen("triangle.temp","r");//3个点索引，面积，法向量
        printf("%d\n",pointer);
        while(fgets(temp,99,ftriangle) != NULL)
        {
            int pointa,pointb,pointc;
            double AreaNow;
            double NormalNow[3];
            //printf("%d %s\n",pointer,temp);
            sscanf(temp,"%d %d %d %lf %lf %lf %lf",
                   &pointa,&pointb,&pointc,&AreaNow
                   ,&NormalNow[0],&NormalNow[1],&NormalNow[2]);
            if(pointa != pointer && pointb != pointer && pointc != pointer)
                continue;
            AreaInTotal += AreaNow;
            if(pointb == pointer)
            {
                int tmp = pointb;
                pointb = pointa;
                pointa = tmp;
            }
            if(pointc == pointer)
            {
                int tmp = pointc;
                pointc = pointa;
                pointa = tmp;
            }//将当前点挪到A
            //计算AB的T并加入Mpi的计算，由于每一个相邻点都要加入两次，故权重对半
            double T[3]={};
            double VecIJ[3] = {Point[pointb][0]-Point[pointa][0],
                                Point[pointb][1]-Point[pointa][1],
                                Point[pointb][2]-Point[pointa][2]};
            double xishu = diancheng(NormalNow,VecIJ);
            T[0] = xishu * NormalNow[0];
            T[1] = xishu * NormalNow[1];
            T[2] = xishu * NormalNow[2];
            T[0] -= VecIJ[0];
            T[1] -= VecIJ[1];
            T[2] -= VecIJ[2];
            FunNormalize(T);
            //计算Tij相乘
            //先计算ij的方向曲率
            double FangXiangQuLv = 2 * diancheng(NormalNow,VecIJ)/
                (VecIJ[0] * VecIJ[0] + VecIJ[1] * VecIJ[1] + VecIJ[2] * VecIJ[2]);
            for(int i = 0; i < 3 ; i++)
            {
                M[0][i] += T[i] * T[0] * FangXiangQuLv * AreaNow / 2;
                M[1][i] += T[i] * T[1] * FangXiangQuLv * AreaNow / 2;
                M[2][i] += T[i] * T[2] * FangXiangQuLv * AreaNow / 2;
            }
            //计算AC的T并加入Mpi的计算，由于每一个相邻点都要加入两次，故权重对半
            VecIJ[0] = Point[pointb][0]-Point[pointa][0];
            VecIJ[1] = Point[pointb][1]-Point[pointa][1];
            VecIJ[2] = Point[pointb][2]-Point[pointa][2];
            xishu = diancheng(NormalNow,VecIJ);
            T[0] = xishu * NormalNow[0];
            T[1] = xishu * NormalNow[1];
            T[2] = xishu * NormalNow[2];
            T[0] -= VecIJ[0];
            T[1] -= VecIJ[1];
            T[2] -= VecIJ[2];
            FunNormalize(T);
            //计算Tij相乘
            //先计算ij的方向曲率
            FangXiangQuLv = 2 * diancheng(NormalNow,VecIJ)/
                (VecIJ[0] * VecIJ[0] + VecIJ[1] * VecIJ[1] + VecIJ[2] * VecIJ[2]);
            for(int i = 0; i < 3 ; i++)
            {
                M[0][i] += T[i] * T[0] * FangXiangQuLv * AreaNow / 2;
                M[1][i] += T[i] * T[1] * FangXiangQuLv * AreaNow / 2;
                M[2][i] += T[i] * T[2] * FangXiangQuLv * AreaNow / 2;
            }
        }
        //if(pointer == 317)
            //system("pause");
        for(int i = 0; i < 3; i ++)
        {
            for(int j = 0 ;j < 3; j++)
            {
                M[i][j] /= AreaInTotal;
            }
        }
        //计算完毕Mpi
        //计算Wpi
        double W[3] = {Normal[pointer][0],Normal[pointer][1],Normal[pointer][2]};
        if(fabs(1-Normal[pointer][0]) > fabs(1+Normal[pointer][0]))
        {
            W[0] = 1 - W[0];
            W[1] = -W[1];
            W[2] = -W[2];
        }
        else
            W[0] += 1;
        FunNormalize(W);
        //Wpi计算完毕
        //计算Qpi,3x3矩阵
        double Q[3][3] = {};
        for(int i = 0; i < 3 ; i++)
        {
            Q[0][i] = -2 * W[0] * W[i];
            Q[1][i] = -2 * W[1] * W[i];
            Q[2][i] = -2 * W[2] * W[i];
        }
        Q[0][0] += 1;Q[1][1] += 1; Q[2][2] += 1;
        //Qpi估计完毕
        double T1Estimate[3] = {Q[1][0],Q[1][1],Q[1][2]};
        double T2Estimate[3] = {Q[2][0],Q[2][1],Q[2][2]};
        double GetSmallM[3][3];
        double TempMatrix[3][3];
        double QT[3][3];//Q的转置
        for(int i = 0; i < 3; i ++)
            for(int j = 0; j < 3; j++)
                QT[i][j] = Q[j][i];
        JuZhenXiangCheng(QT,M,TempMatrix);
        JuZhenXiangCheng(TempMatrix,Q,GetSmallM);
        //GetSmall的右下角的子矩阵进行操作
        if(GetSmallM[1][2] == 0 && GetSmallM[2][2] == 0)
        {
            fprintf(fout,"%lf %lf %lf %lf %lf %lf\n",0.0,0.0,1.0,0.0,0.0,1.0);
        }
        else
        {
            double costh = (GetSmallM[1][2]*GetSmallM[1][2])/(GetSmallM[1][2]*GetSmallM[1][2]+GetSmallM[2][2]*GetSmallM[2][2]);
            double sinth = (GetSmallM[2][2]*GetSmallM[2][2])/(GetSmallM[1][2]*GetSmallM[1][2]+GetSmallM[2][2]*GetSmallM[2][2]);
            costh = sqrt(costh);
            sinth = sqrt(sinth);
            double T1[3] = {};
            double T2[3] = {};
            for(int i = 0; i < 3 ; i++)
            {
                T1[i] = T1Estimate[i]*costh + T2Estimate[i]*sinth;
                T2[i] = T1Estimate[i]*sinth + T2Estimate[i]*costh;
            }
            fprintf(fout,"%lf %lf %lf %lf %lf %lf\n",T1[0],T1[1],T1[2],T2[0],T2[1],T2[2]);
        }
        fclose(ftriangle);
    }
    return 0;
}
