//
// Created by menegais1 on 02/05/2020.
//

#include <iostream>
#include "Fatorial.h"


double Fatorial::fatorial(int n) {
    static Fatorial *instance = new Fatorial();
    if (n == 0 || n == 1) return 1;
    if (instance->fat_lookUpTable[n] != -1) return instance->fat_lookUpTable[n];
    double fat = n * fatorial(n - 1);
    instance->fat_lookUpTable[n] = fat;
    return fat;
}

Fatorial::Fatorial() {
    for (int i = 0; i < 40; ++i) {
        Fatorial::fat_lookUpTable[i] = -1;
    }
}
