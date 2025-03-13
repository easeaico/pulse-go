# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import numpy as np
import random as rand
import pandas as pd
import matplotlib.pyplot as plt
from scipy.stats import poisson


def SyntheticDataGeneration(populationSize, statisticsFileName, syntheticDataFile):
    print("generate data with " + str(populationSize) + " samples")
    statisticsData = pd.read_csv(statisticsFileName, index_col='Parameters')
    
    #Gender
    gender = ["female", "male"]
    femaleFrequency = statisticsData.loc['GenderSplit', 'Gender'] 
    genderSplit = [femaleFrequency, 100-femaleFrequency]   
    genderList = rand.choices(gender, weights = genderSplit, k=populationSize)
    populationData = pd.DataFrame({'Gender': genderList})

    #Height and BMI
    numberGender = populationData.value_counts("Gender")

    numberFemale = numberGender['female']

    heightFemaleList = np.random.normal(loc=statisticsData.loc['mean', 'FemaleHeight'], scale=statisticsData.loc['std', 'FemaleHeight'], size=numberFemale)
    heightMaleList = np.random.normal(loc=statisticsData.loc['mean', 'MaleHeight'], scale=statisticsData.loc['std', 'MaleHeight'], size=populationSize - numberFemale)

    BMIFemaleList = np.random.normal(loc=statisticsData.loc['mean', 'FemaleBMI'], scale=statisticsData.loc['std', 'FemaleBMI'], size=numberFemale)
    BMIMaleList = np.random.normal(loc=statisticsData.loc['mean', 'MaleBMI'], scale=statisticsData.loc['std', 'MaleBMI'], size=populationSize - numberFemale)

    femaleCount = 0
    maleCount = 0
    heightList = []
    BMIList = []
    for index, value in populationData['Gender'].items():
        if value == 'female':
            heightList.append(heightFemaleList[femaleCount])
            BMIList.append(BMIFemaleList[femaleCount])
            femaleCount=femaleCount+1
        else :
            heightList.append(heightMaleList[maleCount])
            BMIList.append(BMIMaleList[maleCount])
            maleCount=maleCount+1

    #heart rate
    hrList = np.random.normal(statisticsData.loc['mean', 'HeartRate'], statisticsData.loc['std', 'HeartRate'], populationSize)

    #Alternative Age Distribution
    ageGroups = ['Under25', '26To30','31To35','36To40','41Plus']
    ageCounts = [191975,103628,70783,52055,43215]
    ageWeights = [41.6, 22.4, 15.3, 11.3, 9.4]

    #Create bins of ages
    ageBinList = rand.choices(ageGroups, weights = ageWeights, k=populationSize)
    ageBinData = pd.DataFrame({'AgeBins': ageBinList})

    #Assume normal distribution for each bin
    ageBinNumber = ageBinData.value_counts("AgeBins")

    under25List = np.random.normal(loc=21.5, scale=2, size=ageBinNumber['Under25'])
    bin26To30List = np.random.normal(loc=28, scale=1, size=ageBinNumber['26To30'])
    bin31To35List = np.random.normal(loc=33, scale=1, size=ageBinNumber['31To35'])
    bin36To40List = np.random.normal(loc=38, scale=1, size=ageBinNumber['36To40'])
    over41List = np.random.normal(loc=48, scale=3, size=ageBinNumber['41Plus'])

    ageList = []
    under25Count = 0
    bin26To30Count = 0
    bin31To35Count = 0
    bin36To40Count = 0
    over41Count = 0
    for index, value in ageBinData['AgeBins'].items():
        if value == 'Under25':
            ageList.append(under25List[under25Count])
            under25Count = under25Count + 1
        elif value == '26To30':
            ageList.append(bin26To30List[bin26To30Count])
            bin26To30Count = bin26To30Count + 1
        elif value == '31To35':
            ageList.append(bin31To35List[bin31To35Count])
            bin31To35Count = bin31To35Count + 1
        elif value == '36To40':
            ageList.append(bin36To40List[bin36To40Count])
            bin36To40Count = bin36To40Count + 1
        elif value == '41Plus':
            ageList.append(over41List[over41Count])
            over41Count = over41Count + 1
        else :
            print("Unknown Age Range: " + value)

    populationData['Age'] = ageList
    populationData['Height'] = heightList
    populationData['BMI'] = BMIList
    populationData['Heart Rate'] = hrList

    populationData.head()
    populationData.describe()

    #Apply Injuries
    injuryData = GenerateSyntheticInjuries(populationSize)

    populationInjuryData = pd.concat([populationData, injuryData], axis=1)
   
    populationInjuryData.to_csv(syntheticDataFile + '.csv', index=False)


