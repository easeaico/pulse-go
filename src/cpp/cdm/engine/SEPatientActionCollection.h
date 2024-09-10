/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
class SEAction;
class SEPatientAction;
class SEAcuteRespiratoryDistressSyndromeExacerbation;
class SEAcuteStress;
class SEAirwayObstruction;
class SEAsthmaAttack;
class SEArrhythmia;
class SEBrainInjury;
class SEBronchoconstriction;
class SECardiovascularMechanicsModification;
class SEChestCompression;
class SEChestCompressionAutomated;
class SEChestCompressionInstantaneous;
class SEChestOcclusiveDressing;
class SEConsciousRespiration;
class SEConsumeNutrients;
class SEChronicObstructivePulmonaryDiseaseExacerbation;
class SEDyspnea;
class SEExercise;
class SEHemorrhage;
enum class eHemorrhage_Compartment;
class SEHemothorax;
class SEImpairedAlveolarExchangeExacerbation;
class SEIntubation;
class SEMechanicalVentilation;
class SENeedleDecompression;
class SEPericardialEffusion;
class SEPneumoniaExacerbation;
class SEPulmonaryShuntExacerbation;
class SERespiratoryFatigue;
class SERespiratoryMechanicsConfiguration;
class SERespiratoryMechanicsModification;
class SESubstanceBolus;
class SESubstanceCompoundInfusion;
class SESubstanceInfusion;
class SESupplementalOxygen;
class SETensionPneumothorax;
class SETubeThoracostomy;
class SEUrinate;
class SEPatientAssessmentRequest;
class SESubstance;
class SESubstanceCompound;
class SESubstanceManager;

class CDM_DECL SEPatientActionCollection : public Loggable
{
  friend class PBEngine;//friend the serialization class
  friend class SEActionManager;
protected:
  SEPatientActionCollection(SESubstanceManager& subMgr);
public:
  ~SEPatientActionCollection();

  bool HasAcuteRespiratoryDistressSyndromeExacerbation() const;
  SEAcuteRespiratoryDistressSyndromeExacerbation& GetAcuteRespiratoryDistressSyndromeExacerbation();
  const SEAcuteRespiratoryDistressSyndromeExacerbation* GetAcuteRespiratoryDistressSyndromeExacerbation() const;
  void RemoveAcuteRespiratoryDistressSyndromeExacerbation();

  bool HasAcuteStress() const;
  SEAcuteStress& GetAcuteStress();
  const SEAcuteStress* GetAcuteStress() const;
  void RemoveAcuteStress();

  bool HasAirwayObstruction() const;
  SEAirwayObstruction& GetAirwayObstruction();
  const SEAirwayObstruction* GetAirwayObstruction() const;
  void RemoveAirwayObstruction();

  bool HasArrhythmia() const;
  SEArrhythmia& GetArrhythmia();
  const SEArrhythmia* GetArrhythmia() const;
  void RemoveArrhythmia();

  bool HasAsthmaAttack() const;
  SEAsthmaAttack& GetAsthmaAttack();
  const SEAsthmaAttack* GetAsthmaAttack() const;
  void RemoveAsthmaAttack();

  bool HasBrainInjury() const;
  SEBrainInjury& GetBrainInjury();
  const SEBrainInjury* GetBrainInjury() const;
  void RemoveBrainInjury();

  bool HasBronchoconstriction() const;
  SEBronchoconstriction& GetBronchoconstriction();
  const SEBronchoconstriction* GetBronchoconstriction() const;
  void RemoveBronchoconstriction();

  bool HasCardiovascularMechanicsModification() const;
  SECardiovascularMechanicsModification& GetCardiovascularMechanicsModification();
  const SECardiovascularMechanicsModification* GetCardiovascularMechanicsModification() const;
  void RemoveCardiovascularMechanicsModification();

  bool HasActiveCPRAction() const;

  bool HasChestCompression() const;
  SEChestCompression& GetChestCompression();
  const SEChestCompression* GetChestCompression() const;
  void RemoveChestCompression();

  bool HasChestCompressionAutomated() const;
  SEChestCompressionAutomated& GetChestCompressionAutomated();
  const SEChestCompressionAutomated* GetChestCompressionAutomated() const;
  void RemoveChestCompressionAutomated();

