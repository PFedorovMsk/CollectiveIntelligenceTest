#include "ActorsBase.h"


ActorsBase::ActorsBase()
{
    clear();
}

void ActorsBase::clear()
{
    m_idG.clear();
    m_idE.clear();
    m_betaG.clear();
    m_betaE.clear();
    m_thetaG.clear();
    m_thetaE.clear();
    m_pGr.clear();
    m_pGn.clear();
    m_pEr.clear();
    m_pEn.clear();
}

void ActorsBase::setData(const Table<Answer> &answersToOpened, const Table<Answer> &answersToClosed)
{
    assert(answersToClosed.size() == answersToOpened.size());

    clear();

    m_idG.resize(answersToOpened.size());
    for (size_t i = 0; i < m_idG.size(); ++i) {
        m_idG[i] = 10001 + int(i);
    }
    m_idE = m_idG;

    Certification certification;

    certification.setData(m_idG, answersToOpened);
    certification.getResults(m_idG, m_betaG, m_thetaG, m_pGr, m_pGn);

    certification.setData(m_idE, answersToClosed);
    certification.getResults(m_idE, m_betaE, m_thetaE, m_pEr, m_pEn);
}

inline double linearInterpolation(size_t index, double beta, const Array<double> &arrBeta, const Table<double> &values)
{
    size_t i0 = 0;
    size_t i1 = 0;
    for (size_t i = 0; i < arrBeta.size() - 1; ++i) {
        if (arrBeta[i] > beta && beta > arrBeta[i + 1]) {
            i0 = i;
            i1 = i + 1;
            break;
        }
    }
    if (i0 == i1) {
        if (beta > arrBeta[0]) {
            i0 = 0;
            i1 = 1;
        } else {
            i0 = arrBeta.size() - 2;
            i1 = arrBeta.size() - 1;
        }
    }

    double b0 = arrBeta[i0];
    double b1 = arrBeta[i1];
    double v0 = values[index][i0];
    double v1 = values[index][i1];

    return v0 + (v1 - v0) * (beta - b0) / (b1 - b0);
}

Array<Actor> ActorsBase::getActors() const
{
    Array<Actor> actors(m_thetaG.size());

    for (size_t i = 0; i < actors.size(); ++i) {
        int    id     = m_idG[i];
        double thetaG = m_thetaG[i];
        size_t J      = 0;
        double thetaE = 0.0;
        for (size_t j = 0; j < actors.size(); ++j) {
            if (id == m_idE[j]) {
                J = j;
                break;
            }
        }
        thetaE = m_thetaE[J];

        double ag = linearInterpolation(i, thetaG, m_betaG, m_pGn);
        double ae = linearInterpolation(J, thetaE, m_betaE, m_pEn);

        actors[i] = Actor(id, thetaG, thetaE, ag, ae);
    }

    return actors;
}
