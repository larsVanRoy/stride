//
// Created by benji on 21/03/19.
//

#include <H5Cpp.h>
#include <string>
#include <iostream>

using namespace H5;

void createFileAndWrite(){
    H5File file("hdf5_test.h5", H5F_ACC_TRUNC);
    hsize_t dimensions[2];
    dimensions[0] = 3;
    dimensions[1] = 3;
    int rank = 2; // matrix van 3x3 opslaan (rank = # dimensies)
    DataSpace dataSpace(rank, dimensions);

    StrType variable_length_string_type(0, H5T_VARIABLE); //Variable length string

    DataSet dataSet = file.createDataSet("test", PredType::NATIVE_INT, dataSpace);

    int data[3][3];
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            data[i][j] = i*j;
        }
    }
    dataSet.write(data, PredType::NATIVE_INT);
    file.close();
}

void openFileAndRead(){
    H5File file("hdf5_test.h5", H5F_ACC_RDONLY);
    DataSet dataSet = file.openDataSet("test");
    DataSpace dataSpace = dataSet.getSpace();
    int rank = dataSpace.getSimpleExtentNdims();
    auto dims = new hsize_t[rank];
    dataSpace.getSimpleExtentDims(dims);

    int* buffer = new int[dims[0]*dims[1]];
    dataSet.read(buffer, PredType::NATIVE_INT);
    for(int i = 0; i < dims[0]*dims[1]; ++i){
        std::cout << buffer[i] << std::endl;
    }

}

int main(){
    createFileAndWrite();
    openFileAndRead();
    return 0;
}