  bool HasChestCompressionInstantaneous() const;
  SEChestCompressionInstantaneous& GetChestCompressionInstantaneous();
  const SEChestCompressionInstantaneous* GetChestCompressionInstantaneous() const;
  void RemoveChestCompressionInstantaneous();

  bool HasChestOcclusiveDressing() const;
  bool HasLeftChestOcclusiveDressing() const;
  SEChestOcclusiveDressing& GetLeftChestOcclusiveDressing();
  const SEChestOcclusiveDressing* GetLeftChestOcclusiveDressing() const;
  void RemoveLeftChestOcclusiveDressing();
  bool HasRightChestOcclusiveDressing() const;
  SEChestOcclusiveDressing& GetRightChestOcclusiveDressing();
  const SEChestOcclusiveDressing* GetRightChestOcclusiveDressing() const;
  void RemoveRightChestOcclusiveDressing();

  bool HasChronicObstructivePulmonaryDiseaseExacerbation() const;
  SEChronicObstructivePulmonaryDiseaseExacerbation& GetChronicObstructivePulmonaryDiseaseExacerbation();
  const SEChronicObstructivePulmonaryDiseaseExacerbation* GetChronicObstructivePulmonaryDiseaseExacerbation() const;
  void RemoveChronicObstructivePulmonaryDiseaseExacerbation();

  bool HasConsciousRespiration() const;
  SEConsciousRespiration& GetConsciousRespiration();
  const SEConsciousRespiration* GetConsciousRespiration() const;
  void RemoveConsciousRespiration();

  bool HasConsumeNutrients() const;
  SEConsumeNutrients& GetConsumeNutrients();
  const SEConsumeNutrients* GetConsumeNutrients() const;
  void RemoveConsumeNutrients();

  bool HasDyspnea() const;
  SEDyspnea& GetDyspnea();
  const SEDyspnea* GetDyspnea() const;
  void RemoveDyspnea();

  bool HasExercise() const;
  SEExercise& GetExercise();
  const SEExercise* GetExercise() const;
  void RemoveExercise();

  bool HasHemorrhage() const;
  bool HasHemorrhage(eHemorrhage_Compartment cmpt) const;
  SEHemorrhage& GetHemorrhage(eHemorrhage_Compartment cmpt);
  const SEHemorrhage* GetHemorrhage(eHemorrhage_Compartment cmpt) const;
  const std::vector<SEHemorrhage*>& GetHemorrhages();
  const std::vector<const SEHemorrhage*> GetHemorrhages() const;
  void RemoveHemorrhage(eHemorrhage_Compartment cmpt);

  bool HasHemothorax() const;

  bool HasLeftHemothorax() const;
  SEHemothorax& GetLeftHemothorax();
  const SEHemothorax* GetLeftHemothorax() const;
  void RemoveLeftHemothorax();

  bool HasRightHemothorax() const;
  SEHemothorax& GetRightHemothorax();
  const SEHemothorax* GetRightHemothorax() const;
  void RemoveRightHemothorax();

  bool HasImpairedAlveolarExchangeExacerbation() const;
  SEImpairedAlveolarExchangeExacerbation& GetImpairedAlveolarExchangeExacerbation();
  const SEImpairedAlveolarExchangeExacerbation* GetImpairedAlveolarExchangeExacerbation() const;
  void RemoveImpairedAlveolarExchangeExacerbation();

  bool HasIntubation() const;
  SEIntubation& GetIntubation();
  const SEIntubation* GetIntubation() const;
  void RemoveIntubation();

  bool HasMechanicalVentilation() const;
  SEMechanicalVentilation& GetMechanicalVentilation();
  const SEMechanicalVentilation* GetMechanicalVentilation() const;
  void RemoveMechanicalVentilation();

  bool HasNeedleDecompression() const;
  bool HasLeftNeedleDecompression() const;
  SENeedleDecompression& GetLeftNeedleDecompression();
  const SENeedleDecompression* GetLeftNeedleDecompression() const;
  void RemoveLeftNeedleDecompression();

  bool HasRightNeedleDecompression() const;
  SENeedleDecompression& GetRightNeedleDecompression();
  const SENeedleDecompression* GetRightNeedleDecompression() const;
  void RemoveRightNeedleDecompression();

  bool HasPericardialEffusion() const;
  SEPericardialEffusion& GetPericardialEffusion();
  const SEPericardialEffusion* GetPericardialEffusion() const;
  void RemovePericardialEffusion();

