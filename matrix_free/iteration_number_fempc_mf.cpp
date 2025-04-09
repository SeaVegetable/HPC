#include <petscksp.h>
#include <petscpc.h>
#include "FileManager.hpp"
#include "GlobalAssemblyMF.hpp"
#include "GlobalAssembly.hpp"

typedef struct {
    KSP innerksp;
} MyPCCtx;

PetscErrorCode MyPCApply(PC pc, Vec x, Vec y)
{
    MyPCCtx *ctx;
    PCShellGetContext(pc, (void**)&ctx);
    KSPSolve(ctx->innerksp, x, y);
    return 0;
}

PetscErrorCode MyPCDestroy(PC pc)
{
    MyPCCtx *ctx;
    PCShellGetContext(pc, (void**)&ctx);
    KSPDestroy(&ctx->innerksp);
    PetscFree(ctx);
    return 0;
}

typedef struct {
    std::vector<double> CP;
    std::vector<int> ID;
    std::vector<int> ghostID;
    std::vector<int> IEN;
    std::vector<double> elem_size1;
    std::vector<double> elem_size2;
    std::vector<double> NURBSExtraction1;
    std::vector<double> NURBSExtraction2;
    GlobalAssemblyMF * globalassem;
    LocalAssemblyMF * locassem;
    ElementMF * elem;
} MyMeshData;

PetscErrorCode MyMatMult(Mat A, Vec x, Vec y)
{
    MyMeshData *data;
    MatShellGetContext(A, (void**)&data);
    
    data->globalassem->MatMulMF(data->locassem,
        data->IEN, data->ID, data->CP,
        data->NURBSExtraction1, data->NURBSExtraction2,
        data->elem_size1, data->elem_size2,
        data->elem, x, y);

    return 0;
}