def ValidateSyntheticPopulationData(synetheticDataFile, statsFileName):
    print("validate data in " + synetheticDataFile)
    
    #Load synthetic data
    populationData = pd.read_csv(synetheticDataFile + '.csv')

    #Load statistics file 
    statisticsData = pd.read_csv(statsFileName, index_col='Parameters')

    #Gender Split
    numberGender = populationData.value_counts("Gender")
    numberFemale = numberGender['female']
    numberMale = numberGender['male']
    percentFemale =100*numberFemale/(numberFemale+numberMale)
    actualPercentFemale = statisticsData.loc['GenderSplit', 'Gender']
    genderSplitError = percentFemale - actualPercentFemale

    #Need a list of means, stds, errors
    means = []
    actualMeans = []
    stds = []
    actualSTDs = []
    errors = []

    ##Height and BMI
    femaleHeightList = []
    maleHeightList = []
    femaleBMIList = []
    maleBMIList = []
    for index, value in populationData['Gender'].items():
        if value == 'female':
            femaleHeightList.append(populationData.loc[index,'Height'])
            femaleBMIList.append(populationData.loc[index,'BMI'])
        else :
            maleHeightList.append(populationData.loc[index,'Height'])
            maleBMIList.append(populationData.loc[index,'BMI'])

    femaleHeight = np.array(femaleHeightList)
    maleHeight = np.array(maleHeightList)
    femaleBMI = np.array(femaleBMIList)
    maleBMI = np.array(maleBMIList)
    
    means.append(np.mean(femaleHeight))
    stds.append(np.std(femaleHeight))
    means.append(np.mean(femaleBMI))
    stds.append(np.std(femaleBMI))

    means.append(np.mean(maleHeight))
    stds.append(np.std(maleHeight))
    means.append(np.mean(maleBMI))
    stds.append(np.std(maleBMI))

    actualMeans.append(statisticsData.loc['mean', 'FemaleHeight'])
    actualSTDs.append(statisticsData.loc['std', 'FemaleHeight'])
    actualMeans.append(statisticsData.loc['mean', 'FemaleBMI'])
    actualSTDs.append(statisticsData.loc['std', 'FemaleBMI'])
    
    actualMeans.append(statisticsData.loc['mean', 'MaleHeight'])
    actualSTDs.append(statisticsData.loc['std', 'MaleHeight'])
    actualMeans.append(statisticsData.loc['mean', 'MaleBMI'])
    actualSTDs.append(statisticsData.loc['std', 'MaleBMI'])

    #Heart Rate
    means.append(populationData['Heart Rate'].mean())
    stds.append(populationData['Heart Rate'].std())

    actualMeans.append(statisticsData.loc['mean', 'HeartRate'])
    actualSTDs.append(statisticsData.loc['std', 'HeartRate'])

    #Calculate Error
    meanError = []
    stdError = []
    for i in range(len(means)):
        meanError.append(means[i] - actualMeans[i])
        stdError.append(stds[i] - actualSTDs[i])

    #Age
    ageList = populationData['Age']
    ageBins = [18, 25, 31, 35, 41, 55]
    counts, bins = np.histogram(ageList, bins=ageBins)
    normalized_counts = counts / counts.sum()

    fig, axes = plt.subplots(1, 2, figsize=(10, 5)) 
    #axes[0].hist(ageList, bins=ageBins, density=True, color='skyblue', edgecolor='black')
    axes[0].hist(ageBins[:-1], ageBins, weights=normalized_counts, color='skyblue', edgecolor='black')
    axes[0].set_title('Normalized Synthetic Age Data')
    axes[0].set_xlabel('Age')
    axes[0].set_ylabel('Normalized Frequency')

    actualAgeCounts = [191975,103628,70783,52055,43215]
    totalPopulation = sum(actualAgeCounts)
    normalizedAgeCounts = [x / totalPopulation for x in actualAgeCounts]
    
    axes[1].hist(ageBins[:-1], ageBins, weights=normalizedAgeCounts, color='green', edgecolor='black')
    axes[1].set_title('Normalized Actual Age Data')
    axes[1].set_xlabel('Age')
    axes[1].set_ylabel('Normalized Frequency')
    
    plt.savefig(syntheticDataFile + '_AgeHistogram.jpg', format='jpeg') #Saves as JPEG

    #Gender Comparison Table
    genderSplitTable = pd.DataFrame({'Synthetic Gender Split': [percentFemale], 'Actual Gender Split' : [actualPercentFemale], 'Error' : genderSplitError})
    print(genderSplitTable)
    genderSplitTable.to_csv(syntheticDataFile + '_GenderSplitTable.csv', index=False)
    genderSplitTable.to_html(syntheticDataFile + '_GenderSplitTable.html', index=False)

    #Other parameter comparison
    descriptors  = ['Female Height', 'Female BMI', 'Male Height', 'Male BMI', 'Heart Rate']
    statisticsValidationTable = pd.DataFrame({'Demographic Descriptor': descriptors, 'Synthetic Mean': means, 'Actual Mean': actualMeans, 'Mean Error': meanError, 'Synthetic Standard Deviation': stds, 'Actual Standard Deviation': actualSTDs, 'Std Error': stdError})
    print(statisticsValidationTable)
    statisticsValidationTable.to_csv(syntheticDataFile + '_SyntheticPopulationValidationTable.csv', index=False)
    statisticsValidationTable.to_html(syntheticDataFile + '_SyntheticPopulationValidationTable.html', index=False)


