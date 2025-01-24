/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
class SEAction;
class SEEquipmentAction;
class SEAnesthesiaMachine;
class SEAnesthesiaMachineAction;
class SEAnesthesiaMachineConfiguration;
class SEAnesthesiaMachineOxygenTankPressureLoss;
class SEAnesthesiaMachineOxygenWallPortPressureLoss;
class SEAnesthesiaMachineExpiratoryValveLeak;
class SEAnesthesiaMachineExpiratoryValveObstruction;
class SEAnesthesiaMachineInspiratoryValveLeak;
class SEAnesthesiaMachineInspiratoryValveObstruction;
class SEAnesthesiaMachineMaskLeak;
class SEAnesthesiaMachineSodaLimeFailure;
class SEAnesthesiaMachineTubeCuffLeak;
class SEAnesthesiaMachineVaporizerFailure;
class SEAnesthesiaMachineVentilatorPressureLoss;
class SEAnesthesiaMachineYPieceDisconnect;
class SEBagValveMask;
class SEBagValveMaskAction;
class SEBagValveMaskConfiguration;
class SEBagValveMaskAutomated;
class SEBagValveMaskInstantaneous;
class SEBagValveMaskSqueeze;
class SEECMO;
class SEECMOAction;
class SEECMOConfiguration;
class SEInhaler;
class SEInhalerAction;
class SEInhalerConfiguration;
class SEMechanicalVentilator;
class SEMechanicalVentilatorAction;
class SEMechanicalVentilatorConfiguration;
class SEMechanicalVentilatorHold;
class SEMechanicalVentilatorLeak;
class SEMechanicalVentilatorContinuousPositiveAirwayPressure;
class SEMechanicalVentilatorPressureControl;
class SEMechanicalVentilatorVolumeControl;
class SESubstanceManager;
CDM_BIND_DECL(ActionListData)
CDM_BIND_DECL(AnyEquipmentActionData)

class CDM_DECL SEEquipmentActionCollection : public Loggable
{
  friend class PBEngine;//friend the serialization class
  friend class SEActionManager;
protected:
  SEEquipmentActionCollection(SESubstanceManager& subMgr);
public:
  ~SEEquipmentActionCollection();

  // STATE ACTION
  bool HasAnesthesiaMachineConfiguration() const;
  SEAnesthesiaMachineConfiguration& GetAnesthesiaMachineConfiguration();
  const SEAnesthesiaMachineConfiguration* GetAnesthesiaMachineConfiguration() const;
  void RemoveAnesthesiaMachineConfiguration();

  bool HasBagValveMaskConfiguration() const;
  SEBagValveMaskConfiguration& GetBagValveMaskConfiguration();
  const SEBagValveMaskConfiguration* GetBagValveMaskConfiguration() const;
  void RemoveBagValveMaskConfiguration();

  bool HasECMOConfiguration() const;
  SEECMOConfiguration& GetECMOConfiguration();
  const SEECMOConfiguration* GetECMOConfiguration() const;
  void RemoveECMOConfiguration();

  bool HasInhalerConfiguration() const;
  SEInhalerConfiguration& GetInhalerConfiguration();
  const SEInhalerConfiguration* GetInhalerConfiguration() const;
  void RemoveInhalerConfiguration();

  bool HasMechanicalVentilatorConfiguration() const;
  SEMechanicalVentilatorConfiguration& GetMechanicalVentilatorConfiguration();
  const SEMechanicalVentilatorConfiguration* GetMechanicalVentilatorConfiguration() const;
  void RemoveMechanicalVentilatorConfiguration();

  // INCIDENT ACTIONS

  bool HasAnesthesiaMachineOxygenTankPressureLoss() const;
  SEAnesthesiaMachineOxygenTankPressureLoss& GetAnesthesiaMachineOxygenTankPressureLoss();
  const SEAnesthesiaMachineOxygenTankPressureLoss* GetAnesthesiaMachineOxygenTankPressureLoss() const;
  void RemoveAnesthesiaMachineOxygenTankPressureLoss();

  bool HasAnesthesiaMachineOxygenWallPortPressureLoss() const;
  SEAnesthesiaMachineOxygenWallPortPressureLoss& GetAnesthesiaMachineOxygenWallPortPressureLoss();
  const SEAnesthesiaMachineOxygenWallPortPressureLoss* GetAnesthesiaMachineOxygenWallPortPressureLoss() const;
  void RemoveAnesthesiaMachineOxygenWallPortPressureLoss();

  // FAILURE ACTIONS

  bool HasAnesthesiaMachineExpiratoryValveLeak() const;
  SEAnesthesiaMachineExpiratoryValveLeak& GetAnesthesiaMachineExpiratoryValveLeak();
  const SEAnesthesiaMachineExpiratoryValveLeak* GetAnesthesiaMachineExpiratoryValveLeak() const;
  void RemoveAnesthesiaMachineExpiratoryValveLeak();

