//
// Created by Daniel Seifert on 28.04.22.
//

#include "table/PrettyTable.h"
#include <iostream>

using namespace std;

void main_test() {
    PrettyTable t1;
    t1 << "a" << "b" << "c" << row;
    t1 << "aaa" << "bbb" << "ccc" << "ddd";

    PrettyTable t2;
    t2 << "a" << "b" << "c" << row;
    t2 << "aaa" << "bbb" << "ccc" << "dd";

    cout << "t1" << endl;
    cout << t1 << endl;
    cout << "t2" << endl;
    cout << t2 << endl;

    cout << "--------" << endl << endl;

    cout << "t1+t2" << endl;
    cout << (t1 + t2) << endl;

    cout << "t1&t2" << endl;
    cout << (t1 & t2) << endl;

    cout << "t1^t2" << endl;
    cout << (t1 ^ t2) << endl;

    cout << "t1-t2" << endl;
    cout << (t1 - t2) << endl;
    cout << "t2-t1" << endl;
    cout << (t2 - t1) << endl;
}

