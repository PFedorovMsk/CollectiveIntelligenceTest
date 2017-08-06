#include "Actor.h"

#include "GroupActor.h"

#include <cmath>


Actor::Actor()
    : m_id(-1)
    , m_thetaG(0.0)
    , m_thetaE(0.0)
    , m_ag(0.5)
    , m_ae(0.5)
{
}

Actor::Actor(int id, double thetaG, double thetaE, double ag, double ae)
    : m_id(id)
    , m_thetaG(thetaG)
    , m_thetaE(thetaE)
    , m_ag(ag)
    , m_ae(ae)
{
}

int Actor::id() const
{
    return m_id;
}

double Actor::thetaG() const
{
    return m_thetaG;
}

double Actor::thetaE() const
{
    return m_thetaE;
}

double Actor::gr(double beta) const
{
    return 1.0 / (1.0 + std::exp(beta - m_thetaG));
}

double Actor::gn(double beta) const
{
    return m_ag * std::exp(-1.0 * std::pow(beta - m_thetaG, 2.0));
}

double Actor::gs(double beta) const
{
    return gr(beta) + gn(beta);
}

double Actor::gv(double beta) const
{
    return 1.0 - gs(beta);
}

double Actor::er(double beta) const
{
    return 1.0 / (1.0 + std::exp(beta - m_thetaE));
}

double Actor::en(double beta) const
{
    return m_ae * std::exp(-1.0 * std::pow(beta - m_thetaE, 2.0));
}

double Actor::es(double beta) const
{
    return er(beta) + en(beta);
}

double Actor::ev(double beta) const
{
    return 1.0 - es(beta);
}

double Actor::iq(double beta) const
{
    return 90.0 + 165.0 * 25.0 * (gr(beta) + er(beta)) / 90.0;
}

double Actor::ip(double beta, size_t &count, double &qr, double &qn, size_t maxCount,
                 size_t iterations) const
{
    assert(iterations > 0);
    assert(maxCount >= 3);

    qr    = 0.0;
    qn    = 0.0;
    count = 3;

    do {
        std::vector<Actor> arr(count, *this);

        GroupActor::compute(arr, beta, qr, qn, iterations);
        if (qr >= 0.99) {
            return 1000.0 / count;
        }

        if (count + 2 > maxCount) {
            return 0.0;
        }
        count += 2;
    } while (true);
}
