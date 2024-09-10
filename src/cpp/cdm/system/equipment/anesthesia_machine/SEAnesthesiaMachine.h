/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/system/equipment/SEEquipment.h"
class SEEventHandler;
class SESubstanceManager;
class SEAnesthesiaMachineChamber;
class SEAnesthesiaMachineOxygenBottle;
class SEAnesthesiaMachineConfiguration;
class SEGasSubstanceQuantity;


// Keep enums in sync with appropriate schema/cdm/AnesthesiaMachineEnums.proto file !!
enum class eAnesthesiaMachine_OxygenSource { NullSource = 0, NoSource, Wall, BottleOne, BottleTwo };
extern CDM_DECL const std::string& eAnesthesiaMachine_OxygenSource_Name(eAnesthesiaMachine_OxygenSource m);

// Keep enums in sync with appropriate schema/cdm/AnesthesiaMachineEnums.proto file !!
enum class eAnesthesiaMachine_PrimaryGas { NullGas = 0, NoGas, Air, Nitrogen };
extern CDM_DECL const std::string& eAnesthesiaMachine_PrimaryGas_Name(eAnesthesiaMachine_PrimaryGas m);


class CDM_DECL SEAnesthesiaMachine : public SEEquipment
{
  friend class PBAnesthesiaMachine;//friend the serialization class
protected:
  friend SEAnesthesiaMachineConfiguration;
public:

  SEAnesthesiaMachine(Logger* logger);
  virtual ~SEAnesthesiaMachine();

  void Clear() override;

  bool SerializeToString(std::string& output, eSerializationFormat m) const;
  bool SerializeToFile(const std::string& filename) const;
  bool SerializeFromString(const std::string& src, eSerializationFormat m, const SESubstanceManager& subMgr);
  bool SerializeFromFile(const std::string& filename, const SESubstanceManager& subMgr);

protected:

  /** @name StateChange
  *   @brief - This method is called when ever there is a state change
  *            Specically a new file has been loaded, configuration action, or the system reset
  *            Engine specific methodology can then update their logic.
  */
  virtual void StateChange(){};
  virtual void Merge(const SEAnesthesiaMachine& from, SESubstanceManager& subMgr);
  virtual void ProcessConfiguration(SEAnesthesiaMachineConfiguration& config, SESubstanceManager& subMgr);

public:

  const SEScalar* GetScalar(const std::string& name) override;

  virtual eSwitch GetConnection() const;
  virtual void SetConnection(eSwitch c);

  virtual bool HasInletFlow() const;
  virtual SEScalarVolumePerTime& GetInletFlow();
  virtual double GetInletFlow(const VolumePerTimeUnit& unit) const;
  
  virtual bool HasInspiratoryExpiratoryRatio() const;
  virtual SEScalar& GetInspiratoryExpiratoryRatio();
  virtual double GetInspiratoryExpiratoryRatio() const;
    
  virtual bool HasOxygenFraction() const;
  virtual SEScalar0To1& GetOxygenFraction();
  virtual double GetOxygenFraction() const;

  virtual eAnesthesiaMachine_OxygenSource GetOxygenSource() const;
  virtual void SetOxygenSource(eAnesthesiaMachine_OxygenSource name);

  virtual bool HasPositiveEndExpiratoryPressure() const;
  virtual SEScalarPressure& GetPositiveEndExpiratoryPressure();
  virtual double GetPositiveEndExpiratoryPressure(const PressureUnit& unit) const;
  
  virtual eAnesthesiaMachine_PrimaryGas GetPrimaryGas() const;
  virtual void SetPrimaryGas(eAnesthesiaMachine_PrimaryGas name);
  
  virtual bool HasRespiratoryRate() const;
  virtual SEScalarFrequency& GetRespiratoryRate();
  virtual double GetRespiratoryRate(const FrequencyUnit& unit) const;

  virtual bool HasReliefValvePressure() const;
  virtual SEScalarPressure& GetReliefValvePressure();
  virtual double GetReliefValvePressure(const PressureUnit& unit) const;

  virtual bool HasPeakInspiratoryPressure() const;
  virtual SEScalarPressure& GetPeakInspiratoryPressure();
  virtual double GetPeakInspiratoryPressure(const PressureUnit& unit) const;

  virtual bool HasLeftChamber() const;
  virtual SEAnesthesiaMachineChamber& GetLeftChamber();
  virtual const SEAnesthesiaMachineChamber* GetLeftChamber() const;
  virtual void RemoveLeftChamber();

  virtual bool HasRightChamber() const;
  virtual SEAnesthesiaMachineChamber& GetRightChamber();
  virtual const SEAnesthesiaMachineChamber* GetRightChamber() const;
  virtual void RemoveRightChamber();

  virtual bool HasOxygenBottleOne() const;
  virtual SEAnesthesiaMachineOxygenBottle& GetOxygenBottleOne();
  virtual const SEAnesthesiaMachineOxygenBottle* GetOxygenBottleOne() const;
  virtual void RemoveOxygenBottleOne();

  virtual bool HasOxygenBottleTwo() const;
  virtual SEAnesthesiaMachineOxygenBottle& GetOxygenBottleTwo();
  virtual const SEAnesthesiaMachineOxygenBottle* GetOxygenBottleTwo() const;
  virtual void RemoveOxygenBottleTwo();

protected:

  eSwitch                                                m_Connection;
  SEScalarVolumePerTime*                                 m_InletFlow;
  SEScalar*                                              m_InspiratoryExpiratoryRatio;
  SEScalar0To1*                                          m_OxygenFraction;
  eAnesthesiaMachine_OxygenSource                        m_OxygenSource;
  SEScalarPressure*                                      m_PositiveEndExpiratoryPressure;
  eAnesthesiaMachine_PrimaryGas                          m_PrimaryGas;
  SEScalarFrequency*                                     m_RespiratoryRate;
  SEScalarPressure*                                      m_ReliefValvePressure;

  SEScalarPressure*                                      m_PeakInspiratoryPressure;

  SEAnesthesiaMachineChamber*                            m_LeftChamber;
  SEAnesthesiaMachineChamber*                            m_RightChamber;

  SEAnesthesiaMachineOxygenBottle*                       m_OxygenBottleOne;
  SEAnesthesiaMachineOxygenBottle*                       m_OxygenBottleTwo;
};