def GenerateSyntheticInjuries(populationSize):

    #Number and Location of Injuries
    injuryLocation = ['HeadAndNeck','Thorax','Abdomen','Extremity']
    injuryLocationWeights = [36.2, 8.6, 6.9, 49.4]
    injuryLocationCountMean = 1.011 #going to assume 1 location

    injuryLocationList = rand.choices(injuryLocation, weights = injuryLocationWeights, k=populationSize)

    injuryData = pd.DataFrame({'Injury Location': injuryLocationList})

    #Type and Number of Injuries in a location
    injuryHeadNeckType = ['TBI', 'Airway Obstruction', 'Superficial Injuries']
    injuryHeadNeckWeights = [22, 18, 60]
    injuryHeadNeckNumberMean = 1.0
    injuryThoraxType = ['Pneumothorax', 'Pulmonary Contusion', 'Fractures', 'Hemothorax', 'Hemorrhage', 'Spinal Injury']
    injuryThoraxWeights = [51.8, 50.2, 51.2, 30, 34.6, 14.6]
    injuryThoraxNumberMean = 2.3
    injuryAbdomenType = ['Hemorrhage', 'Lacerations/Contusions']
    injuryAbdomenWeights = [34.6, 65.4]
    injuryAbdomenNumberMean = 1.0
    injuryExtremityType = ['Hemorrhage', 'Fractures/Dislocations', 'Contusions/Sprains/Strains', 'Burns/Nerves']
    injuryExtremityWeight = [52, 22, 20, 5]
    injuryExtremityNumberMean = 1.0

    # count types of injuries for the loop below
    numberInjuryLocations = injuryData.value_counts("Injury Location")

    headNeckInjuryList = rand.choices(injuryHeadNeckType, weights = injuryHeadNeckWeights, k=numberInjuryLocations['HeadAndNeck'])
    thoraxInjuryList = rand.choices(injuryThoraxType, weights = injuryThoraxWeights, k=numberInjuryLocations['Thorax'])
    abdomenInjuryList = rand.choices(injuryAbdomenType, weights = injuryAbdomenWeights, k=numberInjuryLocations['Abdomen'])
    extremityInjuryList = rand.choices(injuryExtremityType, weights = injuryExtremityWeight, k=numberInjuryLocations['Extremity'])

    injuryTypeList = []
    headNeckCount = 0
    thoraxCount = 0
    abdomenCount = 0
    extremityCount = 0
    for i in range(len(injuryLocationList)):
        loc = injuryLocationList[i]
        if loc == 'HeadAndNeck':
            injuryTypeList.append(headNeckInjuryList[headNeckCount])
            headNeckCount = headNeckCount + 1
        elif loc == 'Thorax':
            injuryTypeList.append(thoraxInjuryList[thoraxCount])
            thoraxCount = thoraxCount + 1
        elif loc == 'Abdomen':
            numberInjuries = 1
            injuryTypeList.append(abdomenInjuryList[abdomenCount])
            abdomenCount = abdomenCount + 1
        elif loc == 'Extremity':
            numberInjuries = 1
            injuryTypeList.append(extremityInjuryList[extremityCount])
            extremityCount = extremityCount + 1
        else :
            print("Unknown Injury Location: " + loc)

    injuryData['Injury Type'] =  injuryTypeList
    
    #Apply Severities
    numberInjuryTypes = injuryData.value_counts("Injury Type")
    
    meanTBISeverity = 3.5
    stdSeverity = 0.25
    meanAirwaySeverity = 4.0
    meanThoraxAbdomenSeverity = 2.85
    severityExtremityValue = [1, 2.5, 3.5, 4.5]
    severityExtremityWeight = [56, 23, 17, 7]
    TBISeverityList = np.random.normal(loc=meanTBISeverity, scale=stdSeverity, size=numberInjuryTypes['TBI'])
    airwaySeverityList = np.random.normal(loc=meanAirwaySeverity, scale=stdSeverity, size=numberInjuryTypes['Airway Obstruction'])
    thoraxAndAbdomenSeverityList = np.random.normal(loc=meanThoraxAbdomenSeverity, scale=stdSeverity, size=numberInjuryLocations['Thorax']+numberInjuryLocations['Abdomen'])
    extremitySeverityList = rand.choices(severityExtremityValue, severityExtremityWeight, k=numberInjuryLocations['Extremity'])

    injurySeverityList = []
    TBICount = 0
    airwayCount = 0
    thoraxAbdomenCount=0
    extremityCount=0
    for i in range(len(injuryLocationList)):
        loc = injuryLocationList[i]
        if loc == 'HeadAndNeck':
            loc2 = injuryTypeList[i]
            if loc2 == 'TBI':
                injurySeverityList.append(TBISeverityList[TBICount])
                TBICount = TBICount + 1
            elif loc2 == 'Airway Obstruction':
                injurySeverityList.append(airwaySeverityList[airwayCount])
                airwayCount = airwayCount + 1
            elif loc2 == 'Superficial Injuries':
                injurySeverityList.append(1.0)
            else:
                print("Unknown Injury Type: " + loc2)
        elif loc == 'Abdomen' or loc == 'Thorax':
            injurySeverityList.append(thoraxAndAbdomenSeverityList[thoraxAbdomenCount])
            thoraxAbdomenCount = thoraxAbdomenCount + 1
        elif loc == 'Extremity':
            injurySeverityList.append(extremitySeverityList[extremityCount])
            extremityCount = extremityCount + 1
        else :
            print("Unknown Injury Location: " + loc)

    injuryData['Injury Severity'] =  injurySeverityList

    return injuryData

