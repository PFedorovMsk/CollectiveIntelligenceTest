#include "Certification.h"

#include <algorithm>
#include <cmath>


Certification::Certification()
    : m_runed(false)
    , m_experts(0)
    , m_questions(0)
{
}


// ---- public methdods:

void Certification::setData(const Array<int> id, const Table<Answer> &answers)
{
    m_experts = id.size();
    assert(m_experts > 0);
    assert(answers.size() == m_experts);

    m_questions = answers[0].size();
    assert(m_questions > 0);

    m_id      = id;
    m_answers = answers;

    m_beta.clear();
    m_beta.resize(m_questions, 0.0);

    m_theta.clear();
    m_theta.resize(m_experts, 0.0);

    m_probabilityRight.clear();
    m_probabilityRight.resize(m_experts, Array<double>(m_questions, 0.0));

    m_probabilityNotRight.clear();
    m_probabilityNotRight.resize(m_experts, Array<double>(m_questions, 0.0));

    m_runed = false;
}

void Certification::getResults(Array<int> &id, Array<double> &beta, Array<double> &theta,
                               Table<double> &probabilityRight, Table<double> &probabilityNotRight)
{
    if (!m_runed) {
        run();
    }

    id                  = m_id;
    beta                = m_beta;
    theta               = m_theta;
    probabilityRight    = m_probabilityRight;
    probabilityNotRight = m_probabilityNotRight;
}

const Array<int> &Certification::id()
{
    if (!m_runed) {
        run();
    }
    return m_id;
}

const Array<double> &Certification::beta()
{
    if (!m_runed) {
        run();
    }
    return m_beta;
}

const Array<double> &Certification::theta()
{
    if (!m_runed) {
        run();
    }
    return m_theta;
}

const Table<double> &Certification::probabilityRight()
{
    if (!m_runed) {
        run();
    }
    return m_probabilityRight;
}

const Table<double> &Certification::probabilityNotRight()
{
    if (!m_runed) {
        run();
    }
    return m_probabilityNotRight;
}


// ---- private methdods:

void Certification::run()
{
    sortByRows();
    sortByCols();
    computeProbabilities();
    iterate();

    m_runed = true;
}

using Pair = std::pair<int, size_t>;

void Certification::sortByRows()
{
    Array<Pair> sum(m_experts);
    for (size_t i = 0; i < m_experts; ++i) {
        sum[i].first  = 0;
        sum[i].second = i;
        for (size_t j = 0; j < m_questions; ++j) {
            if (m_answers[i][j] == Answer::RIGHT) {
                sum[i].first += 1;
            }
        }
    }

    static struct {
        bool operator()(const Pair &a, const Pair &b) const
        {
            return a.first < b.first;
        }
    } comparator;
    std::sort(sum.begin(), sum.end(), comparator);

    Array<size_t> indexes(m_experts, 0);
    for (size_t i = 0; i < m_experts; ++i) {
        indexes[i] = sum[i].second;
    }

    sortByRows(indexes);
}

void Certification::sortByRows(const Array<size_t> &indexes)
{
    auto tmpId                  = m_id;
    auto tmpAnswers             = m_answers;
    auto tmpTheta               = m_theta;
    auto tmpProbabilityRight    = m_probabilityRight;
    auto tmpProbabilityNotRight = m_probabilityNotRight;

    for (size_t i = 0; i < m_experts; ++i) {
        size_t k                 = indexes[i];
        m_id[i]                  = tmpId[k];
        m_answers[i]             = tmpAnswers[k];
        m_theta[i]               = tmpTheta[k];
        m_probabilityRight[i]    = tmpProbabilityRight[k];
        m_probabilityNotRight[i] = tmpProbabilityNotRight[k];
    }
}

void Certification::sortByCols()
{
    Array<Pair> sum(m_questions);
    for (size_t j = 0; j < m_questions; ++j) {
        sum[j].first  = 0;
        sum[j].second = j;
        for (size_t i = 0; i < m_experts; ++i) {
            if (m_answers[i][j] == Answer::RIGHT) {
                sum[j].first += 1;
            }
        }
    }

    static struct {
        bool operator()(const Pair &a, const Pair &b) const
        {
            return a.first < b.first;
        }
    } comparator;
    std::sort(sum.begin(), sum.end(), comparator);

    Array<size_t> indexes(m_questions, 0);
    for (size_t j = 0; j < m_questions; ++j) {
        indexes[j] = sum[j].second;
    }

    sortByCols(indexes);
}

void Certification::sortByCols(const Array<size_t> &indexes)
{
    auto tmpAnswers             = m_answers;
    auto tmpProbabilityRight    = m_probabilityRight;
    auto tmpProbabilityNotRight = m_probabilityNotRight;
    auto tmpBeta                = m_beta;

    for (size_t j = 0; j < m_questions; ++j) {
        size_t k = indexes[j];
        for (size_t i = 0; i < m_experts; ++i) {
            m_answers[i][j]             = tmpAnswers[i][k];
            m_probabilityRight[i][j]    = tmpProbabilityRight[i][k];
            m_probabilityNotRight[i][j] = tmpProbabilityNotRight[i][k];
        }
        m_beta[j] = tmpBeta[k];
    }
}