int main(int argc, char *argv[])
{
    int p, q, nElemX, nElemY, part_num_1d, dim;
    double Lx, Ly;
    std::string base_name;

    std::string file_info = "info.txt";

    FileManager * fm = new FileManager();
    fm->ReadPreprocessInfo(file_info, p, q, Lx, Ly, nElemX, nElemY, part_num_1d, dim, base_name);

    PetscInitialize(&argc, &argv, NULL, NULL);

    PetscMPIInt rank, size;
    MPI_Comm_rank(PETSC_COMM_WORLD, &rank);
    MPI_Comm_size(PETSC_COMM_WORLD, &size);

    if (rank == 0)
    {
        std::cout << "p: " << p << std::endl;
        std::cout << "q: " << q << std::endl;
        std::cout << "Lx: " << Lx << std::endl;
        std::cout << "Ly: " << Ly << std::endl;
        std::cout << "nElemX: " << nElemX << std::endl;
        std::cout << "nElemY: " << nElemY << std::endl;
        std::cout << "part_num_1d: " << part_num_1d << std::endl;
        std::cout << "dim: " << dim << std::endl;
        std::cout << "base_name: " << base_name << std::endl;
    }

    int nlocalfunc;
    int nlocalelemx;
    int nlocalelemy;
    ElementMF * elemmf = new ElementMF(p, q);
    const int nLocBas = elemmf->GetNumLocalBasis();
    LocalAssemblyMF * locassemmf = new LocalAssemblyMF(p, q);

    MyMeshData *data;
    PetscNew(&data);
    
    std::string filename = fm->GetPartitionFilename(base_name, rank);
    fm->ReadPartition(filename, nlocalfunc,
        nlocalelemx, nlocalelemy,
        data->elem_size1, data->elem_size2,
        data->CP, data->ID, data->ghostID, data->IEN,
        data->NURBSExtraction1, data->NURBSExtraction2);
    
    data->elem = elemmf;
    data->locassem = locassemmf;
    data->globalassem = new GlobalAssemblyMF(nLocBas, nlocalfunc,
        nlocalelemx, nlocalelemy, data->ghostID);

    data->globalassem->AssemLoad(data->locassem, data->IEN,
        data->ID, data->CP,
        data->NURBSExtraction1, data->NURBSExtraction2,
        data->elem_size1, data->elem_size2, elemmf);

    MPI_Barrier(PETSC_COMM_WORLD);

    std::vector<double> CP_fem;
    std::vector<int> ID_fem;
    std::vector<int> IEN_fem;
    int nlocalfunc_fem;
    int nlocalelemx_fem;
    int nlocalelemy_fem;

    std::string base_name_fem = "part_fem";
    std::string filename_fem = fm->GetPartitionFilename(base_name_fem, rank);
    fm->ReadPartition(filename_fem, nlocalfunc_fem,
        nlocalelemx_fem, nlocalelemy_fem,
        CP_fem, ID_fem, IEN_fem);

    ElementFEM * elem_fem = new ElementFEM(1, 1);
    LocalAssembly * locassem_fem = new LocalAssembly(1, 1);
    GlobalAssembly * globalassem_fem = new GlobalAssembly(IEN_fem, ID_fem, locassem_fem,
        4, nlocalfunc_fem, nlocalelemx_fem, nlocalelemy_fem);
    
    MatSetOption(globalassem_fem->K, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_TRUE);
    
    globalassem_fem->AssemStiffnessLoad(locassem_fem, IEN_fem, ID_fem, CP_fem, elem_fem);

    MPI_Barrier(PETSC_COMM_WORLD);
    PetscPrintf(PETSC_COMM_WORLD, "Assembling stiffness matrix and load vector...done\n");

    PetscLogDouble tstart, tend;
    PetscTime(&tstart);

    Mat K;
    MatCreateShell(PETSC_COMM_WORLD, nlocalfunc, nlocalfunc,
        PETSC_DECIDE, PETSC_DECIDE, data, &K);
    MatShellSetOperation(K, MATOP_MULT, (void(*)(void))MyMatMult);

    KSP ksp;
    KSPCreate(PETSC_COMM_WORLD, &ksp);
    KSPSetOperators(ksp, K, K);
    KSPSetFromOptions(ksp);

    PC pc;
    KSPGetPC(ksp, &pc);
    PCSetType(pc, PCSHELL);

    MyPCCtx *ctx;
    PetscNew(&ctx);
    KSPCreate(PETSC_COMM_WORLD, &ctx->innerksp);
    KSPSetOperators(ctx->innerksp, globalassem_fem->K, globalassem_fem->K);
    KSPSetType(ctx->innerksp, KSPCG);

    PC innerpc;
    KSPGetPC(ctx->innerksp, &innerpc);
    PCSetType(innerpc, PCJACOBI);
    PCSetFromOptions(innerpc);

    PetscReal rtol = 1e-10;
    PetscReal abstol = 1e-10;
    PetscReal divtol = 1e4;
    PetscInt maxits = 10000;
    KSPSetTolerances(ctx->innerksp, rtol, abstol, divtol, maxits);
    KSPSetTolerances(ksp, rtol, abstol, divtol, maxits);

    PCShellSetContext(pc, ctx);
    PCShellSetApply(pc, MyPCApply);
    PCShellSetDestroy(pc, MyPCDestroy);

    Vec u;
    VecDuplicate(data->globalassem->F, &u);
    KSPSetFromOptions(ksp);
    KSPSolve(ksp, data->globalassem->F, u);

    // PetscTime(&tend);
    // PetscLogDouble time = tend - tstart;
    // PetscPrintf(PETSC_COMM_WORLD, "Time: %f\n", time);

    // PetscInt num_iterations;
    // KSPGetIterationNumber(ksp, &num_iterations);

    // if (rank == 0)
    // {
    //     std::cout << "Number of KSP iterations: " << num_iterations << std::endl;
    // }

    // delete fm; fm = nullptr;
    // delete data->elem; data->elem = nullptr;
    // delete data->locassem; data->locassem = nullptr;
    // delete data->globalassem; data->globalassem = nullptr;
    // delete data; data = nullptr;
    // delete elem_fem; elem_fem = nullptr;
    // delete locassem_fem; locassem_fem = nullptr;
    // delete globalassem_fem; globalassem_fem = nullptr;

    // VecDestroy(&u);
    // KSPDestroy(&ksp);
    
    PetscFinalize();
    return 0;
}