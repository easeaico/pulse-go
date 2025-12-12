/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include <iostream>
#include <iomanip>
class SESubstance;
class SEElectricalCircuit;
class SEFluidCircuit;
class SEThermalCircuit;
class SEGasCompartmentGraph;
class SELiquidCompartmentGraph;
#include "cdm/engine/SEDecimalFormat.h"

class CDM_DECL DataTrack : public Loggable
{
protected:
  struct CDM_DECL Element
  {
    size_t              idx;
    std::string         name;
    double              probe=std::numeric_limits<double>::quiet_NaN();
    std::vector<double> track;
    SEDecimalFormat     format;
  };
  typedef std::vector<Element> ElementVector;
public:
  DataTrack();
  DataTrack(Logger *m_Log);
  ~DataTrack();

  void Clear();
  void Reset();

  void UseTabDelimiter() { m_Delimiter = '\t'; }
  void UseCommaDelimiter() { m_Delimiter = ','; }

  std::vector<double> const& GetTimes() const;
  size_t NumTracks() const { return m_Elements.size(); }

  void SetFormatting(const std::string& name, const SEDecimalFormat& f);
  void SetFormatting(const std::string& name, std::streamsize precision);
  void SetDefaultFormatting(std::streamsize precision);


  void Probe(size_t idx, double value);
  // Returning the index of this element, use it to avoid a string lookup
  size_t Probe(const std::string& name, double value);
  size_t Probe(const std::string& name, double value, int i);// Append value of i to the name
  void Probe(const SEFluidCircuit& c);
  void Probe(const SEElectricalCircuit& c);
  void Probe(const SEThermalCircuit& c);
  void Probe(const SELiquidCompartmentGraph& graph);

  double GetProbe(size_t idx) const;
  double GetProbe(const std::string& name) const;
  std::string GetProbeName(size_t idx) const;

  void Track(size_t idx, double time, double value);
  // Returning the index of this element, use it to avoid a string lookup
  size_t Track(const std::string& name, double time, double value);
  void Track(double time, const SEElectricalCircuit& circuit);
  void Track(double time, const SEFluidCircuit& circuit);
  void Track(double time, const SEThermalCircuit& circuit);
  void Track(double time, const SEGasCompartmentGraph& graph, std::vector<SESubstance*>* substances = nullptr);
  void Track(double time, const SELiquidCompartmentGraph& graph, std::vector<SESubstance*>* substances = nullptr);

  // Get a specific track value at a specific time
  double GetTrack(size_t idx, double time);
  double GetTrack(const std::string& name, double time);

  // Reads the entire file and stores contents into memory, returns the column headings
  std::vector<std::string> ReadTrackFromFile(const char* fileName);
  // Only Reads the column headings and returns them
  // Holds onto the file handle for streaming line by line
  std::vector<std::string> StreamDataFromFile(const char* fileName);

  // Reads a line from the file and returns the time associated with the time
  double StreamDataFromFile(std::vector<std::string>* headings);

  // Creates the file and writes the headers to that file
  void CreateFile(const char* fileName, std::ofstream& newFile);// TODO C++11
  // Write all the track to a file
  void WriteTrackToFile(const char* fileName);
  // Writes data from the provided headings to the file, in the order things were tracked
  void StreamTrackToFile(std::ofstream& file);
  // Writes prob values to file in the order things were tracked
  void StreamProbesToFile(double time, std::ofstream& file);

protected:
  Element& GetElement(size_t idx);
  Element& GetElement(std::string const& name);
  ElementVector m_Elements;

  char                              m_Delimiter;
  double                            m_LastTime = -1.0;
  std::vector<double>               m_Times;
  std::streamsize                   m_DefaultPrecision = 3;

  std::ifstream m_FileStream;
};
