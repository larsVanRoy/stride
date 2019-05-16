#################################################################
# Script to perform a given amount of simmulations              #
#                                                               #
# usage: python3.X general_simulate.py                          #
#################################################################

import pystride
import time
import os
import csv
import sys
import re
import random
import matplotlib.pyplot as plt

from pystride.Event import Event, EventType
from pystride.PyRunner import PyRunner
from pystride.PyController import PyController

import xml.etree.ElementTree as ET


def trackCases(simulator, event):
    """
        Callback function to track cumulative cases
        after each time-step.
    """
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, "cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})


def plotNewCases(outputPrefix):
    """
        Plot new cases per day for a list of levels.
    """
    days = [i for i in range(0, sim_days)]
    newCasesPerDay = [0] * sim_days
    prevCumulativeCases = 0
    with open(os.path.join(outputPrefix + "_" + config.split(".")[0], "cases.csv")) as csvfile:
        reader = csv.DictReader(csvfile)
        day_index = 0
        for row in reader:
            if row["timestep"] == "timestep":
                if day_index != 0:
                    newCasesPerDay[0] = 0
                    plt.plot(days, newCasesPerDay)
                day_index = 0
                prevCumulativeCases = 0
                continue
            cumulativeCases = int(row["cases"])
            newCasesPerDay[day_index] = cumulativeCases - prevCumulativeCases
            prevCumulativeCases = cumulativeCases
            day_index += 1
    newCasesPerDay[0] = 0
    plt.plot(days, newCasesPerDay)
    plt.xlabel("Simulation day")
    plt.ylabel("New cases per day")
    plt.savefig(os.path.join(outputPrefix + "_plots", "new_{}".format(config.split(".")[0])))
    plt.show()


def plotCummulativeCases(outputPrefix):
    """
        Plot cummulative cases per day for a list of levels
    """
    days = [i for i in range(0, sim_days)]
    cumulativeCasesPerDay = [0] * sim_days
    with open(os.path.join(outputPrefix + "_" + config.split(".")[0], "cases.csv")) as csvfile:
        reader = csv.DictReader(csvfile)
        day_index = 0
        for row in reader:
            if row["timestep"] == "timestep":
                if day_index != 0:
                    cumulativeCasesPerDay[0] = 0
                    plt.plot(days, cumulativeCasesPerDay)
                day_index = 0
                continue
            cumulativeCases = int(row["cases"])
            cumulativeCasesPerDay[day_index] = cumulativeCases
            day_index += 1
    cumulativeCasesPerDay[0] = 0
    plt.plot(days, cumulativeCasesPerDay)
    plt.xlabel("Simulation day")
    plt.ylabel("Cummulative cases per day")
    plt.savefig(os.path.join(outputPrefix + "_plots", "cumul_{}".format(config.split(".")[0])))
    plt.show()

def runSimulation(outputPrefix):
    for run in range(0, runs):
        print("run number {0} of {1}".format(run+1, runs))
        controller = PyController(data_dir="data")
        controller.loadRunConfig(os.path.join("config", config))
        controller.runConfig.setParameter("num_days", sim_days)
        controller.runConfig.setParameter("output_prefix", outputPrefix + "_" + config.split(".")[0])
        seed = ""
        for _ in range(4):
            seed += str(random.randint(1, 999999)) + ","
        seed = seed[:-1]
        controller.runConfig.setParameter("rng_seed", seed) 
        controller.registerCallback(trackCases, EventType.Stepped)
        controller.control()


def main():
    if not os.path.exists("SimulationResults"):
        os.mkdir("SimulationResults")
    if not os.path.exists("SimulationResults/simulation_plots"):
        os.mkdir("SimulationResults/simulation_plots")
    if os.path.exists("SimulationResults/simulation_{}".format(config.split(".")[0])):
        os.system("rm -rf SimulationResults/simulation_{}".format(config.split(".")[0]))
    os.mkdir("SimulationResults/simulation_{}".format(config.split(".")[0]))
    outputPrefix = "SimulationResults/simulation"
    t_start = time.mktime(time.localtime())
    runSimulation(outputPrefix)
    plotNewCases(outputPrefix)
    plotCummulativeCases(outputPrefix)
    t_elapsed = time.mktime(time.localtime()) - t_start
    print("Total time elapsed: " + str(round(t_elapsed)) + " seconds")

# the used config file
config = "stochastic_analysis.xml"

# the number of simulations
runs = 10

# the number of days per simulation
sim_days = 500


if __name__=="__main__":
    main()
