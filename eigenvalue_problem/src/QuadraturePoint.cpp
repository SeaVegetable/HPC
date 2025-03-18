#include "QuadraturePoint.hpp"

QuadraturePoint::QuadraturePoint(const int &nqp, const int &left, const int &right)
    : nqp(nqp)
{
    switch (nqp)
    {
        case 1:
            qp = {0.0};
            w = {2.0};
            break;
        case 2:
            qp = {-0.5773502691896257, 0.5773502691896257};
            w = {1.0, 1.0};
            break;
        case 3:
            qp = {-0.7745966692414834, 0.0, 0.7745966692414834};
            w = {0.5555555555555556, 0.8888888888888888, 0.5555555555555556};
            break;
        case 4:
            qp = {-0.8611363115940526, -0.3399810435848563, 0.3399810435848563, 0.8611363115940526};
            w = {0.3478548451374538, 0.6521451548625461, 0.6521451548625461, 0.3478548451374538};
            break;
        case 5:
            qp = {-0.9061798459386640, -0.5384693101056831, 0.0, 0.5384693101056831, 0.9061798459386640};
            w = {0.2369268850561891, 0.4786286704993665, 0.5688888888888889, 0.4786286704993665, 0.2369268850561891};
            break;
        case 6:
            qp = {-0.9324695142031521, -0.6612093864662645, -0.2386191860831969, 0.2386191860831969, 0.6612093864662645, 0.9324695142031521};
            w = {0.1713244923791704, 0.3607615730481386, 0.4679139345726910, 0.4679139345726910, 0.3607615730481386, 0.1713244923791704};
            break;
        case 7:
            qp = {-0.9491079123427585, -0.7415311855993945, -0.4058451513773972, 0.0, 0.4058451513773972, 0.7415311855993945, 0.9491079123427585};
            w = {0.1294849661688697, 0.2797053914892766, 0.3818300505051189, 0.4179591836734694, 0.3818300505051189, 0.2797053914892766, 0.1294849661688697};
            break;
        case 8:
            qp = {-0.9602898564975363, -0.7966664774136267, -0.5255324099163290, -0.1834346424956498, 0.1834346424956498, 0.5255324099163290, 0.7966664774136267, 0.9602898564975363};
            w = {0.1012285362903763, 0.2223810344533745, 0.3137066458778873, 0.3626837833783620, 0.3626837833783620, 0.3137066458778873, 0.2223810344533745, 0.1012285362903763};
            break;
        case 9;
            qp = {-0.9681602395076261, -0.8360311073266358, -0.6133714327005904, -0.3242534234038089, 0.0, 0.3242534234038089, 0.6133714327005904, 0.8360311073266358, 0.9681602395076261};
            w = {0.0812743883615744, 0.1806481606948574, 0.2606106964029354, 0.3123470770400029, 0.3302393550012598, 0.3123470770400029, 0.2606106964029354, 0.1806481606948574, 0.0812743883615744};
            break;
        case 10:
            qp = {-0.9739065285171717, -0.8650633666889845, -0.6794095682990244, -0.4333953941292472, -0.1488743389816312, 0.1488743389816312, 0.4333953941292472, 0.6794095682990244, 0.8650633666889845, 0.9739065285171717};
            w = {0.0666713443086881, 0.1494513491505806, 0.2190863625159820, 0.2692667193099963, 0.2955242247147529, 0.2955242247147529, 0.2692667193099963, 0.2190863625159820, 0.1494513491505806, 0.0666713443086881};
            break;
        default:
            std::cerr << "Error: Number of quadrature points not supported" << std::endl;
            exit(1);
    }

    for (int i = 0; i < nqp; ++i)
    {
        qp[i] = 0.5 * (right - left) * qp[i] + 0.5 * (right + left);
        w[i] = 0.5 * (right - left) * w[i];
    }
}