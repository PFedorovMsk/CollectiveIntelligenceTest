#ifndef UTILS_H
#define UTILS_H

#include "ActorsBase.h"
#include <QString>


void GenerateTable(size_t experts, size_t questions, Table<Answer> &table);
void GenerateFile(const QString &fileName, size_t experts, size_t questions);
void GetActorsBaseFromFile(const QString &fileName, ActorsBase &actorsBase, Array<QString> &names);


#endif // UTILS_H
