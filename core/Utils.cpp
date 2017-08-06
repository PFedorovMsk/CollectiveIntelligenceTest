#include "Utils.h"

#include "Random.h"


#include <QFile>
#include <QTextStream>


void GenerateTable(size_t experts, size_t questions, Table<Answer> &table)
{
    static UniformReal randReal;
    static bool        seedIsSeted = false;
    if (!seedIsSeted) {
        randReal.setSeed(RandomProperties::randomSeed());
        seedIsSeted = true;
    }

    Array<double> beta(questions, 0.0);
    for (size_t i = 0; i < questions; ++i) {
        beta[i] = randReal(-12, 12);
    }

    Array<double> theta(questions, 0.0);
    for (size_t i = 0; i < questions; ++i) {
        theta[i] = randReal(-10, 10);
    }

    table.resize(experts);
    for (size_t i = 0; i < experts; ++i) {
        table[i].resize(questions);
        for (size_t j = 0; j < questions; ++j) {
            double gr = 1.0 / (1.0 + std::exp(beta[j] - theta[i]));
            double gs = gr + 0.5 * (1.0 - gr);
            double r  = randReal(0.0, 1.0);

            if (r <= gr) {
                table[i][j] = Answer::RIGHT;
            } else if (r <= gs) {
                table[i][j] = Answer::NOT_RIGHT;
            } else {
                table[i][j] = Answer::NO;
            }
        }
    }
}

void GenerateFile(const QString &fileName, size_t experts, size_t questions)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    Array<QString> names{
        "Энгалычев В.Т.",    "Пустохина Д.Б.",   "Глебов А.Ч.",      "Яманова Л.Р.",      "Шубин К.С.",
        "Таначёв Д.Е.",      "Шубина Г.Д.",      "Косарёв И.И.",     "Цыцын М.И.",        "Лашкин П.П.",
        "Кебина А.Р.",       "Козарь Ф.Ф.",      "Швечиков П.Н.",    "Никитин А.Ю.",      "Нагиева В.Б.",
        "Илюшкин К.И.",      "Янаслов С.М.",     "Лобана Е.Е.",      "Малахов Е.А.",      "Егоров Н.В.",
        "Закруткин В.А.",    "Громов В.Г.",      "Искрицкий П.Г.",   "Эмских Ф.З.",       "Якобсон М.Е.",
        "Стиплин И.П.",      "Вотякова А.С.",    "Евремович А.Л.",   "Краков Д.Б.",       "Каблукова Е.Р.",
        "Кашникова Е.В.",    "Спирьянов М.К.",   "Пудина И.К.",      "Коньяков П.И.",     "Лещенко К.М.",
        "Порсев К.П.",       "Баскаков А.Ф.",    "Сиянковский В.Р.", "Веселкова О.Р.",    "Рыжкова Л.Н.",
        "Чигракова М.А.",    "Витюгов Т.Н.",     "Тальдаева С.Ю.",   "Николина И.А.",     "Януковича Л.В.",
        "Телицына В.Т.",     "Кутепов М.В.",     "Шишканов Ф.К.",    "Абросимова Ф.Ф.",   "Кузинова Э.С.",
        "Усилов Т.П.",       "Ануфриева З.П.",   "Уржумцева М.С.",   "Колобкова Л.С.",    "Копцева Н.Р.",
        "Красильников О.В.", "Левченко И.К.",    "Бояринова Э.Я.",   "Ермилова В.Г.",     "Волкова М.К.",
        "Хрущёва А.К.",      "Енина Э.В.",       "Сумкин П.И.",      "Хигир В.И.",        "Ямлиханова Н.Л.",
        "Пыхтина А.И.",      "Арчибасов Л.Э.",   "Лобачёва Ж.И.",    "Староволкова В.А.", "Корнейчук В.М.",
        "Кондра М.Я.",       "Сахаровский М.Е.", "Орлова Н.Н.",      "Панфилова П.Ф.",    "Форопонова О.В.",
        "Абросимова О.А.",   "Якушевича У.И.",   "Дворникова И.Г.",  "Челомей О.М.",      "Лапин Е.С.",
        "Петрищев Л.И.",     "Сыров В.Н.",       "Мартьянова Р.С.",  "Муханова М.Г.",     "Захарьина К.И.",
        "Усачёв А.А.",       "Разуваев П.М.",    "Лепёхин И.И.",     "Капралова М.С.",    "Глинин Б.М.",
        "Казакевича А.С.",   "Сиянгулова А.С.",  "Конака Р.А.",      "Сиянин В.З.",       "Благово А.А.",
        "Косма Р.М.",        "Живенков Ф.А.",    "Табаков С.Е.",     "Лысова П.Е.",       "Аксенчук Р.В."};

    if (experts > names.size()) {
        experts = names.size();
    }
    if (questions < 20) {
        questions = 20;
    }

    Table<Answer> closed;
    Table<Answer> opened;
    GenerateTable(experts, questions, opened);
    GenerateTable(experts, questions, closed);

    for (size_t i = 0; i < experts; ++i) {
        out << names[i] << ";";
        for (size_t j = 0; j < questions; ++j) {
            if (opened[i][j] == Answer::RIGHT) {
                out << 1;
            } else if (opened[i][j] == Answer::NOT_RIGHT) {
                out << 0;
            } else {
                out << -1;
            }

            if (j < experts - 1) {
                out << ",";
            } else {
                out << ";";
            }
        }
        for (size_t j = 0; j < questions; ++j) {
            if (closed[i][j] == Answer::RIGHT) {
                out << 1;
            } else if (closed[i][j] == Answer::NOT_RIGHT) {
                out << 0;
            } else {
                out << -1;
            }

            if (j < questions - 1) {
                out << ",";
            } else {
                out << "\n";
            }
        }
    }

    file.close();
}

