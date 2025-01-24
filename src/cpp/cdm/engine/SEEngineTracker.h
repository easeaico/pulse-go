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

class CDM_DECL SEEngineTracker : public Loggable
{
public:
  SEEngineTracker(SEPatient&, SEActionManager&, SESubstanceManager&, SECompartmentManager&, Logger* logger=nullptr);
  virtual ~SEEngineTracker();

  void Clear();// Remove all requests and close the results file

  DataTrack& GetDataTrack();
  SEActionManager& GetActionManager() { return m_ActionMgr; }
  SESubstanceManager& GetSubstanceManager() { return m_SubMgr; }
  SEDataRequestManager& GetDataRequestManager() { return *m_DataRequestMgr; }

  // Add a system to look for data in
  void AddSystem(SESystem& system);

  void ResetFile();// Close file, so next Track Data will re hook up everything and make a new file

  bool ConnectRequest(SEDataRequest& dr, SEDataRequestScalar& ds);

  virtual bool SetupRequests();
  // Set to false if you have already pulled the latest values
  virtual void LogRequestedValues();
  virtual void TrackData(double currentTime_s);
  virtual void PullData(double currentTime_s);
  virtual bool TrackRequest(SEDataRequest& dr);
  virtual void ForceConnection() { m_ForceConnection = true; }

  void SetTrackMode(TrackMode m) { m_Mode = m; }
  TrackMode GetTrackMode() { return m_Mode; }


  double GetValue(const SEDataRequest& dr) const;
  std::string GetUnit(const SEDataRequest& dr) const;

protected:
  const SEDataRequestScalar* GetScalar(const SEDataRequest& dr) const;

  TrackMode                    m_Mode= TrackMode::CSV;
  bool                         m_ForceConnection;
  double                       m_LastPullTime_s;
  DataTrack*                   m_DataTrack;

  std::stringstream            m_ss;
  std::ofstream                m_ResultsStream;
  SEDataRequestManager*        m_DataRequestMgr;
  
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