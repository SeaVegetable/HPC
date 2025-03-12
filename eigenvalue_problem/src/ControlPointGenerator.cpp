#include "ControlPointGenerator.hpp"

std::vector<double> ControlPointGenerator::GenerateControlPoints1D(
    const BSplineBasis * const &basis,
    const double &Pmax, const double &Pmin)
{
    const int p = basis->GetDegree();
    const std::vector<double> S = basis->GetKnotVector();
    const int nFunc = static_cast<int>(S.size()) - p - 1;

    const double ds = (S[p+1] - S[p]) / (p + 1);
    std::vector<double> s{};
    for (int i = 0; i < p; ++i) s.push_back(S[p]+(i+1)*ds);

    const double J = (Pmax - Pmin) / (S[nFunc] - S[p]);

    Eigen::MatrixXd A(p, p);
    A.setZero();
    Eigen::VectorXd b(p);
    b.setConstant(J);
    for (int i = 0; i < p; ++i)
    {
        int span = basis->FindSpan(s[i]);
        std::vector<double> dN = basis->DerBasisFuns(s[i], span, 1);
        std::vector<double> dNl(dN.begin()+1, dN.end());
        Eigen::Map<Eigen::RowVectorXd> rowMap(dNl.data(), dNl.size());
        A.row(i) = rowMap;
        b(i) -= Pmin dN[0];
    }
    VectorXd x = A.partialPivLu().solve(b);
    std::vector<double> solution(x.data(), x.data() + x.size());

    std::vector<double> CP{};
    CP.push_back(Pmin);
    for (int i = 0; i < p; ++i) CP.push_back(solution[i]);
    int L = nFunc - 2 * p;
    if (L > 0)
    {
        double d = (Pmax - Pmin - 2 * (CP.back())) / (L + 1);
        for (int i = 0; i < L; ++i) CP.push_back(CP.back() + d);
    }
    for (int i = p; i > 0; --i) CP.push_back(Pmax - (Pmin - CP[i]));
    CP.push_back(Pmax);

    return CP;
}

std::vector<double> ControlPointGenerator::GenerateControlPoints2D(
    const BSplineBasis * const &basis1,
    const BSplineBasis * const &basis2,
    const double &P1max, const double &P1min,
    const double &P2max, const double &P2min)
{
    std::vector<double> CP1 = GenerateControlPoints1D(basis1, P1max, P1min);
    std::vector<double> CP2 = GenerateControlPoints1D(basis2, P2max, P2min);

    std::vector<double> CP{};
    for (auto i : CP2)
    {
        for (auto j : CP1)
        {
            CP.push_back(i);
            CP.push_back(j);
        }
    }

    return CP;
}