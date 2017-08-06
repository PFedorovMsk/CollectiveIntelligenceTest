#include "GroupActor.h"

#include <cassert>
#include <cmath>

UniformInteger GroupActor::randInt;
UniformReal    GroupActor::randReal;

GroupActor::GroupActor()
{
    randInt.setSeed(RandomProperties::randomSeed());
    randReal.setSeed(RandomProperties::randomSeed());
}

void GroupActor::compute(const Array<Actor> &actors, double beta, double &qr, double &qn, size_t iterations)
{
    Array<double> pr, pn;
    compute(actors, beta, qr, qn, pr, pn, iterations);
}

void GroupActor::compute(const Array<Actor> &actors, double beta, double &qr, double &qn, Array<double> &pr,
                         Array<double> &pn, size_t iterations)
{
    assert(iterations > 0);

    pr.resize(actors.size());
    pn.resize(actors.size());
    for (size_t i = 0; i < actors.size(); ++i) {
        pr[i] = pn[i] = 0.0;
    }

    size_t r = 0;
    size_t n = 0;

    Array<size_t> ar(actors.size(), 0);
    Array<size_t> an(actors.size(), 0);

    size_t        countRight;
    Array<size_t> countNotRight(actors.size());
    Array<Answer> answers(actors.size());

    for (size_t it = 0; it < iterations; ++it) {
        generation(actors, beta, answers);
        examination(actors, beta, answers, countRight, countNotRight);
        if (countRight > actors.size() / 2) {
            r += 1;
        }
        for (size_t i = 0; i < countNotRight.size(); ++i) {
            if (countNotRight[i] > actors.size() / 2) {
                n += 1;
            }
        }

        // ответы отдельных акторов после согласования:
        for (size_t i = 0; i < answers.size(); ++i) {
            if (answers[i] == Answer::RIGHT) {
                ar[i] += 1;
            } else if (answers[i] == Answer::NOT_RIGHT) {
                an[i] += 1;
            }
        }
    }

    qr = double(r) / iterations;
    qn = double(n) / iterations;

    for (size_t i = 0; i < answers.size(); ++i) {
        pr[i] = double(ar[i]) / iterations;
        pn[i] = double(an[i]) / iterations;
    }
}

double GroupActor::groupIP(const Array<Actor> &actors, double beta, double &qr, double &qn, size_t iterations)
{
    compute(actors, beta, qr, qn, iterations);
    return qr * 1000.0;
}

Array<Actor> GroupActor::searchOptimalGroup(Array<Actor> &allActors, double beta, size_t count)
{
    size_t first = 0;

    while (first < allActors.size() - count) {
        Array<Actor> group(count);
        for (size_t i = first; i < first + count; ++i) {
            group[i - first] = allActors[i];
        }

        double q = 0, qn = 0;
        compute(group, beta, q, qn, 10000);
        if (q >= 0.99) {
            return group;
        }
        ++first;
    }

    return Array<Actor>(0);
}

// --- private methods:

void GroupActor::computeAnswersCount(const Array<Answer> &answers, size_t &countRight, Array<size_t> &countNotRight)
{
    countRight = 0;
    countNotRight.resize(answers.size());
    for (size_t i = 0; i < answers.size(); ++i) {
        countNotRight[i] = 0;
    }

    for (size_t i = 0; i < answers.size(); ++i) {
        if (answers[i] == Answer::RIGHT) {
            countRight += 1;
        } else if (answers[i] == Answer::NOT_RIGHT) {
            size_t index = randInt(0, answers.size() - 1);
            countNotRight[index] += 1;
        }
    }
}

void GroupActor::generation(const Array<Actor> &actors, double beta, Array<Answer> &answers)
{
    answers.resize(actors.size());
    for (size_t i = 0; i < actors.size(); ++i) {
        double p = randReal(0.0, 1.0);
        if (p < actors[i].gr(beta)) {
            answers[i] = Answer::RIGHT;
        } else if (p < actors[i].gs(beta)) {
            answers[i] = Answer::NOT_RIGHT;
        } else {
            answers[i] = Answer::NO;
        }
    }
}

void GroupActor::examination(const Array<Actor> &actors, double beta, Array<Answer> &answers, size_t &countRight,
                             Array<size_t> &countNotRight)
{
    size_t itExam = actors.size() / 2;
    if (actors.size() % 2 == 1) {
        itExam += 1;
    }

    for (size_t j = 0; j < itExam; ++j) {
        computeAnswersCount(answers, countRight, countNotRight);

        Array<size_t> indexesNotRight;
        for (size_t i = 0; i < actors.size(); ++i) {
            if (countNotRight[i] > 0) {
                indexesNotRight.push_back(i);
            }
        }

        for (size_t i = 0; i < actors.size(); ++i) {
            if (answers[i] == Answer::NO) {
                double p = randReal(0.0, 1.0);
                if (p < actors[i].er(beta)) {
                    if (countRight > 0) {
                        answers[i] = Answer::RIGHT;
                    }
                } else if (p < actors[i].es(beta) && indexesNotRight.size() > 0) {
                    size_t index = indexesNotRight[randInt(0, indexesNotRight.size() - 1)];
                    answers[i]   = answers[index];
                }
            }
        }
    }
    computeAnswersCount(answers, countRight, countNotRight);
}
