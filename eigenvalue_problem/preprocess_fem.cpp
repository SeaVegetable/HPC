#include <filesystem>
#include "AbscissaeGenerator.hpp"
#include "IENGenerator.hpp"
#include "IDGenerator.hpp"
#include "Partition.hpp"

int main(int argc, char *argv[])
{
    int p, q, nElemX, nElemY, part_num_1d, dim;
    double Lx, Ly;
    std::string base_name;

    std::string file_info = "info.txt";

    FileManager * fm = new FileManager();
    fm->ReadPreprocessInfo(file_info, p, q, Lx, Ly, nElemX, nElemY, part_num_1d, dim, base_name);

    std::string base_name_fem = "part_fem";
    
    for (const auto& entry : std::__fs::filesystem::directory_iterator(std::__fs::filesystem::current_path())) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find(base_name_fem) == 0) {
                std::cout << "Deleting file: " << filename << std::endl;
                std::__fs::filesystem::remove(entry.path());
            }
        }
    }

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

    IENGenerator * ien = new IENGenerator();
    std::vector<int> IEN = ien->GenerateIEN(nElemX, nElemY, p, q);

    IDGenerator * idgen = new IDGenerator();
    std::vector<int> ID = idgen->GenerateID(nFuncX, nFuncY, nFunc);

    AbscissaeGenerator * absgen = new AbscissaeGenerator();
    std::vector<double> CP = absgen->GenerateAbscissae2D(S, T, p, q);

    Partition * part = new Partition(part_num_1d, part_num_1d, dim, base_name_fem);
    part->GeneratePartition(p, q, hx, hy, CP, ID, IEN);

    delete fm; fm = nullptr;
    delete ien; ien = nullptr;
    delete idgen; idgen = nullptr;
    delete absgen; absgen = nullptr;
    delete part; part = nullptr;
}