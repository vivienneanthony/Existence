#ifndef OLSENNOISE2D_H
#define OLSENNOISE2D_H

class OlsenNoise2D
{

public:
    OlsenNoise2D();
    virtual ~OlsenNoise2D();

    int * olsennoise(int x, int y, int width, int height);

private:
    int hashrandom(std::vector<long long int> elements);
    long long hash(long long v);

};

#endif // OLSENNOISE2D_H
