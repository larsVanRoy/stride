import random
import csv


# numbers are derived from https://www.statistiekvlaanderen.be/bevolking-naar-leeftijd-en-geslacht
def get_age(rand, ratio):
    young_percentage = 0.122531262 * ratio / 100

    if rand < 0.051954709:
        return random.randint(0, 4)
    elif rand < 0.108614857:
        return random.randint(5, 9)
    elif rand < 0.1627503:
        return random.randint(10, 14)
    elif rand < 0.22465016:
        return random.randint(25, 29)
    elif rand < 0.285687188:
        return random.randint(30, 34)
    elif rand < 0.350517369:
        return random.randint(35, 39)
    elif rand < 0.412683688:
        return random.randint(40, 44)
    elif rand < 0.481371137:
        return random.randint(45, 49)
    elif rand < 0.555028643:
        return random.randint(50, 54)
    elif rand < 0.610532957:
        return random.randint(65, 69)
    elif rand < 0.658433699:
        return random.randint(70, 74)
    elif rand < 0.694246252:
        return random.randint(75, 79)
    elif rand < 0.725031979:
        return random.randint(80, 84)
    elif rand < 0.745117615:
        return random.randint(85, 89)
    elif rand < 0.753119334:
        return random.randint(90, 94)
    elif rand < 0.754937476:
        return random.randint(95, 99)
    elif rand < 0.754937476 + young_percentage:
        return random.randint(15, 24)
    else:
        return random.randint(55, 64)


# numbers are derived from https://www.wonenvlaanderen.be/sites/wvl/files/wysiwyg/huishoudens_naar_hh-grootte.pdf
def get_household_size(rand):
    if rand < 0.3140:
        return 1
    elif rand < 0.6566:
        return 2
    elif rand < 0.8014:
        return 3
    elif rand < 0.9348:
        return 4
    elif rand < 0.9804:
        return 5
    else:
        return random.randint(6, 12)


# number is derived from the original used household reference file
nr_of_households = 8767

# this is a list of the needed files, allong with their respective young/old ratio, found here
# https://www.vlaanderen.be/publicaties/beleidsbarometer-2012-de-vlaamse-arbeidsmarkt-in-kaart
households_regions = [
    ["general", "78"],
    ["Antwerp", "81"],
    ["Flemish_Brabant", "85"],
    ["West-Flanders", "71"],
    ["East-Flanders", "82"],
    ["Limburg", "70"],
    ["Major_Cities", "102"]
]


def main():
    for region in households_regions:
        region_name = region[0]
        ratio = int(region[1])
        csv_data = [["hh_age1", "hh_age2", "hh_age3", "hh_age4",  "hh_age5", "hh_age6",
                    "hh_age7", "hh_age8", "hh_age9", "hh_age10", "hh_age11", "hh_age12"]]
        for _ in range(nr_of_households):
            household = []
            household_size = get_household_size(random.randint(0, 999999999)/999999999)
            for i in range(12):
                if i >= household_size:
                    household.append("NA")
                else:
                    household.append(get_age(random.randint(0, 999999999)/999999999, ratio))
            csv_data.append(household)

        with open("households_{}.csv".format(region_name), 'w') as csvFile:
            writer = csv.writer(csvFile)
            writer.writerows(csv_data)

        csvFile.close()


if __name__ == '__main__':
    main()