def ValidateSyntheticInjuryData(syntheticDataFile):
    
    #Load synthetic data
    populationData = pd.read_csv(syntheticDataFile + '.csv')

    actualInjuryLocationPercents = [36.2, 8.6, 6.9, 49.4]

    injuryHeadNeckType = ['TBI','Airway Obstruction', 'Superficial Injuries']
    injuryThoraxType = ['Pneumothorax', 'Pulmonary Contusion', 'Fractures', 'Hemothorax', 'Hemorrhage', 'Spinal Injury']
    injuryAbdomenType = ['Hemorrhage', 'Lacerations/Contusions']
    injuryExtremityType = ['Hemorrhage', 'Fractures/Dislocations', 'Contusions/Sprains/Strains', 'Burns/Nerves']
    actualInjuryHeadNeckPercents = [22, 18, 60]
    actualInjuryThoraxPercents = [51.8, 50.2, 51.2, 30, 34.6, 14.6]
    actualInjuryAbdomenPercents = [34.6, 65.4]
    actualInjuryExtremityPercents = [52, 22, 20, 5]
    actualSeverityMean = [2.69, 2.85, 2.85, 2.05]

    #Location of Injury
    countInjuryLocations = populationData.value_counts("Injury Location")
    injuryLocationCounts = [countInjuryLocations['HeadAndNeck'], countInjuryLocations['Abdomen'], countInjuryLocations['Thorax'], countInjuryLocations['Extremity']]
    
    numberInjuries = len(populationData)
    severity = populationData['Injury Severity']

    percentInjuryLocations = []
    errorPercentInjuryLocations = []
    for i in range(len(injuryLocationCounts)):
        percentInjuryLocations.append(100*injuryLocationCounts[i]/numberInjuries)
        errorPercentInjuryLocations.append(percentInjuryLocations[i] - actualInjuryLocationPercents[i])

    countInjuryTypes = populationData.value_counts("Injury Type")
    
    abdomenHemorrhage = 0
    thoraxHemorrhage = 0
    extremityHemorrhage = 0
    headNeckSeverity = 0
    thoraxSeverity = 0
    abdomenSeverity = 0
    extremitySeverity = 0
    injuryType = populationData['Injury Type']
    injuryLocation = populationData['Injury Location']
    for i in range(len(injuryType)):
        iType = injuryType[i]
        iLoc = injuryLocation[i]
        if iLoc == "Abdomen":
            abdomenSeverity = abdomenSeverity + severity[i] 
            if iType == "Hemorrhage":
                abdomenHemorrhage = abdomenHemorrhage + 1
        elif iLoc == "Thorax":
            thoraxSeverity = thoraxSeverity + severity[i]
            if iType == "Hemorrhage":
                thoraxHemorrhage = thoraxHemorrhage + 1
        elif iLoc == "Extremity":
            extremitySeverity = extremitySeverity + severity[i]
            if iType == "Hemorrhage":
                extremityHemorrhage = extremityHemorrhage + 1
        elif iLoc =="HeadAndNeck":
            headNeckSeverity = headNeckSeverity + severity[i] 
        else:
            print("Uknown injury location " + iLoc)
    

    injuryHeadNeckTypeCounts = [countInjuryTypes['TBI'], countInjuryTypes['Airway Obstruction'], countInjuryTypes['Superficial Injuries']]
    headNeckInjuryTotal = sum(injuryHeadNeckTypeCounts)
    injuryAbdomenTypeCounts = [abdomenHemorrhage, countInjuryTypes['Lacerations/Contusions']]
    abdomenInjuryTotal = sum(injuryAbdomenTypeCounts)
    injuryThoraxTypeCounts = [countInjuryTypes['Pneumothorax'], countInjuryTypes['Pulmonary Contusion'], countInjuryTypes['Fractures'], countInjuryTypes['Hemothorax'], thoraxHemorrhage, countInjuryTypes['Spinal Injury']]
    thoraxInjuryTotal = sum(injuryThoraxTypeCounts)
    injuryExtremityTypeCounts = [extremityHemorrhage, countInjuryTypes['Fractures/Dislocations'], countInjuryTypes['Contusions/Sprains/Strains'], countInjuryTypes['Burns/Nerves']]
    extremityInjuryTotal = sum(injuryExtremityTypeCounts)

    severityMean = [headNeckSeverity/headNeckInjuryTotal, thoraxSeverity/thoraxInjuryTotal, abdomenSeverity/abdomenInjuryTotal, extremitySeverity/extremityInjuryTotal]
    severityMeanError = []
    for i in range(len(severityMean)):
        severityMeanError.append(severityMean[i] - actualSeverityMean[i])

    percentInjuries = []
    errorPercentInjuries = []
    actualPercentInjuries = []
    #start with head neck injuries
    percentInjuries.append(percentInjuryLocations[0])
    errorPercentInjuries.append(errorPercentInjuryLocations[0])
    actualPercentInjuries.append(actualInjuryLocationPercents[0])
    for i in range(len(injuryHeadNeckTypeCounts)):
        val = 100*injuryHeadNeckTypeCounts[i]/headNeckInjuryTotal
        percentInjuries.append(val)
        errorPercentInjuries.append(val - actualInjuryHeadNeckPercents[i])
        actualPercentInjuries.append(actualInjuryHeadNeckPercents[i])

    #Add Thorax Injuries
    percentInjuries.append(percentInjuryLocations[1])
    errorPercentInjuries.append(errorPercentInjuryLocations[1])
    actualPercentInjuries.append(actualInjuryLocationPercents[1])
    for i in range(len(injuryThoraxTypeCounts)):
        val = 100*injuryThoraxTypeCounts[i]/thoraxInjuryTotal
        percentInjuries.append(val)
        errorPercentInjuries.append(val - actualInjuryThoraxPercents[i])
        actualPercentInjuries.append(actualInjuryThoraxPercents[i])

    #Add Abdomen Injuries
    percentInjuries.append(percentInjuryLocations[2])
    errorPercentInjuries.append(errorPercentInjuryLocations[2])
    actualPercentInjuries.append(actualInjuryLocationPercents[2])
    for i in range(len(injuryAbdomenTypeCounts)):
        val = 100*injuryAbdomenTypeCounts[i]/abdomenInjuryTotal
        percentInjuries.append(val)
        errorPercentInjuries.append(val - actualInjuryAbdomenPercents[i])
        actualPercentInjuries.append(actualInjuryAbdomenPercents[i])

    #Add Extremity Injuries
    percentInjuries.append(percentInjuryLocations[3])
    errorPercentInjuries.append(errorPercentInjuryLocations[3])
    actualPercentInjuries.append(actualInjuryLocationPercents[3])
    for i in range(len(injuryExtremityTypeCounts)):
        val = 100*injuryExtremityTypeCounts[i]/extremityInjuryTotal
        percentInjuries.append(val)
        errorPercentInjuries.append(val - actualInjuryExtremityPercents[i])
        actualPercentInjuries.append(actualInjuryExtremityPercents[i])

    #Create table for validation
    locationDescriptors  = ['Head and Neck', '', '', '', 'Thorax', '', '', '', '', '', '', 'Abdomen', '', '', 'Extremity', '', '', '', '']
    severityMeanList = [severityMean[0], '', '', '', severityMean[1], '', '', '', '', '', '', severityMean[2], '', '', severityMean[3], '', '', '', '']
    actualSeverityMeanList = [actualSeverityMean[0], '', '', '', actualSeverityMean[1], '', '', '', '', '', '', actualSeverityMean[2], '', '', actualSeverityMean[3], '', '', '', '']
    severityErrorList = [severityMeanError[0], '', '', '', severityMeanError[1], '', '', '', '', '', '', severityMeanError[2], '', '', severityMeanError[3], '', '', '', '']
    typeDescriptors = ['', 'TBI', 'Airway Obstruction', 'Superficial Injuries', '', 'Pneumothorax', 'Pulmonary Contusion', 'Fractures', 'Hemothorax', 'Hemorrhage', 'Spinal Injury','', 'Hemorrhage', 'Lacerations/Contusions', '', 'Hemorrhage', 'Fractures/Dislocations', 'Contusions/Sprains/Strains', 'Burns/Nerves']
    injuryLocationValidationTable = pd.DataFrame({'Injury Locations': locationDescriptors, 'Mean Injury Severity': severityMeanList, 'Actual Severity Mean': actualSeverityMeanList, 'Severity Error': severityErrorList,'Injury Types': typeDescriptors, 'Synthetic Injury Distribution (%)': percentInjuries, 'Actual Injury Distribution (%)': actualPercentInjuries, 'Distribution Error': errorPercentInjuries})
    print(injuryLocationValidationTable)
    injuryLocationValidationTable.to_csv(syntheticDataFile + '_InjuryValidationTable.csv', index=False)
    injuryLocationValidationTable.to_html(syntheticDataFile + '_InjuryValidationTable.html', index=False)
    

#Total population size
populationSize = 3000

statsFileName = "ArmyDemographicStats.csv"

syntheticDataFile = 'SyntheticArmyInjuryPopulation_' + str(populationSize) + 'samples'

SyntheticDataGeneration(populationSize, statsFileName, syntheticDataFile)

ValidateSyntheticPopulationData(syntheticDataFile,statsFileName)

ValidateSyntheticInjuryData(syntheticDataFile)
