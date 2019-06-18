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
    plt.xticks([1, 2], ["original stride", "improved workplace implementation"])
    plt.ylabel("New Cases per day")
    plt.xlabel("implementation type")
    plt.savefig(os.path.join(outputPrefix + "_boxplot"))
    plt.show()


def plotNewCases(OutputPrefix, first_file, second_file):
    data = []
    for file in [first_file, second_file]:
        data_set = []
        with open(os.path.join(file)) as csvfile:
            reaer = list(csv.DictReader(csvfile))
            for element in range(1, 51):
                for line in reader:
                    if len(data_set) == element:
                        data_set[element-1] += int(line[str(element)])
                    else:


def main():
    if not os.path.exists("SimulationResults"):
        os.mkdir("SimulationResults")
    if not os.path.exists("SimulationResults/workplaceDistribution_plots"):
        os.mkdir("SimulationResults/workplaceDistribution_plots")
    outputPrefix = "SimulationResults/workplaceDistribution_plots"
    first_file = sys.argv[1]
    second_file = sys.argv[2]
    createBoxPlot(outputPrefix, first_file, second_file)


if __name__== "__main__":
    main()