  bool HasAnesthesiaMachineExpiratoryValveObstruction() const;
  SEAnesthesiaMachineExpiratoryValveObstruction& GetAnesthesiaMachineExpiratoryValveObstruction();
  const SEAnesthesiaMachineExpiratoryValveObstruction* GetAnesthesiaMachineExpiratoryValveObstruction() const;
  void RemoveAnesthesiaMachineExpiratoryValveObstruction();

  bool HasAnesthesiaMachineInspiratoryValveLeak() const;
  SEAnesthesiaMachineInspiratoryValveLeak& GetAnesthesiaMachineInspiratoryValveLeak();
  const SEAnesthesiaMachineInspiratoryValveLeak* GetAnesthesiaMachineInspiratoryValveLeak() const;
  void RemoveAnesthesiaMachineInspiratoryValveLeak();

  bool HasAnesthesiaMachineInspiratoryValveObstruction() const;
  SEAnesthesiaMachineInspiratoryValveObstruction& GetAnesthesiaMachineInspiratoryValveObstruction();
  const SEAnesthesiaMachineInspiratoryValveObstruction* GetAnesthesiaMachineInspiratoryValveObstruction() const;
  void RemoveAnesthesiaMachineInspiratoryValveObstruction();

  bool HasAnesthesiaMachineMaskLeak() const;
  SEAnesthesiaMachineMaskLeak& GetAnesthesiaMachineMaskLeak();
  const SEAnesthesiaMachineMaskLeak* GetAnesthesiaMachineMaskLeak() const;
  void RemoveAnesthesiaMachineMaskLeak();

  bool HasAnesthesiaMachineSodaLimeFailure() const;
  SEAnesthesiaMachineSodaLimeFailure& GetAnesthesiaMachineSodaLimeFailure();
  const SEAnesthesiaMachineSodaLimeFailure* GetAnesthesiaMachineSodaLimeFailure() const;
  void RemoveAnesthesiaMachineSodaLimeFailure();

  bool HasAnesthesiaMachineTubeCuffLeak() const;
  SEAnesthesiaMachineTubeCuffLeak& GetAnesthesiaMachineTubeCuffLeak();
  const SEAnesthesiaMachineTubeCuffLeak* GetAnesthesiaMachineTubeCuffLeak() const;
  void RemoveAnesthesiaMachineTubeCuffLeak();

  bool HasAnesthesiaMachineVaporizerFailure() const;
  SEAnesthesiaMachineVaporizerFailure& GetAnesthesiaMachineVaporizerFailure();
  const SEAnesthesiaMachineVaporizerFailure* GetAnesthesiaMachineVaporizerFailure() const;
  void RemoveAnesthesiaMachineVaporizerFailure();

  bool HasAnesthesiaMachineVentilatorPressureLoss() const;
  SEAnesthesiaMachineVentilatorPressureLoss& GetAnesthesiaMachineVentilatorPressureLoss();
  const SEAnesthesiaMachineVentilatorPressureLoss* GetAnesthesiaMachineVentilatorPressureLoss() const;
  void RemoveAnesthesiaMachineVentilatorPressureLoss();

  bool HasAnesthesiaMachineYPieceDisconnect() const;
  SEAnesthesiaMachineYPieceDisconnect& GetAnesthesiaMachineYPieceDisconnect();
  const SEAnesthesiaMachineYPieceDisconnect* GetAnesthesiaMachineYPieceDisconnect() const;
  void RemoveAnesthesiaMachineYPieceDisconnect();

  // ACTIONS
  bool HasActiveBagValveMaskAction() const;

  bool HasBagValveMaskAutomated() const;
  SEBagValveMaskAutomated& GetBagValveMaskAutomated();
  const SEBagValveMaskAutomated* GetBagValveMaskAutomated() const;
  void RemoveBagValveMaskAutomated();

  bool HasBagValveMaskInstantaneous() const;
  SEBagValveMaskInstantaneous& GetBagValveMaskInstantaneous();
  const SEBagValveMaskInstantaneous* GetBagValveMaskInstantaneous() const;
  void RemoveBagValveMaskInstantaneous();

  bool HasBagValveMaskSqueeze() const;
  SEBagValveMaskSqueeze& GetBagValveMaskSqueeze();
  const SEBagValveMaskSqueeze* GetBagValveMaskSqueeze() const;
  void RemoveBagValveMaskSqueeze();

  bool HasMechanicalVentilatorHold() const;
  SEMechanicalVentilatorHold& GetMechanicalVentilatorHold();
  const SEMechanicalVentilatorHold* GetMechanicalVentilatorHold() const;
  void RemoveMechanicalVentilatorHold();

