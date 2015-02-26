#ifndef PROCEDURAL_H
#define PROCEDURAL_H

#include "ArrayPtr.h"
#include "Resource.h"
#include "Rules.h"

class Procedural
{
public:
    /// Constructor and Destructor
    Procedural();
    virtual ~Procedural();

    /// Main Functions
    bool Initialize(const int &width, const int &height);
    bool SetOffSets(const int &offsetx, const int &offsety);
    bool SetOctaves(const unsigned &octaves, const float &opersistence, const bool &ooverride, const float &o1, const float &o2, const float &o3, const  float &o4, const float &o5, const float &o6, const float &o7, const float &o8);

    bool GenerateProceduralPerlin(const float &scale);
    bool GenerateProceduralDiamond(const float &scale);

    bool GenerateProceduralTerrain(const terrain_rule &terrainrule);

    /// Point pixelData to perlininput1 memory
    unsigned char * GetImage(void);

private:
    bool GenerateOrlen2DNoise1(float * inputData1, int x, int y, int iterations);
    bool GenerateBuild(float * inputData1, unsigned * output);
    /// Create a perlin noise input date - Version 2
    bool generatePerlinNoise1(float * inputData1, const unsigned &octaves, const int &xOffset, const int  &yOffset, const float &zOffset,const int &xWrap,const int &yWrap,const int &zWrap,const float &o1, const float &o2,const float &o3, const float &o4, const float &o5, const float &o6, const float &o7, const float &o8);
    /// Create a displacemeent input data noise version 1
    bool generateDiamondMethod1(float * inputData1, const float &maxYcoords,const float &minYcoords);
    /// Create a control perlin noise input date
    bool generateControlPerlinNoise1(float * inputData1, const unsigned &octaves, const int &xOffset, const int &yOffset, const float &zOffset,const int &xWrap,const int &yWrap,const int &zWrap,const float &o1, const float &o2,const float &o3, const float &o4, const float &o5, const float &o6, const float &o7, const float &o8);
    /// Scale input data
    bool GenerateScale(float *inputData1,const float scale);
    /// Select to build new output data - version 1
    float * GenerateSelectBuild(float * inputData1, float * inputData2,  float * controldata1);
    /// Select to build new output data - version 2 (based o libnoise select)
    float * GenerateSelectBuild2(const float &lowerBound, const float &upperBound, const float &edgeFalloff , float * inputData1, float * inputData2, float *controldata1);
    /// Copy inputData to image buffer
    void CopyData(const unsigned char* pixelData);
    /// GeneratePerlin
    unsigned rgba32ToUInt(unsigned r, unsigned g, unsigned b, unsigned a);
    bool GenerateClamp(float * inputData1, const float &lowerBound, const float &upperBound);
    float * GenerateAdd (float *inputData1, float *inputData2);
    float * GenerateSubtract (float *inputData1, float *inputData2);


private:
    /// required data
    unsigned int width_;
    unsigned int height_;
    int offsetx_;
    int offsety_;
    unsigned int components_;
    unsigned int depth_;

    /// octave information
    bool oct_override;
    float oct_persistence;

    float oct_octaves;
    float oct_o1;
    float oct_o2;
    float oct_o3;
    float oct_o4;
    float oct_o5;
    float oct_o6;
    float oct_o7;
    float oct_o8;

    /// Create a pointer location for pixelData
    unsigned char* pixelData;               /// data location of final output

    unsigned int* perlinOutput;

    /// floats need
    float * diamondinput1;
    float * perlininput1;
    float * perlininput2;
    float * controlinput1;
    float * controloutput1;
};

#endif // PROCEDURAL_H
