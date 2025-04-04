#ifndef LOCALASSEMBLYMF_HPP
#define LOCALASSEMBLYMF_HPP

#include <petscmat.h>
#include "Element.hpp"
#include "ElementFEM.hpp"
#include "QuadraturePoint.hpp"

class LocalAssemblyMF
{
    public:
        PetscScalar *Floc;

        LocalAssemblyMF(const double &p, const double &q)
            : n((p+1)*(q+1))
        {
            quad1 = new QuadraturePoint(p+1, 0, 1);
            quad2 = new QuadraturePoint(q+1, 0, 1);
            Floc = new PetscScalar[n];
        }

        ~LocalAssemblyMF()
        {
            delete quad1; quad1 = nullptr;
            delete quad2; quad2 = nullptr;
            delete[] Floc; Floc = nullptr;
        }

        void AssemLocalStiffnessLoad(const Element * const &elem,
            const std::vector<double> &eCP);

    private:
        const int n;
        QuadraturePoint * quad1;
        QuadraturePoint * quad2;

        double Getf(const double &xi, const double &eta)
        {
            return xi*(1-xi)*eta*(1-eta);
        }

        void ResetStiffnessLoad()
        {
            for (int i = 0; i < n; ++i)
                Floc[i] = 0.0;
        }
};

#endif