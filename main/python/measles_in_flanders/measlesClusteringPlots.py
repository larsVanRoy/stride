import argparse
import csv
import matplotlib.pyplot as plt
import os
import xml.etree.ElementTree as ET

def getRngSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + "Seeds.csv")
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            for s in row:
                seeds.append(int(s))
    return seeds

def getTargetSusceptibilityRates(outputDir):
    targetRatesChildTree = ET.parse(os.path.join(outputDir, 'data', 'measles_child_immunity.xml'))
    targetRatesChild = []
    for r in targetRatesChildTree.iter():
        if r.tag not in ['immunity', 'data_source', 'data_manipulation']:
            targetRatesChild.append(float(r.text))
    targetRatesAdultTree = ET.parse(os.path.join(outputDir, 'data', 'measles_adult_immunity.xml'))
    targetRatesAdult = []
    for r in targetRatesAdultTree.iter():
        if r.tag not in ['immunity', 'data_source', 'data_manipulation']:
            targetRatesAdult.append(float(r.text))
    # 1 - immunityRate = susceptibilityRate
    return [1 - (x + y) for x, y in zip(targetRatesChild, targetRatesAdult)]

def getActualSusceptibilityRates(outputDir, scenarioName, seed):
    susceptiblesFile = os.path.join(outputDir, scenarioName + str(seed), 'susceptibles.csv')
    maxAge = 99
    totalsByAge = [0] * (maxAge + 1)
    susceptiblesByAge = [0] * (maxAge + 1)
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(float(row['age']))
            isSusceptible = int(row['susceptible'])
            totalsByAge[age] += 1
            if isSusceptible:
                susceptiblesByAge[age] += 1
    return [x / y for x, y in zip(susceptiblesByAge, totalsByAge)]

def getActualAvgSusceptibilityRates(outputDir, scenarioName):
    maxAge = 99
    seeds = getRngSeeds(outputDir, scenarioName)
    totalRates = [0] * (maxAge + 1)
    for s in seeds:
        actualRates = getActualSusceptibilityRates(outputDir, scenarioName, s)
        totalRates = [x + y for x,y in zip(totalRates, actualRates)]
    totalRates = [x / len(seeds) for x in totalRates]
    return totalRates

def createAgeImmunityPlots(outputDir, scenarioNames):
    # Plot target immunity rates
    targetRates = getTargetSusceptibilityRates(outputDir)
    plt.plot(targetRates, 'bo')
    # Plot immunity rates per scenario
    maxAge = 99
    ages = range(maxAge + 1)
    for scenario in scenarioNames:
        actualRates = getActualAvgSusceptibilityRates(outputDir, scenario)
        plt.plot(ages, actualRates)

    # Plot layout
    plt.xlabel("Age")
    plt.xlim(0,100)
    plt.ylabel("Fraction susceptible")
    plt.ylim(0,1)
    plt.legend(["Data", "Uniform +\nnot clustered",
                "Age-dependent +\nnot clustered", "Uniform +\nclustered",
                "Age-dependent +\nclustered"])
    # Save plot and clear figure
    plt.savefig(os.path.join(outputDir, "AgeImmunityPlots.png"))
    plt.clf()

def getFinalOutbreaksSizes(outputDir, scenarioName, numDays):
    finalSizes = []
    seeds = getRngSeeds(outputDir, scenarioName)
    for s in seeds:
        casesFile = os.path.join(outputDir, scenarioName + str(s), "cases.csv")
        with open(casesFile) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                if int(row['timestep']) == (numDays - 1):
                    finalSizes.append(int(row['cases']))
                    break
    return finalSizes

def createOutbreakSizesPlots(outputDir, scenarioNames, numDays):
    allFinalSizes = []
    for scenario in scenarioNames:
        finalSizes = getFinalOutbreaksSizes(outputDir, scenario, numDays)
        allFinalSizes.append(finalSizes)
    plt.boxplot(allFinalSizes, labels=["Uniform +\nnot clustered",
                "Age-dependent +\nnot clustered", "Uniform +\nclustered",
                "Age-dependent +\nclustered"])
    plt.ylabel("Final outbreak sizes after {} days".format(numDays + 1))
    plt.savefig(os.path.join(outputDir, "FinalSizesPlot.png"))
    plt.clf()

