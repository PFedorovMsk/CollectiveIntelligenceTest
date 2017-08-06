#ifndef GROUPACTOR_H
#define GROUPACTOR_H

#include "Actor.h"
#include "Random.h"

class GroupActor
{
public:
    GroupActor();

    static void compute(const Array<Actor> &actors, double beta, double &qr, double &qn, size_t iterations);
    static void compute(const Array<Actor> &actors, double beta, double &qr, double &qn, Array<double> &pr,
                        Array<double> &pn, size_t iterations);
    static double groupIP(const Array<Actor> &actors, double beta, double &qr, double &qn, size_t iterations);
    static Array<Actor> searchOptimalGroup(Array<Actor> &allActors, double beta, size_t count);

private:
    static void computeAnswersCount(const Array<Answer> &answers, size_t &countRight, Array<size_t> &countNotRight);
    static void generation(const Array<Actor> &actors, double beta, Array<Answer> &answers);
    static void examination(const Array<Actor> &actors, double beta, Array<Answer> &answers, size_t &countRight,
                            Array<size_t> &countNotRight);

private:
    static UniformInteger randInt;
    static UniformReal    randReal;

    friend class Actor;
};

#endif // GROUPACTOR_H
