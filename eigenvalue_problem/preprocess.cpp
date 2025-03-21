#include <filesystem>
#include "ControlPointGenerator.hpp"
#include "IENGenerator.hpp"
#include "IDGenerator.hpp"
#include "NURBSExtractionGenerator.hpp"
#include "Partition.hpp"

int main(int argc, char *argv[])
{
    int p = 2;
    int q = 2;

    double Lx = 1.0;
    double Ly = 1.0;

    int nElemX =7;
    int nElemY =7;

    int part_num_1d = 3;
    int dim = 2;
    std::string base_name = "part";

    std::string file_info = "info.txt";

    for (const auto& entry : std::__fs::filesystem::directory_iterator(std::__fs::filesystem::current_path())) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find(base_name) == 0 || filename == file_info) {
                std::cout << "Deleting file: " << filename << std::endl;
                std::__fs::filesystem::remove(entry.path());
            }
        }
    }
    
    FileManager * fm = new FileManager();
    fm->WritePreprocessInfo(file_info, p, q, Lx, Ly, nElemX, nElemY, part_num_1d, dim, base_name);

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

    std::cout << "Generating IEN..." << std::endl;
    IENGenerator * igen = new IENGenerator();
    std::vector<int> IEN = igen->GenerateIEN2D(basis1, basis2);

    std::cout << "Generating ID..." << std::endl;
    IDGenerator * idgen = new IDGenerator();
    std::vector<int> ID = idgen->GenerateID2D(basis1, basis2);

    std::cout << "Generating control points..." << std::endl;
    std::vector<double> CP = cpg->GenerateControlPoints2D(basis1, basis2, P1min, P1max, P2min, P2max);

    std::cout << "Generating NURBS extraction..." << std::endl;
    NURBSExtractionGenerator * neg = new NURBSExtractionGenerator();
    std::cout << "Generating extraction 1..." << std::endl;
    std::vector<double> NURBSExtraction1 = neg->GenerateExtraction1D(basis1);
    std::cout << "Generating extraction 2..." << std::endl;
    std::vector<double> NURBSExtraction2 = neg->GenerateExtraction1D(basis2);

    std::cout << "Generating partition..." << std::endl;
    Partition * part = new Partition(part_num_1d, part_num_1d, dim, base_name);
    part->GeneratePartition(basis1, basis2, CP, IEN, ID, NURBSExtraction1, NURBSExtraction2);

    delete cpg; cpg = nullptr;
    delete basis1; basis1 = nullptr;
    delete basis2; basis2 = nullptr;
    delete igen; igen = nullptr;
    delete idgen; idgen = nullptr;
    delete neg; neg = nullptr;
    delete part; part = nullptr;

    return 0;
}