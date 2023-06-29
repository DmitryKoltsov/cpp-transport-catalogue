#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

using namespace std;

int main()
{
    auto catalogue = reader::ReadData();
    print::ReadAndExecuteQueries(catalogue);

    return 0;
}