#ifndef ACTORSBASE_H
#define ACTORSBASE_H

#include "Actor.h"
#include "Certification.h"

#include <algorithm>


class ActorsBase
{
public:
    ActorsBase();

    void setData(const Table<Answer> &answersToOpened, const Table<Answer> &answersToClosed);

    Array<Actor> getActors() const;

private:
    void clear();


private:
    Array<int> m_idG;
    Array<int> m_idE;

    Array<double> m_betaG;
    Array<double> m_betaE;
    Array<double> m_thetaG;
    Array<double> m_thetaE;

    Table<double> m_pGr;
    Table<double> m_pGn;
    Table<double> m_pEr;
    Table<double> m_pEn;
};

#endif // ACTORSBASE_H
