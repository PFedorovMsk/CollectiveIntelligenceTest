#ifndef CERTIFICATION_H
#define CERTIFICATION_H

#include "Defines.h"


class Certification
{
public:
    Certification();

    void setData(const Array<int> id, const Table<Answer> &answers);

    void getResults(Array<int> &id, Array<double> &beta, Array<double> &theta,
                    Table<double> &probabilityRight, Table<double> &probabilityNotRight);

    const Array<int> &   id();
    const Array<double> &beta();
    const Array<double> &theta();
    const Table<double> &probabilityRight();
    const Table<double> &probabilityNotRight();


private:
    void run();
    void sortByRows();
    void sortByCols();
    void sortByRows(const Array<size_t> &indexes);
    void sortByCols(const Array<size_t> &indexes);
    void computeProbabilities();
    void fillCorners(size_t d);
    void iterate();

    Table<double> createBetaMinusTheta();
    bool isConvergence(const Array<double> &a, const Array<double> &b);
    void processBad(const Array<size_t> &indexes, Array<double> &arr);


private:
    bool m_runed;

    size_t m_experts;
    size_t m_questions;

    Table<Answer> m_answers;
    Array<int>    m_id;

    Array<double> m_beta;
    Array<double> m_theta;
    Table<double> m_probabilityRight;
    Table<double> m_probabilityNotRight;
};

#endif // CERTIFICATION_H
