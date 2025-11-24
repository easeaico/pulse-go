/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

class DataTrack;
class SESystem;
class SEPatient;
class SEEnvironment;
class PhysiologyEngine;
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

class CDM_DECL SETrackedData : public Loggable
{
public:
  SETrackedData(Logger* logger) : Loggable(logger) {}
  virtual ~SETrackedData() {}

  virtual size_t NumProbes() const = 0;
  virtual double GetValue(size_t idx) const = 0;
  virtual double GetValue(const SEDataRequest& dr) const = 0;
  virtual std::string GetUnit(const SEDataRequest& dr) const = 0;

  virtual void LogRequestedValues() const = 0;

  virtual const SEDataRequestManager& GetDataRequestManager() const = 0;
};

class CDM_DECL SEEngineTracker : public SETrackedData
{
public:
  SEEngineTracker(SEPatient&, SEActionManager&, SESubstanceManager&, SECompartmentManager&, Logger* logger=nullptr);
  virtual ~SEEngineTracker();

  void Clear();// Remove all requests and close the results file
  void ResetFile();// Close file, so next Track Data will re hook up everything and make a new file
  void ForceConnection() { m_ForceConnection = true; }

  // Add a system to look for data in
  void AddSystem(SESystem& system); // This should also be in the ctor...

  bool SetupDataRequests(const SEDataRequestManager& drMgr);

  void SetTrackMode(TrackMode m) { m_Mode = m; }
  TrackMode GetTrackMode() { return m_Mode; }

  size_t NumProbes() const override;
  double GetValue(size_t idx) const override;
  double GetValue(const SEDataRequest& dr) const override;
  std::string GetUnit(const SEDataRequest& dr) const override;

  DataTrack& GetDataTrack() { return *m_DataTrack; }
  //const DataTrack& GetDataTrack() const { return *m_DataTrack;}
  //SEActionManager& GetActionManager() { return m_ActionMgr; }
  //SESubstanceManager& GetSubstanceManager() { return m_SubMgr; }
  //SEDataRequestManager& GetDataRequestManager() { return *m_DataRequestMgr; }
  const SEDataRequestManager& GetDataRequestManager() const override { return *m_DataRequestMgr; }

  void LogRequestedValues() const override;

  // These methods are if used for more manual control of tracking
  // (I think I would like these to be protected)
  bool ConnectRequest(SEDataRequest& dr, SEDataRequestScalar& ds);
  void PullData(double currentTime_s);
  void TrackData(double currentTime_s, double dt_s);
  bool TrackRequest(SEDataRequest& dr);

protected:
  const SEDataRequestScalar* GetScalar(const SEDataRequest& dr) const;

  TrackMode                    m_Mode= TrackMode::CSV;
  bool                         m_ForceConnection;
  double                       m_LastPullTime_s;
  double                       m_CurrentSampleTime_s;

  std::stringstream            m_ss;
  std::ofstream                m_ResultsStream;

  SEDataRequestManager*        m_DataRequestMgr;
  DataTrack*                   m_DataTrack;

  SEPatient&                   m_Patient;
  SEActionManager&             m_ActionMgr;
  SESubstanceManager&          m_SubMgr;
  SECompartmentManager&        m_CmptMgr;

  SEEnvironment*               m_Environment = nullptr;
  std::vector<SESystem*>       m_PhysiologySystems;
  SESystem*                    m_AnesthesiaMachine = nullptr;
  SESystem*                    m_ECG = nullptr;
  SESystem*                    m_Inhaler = nullptr;
  SESystem*                    m_MechanicalVentilator = nullptr;
  std::map<const SEDataRequest*, SEDataRequestScalar*> m_Request2Scalar;
};