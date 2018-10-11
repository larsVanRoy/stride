import argparse
import csv
import os
import time
import xml.etree.ElementTree as ET

from pystride.Event import Event, EventType
from pystride.PyController import PyController

# Global variable to keep track of the susceptibility rates
# at the beginning of each simulation
# Used to calculated immunity rates 'on the go' without having to read in files
SUSCEPTIBILITY_RATES = []

# Callback function to register person's age, household id
# adn immunity status at the beginning of the simulation
def registerSusceptibles(simulator, event):
    outputPrefix = simulator.GetConfigValue('run.output_prefix')
    pop = simulator.GetPopulation()
    totalPopulation = 0
    totalSusceptible = 0
    with open(os.path.join(outputPrefix, 'susceptibles.csv'), 'w') as csvfile:
        fieldnames = ["age", "susceptible", "household"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        # Write header
        writer.writeheader()
        # Write susceptibility info per person
        for i in range(pop.size()):
            totalPopulation += 1
            person = pop[i]
            age = person.GetAge()
            isSusceptible = person.GetHealth().IsSusceptible()
            if isSusceptible:
                isSusceptible = 1
                totalSusceptible += 1
            else:
                isSusceptible = 0
            hhId = person.GetHouseholdId()
            writer.writerow({"age": age, "susceptible": isSusceptible, "household": hhId})
    global SUSCEPTIBILITY_RATES
    SUSCEPTIBILITY_RATES.append(totalSusceptible / totalPopulation)

# Callback function to track the cumulative cases
# at each timestep
def trackCases(simulator, event):
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, "cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})

def getAvgImmunityRate():
    avgSusceptibilityRate = sum(SUSCEPTIBILITY_RATES) / len(SUSCEPTIBILITY_RATES)
    return 1 - avgSusceptibilityRate

def createRandomImmunityDistributionFiles(immunityRate):
    randomChildImmunity = ET.Element('immunity')
    randomAdultImmunity = ET.Element('immunity')
    for age in range(18):
        elemChild = ET.SubElement(randomChildImmunity, 'age' + str(age))
        elemChild.text = str(immunityRate)
        elemAdult = ET.SubElement(randomAdultImmunity, 'age' + str(age))
        elemAdult.text = str(0)
    for age in range(18, 100):
        elemChild = ET.SubElement(randomChildImmunity, 'age' + str(age))
        elemChild.text = str(0)
        elemAdult = ET.SubElement(randomAdultImmunity, 'age' + str(age))
        elemAdult.text = str(immunityRate)

    ET.ElementTree(randomChildImmunity).write(os.path.join('data', 'measles_random_child_immunity.xml'))
    ET.ElementTree(randomAdultImmunity).write(os.path.join('data', 'measles_random_adult_immunity.xml'))

def generateRngSeeds(numSeeds):
    seeds = []
    for i in range(numSeeds):
        random_data = os.urandom(4)
        seeds.append(int.from_bytes(random_data, byteorder='big'))
    return seeds

def writeSeeds(scenarioName, seeds):
    with open(scenarioName + "Seeds.csv", "a") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(seeds)

def runSimulations(scenarioName, numRuns, r0s, startDates, extraParams={}):
    configFile = os.path.join("config", "measles" + scenarioName + "Immunity.xml")
    for r0 in r0s:
        for startDate in startDates:
            seeds = generateRngSeeds(numRuns)
            writeSeeds(scenarioName, seeds)
            for s in seeds:
                print("Running " + scenarioName + " with R0 " + str(r0) + " and start " + startDate)
                control = PyController(data_dir="data")
                control.loadRunConfig(configFile)
                control.runConfig.setParameter("output_prefix", scenarioName + str(s))
                control.runConfig.setParameter("rng_seed", s)
                control.runConfig.setParameter("r0", r0)
                control.runConfig.setParameter("start_date", startDate)
                for paramName, paramValue in extraParams.items():
                    control.runConfig.setParameter(paramName, paramValue)
                control.registerCallback(registerSusceptibles, EventType.AtStart)
                control.registerCallback(trackCases, EventType.Stepped)
                control.control()

def main(numRuns):
    start = time.perf_counter()
    r0s = [12]
    #startDates = ["2017-01-01", "2017-01-02", "2017-01-03", "2017-01-04", "2017-01-05", "2017-01-06", "2017-01-07"]
    startDates = ["2017-01-01"]
    # Run AgeClustering scenarios
    runSimulations("AgeClustering", numRuns, r0s, startDates)
    # Run AgeAndHouseholdClustering scenario
    runSimulations("AgeAndHouseholdClustering", numRuns, r0s, startDates)
    immunityRate = getAvgImmunityRate()
    createRandomImmunityDistributionFiles(immunityRate)
    # Run Random scenario
    runSimulations("Random", numRuns, r0s, startDates, {'immunity_rate': immunityRate})
    # Run HouseholdClustering scenario
    runSimulations('HouseholdClustering',
                    numRuns, r0s, startDates,
                    {'immunity_distribution_file': 'data/measles_random_adult_immunity.xml',
                     'vaccine_distribution_file': 'data/measles_random_child_immunity.xml'})
    end = time.perf_counter()
    print("Total time: " + str(end - start))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--numRuns", type=int, default=10, help="Number of simulation runs per scenario")
    args = parser.parse_args()
    main(args.numRuns)