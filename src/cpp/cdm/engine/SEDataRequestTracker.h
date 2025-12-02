/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

class DataTrack;
class SESystem;
class SEPatient;
class SEEnvironment;
class SEAnesthesiaMachine;
class SEBagValveMask;
class SEElectroCardioGram;
class SEECMO;
class SEInhaler;
class SEMechanicalVentilator;
class SEActionManager;
class SESubstanceManager;
class SECompartmentManager;
class SEDataRequest;
class SEDataRequestManager;
class SEGasCompartment;
class SEGasSubstanceQuantity;
class SELiquidCompartment;
class SELiquidSubstanceQuantity;
class SEThermalCompartment;
#include "cdm/properties/SEScalar.h"

enum class CompartmentUpdate {None,
                              InFlow, OutFlow,
                              HeatTransferRateIn, HeatTransferRateOut,
                              Volume,Pressure,
                              Heat, Temperature,
                              VolumeFraction,
                              Mass, Concentration, 
                              pH, Molarity,
                              PartialPressure, Saturation};
enum class TrackMode { CSV, Dynamic };
class SEDataRequestScalar : public SEGenericScalar
{
  friend class SEEngineTracker;
  friend class SEDynamicStabilizationPropertyConvergence;

protected:
  SEDataRequestScalar(Logger* logger) : SEGenericScalar(logger)
  {
    UpdateProperty = CompartmentUpdate::None;
    GasCmpt = nullptr;
    GasSubstance = nullptr;
    LiquidCmpt = nullptr;
    LiquidSubstance = nullptr;
    ThermalCmpt = nullptr;
  }
  
  void UpdateScalar();
  void SetScalarRequest(const SEScalar& s, SEDataRequest& dr);

  size_t                        idx;
  std::string                   Heading;

  // Compartment related variables
  CompartmentUpdate             UpdateProperty;
  SEGasCompartment*             GasCmpt;
  SEGasSubstanceQuantity*       GasSubstance;
  SELiquidCompartment*          LiquidCmpt;
  SELiquidSubstanceQuantity*    LiquidSubstance;
  SEThermalCompartment*         ThermalCmpt;
  // Tissue cmpts don't have children and they don't have computed data that changes on call (like flow)
};

// This class connects the data requests to the CDM objects to the data tracker to an optional file

class CDM_DECL SEDataRequestTracker : public Loggable
{
public:
  SEDataRequestTracker(Logger* logger) : Loggable(logger) {}
  virtual ~SEDataRequestTracker() {}

  // Close the active stream file using this method
  // Will no longer stream data to this file until SetupDataRequests is called
  virtual void CloseResultsFile() = 0;

  // Makes a copy of this data request
  // Creates, connects, and manages SEDataRequestScalars for all data requests provided
  // DataRequests and order of DataRequests from this manager can be used to easily get values of the connected engine scalars
  virtual bool SetupDataRequests(const SEDataRequestManager& drMgr) = 0;

  virtual size_t NumTracks() const = 0;
  virtual double GetValue(size_t idx) const = 0;
  virtual double GetValue(const SEDataRequest& dr) const = 0;
  virtual std::string GetUnit(const SEDataRequest& dr) const = 0;

  virtual void LogRequestedValues() const = 0;

  virtual const SEDataRequestManager& GetDataRequestManager() const = 0;
};

class CDM_DECL SEEngineTracker : public SEDataRequestTracker
{
  friend class SEDynamicStabilizationPropertyConvergence;
public:
  SEEngineTracker(Logger* logger=nullptr);
  virtual ~SEEngineTracker();

  void Clear();// Reset + Sets CDM objects to nullptr
  void Reset();

  void CloseResultsFile() override;

  bool SetupDataRequests(const SEDataRequestManager& drMgr) override;

  size_t NumTracks() const override;
  double GetValue(size_t idx) const override;
  double GetValue(const SEDataRequest& dr) const override;
  std::string GetUnit(const SEDataRequest& dr) const override;

  void LogRequestedValues() const override;

  const SEDataRequestManager& GetDataRequestManager() const override { return *m_DataRequestMgr; }

  // Add a CDM objecst to look for data in
  void SetPatient(SEPatient& p);
  void SetEnvironment(SEEnvironment& e);
  void AddPhysiologySystem(SESystem& s);
  void SetAnesthesiaMachine(SEAnesthesiaMachine& am);
  void SetBagValveMask(SEBagValveMask& bvm);
  void SetElectroCardioGram(SEElectroCardioGram& ecg);
  void SetECMO(SEECMO& ecmo);
  void SetInhaler(SEInhaler& i);
  void SetMechanicalVentilator(SEMechanicalVentilator& mv);
  void SetActionManager(SEActionManager& am);
  void SetCompartmentManager(SECompartmentManager& cMgr);
  void SetSubstanceManager(SESubstanceManager& sMgr);

  DataTrack& GetDataTrack() { return *m_DataTrack; }

  // CDM Values to DataTrack, also write to file if a results file was specified
  void TrackData(double currentTime_s, double dt_s);

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // These 2 methods instantiates the scalar object in the engine associated with the data request
  // That scalar will be set on a SEDataRequestScalar object
  // Note, if you do not provide a unit in the dr, The unit the engine uses will set on the dr
  //
  // Connects all the managed SEDataRequestScalar's associated with the provided data request manager
  bool ConnectRequest(SEDataRequest& dr);
  // Connects the provided SEDataRequestScalar, which is NOT copied/captured by this class
  // (No index associated for GetValue/GetUnit)
  bool ConnectRequest(SEDataRequest& dr, SEDataRequestScalar& ds);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Finds the scalar associated with this data request
  virtual const SEDataRequestScalar* GetScalar(const SEDataRequest& dr) const;

protected:

  TrackMode                    m_Mode;
  double                       m_LastPullTime_s;
  double                       m_CurrentSampleTime_s;

  std::stringstream            m_ss;
  std::ofstream*               m_ResultsStream;

  SEDataRequestManager*        m_DataRequestMgr;
  DataTrack*                   m_DataTrack;

  SEPatient*                   m_Patient;
  SEActionManager*             m_ActionMgr;
  SECompartmentManager*        m_CmptMgr;
  SESubstanceManager*          m_SubMgr;

  SEEnvironment*               m_Environment;
  std::vector<SESystem*>       m_PhysiologySystems;
  SEAnesthesiaMachine*         m_AnesthesiaMachine;
  SEBagValveMask*              m_BVM;
  SEElectroCardioGram*         m_ECG;
  SEECMO*                      m_ECMO;
  SEInhaler*                   m_Inhaler;
  SEMechanicalVentilator*      m_MechanicalVentilator;
  std::map<const SEDataRequest*, SEDataRequestScalar*> m_Request2Scalar;
  std::map<std::string, std::ofstream> m_OutputFiles;
};