  bool HasPneumoniaExacerbation() const;
  SEPneumoniaExacerbation& GetPneumoniaExacerbation();
  const SEPneumoniaExacerbation* GetPneumoniaExacerbation() const;
  void RemovePneumoniaExacerbation();

  bool HasPulmonaryShuntExacerbation() const;
  SEPulmonaryShuntExacerbation& GetPulmonaryShuntExacerbation();
  const SEPulmonaryShuntExacerbation* GetPulmonaryShuntExacerbation() const;
  void RemovePulmonaryShuntExacerbation();

  bool HasRespiratoryFatigue() const;
  SERespiratoryFatigue& GetRespiratoryFatigue();
  const SERespiratoryFatigue* GetRespiratoryFatigue() const;
  void RemoveRespiratoryFatigue();

  bool HasRespiratoryMechanicsConfiguration() const;
  SERespiratoryMechanicsConfiguration& GetRespiratoryMechanicsConfiguration();
  const SERespiratoryMechanicsConfiguration* GetRespiratoryMechanicsConfiguration() const;
  void RemoveRespiratoryMechanicsConfiguration();

  bool HasRespiratoryMechanicsModification() const;
  SERespiratoryMechanicsModification& GetRespiratoryMechanicsModification();
  const SERespiratoryMechanicsModification* GetRespiratoryMechanicsModification() const;
  void RemoveRespiratoryMechanicsModification();

  bool HasSubstanceBolus() const;
  bool HasSubstanceBolus(const SESubstance& sub) const;
  SESubstanceBolus& GetSubstanceBolus(const SESubstance& sub);
  const SESubstanceBolus* GetSubstanceBolus(const SESubstance& sub) const;
  const std::vector<SESubstanceBolus*>& GetSubstanceBoluses();
  const std::vector<const SESubstanceBolus*> GetSubstanceBoluses() const;
  void RemoveSubstanceBolus(const SESubstance& sub);

  bool HasSubstanceInfusion() const;
  bool HasSubstanceInfusion(const SESubstance& sub) const;
  SESubstanceInfusion& GetSubstanceInfusion(const SESubstance& sub);
  const SESubstanceInfusion* GetSubstanceInfusion(const SESubstance& sub) const;
  const std::vector<SESubstanceInfusion*>& GetSubstanceInfusions();
  const std::vector<const SESubstanceInfusion*> GetSubstanceInfusions() const;
  void RemoveSubstanceInfusion(const SESubstance& sub);

  bool HasSubstanceCompoundInfusion() const;
  bool HasSubstanceCompoundInfusion(const SESubstanceCompound& sub) const;
  SESubstanceCompoundInfusion& GetSubstanceCompoundInfusion(const SESubstanceCompound& sub);
  const SESubstanceCompoundInfusion* GetSubstanceCompoundInfusion(const SESubstanceCompound& sub) const;
  const std::vector<SESubstanceCompoundInfusion*>& GetSubstanceCompoundInfusions();
  const std::vector<const SESubstanceCompoundInfusion*>  GetSubstanceCompoundInfusions() const;
  void RemoveSubstanceCompoundInfusion(const SESubstanceCompound& sub);

  bool HasSupplementalOxygen() const;
  SESupplementalOxygen& GetSupplementalOxygen();
  const SESupplementalOxygen* GetSupplementalOxygen() const;
  void RemoveSupplementalOxygen();

  bool HasTensionPneumothorax() const;

  bool HasLeftClosedTensionPneumothorax() const;
  SETensionPneumothorax& GetLeftClosedTensionPneumothorax();
  const SETensionPneumothorax* GetLeftClosedTensionPneumothorax() const;
  void RemoveLeftClosedTensionPneumothorax();

  bool HasLeftOpenTensionPneumothorax() const;
  SETensionPneumothorax& GetLeftOpenTensionPneumothorax();
  const SETensionPneumothorax* GetLeftOpenTensionPneumothorax() const;
  void RemoveLeftOpenTensionPneumothorax();

  bool HasRightClosedTensionPneumothorax() const;
  SETensionPneumothorax& GetRightClosedTensionPneumothorax();
  const SETensionPneumothorax* GetRightClosedTensionPneumothorax() const;
  void RemoveRightClosedTensionPneumothorax();

  bool HasRightOpenTensionPneumothorax() const;
  SETensionPneumothorax& GetRightOpenTensionPneumothorax();
  const SETensionPneumothorax* GetRightOpenTensionPneumothorax() const;
  void RemoveRightOpenTensionPneumothorax();