void Certification::computeProbabilities()
{
    size_t d = std::min(m_experts, m_questions) / 3;

    for (size_t i = 0; i < m_experts; ++i) {
        for (size_t j = 0; j < m_questions; ++j) {
            size_t l = d;
            while (int(i) - int(l) < 0 || i + l >= m_experts || int(j) - int(l) < 0 ||
                   j + l >= m_questions) {
                l -= 1;
            }

            double sumNotRight = 0.0;
            double sumRight    = 0.0;
            for (size_t p = i - l; p <= i + l; ++p) {
                for (size_t r = j - l; r <= j + l; ++r) {
                    if (m_answers[p][r] == Answer::RIGHT) {
                        sumRight += 1.0;
                    } else if (m_answers[p][r] == Answer::NOT_RIGHT) {
                        sumNotRight += 1.0;
                    }
                }
            }
            m_probabilityRight[i][j]    = sumRight / std::pow(2.0 * l + 1.0, 2.0);
            m_probabilityNotRight[i][j] = sumNotRight / std::pow(2.0 * l + 1.0, 2.0);
        }
    }

    fillCorners(d);
}

void Certification::fillCorners(size_t d)
{
    // левый верхний угол:
    for (size_t i = 0; i < d; ++i) {
        for (size_t j = 0; j < d - i; ++j) {
            m_probabilityRight[i][j]    = 0.0;
            m_probabilityNotRight[i][j] = 1.0; // WARNING
        }
    }

    // правый нижний угол:
    for (size_t i = m_experts - 1; i >= m_experts - d; --i) {
        for (size_t j = m_experts + m_questions - d - i - 1; j < m_questions; ++j) {
            m_probabilityRight[i][j]    = 1.0;
            m_probabilityNotRight[i][j] = 0.0; // WARNING
        }
    }
}

inline void pushWithoutRepeats(Array<size_t> &arr, size_t value)
{
    for (auto a : arr) {
        if (a == value) {
            return;
        }
    }
    arr.push_back(value);
}

void Certification::iterate()
{
    Array<size_t> badTheta, badBeta;

    auto   betaMinusTheta = createBetaMinusTheta();
    size_t iter           = 0;

    while (iter < 100) {
        std::vector<double> tmpTheta = m_theta;
        std::vector<double> tmpBeta  = m_beta;

        for (size_t i = 0; i < m_experts; ++i) {
            double theta = 0.0;
            int    m     = 0;
            for (size_t j = 0; j < m_questions; ++j) {
                if (!std::isinf(betaMinusTheta[i][j]) && !std::isnan(betaMinusTheta[i][j])) {
                    m += 1;
                    theta += (m_beta[j] - betaMinusTheta[i][j]);
                }
            }
            if (m > 0) {
                m_theta[i] = theta / m;
            } else {
                pushWithoutRepeats(badTheta, i);
            }
        }

        double thetaS = 0.0;
        for (size_t i = 0; i < m_theta.size(); ++i) {
            thetaS += m_theta[i];
        }
        thetaS /= m_theta.size();

        for (size_t i = 0; i < m_theta.size(); ++i) {
            m_theta[i] -= thetaS;
        }

        for (size_t j = 0; j < m_questions; ++j) {
            double beta = 0.0;
            int    n    = 0;
            for (size_t i = 0; i < m_experts; ++i) {
                if (!std::isinf(betaMinusTheta[i][j]) && !std::isnan(betaMinusTheta[i][j])) {
                    n += 1;
                    beta += (m_theta[i] + betaMinusTheta[i][j]);
                }
            }
            if (n > 0) {
                m_beta[j] = beta / n;
            } else {
                pushWithoutRepeats(badBeta, j);
            }
        }

        if (isConvergence(m_beta, tmpBeta) && isConvergence(m_theta, tmpTheta)) {
            break;
        }
        iter += 1;
    }

    processBad(badTheta, m_theta);
    processBad(badBeta, m_beta);
}

Table<double> Certification::createBetaMinusTheta()
{
    Table<double> betaMinusTheta(m_experts, Array<double>(m_questions, 0.0));
    for (size_t i = 0; i < m_experts; ++i) {
        for (size_t j = 0; j < m_questions; ++j) {
            if (0.0 < m_probabilityRight[i][j] && m_probabilityRight[i][j] < 1.0) {
                betaMinusTheta[i][j] =
                    std::log((1.0 - m_probabilityRight[i][j]) / m_probabilityRight[i][j]);
            } else {
                betaMinusTheta[i][j] = std::numeric_limits<double>::infinity();
            }
        }
    }
    return betaMinusTheta;
}

bool Certification::isConvergence(const Array<double> &a, const Array<double> &b)
{
    double sub = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        sub += std::fabs(std::fabs(a[i]) - std::fabs(b[i]));
    }

    if (sub < 0.0000001) {
        return true;
    }
    return false;
}

void Certification::processBad(const Array<size_t> &indexes, Array<double> &arr)
{
    size_t        mid = arr.size() / 2;
    Array<size_t> leftIndexes, rightIndexes;
    for (size_t i = 0; i < indexes.size(); ++i) {
        if (indexes[i] < mid) {
            leftIndexes.push_back(indexes[i]);
        } else {
            rightIndexes.push_back(indexes[i]);
        }
    }

    if (leftIndexes.size() > 0) {
        std::sort(leftIndexes.begin(), leftIndexes.end());
        size_t i0 = leftIndexes[leftIndexes.size() - 1];

        double d = arr[i0 + 1] - arr[i0 + 2];
        for (size_t i = i0; int(i) >= 0; --i) {
            arr[i] = arr[i + 1] + 0.1 * d;
        }
    }

    if (rightIndexes.size() > 0) {
        std::sort(rightIndexes.begin(), rightIndexes.end());
        size_t i0 = rightIndexes[0];

        double d = arr[i0 - 1] - arr[i0 - 2];
        for (size_t i = i0; i < arr.size(); ++i) {
            arr[i] = arr[i - 1] + 0.1 * d;
        }
    }
}