inline void StringToArrayAnswers(const QString &str, Array<Answer> &vec)
{
    vec.resize(0);
    int start = 0;

    for (int i = 0; i < str.length(); ++i) {
        if (str[i] == ',') {
            int v = str.mid(start, i - start).toInt();
            if (v == 1) {
                vec.push_back(Answer::RIGHT);
            } else if (v == 0) {
                vec.push_back(Answer::NOT_RIGHT);
            } else {
                vec.push_back(Answer::NO);
            }

            start = i + 1;
        }
    }
    int v = str.mid(start, str.length() - start).toInt();
    if (v == 1) {
        vec.push_back(Answer::RIGHT);
    } else if (v == 0) {
        vec.push_back(Answer::NOT_RIGHT);
    } else {
        vec.push_back(Answer::NO);
    }
}

void GetActorsBaseFromFile(const QString &fileName, ActorsBase &actorsBase, Array<QString> &names)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);

    Array<Array<Answer>> closed;
    Array<Array<Answer>> opened;

    names.clear();

    closed.reserve(200);
    opened.reserve(200);
    names.reserve(200);

    size_t count = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();

        QString       name;
        Array<Answer> resultsForOpened;
        Array<Answer> resultsForClosed;

        int start = 0;
        for (int i = start; i < line.length(); i++) {
            if (line[i] == ';') {
                name  = line.mid(start, i - start);
                start = i + 1;
                break;
            }
        }

        int end = start + 1;
        for (int i = start; i < line.length(); i++) {
            if (line[i] == ';') {
                end = i;
                break;
            }
        }
        StringToArrayAnswers(line.mid(start, end - start), resultsForOpened);
        StringToArrayAnswers(line.mid(end + 1), resultsForClosed);

        opened.push_back(resultsForOpened);
        closed.push_back(resultsForClosed);
        names.push_back(name);
        ++count;
    }


    opened.resize(count);
    closed.resize(count);
    names.resize(count);

    actorsBase.setData(opened, closed);

    file.close();
}
