#include <iostream>
#include "ScapegoatTree.h"

int main() {
    ScapegoatTree<int,int> array(0.75,"F:\\ScapegoatTree\\test1.txt");
    array.printTreeByLevel();
    key_value_pair<int,int> newData(8,79);
    array.insert(newData);
    array.printTreeByLevel();
    return 0;
}
