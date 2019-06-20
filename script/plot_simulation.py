#############################################################
# Script to create graphs for stan data                     #
# usage:                                                    #
#    python3.X plot_simulation.py first_file second_file    #
#############################################################

import os
import csv
import sys
import matplotlib.pyplot as plt


def createBoxPlot(outputPrefix, first_file, second_file):
    data = []
    for file in [first_file, second_file]:
        data_set = []
        with open(os.path.join(file)) as csvfile:
            reader = list(csv.DictReader(csvfile))
            line = reader[-1]
            for element in range(1, 51):
                data_set.append(int(line[str(element)]))
        data.append(data_set)
    plt.boxplot(data)
    plt.xticks([1, 2], ["original stride", "improved household implementation"])
    plt.ylabel("New Cases per day")
    plt.xlabel("implementation type")
    plt.savefig(os.path.join(outputPrefix + "_boxplot"))
    plt.show()


def plotCumulCases(outputPrefix, first_file, second_file):
    for file in [first_file, second_file]:
        data_set = []
        with open(os.path.join(file)) as csvfile:
            reader = list(csv.DictReader(csvfile))
            for idx in range(len(reader)):
                for element in range(1, 51):
                    line = reader[idx]
                    if len(data_set) == idx+1:
                        data_set[idx] += int(line[str(element)])
                    else:
                        data_set.append(int(line[str(element)]))
            for i in range(len(data_set)):
                data_set[i] /= 50
            plt.plot(range(len(reader)), data_set)
    plt.xlabel("simulation day")
    plt.ylabel("new cases per day")
    plt.legend(["original stride", "improved household implementation"])
    plt.savefig(os.path.join(outputPrefix + "_cumul"))
    plt.show()


def plotNewCases(outputPrefix, first_file, second_file):
    for file in [first_file, second_file]:
        data_set = []
        with open(os.path.join(file)) as csvfile:
            reader = list(csv.DictReader(csvfile))
            for idx in range(len(reader)):
                for element in range(1, 51):
                    line = reader[idx]
                    if len(data_set) == idx+1:
                        data_set[idx] += int(line[str(element)])
                    else:
                        if len(data_set) == 0:
                            data_set.append(0)
                        else:
                            data_set.append(-data_set[-1])
                        data_set[idx] += int(line[str(element)])
            for i in range(len(data_set)):
                data_set[i] /= 50
            plt.plot(range(len(reader)), data_set)
    plt.xlabel("simulation day")
    plt.ylabel("new cases per day")
    plt.legend(["original stride", "improved household implementation"])
    plt.savefig(os.path.join(outputPrefix + "_new"))
    plt.show()


def main():
    if not os.path.exists("SimulationResults"):
        os.mkdir("SimulationResults")
    if not os.path.exists("SimulationResults/household_plots"):
        os.mkdir("SimulationResults/household_plots")
    outputPrefix = "SimulationResults/household_plots"
    first_file = sys.argv[1]
    second_file = sys.argv[2]
    createBoxPlot(outputPrefix, first_file, second_file)
    plotNewCases(outputPrefix, first_file, second_file)
    plotCumulCases(outputPrefix, first_file, second_file)


if __name__== "__main__":
    main()
