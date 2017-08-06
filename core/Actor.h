#ifndef ACTOR_H
#define ACTOR_H

#include "Defines.h"


class Actor
{
public:
    Actor();
    Actor(int id, double thetaG, double thetaE, double ag, double ae);

    int id() const;

    double thetaG() const;
    double thetaE() const;

    double gr(double beta) const;
    double gn(double beta) const;
    double gs(double beta) const;
    double gv(double beta) const;

    double er(double beta) const;
    double en(double beta) const;
    double es(double beta) const;
    double ev(double beta) const;

    double iq(double beta) const;
    double ip(double beta, size_t &count, double &qr, double &qn, size_t maxCount,
              size_t iterations = 10000) const;


private:
    int    m_id;
    double m_thetaG;
    double m_thetaE;

    double m_ag;
    double m_ae;
};


#endif // ACTOR_H
