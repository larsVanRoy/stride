//
// Created by benji on 21/03/19.
//

#include <H5Cpp.h>
#include <string>

using namespace H5;

int main(){
    H5File file("hdf5_test.h5", H5F_ACC_TRUNC);
    hsize_t dimensions[2];
    dimensions[0] = 3;
    dimensions[1] = 3;
    int rank = 2; // matrix van 3x3 opslaan (rank = # dimensies)
    DataSpace dataSpace(rank, dimensions);

    StrType variable_length_string_type(0, H5T_VARIABLE);

    DataSet dataSet = file.createDataSet("test", variable_length_string_type, dataSpace);
    file.close();
    return 0;
}