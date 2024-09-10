/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
class SECondition;
//Patient Conditions
class SEAcuteRespiratoryDistressSyndrome;
class SEChronicAnemia;
class SEChronicObstructivePulmonaryDisease;
class SEChronicHeartFailure;
class SEChronicPericardialEffusion;
class SEChronicRenalStenosis;
class SEChronicVentricularSystolicDysfunction;
class SEConsumeMeal;
class SEDehydration;
class SEImpairedAlveolarExchange;
class SEPneumonia;
class SEPulmonaryFibrosis;
class SEPulmonaryShunt;
class SESepsis;
// Environment Conditions
class SEInitialEnvironmentalConditions;
class SESubstanceManager;

class CDM_DECL SEConditionManager : public Loggable
{
  friend class PBEngine;//friend the serialization class
public:

  SEConditionManager(Logger* logger=nullptr);
  ~SEConditionManager();

  void Clear();
  void Copy(const SEConditionManager& src, const SESubstanceManager& subMgr);
  bool Copy(const SECondition& condition, const SESubstanceManager&);

  bool SerializeToString(std::string& output, eSerializationFormat m) const;
  bool SerializeToFile(const std::string& filename) const;
  bool SerializeFromString(const std::string& src, eSerializationFormat m, const SESubstanceManager& subMgr);
  bool SerializeFromFile(const std::string& filename, const SESubstanceManager& subMgr);

  // Not too many conditions, so just have one manager
  // If we start getting alot, I will make patient/environment/equipment condition managers, like the action managers

  bool IsEmpty() const;

  // Patient Conditions

  bool HasAcuteRespiratoryDistressSyndrome() const;
  SEAcuteRespiratoryDistressSyndrome& GetAcuteRespiratoryDistressSyndrome();
  const SEAcuteRespiratoryDistressSyndrome* GetAcuteRespiratoryDistressSyndrome() const;

  bool HasChronicAnemia() const;
  SEChronicAnemia& GetChronicAnemia();
  const SEChronicAnemia* GetChronicAnemia() const;

  bool HasChronicObstructivePulmonaryDisease() const;
  SEChronicObstructivePulmonaryDisease& GetChronicObstructivePulmonaryDisease();
  const SEChronicObstructivePulmonaryDisease* GetChronicObstructivePulmonaryDisease() const;

  bool HasChronicHeartFailure() const;
  bool HasChronicVentricularSystolicDysfunction() const;
  SEChronicVentricularSystolicDysfunction& GetChronicVentricularSystolicDysfunction();
  const SEChronicVentricularSystolicDysfunction* GetChronicVentricularSystolicDysfunction() const;

  bool HasChronicPericardialEffusion() const;
  SEChronicPericardialEffusion& GetChronicPericardialEffusion();
  const SEChronicPericardialEffusion* GetChronicPericardialEffusion() const;

  bool HasChronicRenalStenosis() const;
  SEChronicRenalStenosis& GetChronicRenalStenosis();
  const SEChronicRenalStenosis* GetChronicRenalStenosis() const;

  bool HasConsumeMeal() const;
  SEConsumeMeal& GetConsumeMeal();
  const SEConsumeMeal* GetConsumeMeal() const;

  bool HasDehydration() const;
  SEDehydration& GetDehydration();
  const SEDehydration* GetDehydration() const;

  bool HasImpairedAlveolarExchange() const;
  SEImpairedAlveolarExchange& GetImpairedAlveolarExchange();
  const SEImpairedAlveolarExchange* GetImpairedAlveolarExchange() const;

  bool HasPneumonia() const;
  SEPneumonia& GetPneumonia();
  const SEPneumonia* GetPneumonia() const;

  bool HasPulmonaryFibrosis() const;
  SEPulmonaryFibrosis& GetPulmonaryFibrosis();
  const SEPulmonaryFibrosis* GetPulmonaryFibrosis() const;

  bool HasPulmonaryShunt() const;
  SEPulmonaryShunt& GetPulmonaryShunt();
  const SEPulmonaryShunt* GetPulmonaryShunt() const;

  bool HasSepsis() const;
  SESepsis& GetSepsis();
  const SESepsis* GetSepsis() const;

  // Environment Conditions

  bool HasInitialEnvironmentalConditions() const;
  SEInitialEnvironmentalConditions& GetInitialEnvironmentalConditions();
  const SEInitialEnvironmentalConditions* GetInitialEnvironmentalConditions() const;

  // Helper to get a list of the active conditions
  void GetAllConditions(std::vector<const SECondition*>& v) const;

protected:

  SEAcuteRespiratoryDistressSyndrome*      m_ARDS;
  SEChronicAnemia*                         m_Anemia;
  SEChronicObstructivePulmonaryDisease*    m_COPD;
  SEChronicVentricularSystolicDysfunction* m_ChronicVentricularSystolicDysfunction;
  SEConsumeMeal*                           m_ConsumeMeal;
  SEDehydration*                           m_Dehydration;
  SEImpairedAlveolarExchange*              m_ImpairedAlveolarExchange;
  SEPneumonia*                             m_Pneumonia;
  SEChronicPericardialEffusion*            m_PericardialEffusion;
  SEChronicRenalStenosis*                  m_RenalStenosis;
  SEPulmonaryFibrosis*                     m_PulmonaryFibrosis;
  SEPulmonaryShunt*                        m_PulmonaryShunt;
  SESepsis*                                m_Sepsis;

  SEInitialEnvironmentalConditions*        m_InitialEnvironmentalConditions;

  std::stringstream m_ss;
};