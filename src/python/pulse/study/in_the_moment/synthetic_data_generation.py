# Distributed under the Apache License, Version 2.0.
# See accompanying NOTICE file for details.

import logging
import numpy as np
import random as rand
import pandas as pd
import matplotlib.pyplot as plt
from scipy.stats import poisson


def SyntheticDataGeneration(sampleSize, statisticsFileName):
    print("generate data with " + str(sampleSize) + " samples")
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
    heightMaleList = np.random.normal(loc=statisticsData.loc['mean', 'MaleHeight'], scale=statisticsData.loc['std', 'MaleHeight'], size=sampleSize - numberFemale)

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
    hrList = np.random.normal(statisticsData.loc['mean', 'HeartRate'], statisticsData.loc['std', 'HeartRate'], sampleSize)

    #Age distribution
    histogramfileName = statisticsData.loc['histogram', 'Age']
    histogramAgeData = pd.read_csv(histogramfileName)
    bins = histogramAgeData['bins']
    histogramNoNan = histogramAgeData.dropna()
    counts = histogramNoNan['counts']

    #midpoints = 0.5*(bins[:-1] + bins[1:])
    midpoints = []
    for i in range(1, len(bins)):
        midpoints.append(0.5*(bins[i] + bins[i-1]))
    
    ageMean = np.sum(midpoints * counts) / np.sum(counts)
    ageSTD = np.sqrt(np.sum(counts * (midpoints - ageMean)**2) / (np.sum(counts) - 1))

    #gamma distribution - shape and scale parameter calculation
    ageShape = ageMean**2/ageSTD
    ageScale = ageMean/ageShape
    lowerBound = 18
    upperBound = 45
    #ageList = truncated_normal(ageMean, ageSTD, lowerBound, upperBound, sampleSize)
    ageList = poisson.rvs(mu=ageMean, loc=lowerBound, size=sampleSize)
    #ageList = np.random.gamma(ageShape, ageScale, size=populationSize)

    #populationData['Age'] = ageList
    populationData['Height'] = heightList
    populationData['BMI'] = BMIList
    populationData['Heart Rate'] = hrList
    populationData['Age'] = ageList

    populationData.head()
    populationData.describe()

    #print(populationData)
    populationData.to_csv('SyntheticArmyPopulation.csv', index=False)

    #print(populationData)


def ValidateSyntheticData(synetheticDataFile, statsFileName):
    print("validate data in " + synetheticDataFile)
    
    #Load synthetic data
    populationData = pd.read_csv('SyntheticArmyPopulation.csv')

    #Load statistics file 
    statisticsData = pd.read_csv(statsFileName, index_col='Parameters')

    #Gender Split
    numberGender = populationData.value_counts("Gender")
    numberFemale = numberGender['female']
    numberMale = numberGender['male']
    percentFemale =100*numberFemale/(numberFemale+numberMale)
    actualPercentFemale = statisticsData.loc['GenderSplit', 'Gender']
    genderSplitError = 100*(percentFemale - actualPercentFemale)/actualPercentFemale 

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
        meanError.append(100*(means[i] - actualMeans[i])/actualMeans[i])
        stdError.append(100*(stds[i] - actualSTDs[i])/actualSTDs[i])

    #Age
    #TO DO add age comparison for histogram

    #Gender Comparison Table
    genderSplitTable = pd.DataFrame({'Synthetic Gender Split': [percentFemale], 'Actual Gender Split' : [actualPercentFemale], 'Error' : genderSplitError})

    #Other parameter comparison
    descriptors  = ['Female Height', 'Female BMI', 'Male Height', 'Male BMI', 'Heart Rate']
    statisticsValidationTable = pd.DataFrame({'Demographic Descriptor': descriptors, 'Synthetic Mean': means, 'Actual Mean': actualMeans, 'Mean Error': meanError, 'Synthetic Standard Deviation': stds, 'Actual Standard Deviation': actualSTDs, 'Std Error': stdError})
    print(statisticsValidationTable)

#Total population size
populationSize = 25
statsFileName = "ArmyDemographicStats.csv"

SyntheticDataGeneration(populationSize, statsFileName)

syntheticDataFile = 'SyntheticArmyPopulation.csv'

ValidateSyntheticData(syntheticDataFile,statsFileName)