  bool HasTubeThoracostomy() const;

  bool HasLeftTubeThoracostomy() const;
  SETubeThoracostomy& GetLeftTubeThoracostomy();
  const SETubeThoracostomy* GetLeftTubeThoracostomy() const;
  void RemoveLeftTubeThoracostomy();

  bool HasRightTubeThoracostomy() const;
  SETubeThoracostomy& GetRightTubeThoracostomy();
  const SETubeThoracostomy* GetRightTubeThoracostomy() const;
  void RemoveRightTubeThoracostomy();

  bool HasUrinate() const;
  SEUrinate& GetUrinate();
  const SEUrinate* GetUrinate() const;
  void RemoveUrinate();

  void GetAllActions(std::vector<const SEAction*>& v) const;
  const SEScalar* GetScalar(const std::string& actionName, const std::string& cmptName, const std::string& substance, const std::string& property);

protected:
  void Clear();
  bool ProcessAction(const SEPatientAction& action);

  SESubstanceManager&                               m_SubMgr;

  SEAcuteRespiratoryDistressSyndromeExacerbation*   m_ARDSExacerbation;
  SEAcuteStress*                                    m_AcuteStress;
  SEAirwayObstruction*                              m_AirwayObstruction;
  SEArrhythmia*                                     m_Arrhythmia;
  SEAsthmaAttack*                                   m_AsthmaAttack;
  SEBrainInjury*                                    m_BrainInjury;
  SEBronchoconstriction*                            m_Bronchoconstriction;
  SECardiovascularMechanicsModification*            m_CardiovascularMechanicsModification;
  SEChestCompression*                               m_ChestCompression;
  SEChestCompressionAutomated*                      m_ChestCompressionAutomated;
  SEChestCompressionInstantaneous*                  m_ChestCompressionInstantaneous;
  SEChestOcclusiveDressing*                         m_LeftChestOcclusiveDressing;
  SEChestOcclusiveDressing*                         m_RightChestOcclusiveDressing;
  SEConsciousRespiration*                           m_ConsciousRespiration;
  SEConsumeNutrients*                               m_ConsumeNutrients;
  SEChronicObstructivePulmonaryDiseaseExacerbation* m_COPDExacerbation;
  SEDyspnea*                                        m_Dyspnea;
  SEExercise*                                       m_Exercise;
  SEHemothorax*                                     m_LeftHemothorax;
  SEHemothorax*                                     m_RightHemothorax;
  SEIntubation*                                     m_Intubation;
  SEImpairedAlveolarExchangeExacerbation*           m_ImpairedAlveolarExchangeExacerbation;
  SEMechanicalVentilation*                          m_MechanicalVentilation;
  SENeedleDecompression*                            m_LeftNeedleDecompression;
  SENeedleDecompression*                            m_RightNeedleDecompression;
  SEPericardialEffusion*                            m_PericardialEffusion;
  SEPneumoniaExacerbation*                          m_PneumoniaExacerbation;
  SEPulmonaryShuntExacerbation*                     m_PulmonaryShuntExacerbation;
  SERespiratoryFatigue*                             m_RespiratoryFatigue;
  SERespiratoryMechanicsConfiguration*              m_RespiratoryMechanicsConfiguration;
  SERespiratoryMechanicsModification*               m_RespiratoryMechanicsModification;
  SESupplementalOxygen*                             m_SupplementalOxygen;
  SETensionPneumothorax*                            m_LeftClosedTensionPneumothorax;
  SETensionPneumothorax*                            m_LeftOpenTensionPneumothorax;
  SETensionPneumothorax*                            m_RightClosedTensionPneumothorax;
  SETensionPneumothorax*                            m_RightOpenTensionPneumothorax;
  SETubeThoracostomy*                               m_LeftTubeThoracostomy;
  SETubeThoracostomy*                               m_RightTubeThoracostomy;
  SEUrinate*                                        m_Urinate;

  std::vector<SEHemorrhage*>                        m_Hemorrhages;
  std::vector<SESubstanceBolus*>                    m_SubstanceBoluses;
  std::vector<SESubstanceInfusion*>                 m_SubstanceInfusions;
  std::vector<SESubstanceCompoundInfusion*>         m_SubstanceCompoundInfusions;
};