def getCumulativeCasesPerDay(outputDir, scenarioName, numDays):
    cumulativeCasesPerDay = []
    for day in range(numDays):
        cumulativeCasesPerDay.append([])
    seeds = getRngSeeds(outputDir, scenarioName)
    for s in seeds:
        casesFile = os.path.join(outputDir, scenarioName + str(s), 'cases.csv')
        with open(casesFile) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                if int(row['timestep']) < numDays:
                    cumulativeCasesPerDay[int(row['timestep'])].append(int(row['cases']))
    return cumulativeCasesPerDay

def getNewCasesPerDay(cumulativeCases):
    newCasesPerDay = []
    lastDay = [1] * len(cumulativeCases[0])
    for today in cumulativeCases:
        newToday = [x - y for x, y in zip(today, lastDay)]
        newCasesPerDay.append(newToday)
        lastDay = today
    return newCasesPerDay

def createOutbreakEvolutionPlots(outputDir, scenarioNames, numDays):
    dayScale = 10
    days = range(numDays)[::dayScale]
    for scenario in scenarioNames:
        cumulativeCases = getCumulativeCasesPerDay(outputDir, scenario, numDays)
        plt.boxplot(cumulativeCases[::dayScale], labels=days)
        plt.xticks(rotation=90)
        plt.xlabel("Day")
        plt.ylabel("Cumulative cases per day")
        plt.ylim(0, 50000)
        plt.savefig(os.path.join(outputDir, scenario + "CumulativeCasesPlot.png"))
        plt.clf()
        newCases = getNewCasesPerDay(cumulativeCases)
        plt.boxplot(newCases[::dayScale], labels=days)
        plt.xticks(rotation=90)
        plt.xlabel("Day")
        plt.ylabel("New cases per day")
        plt.ylim(0, 600)
        plt.savefig(os.path.join(outputDir, scenario + "NewCasesPlot.png"))
        plt.clf()

def createHouseholdConstitutionPlots(outputDir, scenarioNames):
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        for s in seeds[:1]:
            households = {}
            susceptiblesFile = os.path.join(outputDir, scenario + str(s), 'susceptibles.csv')
            with open(susceptiblesFile) as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    hhId = int(row['household'])
                    isSusceptible = int(row['susceptible'])
                    if hhId in households:
                        households[hhId].append(isSusceptible)
                    else:
                        households[hhId] = [isSusceptible]
            householdImmunities = []
            for hhId, members in households.items():
                pctImmune = 1 - (sum(members) / len(members))
                householdImmunities.append(pctImmune)
            plt.hist(householdImmunities)
            plt.title(scenario)
            plt.show()

'''
def getActualSusceptibilityRates(outputDir, scenarioName, seed):
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(float(row['age']))
            isSusceptible = int(row['susceptible'])
            totalsByAge[age] += 1
            if isSusceptible:
                susceptiblesByAge[age] += 1
    return [x / y for x, y in zip(susceptiblesByAge, totalsByAge)]

def getActualAvgSusceptibilityRates(outputDir, scenarioName):
    maxAge = 99
    seeds = getRngSeeds(outputDir, scenarioName)
    totalRates = [0] * (maxAge + 1)
    for s in seeds:
        actualRates = getActualSusceptibilityRates(outputDir, scenarioName, s)
        totalRates = [x + y for x,y in zip(totalRates, actualRates)]
    totalRates = [x / len(seeds) for x in totalRates]
    return totalRates
'''

def main(outputDir, numDays):
    scenarioNames = ["Random", "AgeClustering", "HouseholdClustering", "AgeAndHouseholdClustering"]
    # Create age-immunity plots
    #createAgeImmunityPlots(outputDir, scenarioNames)
    # Create plots for household constitution
    createHouseholdConstitutionPlots(outputDir, scenarioNames)
    # Create outbreak sizes plots
    createOutbreakSizesPlots(outputDir, scenarioNames, numDays)
    # Create outbreak evolution plots
    #createOutbreakEvolutionPlots(outputDir, scenarioNames, numDays)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dir", type=str, help="Directory containing simulation output files.")
    parser.add_argument("--numDays", type=int, default=365)
    args = parser.parse_args()
    main(args.dir, args.numDays)