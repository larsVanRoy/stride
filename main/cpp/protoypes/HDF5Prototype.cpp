
//
// Created by benji on 21/03/19.
//

#include <H5Cpp.h>
#include <string>
#include <iostream>

using namespace H5;

typedef struct person_t{
    int id;
    int age;
    int k12School;
    int houseHold;
    int workplace;
    int primaryCommunity;
    int secondaryCommunity;
    int college;
} person_t;

void createFileAndWrite(){
    CompType compPerson(sizeof(person_t));
    compPerson.insertMember("id", HOFFSET(person_t, id), PredType::NATIVE_INT);
    compPerson.insertMember("age", HOFFSET(person_t, age), PredType::NATIVE_INT);
    compPerson.insertMember("k12School", HOFFSET(person_t, k12School), PredType::NATIVE_INT);
    compPerson.insertMember("houseHold", HOFFSET(person_t, houseHold), PredType::NATIVE_INT);
    compPerson.insertMember("workplace", HOFFSET(person_t, workplace), PredType::NATIVE_INT);
    compPerson.insertMember("primaryCommunity", HOFFSET(person_t, primaryCommunity), PredType::NATIVE_INT);
    compPerson.insertMember("secondaryCommunity", HOFFSET(person_t, secondaryCommunity), PredType::NATIVE_INT);
    compPerson.insertMember("college", HOFFSET(person_t, college), PredType::NATIVE_INT);

    H5File file("hdf5_test.h5", H5F_ACC_TRUNC);
    hsize_t dimensions[2];
    dimensions[0] = 3;
    dimensions[1] = 3;
    int rank = 2; // matrix van 3x3 opslaan (rank = # dimensies)
    DataSpace dataSpace(rank, dimensions);

    StrType variable_length_string_type(PredType::C_S1, H5T_VARIABLE); //Variable length string

    DataSet test_dataSet = file.createDataSet("test", PredType::NATIVE_INT, dataSpace);

    Attribute str_attribute = file.createAttribute("name", variable_length_string_type, DataSpace(H5S_SCALAR));
    str_attribute.write(variable_length_string_type, H5std_string("temp name"));

    int data[3][3];
    for(int i = 0; i < 3; ++i){
        for(int j = 0; j < 3; ++j){
            data[i][j] = i*j;
        }
    }
    test_dataSet.write(data, PredType::NATIVE_INT);

    hsize_t dimensions2[1] = {5};
    DataSpace dataSpace2(1, dimensions2);
    DataSet dataSet_persons = file.createDataSet("Persons", compPerson, dataSpace2);

    person_t persons[5];
    for(int i = 0; i < 5; ++i){
        persons[i].id = i;
        persons[i].age = 2*i*i;
    }
    dataSet_persons.write(persons, compPerson);

    file.close();
}

void openFileAndRead(){
    CompType compPerson(sizeof(person_t));
    compPerson.insertMember("id", HOFFSET(person_t, id), PredType::NATIVE_INT);
    compPerson.insertMember("age", HOFFSET(person_t, age), PredType::NATIVE_INT);
    compPerson.insertMember("k12School", HOFFSET(person_t, k12School), PredType::NATIVE_INT);
    compPerson.insertMember("houseHold", HOFFSET(person_t, houseHold), PredType::NATIVE_INT);
    compPerson.insertMember("workplace", HOFFSET(person_t, workplace), PredType::NATIVE_INT);
    compPerson.insertMember("primaryCommunity", HOFFSET(person_t, primaryCommunity), PredType::NATIVE_INT);
    compPerson.insertMember("secondaryCommunity", HOFFSET(person_t, secondaryCommunity), PredType::NATIVE_INT);
    compPerson.insertMember("college", HOFFSET(person_t, college), PredType::NATIVE_INT);
    H5File file("hdf5_test.h5", H5F_ACC_RDONLY);
    DataSet dataSet = file.openDataSet("test");
    DataSpace dataSpace = dataSet.getSpace();
    int rank = dataSpace.getSimpleExtentNdims();
    auto dims = new hsize_t[rank];
    dataSpace.getSimpleExtentDims(dims);

    auto buffer = new int[dims[0]*dims[1]];
    dataSet.read(buffer, PredType::NATIVE_INT);
    for(hsize_t i = 0; i < dims[0]*dims[1]; ++i){
        std::cout << buffer[i] << std::endl;
    }
    StrType variable_length_string_type(PredType::C_S1, H5T_VARIABLE); //Variable length string
    H5std_string attribute;
    file.openAttribute("name").read(variable_length_string_type, attribute);
    std::cout << attribute << std::endl;

    delete[] dims;

    DataSet dataSet_persons = file.openDataSet("Persons");
    auto dataSpace2 = dataSet_persons.getSpace();
    rank = dataSpace2.getSimpleExtentNdims();
    auto dims2 = new hsize_t[rank];
    dataSpace2.getSimpleExtentDims(dims2);

    auto person_buffer = new person_t[dims2[0]];
    dataSet_persons.read(person_buffer, compPerson);
    for(hsize_t i = 0; i < dims2[0]; ++i){
        std::cout << person_buffer[i].id << ' ' << person_buffer[i].age << std::endl;
    }
}

/**
 * Huidig voorstel HDF5 : root heeft - een dataset Persons (1D- matrix van Person compound types)
 *                                   - een group Locations
 *                                   - Een attribute geogridname
 *
 *                        Locations heeft - groups Location_i
 *
 *                        Location_i heeft - een dataset Commute (1D-matrix van Commute compound types)
 *                                         - een dataset ContactPools (2D-matrix van [ID, class, people-array])
 *                                         - attributes ID, name, province, population, coordinate
 *
 * */



int main(){
    createFileAndWrite();
    openFileAndRead();
    return 0;
}
