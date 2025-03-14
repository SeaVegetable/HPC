#include <iostream>
#include "ControlPointGenerator.hpp"
// #include "IENGenerator.hpp"
// #include "IDGenerator.hpp"
// #include "NURBSExtractionGenerator.hpp"

int main(int argc, char *argv[])
{
    int p = 3;
    int q = 3;

    double Lx = 1.0;
    double Ly = 1.0;

    int nElemX = 10;
    int nElemY = 10;

    double hx = Lx / nElemX;
    double hy = Ly / nElemY;

    int nElem = nElemX * nElemY;
    int nLocBas = (p + 1) * (q + 1);
    
    int nFuncX = p + nElemX;
    int nFuncY = q + nElemY;
    int nFunc = nFuncX * nFuncY;

    std::vector<double> S;
    std::vector<double> T;

    for (int i = 0; i < p + 1; ++i) S.push_back(0.0);
    for (int i = 1; i < nElemX; ++i) S.push_back(i * hx);
    for (int i = 0; i < p + 1; ++i) S.push_back(Lx);

    for (int i = 0; i < q + 1; ++i) T.push_back(0.0);
    for (int i = 1; i < nElemY; ++i) T.push_back(i * hy);
    for (int i = 0; i < q + 1; ++i) T.push_back(Ly);

    ControlPointGenerator * cpg = new ControlPointGenerator();
    BSplineBasis * basis1 = new BSplineBasis(p, S);
    BSplineBasis * basis2 = new BSplineBasis(q, T);

    double P1min = 0.0;
    double P1max = Lx;
    double P2min = 0.0;
    double P2max = Ly;

    // std::vector<double> CP_1D = cpg->GenerateControlPoints1D(basis1, P1max, P1min);

    // for (int i = 0; i < CP_1D.size(); ++i)
    // {
    //     std::cout << CP_1D[i] << std::endl;
    // }

    std::vector<double> CP = cpg->GenerateControlPoints2D(basis1, basis2, P1min, P1max, P2min, P2max);

    for (int i = 0; i < CP.size(); i += 2)
    {
        std::cout << CP[i] << ' ' << CP[i+1] << std::endl;
    }

    // IENGenerator * igen = new IENGenerator();
    // std::vector<int> IEN = igen->GenerateIEN2D(basis1, basis2);

    // IDGenerator * idgen = new IDGenerator();
    // std::vector<int> ID = idgen->GenerateID2D(basis1, basis2);

    // NURBSExtractionGenerator * neg = new NURBSExtractionGenerator();
    // std::vector<int> NURBSExtraction1 = neg->GenerateNURBSExtraction(basis1);
    // std::vector<int> NURBSExtraction2 = neg->GenerateNURBSExtraction(basis2);
}