  bool HasMechanicalVentilatorLeak() const;
  SEMechanicalVentilatorLeak& GetMechanicalVentilatorLeak();
  const SEMechanicalVentilatorLeak* GetMechanicalVentilatorLeak() const;
  void RemoveMechanicalVentilatorLeak();

  bool HasMechanicalVentilatorContinuousPositiveAirwayPressure() const;
  SEMechanicalVentilatorContinuousPositiveAirwayPressure& GetMechanicalVentilatorContinuousPositiveAirwayPressure();
  const SEMechanicalVentilatorContinuousPositiveAirwayPressure* GetMechanicalVentilatorContinuousPositiveAirwayPressure() const;
  void RemoveMechanicalVentilatorContinuousPositiveAirwayPressure();

  bool HasMechanicalVentilatorPressureControl() const;
  SEMechanicalVentilatorPressureControl& GetMechanicalVentilatorPressureControl();
  const SEMechanicalVentilatorPressureControl* GetMechanicalVentilatorPressureControl() const;
  void RemoveMechanicalVentilatorPressureControl();

  bool HasMechanicalVentilatorVolumeControl() const;
  SEMechanicalVentilatorVolumeControl& GetMechanicalVentilatorVolumeControl();
  const SEMechanicalVentilatorVolumeControl* GetMechanicalVentilatorVolumeControl() const;
  void RemoveMechanicalVentilatorVolumeControl();

  void RemoveMechanicalVentilatorActions();

  void GetAllActions(std::vector<const SEAction*>& v) const;
  const SEScalar* GetScalar(const std::string& actionName, const std::string& cmptName, const std::string& substance, const std::string& property);
  
protected:
  void Clear();
  bool ProcessAction(const SEEquipmentAction& action);

  SESubstanceManager&                                          m_SubMgr;

  SEAnesthesiaMachineConfiguration*                            m_AnesthesiaMachineConfiguration;
  //Anesthesia Machine Incidents
  SEAnesthesiaMachineOxygenTankPressureLoss*                   m_AnesthesiaMachineOxygenTankPressureLoss;
  SEAnesthesiaMachineOxygenWallPortPressureLoss*               m_AnesthesiaMachineOxygenWallPortPressureLoss;
  //Anesthesia Machine Failures
  SEAnesthesiaMachineExpiratoryValveLeak*                      m_AnesthesiaMachineExpiratoryValveLeak;
  SEAnesthesiaMachineExpiratoryValveObstruction*               m_AnesthesiaMachineExpiratoryValveObstruction;
  SEAnesthesiaMachineInspiratoryValveLeak*                     m_AnesthesiaMachineInspiratoryValveLeak;
  SEAnesthesiaMachineInspiratoryValveObstruction*              m_AnesthesiaMachineInspiratoryValveObstruction;
  SEAnesthesiaMachineMaskLeak*                                 m_AnesthesiaMachineMaskLeak;
  SEAnesthesiaMachineSodaLimeFailure*                          m_AnesthesiaMachineSodaLimeFailure;
  SEAnesthesiaMachineTubeCuffLeak*                             m_AnesthesiaMachineTubeCuffLeak;
  SEAnesthesiaMachineVaporizerFailure*                         m_AnesthesiaMachineVaporizerFailure;
  SEAnesthesiaMachineVentilatorPressureLoss*                   m_AnesthesiaMachineVentilatorPressureLoss;
  SEAnesthesiaMachineYPieceDisconnect*                         m_AnesthesiaMachineYPieceDisconnect;

  SEBagValveMaskConfiguration*                                 m_BagValveMaskConfiguration;
  SEBagValveMaskAutomated*                                     m_BagValveMaskAutomated;
  SEBagValveMaskInstantaneous*                                 m_BagValveMaskInstantaneous;
  SEBagValveMaskSqueeze*                                       m_BagValveMaskSqueeze;

  SEECMOConfiguration*                                         m_ECMOConfiguration;

  SEInhalerConfiguration*                                      m_InhalerConfiguration;

  SEMechanicalVentilatorConfiguration*                         m_MechanicalVentilatorConfiguration;
  SEMechanicalVentilatorHold*                                  m_MechanicalVentilatorHold;
  SEMechanicalVentilatorLeak*                                  m_MechanicalVentilatorLeak;
  SEMechanicalVentilatorContinuousPositiveAirwayPressure*      m_MechanicalVentilatorContinuousPositiveAirwayPressure;
  SEMechanicalVentilatorPressureControl*                       m_MechanicalVentilatorPressureControl;
  SEMechanicalVentilatorVolumeControl*                         m_MechanicalVentilatorVolumeControl;
};
