/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once

#include "engine/CommonDefs.h"
#include "cdm/PhysiologyEngine.h"
#include "cdm/utils/ScopedMutex.h"

struct PULSE_DECL PulseBuildInformation
{
  static std::string Time();
  static std::string Hash();
  static std::string Version();
};

enum class eModelType { HumanAdultWholeBody=0, HumanAdultVentilationMechanics, HumanAdultHemodynamics };
PULSE_DECL bool eModelType_ValueOf(const std::string s, eModelType& t);

PULSE_DECL std::unique_ptr<PhysiologyEngine> CreatePulseEngine(eModelType type = eModelType::HumanAdultWholeBody, Logger* logger = nullptr);

namespace pulse
{
  ////////////////////////////////////
  // All Compartments used by Pulse //
  ////////////////////////////////////

  class Graph
  {
  public:
    DEFINE_STATIC_STRING(ActiveCardiovascular);
    DEFINE_STATIC_STRING(Cardiovascular);
    DEFINE_STATIC_STRING(Renal);
    DEFINE_STATIC_STRING(Respiratory);
    DEFINE_STATIC_STRING(RespiratoryAndAnesthesiaMachine);
    DEFINE_STATIC_STRING(RespiratoryAndBagValveMask);
    DEFINE_STATIC_STRING(RespiratoryAndInhaler);
    DEFINE_STATIC_STRING(RespiratoryAndMechanicalVentilation);
    DEFINE_STATIC_STRING(RespiratoryAndMechanicalVentilator);
    DEFINE_STATIC_STRING(RespiratoryAndNasalCannula);
    DEFINE_STATIC_STRING(RespiratoryAndNonRebreatherMask);
    DEFINE_STATIC_STRING(RespiratoryAndSimpleMask);
    DEFINE_STATIC_STRING(Aerosol);
    DEFINE_STATIC_STRING(AerosolAndBagValveMask);
    DEFINE_STATIC_STRING(AerosolAndInhaler);
    DEFINE_STATIC_STRING(AerosolAndMechanicalVentilation);
    DEFINE_STATIC_STRING(AerosolAndMechanicalVentilator);
    DEFINE_STATIC_STRING(AnesthesiaMachine);
    DEFINE_STATIC_STRING(BagValveMask);
    DEFINE_STATIC_STRING(MechanicalVentilator);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(ActiveCardiovascular);
        _values.push_back(Cardiovascular);
        _values.push_back(Renal);
        _values.push_back(Respiratory);
        _values.push_back(RespiratoryAndAnesthesiaMachine);
        _values.push_back(RespiratoryAndBagValveMask);
        _values.push_back(RespiratoryAndInhaler);
        _values.push_back(RespiratoryAndMechanicalVentilation);
        _values.push_back(RespiratoryAndMechanicalVentilator);
        _values.push_back(RespiratoryAndNasalCannula);
        _values.push_back(RespiratoryAndNonRebreatherMask);
        _values.push_back(RespiratoryAndSimpleMask);
        _values.push_back(Aerosol);
        _values.push_back(AerosolAndBagValveMask);
        _values.push_back(AerosolAndInhaler);
        _values.push_back(AerosolAndMechanicalVentilation);
        _values.push_back(AerosolAndMechanicalVentilator);
        _values.push_back(BagValveMask);
        _values.push_back(AnesthesiaMachine);
        _values.push_back(MechanicalVentilator);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  // TODO Rachel
  class CerebrospinalFluidCompartment
  {
  public:
    DEFINE_STATIC_STRING(IntracranialSpace);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(IntracranialSpace);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class ChymeCompartment
  {
  public:
    DEFINE_STATIC_STRING_EX(SmallIntestine, SmallIntestineChyme);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(SmallIntestine);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class ChymeLink
  {
  public:
    DEFINE_STATIC_STRING(SmallIntestineChymeToVasculature);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(SmallIntestineChymeToVasculature);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class PulmonaryCompartment
  {
  public:
    DEFINE_STATIC_STRING(Airway);
    DEFINE_STATIC_STRING(Stomach);
    DEFINE_STATIC_STRING(Pharynx);
    DEFINE_STATIC_STRING(Carina);
    DEFINE_STATIC_STRING_EX(Lungs, PulmonaryLungs);
    DEFINE_STATIC_STRING(Alveoli);
    DEFINE_STATIC_STRING(AlveolarDeadSpace);
    DEFINE_STATIC_STRING(AnatomicDeadSpace);
    /**/DEFINE_STATIC_STRING_EX(LeftLung, LeftLungPulmonary);
    /***/DEFINE_STATIC_STRING(LeftAnatomicDeadSpace);
    /***/DEFINE_STATIC_STRING(LeftAlveolarDeadSpace);
    /***/DEFINE_STATIC_STRING(LeftAlveoli);
    /**/DEFINE_STATIC_STRING_EX(RightLung, RightLungPulmonary);
    /***/DEFINE_STATIC_STRING(RightAnatomicDeadSpace);
    /***/DEFINE_STATIC_STRING(RightAlveolarDeadSpace);
    /***/DEFINE_STATIC_STRING(RightAlveoli);
    DEFINE_STATIC_STRING(PleuralCavity);
    /**/DEFINE_STATIC_STRING(LeftPleuralCavity);
    /**/DEFINE_STATIC_STRING(RightPleuralCavity);

    DEFINE_STATIC_STRING(LeftAlveoliLeak);
    DEFINE_STATIC_STRING(LeftChestLeak);
    DEFINE_STATIC_STRING(LeftNeedle);
    DEFINE_STATIC_STRING(RightAlveoliLeak);
    DEFINE_STATIC_STRING(RightChestLeak);
    DEFINE_STATIC_STRING(RightNeedle);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Airway);
        _values.push_back(Stomach);
        _values.push_back(Pharynx);
        _values.push_back(Carina);
        _values.push_back(Lungs);
        _values.push_back(Alveoli);
        _values.push_back(AlveolarDeadSpace);
        _values.push_back(AnatomicDeadSpace);
        _values.push_back(LeftLung);
        _values.push_back(LeftAnatomicDeadSpace);
        _values.push_back(LeftAlveolarDeadSpace);
        _values.push_back(LeftAlveoli);
        _values.push_back(RightLung);
        _values.push_back(RightAnatomicDeadSpace);
        _values.push_back(RightAlveolarDeadSpace);
        _values.push_back(RightAlveoli);
        _values.push_back(PleuralCavity);
        _values.push_back(LeftPleuralCavity);
        _values.push_back(RightPleuralCavity);
        _values.push_back(LeftAlveoliLeak);
        _values.push_back(LeftChestLeak);
        _values.push_back(LeftNeedle);
        _values.push_back(RightAlveoliLeak);
        _values.push_back(RightChestLeak);
        _values.push_back(RightNeedle);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class PulmonaryLink
  {
  public:
    DEFINE_STATIC_STRING(EnvironmentToAirway);
    DEFINE_STATIC_STRING(AirwayToPharynx);
    DEFINE_STATIC_STRING(AirwayToStomach);
    DEFINE_STATIC_STRING(PharynxToCarina);
    DEFINE_STATIC_STRING(PharynxToEnvironment);
    DEFINE_STATIC_STRING(CarinaToLeftAnatomicDeadSpace);
    DEFINE_STATIC_STRING(LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace);
    DEFINE_STATIC_STRING(LeftAlveolarDeadSpaceToLeftAlveoli);
    DEFINE_STATIC_STRING(CarinaToRightAnatomicDeadSpace);
    DEFINE_STATIC_STRING(RightAnatomicDeadSpaceToRightAlveolarDeadSpace);
    DEFINE_STATIC_STRING(RightAlveolarDeadSpaceToRightAlveoli);
    DEFINE_STATIC_STRING(EnvironmentToLeftChestLeak);
    DEFINE_STATIC_STRING(EnvironmentToRightChestLeak);
    DEFINE_STATIC_STRING(LeftAlveoliLeakToLeftPleural);
    DEFINE_STATIC_STRING(LeftAlveoliToLeftAlveoliLeak);
    DEFINE_STATIC_STRING(LeftChestLeakToLeftPleural);
    DEFINE_STATIC_STRING(LeftNeedleToLeftPleural);
    DEFINE_STATIC_STRING(EnvironmentToLeftNeedle);
    DEFINE_STATIC_STRING(RightAlveoliLeakToRightPleural);
    DEFINE_STATIC_STRING(RightAlveoliToRightAlveoliLeak);
    DEFINE_STATIC_STRING(RightChestLeakToRightPleural);
    DEFINE_STATIC_STRING(RightNeedleToRightPleural);
    DEFINE_STATIC_STRING(EnvironmentToRightNeedle);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(EnvironmentToAirway);
        _values.push_back(AirwayToPharynx);
        _values.push_back(AirwayToStomach);
        _values.push_back(PharynxToCarina);
        _values.push_back(PharynxToEnvironment);
        _values.push_back(CarinaToLeftAnatomicDeadSpace);
        _values.push_back(LeftAnatomicDeadSpaceToLeftAlveolarDeadSpace);
        _values.push_back(LeftAlveolarDeadSpaceToLeftAlveoli);
        _values.push_back(CarinaToRightAnatomicDeadSpace);
        _values.push_back(RightAnatomicDeadSpaceToRightAlveolarDeadSpace);
        _values.push_back(RightAlveolarDeadSpaceToRightAlveoli);
        _values.push_back(EnvironmentToLeftChestLeak);
        _values.push_back(EnvironmentToRightChestLeak);
        _values.push_back(LeftAlveoliLeakToLeftPleural);
        _values.push_back(LeftAlveoliToLeftAlveoliLeak);
        _values.push_back(LeftChestLeakToLeftPleural);
        _values.push_back(LeftNeedleToLeftPleural);
        _values.push_back(EnvironmentToLeftNeedle);
        _values.push_back(RightAlveoliLeakToRightPleural);
        _values.push_back(RightAlveoliToRightAlveoliLeak);
        _values.push_back(RightChestLeakToRightPleural);
        _values.push_back(RightNeedleToRightPleural);
        _values.push_back(EnvironmentToRightNeedle);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v)!=_values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class ExpandedPulmonaryCompartment
  {
  public:

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {

      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class ExpandedPulmonaryLink
  {
  public:

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {

      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class TissueCompartment
  {
  public:
    DEFINE_STATIC_STRING_EX(Bone, BoneTissue);
    DEFINE_STATIC_STRING_EX(Brain, BrainTissue);
    DEFINE_STATIC_STRING_EX(Fat, FatTissue);
    DEFINE_STATIC_STRING_EX(Gut, GutTissue);
    DEFINE_STATIC_STRING_EX(LeftKidney, LeftKidneyTissue);
    DEFINE_STATIC_STRING_EX(LeftLung, LeftLungTissue);
    DEFINE_STATIC_STRING_EX(Liver, LiverTissue);
    DEFINE_STATIC_STRING_EX(Muscle, MuscleTissue);
    DEFINE_STATIC_STRING_EX(Myocardium, MyocardiumTissue);
    DEFINE_STATIC_STRING_EX(RightKidney, RightKidneyTissue);
    DEFINE_STATIC_STRING_EX(RightLung, RightLungTissue);
    DEFINE_STATIC_STRING_EX(Skin, SkinTissue);
    DEFINE_STATIC_STRING_EX(Spleen, SpleenTissue);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Bone);
        _values.push_back(Brain);
        _values.push_back(Fat);
        _values.push_back(Gut);
        _values.push_back(LeftKidney);
        _values.push_back(LeftLung);
        _values.push_back(Liver);
        _values.push_back(Muscle);
        _values.push_back(Myocardium);
        _values.push_back(RightKidney);
        _values.push_back(RightLung);
        _values.push_back(Skin);
        _values.push_back(Spleen);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class ExtravascularCompartment
  {
  public:
    DEFINE_STATIC_STRING_EX(BoneExtracellular, BoneTissueExtracellular);
    DEFINE_STATIC_STRING_EX(BrainExtracellular, BrainTissueExtracellular);
    DEFINE_STATIC_STRING_EX(FatExtracellular, FatTissueExtracellular);
    DEFINE_STATIC_STRING_EX(GutExtracellular, GutTissueExtracellular);
    DEFINE_STATIC_STRING_EX(LeftKidneyExtracellular, LeftKidneyTissueExtracellular);
    DEFINE_STATIC_STRING_EX(LeftLungExtracellular, LeftLungTissueExtracellular);
    DEFINE_STATIC_STRING_EX(LiverExtracellular, LiverTissueExtracellular);
    DEFINE_STATIC_STRING_EX(MuscleExtracellular, MuscleTissueExtracellular);
    DEFINE_STATIC_STRING_EX(MyocardiumExtracellular, MyocardiumTissueExtracellular);
    DEFINE_STATIC_STRING_EX(RightKidneyExtracellular, RightKidneyTissueExtracellular);
    DEFINE_STATIC_STRING_EX(RightLungExtracellular, RightLungTissueExtracellular);
    DEFINE_STATIC_STRING_EX(SkinExtracellular, SkinTissueExtracellular);
    DEFINE_STATIC_STRING_EX(SpleenExtracellular, SpleenTissueExtracellular);

    DEFINE_STATIC_STRING_EX(BoneIntracellular, BoneTissueIntracellular);
    DEFINE_STATIC_STRING_EX(BrainIntracellular, BrainTissueIntracellular);
    DEFINE_STATIC_STRING_EX(FatIntracellular, FatTissueIntracellular);
    DEFINE_STATIC_STRING_EX(GutIntracellular, GutTissueIntracellular);
    DEFINE_STATIC_STRING_EX(LeftKidneyIntracellular, LeftKidneyTissueIntracellular);
    DEFINE_STATIC_STRING_EX(LeftLungIntracellular, LeftLungTissueIntracellular);
    DEFINE_STATIC_STRING_EX(LiverIntracellular, LiverTissueIntracellular);
    DEFINE_STATIC_STRING_EX(MuscleIntracellular, MuscleTissueIntracellular);
    DEFINE_STATIC_STRING_EX(MyocardiumIntracellular, MyocardiumTissueIntracellular);
    DEFINE_STATIC_STRING_EX(RightKidneyIntracellular, RightKidneyTissueIntracellular);
    DEFINE_STATIC_STRING_EX(RightLungIntracellular, RightLungTissueIntracellular);
    DEFINE_STATIC_STRING_EX(SkinIntracellular, SkinTissueIntracellular);
    DEFINE_STATIC_STRING_EX(SpleenIntracellular, SpleenTissueIntracellular);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(BoneExtracellular);
        _values.push_back(BrainExtracellular);
        _values.push_back(FatExtracellular);
        _values.push_back(GutExtracellular);
        _values.push_back(LeftKidneyExtracellular);
        _values.push_back(LeftLungExtracellular);
        _values.push_back(LiverExtracellular);
        _values.push_back(MuscleExtracellular);
        _values.push_back(MyocardiumExtracellular);
        _values.push_back(RightKidneyExtracellular);
        _values.push_back(RightLungExtracellular);
        _values.push_back(SkinExtracellular);
        _values.push_back(SpleenExtracellular);

        _values.push_back(BoneIntracellular);
        _values.push_back(BrainIntracellular);
        _values.push_back(FatIntracellular);
        _values.push_back(GutIntracellular);
        _values.push_back(LeftKidneyIntracellular);
        _values.push_back(LeftLungIntracellular);
        _values.push_back(LiverIntracellular);
        _values.push_back(MuscleIntracellular);
        _values.push_back(MyocardiumIntracellular);
        _values.push_back(RightKidneyIntracellular);
        _values.push_back(RightLungIntracellular);
        _values.push_back(SkinIntracellular);
        _values.push_back(SpleenIntracellular);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class VascularCompartment
  {
  public:
    // Cardio
    DEFINE_STATIC_STRING(Aorta);
    DEFINE_STATIC_STRING(Heart);
    /**/DEFINE_STATIC_STRING_EX(Myocardium, MyocardiumVasculature);
    /**/DEFINE_STATIC_STRING(LeftHeart);
    /**/DEFINE_STATIC_STRING(RightHeart);
    /**/DEFINE_STATIC_STRING(Pericardium);
    DEFINE_STATIC_STRING(VenaCava);
    // Pulmonary
    DEFINE_STATIC_STRING(PulmonaryArteries);
    DEFINE_STATIC_STRING(PulmonaryCapillaries);
    DEFINE_STATIC_STRING(PulmonaryVeins);
    DEFINE_STATIC_STRING_EX(Lungs, LungsVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftLung, LeftLungVasculature);
    /***/DEFINE_STATIC_STRING(LeftPulmonaryArteries);
    /***/DEFINE_STATIC_STRING(LeftPulmonaryCapillaries);
    /***/DEFINE_STATIC_STRING(LeftPulmonaryVeins);
    /**/DEFINE_STATIC_STRING_EX(RightLung, RightLungVasculature);
    /***/DEFINE_STATIC_STRING(RightPulmonaryArteries);
    /***/DEFINE_STATIC_STRING(RightPulmonaryCapillaries);
    /***/DEFINE_STATIC_STRING(RightPulmonaryVeins);
    // Renal
    DEFINE_STATIC_STRING_EX(Kidneys, KidneyVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftKidney, LeftKidneyVasculature);
    /***/DEFINE_STATIC_STRING(LeftRenalArtery);
    /***/DEFINE_STATIC_STRING(LeftNephron);
    /*********/DEFINE_STATIC_STRING(LeftAfferentArteriole);
    /*********/DEFINE_STATIC_STRING(LeftGlomerularCapillaries);
    /*********/DEFINE_STATIC_STRING(LeftEfferentArteriole);
    /*********/DEFINE_STATIC_STRING(LeftPeritubularCapillaries);
    /*********/DEFINE_STATIC_STRING(LeftBowmansCapsules);
    /*********/DEFINE_STATIC_STRING(LeftTubules);
    /***/DEFINE_STATIC_STRING(LeftRenalVein);
    /**/DEFINE_STATIC_STRING_EX(RightKidney, RightKidneyVasculature);
    /***/DEFINE_STATIC_STRING(RightRenalArtery);
    /***/DEFINE_STATIC_STRING(RightNephron);
    /*********/DEFINE_STATIC_STRING(RightAfferentArteriole);
    /*********/DEFINE_STATIC_STRING(RightGlomerularCapillaries);
    /*********/DEFINE_STATIC_STRING(RightEfferentArteriole);
    /*********/DEFINE_STATIC_STRING(RightPeritubularCapillaries);
    /*********/DEFINE_STATIC_STRING(RightBowmansCapsules);
    /*********/DEFINE_STATIC_STRING(RightTubules);
    /***/DEFINE_STATIC_STRING(RightRenalVein);
    // General Organs and Periphery
    DEFINE_STATIC_STRING_EX(Bone, BoneVasculature);
    DEFINE_STATIC_STRING_EX(Brain, BrainVasculature);
    DEFINE_STATIC_STRING_EX(Fat, FatVasculature);
    DEFINE_STATIC_STRING_EX(Gut, GutVasculature);
    /**/DEFINE_STATIC_STRING_EX(Splanchnic, SplanchnicVasculature);
    /**/DEFINE_STATIC_STRING_EX(SmallIntestine, SmallIntestineVasculature);
    /**/DEFINE_STATIC_STRING_EX(LargeIntestine, LargeIntestineVasculature);
    DEFINE_STATIC_STRING_EX(Liver, LiverVasculature);
    DEFINE_STATIC_STRING_EX(Spleen, SpleenVasculature);
    DEFINE_STATIC_STRING_EX(Skin, SkinVasculature);
    DEFINE_STATIC_STRING_EX(Muscle, MuscleVasculature);
    DEFINE_STATIC_STRING_EX(LeftArm, LeftArmVasculature);
    DEFINE_STATIC_STRING_EX(LeftLeg, LeftLegVasculature);
    DEFINE_STATIC_STRING_EX(RightArm, RightArmVasculature);
    DEFINE_STATIC_STRING_EX(RightLeg, RightLegVasculature);
    DEFINE_STATIC_STRING(Abdomen);
    /**/DEFINE_STATIC_STRING(AbdominalCavity);

    DEFINE_STATIC_STRING(Ground);


    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Aorta);
        _values.push_back(Abdomen);
        _values.push_back(Heart);
        _values.push_back(Myocardium);
        _values.push_back(LeftHeart);
        _values.push_back(RightHeart);
        _values.push_back(Pericardium);
        _values.push_back(VenaCava);
        _values.push_back(PulmonaryArteries);
        _values.push_back(PulmonaryCapillaries);
        _values.push_back(PulmonaryVeins);
        _values.push_back(Lungs);
        _values.push_back(LeftLung);
        _values.push_back(LeftPulmonaryArteries);
        _values.push_back(LeftPulmonaryCapillaries);
        _values.push_back(LeftPulmonaryVeins);
        _values.push_back(RightLung);
        _values.push_back(RightPulmonaryArteries);
        _values.push_back(RightPulmonaryCapillaries);
        _values.push_back(RightPulmonaryVeins);
        _values.push_back(Kidneys);
        _values.push_back(LeftKidney);
        _values.push_back(LeftRenalArtery);
        _values.push_back(LeftNephron);
        _values.push_back(LeftAfferentArteriole);
        _values.push_back(LeftGlomerularCapillaries);
        _values.push_back(LeftEfferentArteriole);
        _values.push_back(LeftPeritubularCapillaries);
        _values.push_back(LeftBowmansCapsules);
        _values.push_back(LeftTubules);
        _values.push_back(LeftRenalVein);
        _values.push_back(RightKidney);
        _values.push_back(RightRenalArtery);
        _values.push_back(RightNephron);
        _values.push_back(RightAfferentArteriole);
        _values.push_back(RightGlomerularCapillaries);
        _values.push_back(RightEfferentArteriole);
        _values.push_back(RightPeritubularCapillaries);
        _values.push_back(RightBowmansCapsules);
        _values.push_back(RightTubules);
        _values.push_back(RightRenalVein);
        _values.push_back(Bone);
        _values.push_back(Brain);
        _values.push_back(Fat);
        _values.push_back(Gut);
        _values.push_back(Splanchnic);
        _values.push_back(SmallIntestine);
        _values.push_back(LargeIntestine);
        _values.push_back(Liver);
        _values.push_back(Spleen);
        _values.push_back(Skin);
        _values.push_back(Muscle);
        _values.push_back(LeftArm);
        _values.push_back(LeftLeg);
        _values.push_back(RightArm);
        _values.push_back(RightLeg);
        //_values.push_back(Ground);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class VascularLink
  {
  public:
    // Heart and Lungs
    DEFINE_STATIC_STRING(VenaCavaToRightHeart);
    DEFINE_STATIC_STRING(RightHeartToLeftPulmonaryArteries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteriesToCapillaries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteriesToVeins);
    DEFINE_STATIC_STRING(LeftPulmonaryCapillariesToVeins);
    DEFINE_STATIC_STRING(LeftPulmonaryVeinsLeak);
    DEFINE_STATIC_STRING(LeftPulmonaryVeinsToLeftHeart);
    DEFINE_STATIC_STRING(RightHeartToRightPulmonaryArteries);
    DEFINE_STATIC_STRING(RightPulmonaryArteriesToCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteriesToVeins);
    DEFINE_STATIC_STRING(RightPulmonaryCapillariesToVeins);
    DEFINE_STATIC_STRING(RightPulmonaryVeinsLeak);
    DEFINE_STATIC_STRING(RightPulmonaryVeinsToLeftHeart);
    DEFINE_STATIC_STRING(LeftHeartToAorta);
    // Bone 
    DEFINE_STATIC_STRING(AortaToBone);
    DEFINE_STATIC_STRING(BoneToVenaCava);
    // Brain 
    DEFINE_STATIC_STRING(AortaToBrain);
    DEFINE_STATIC_STRING(BrainToVenaCava);
    // Fat 
    DEFINE_STATIC_STRING(AortaToFat);
    DEFINE_STATIC_STRING(FatToVenaCava);
    // Large Intestine 
    DEFINE_STATIC_STRING(AortaToLargeIntestine);
    DEFINE_STATIC_STRING(LargeIntestineToLiver);
    // Left Arm 
    DEFINE_STATIC_STRING(AortaToLeftArm);
    DEFINE_STATIC_STRING(LeftArmToVenaCava);
    // Left Kidney 
    DEFINE_STATIC_STRING(AortaToLeftKidney);
    DEFINE_STATIC_STRING(LeftKidneyToVenaCava);
    // Left Leg 
    DEFINE_STATIC_STRING(AortaToLeftLeg);
    DEFINE_STATIC_STRING(LeftLegToVenaCava);
    // Liver 
    DEFINE_STATIC_STRING(AortaToLiver);
    DEFINE_STATIC_STRING(LiverToVenaCava);
    // Muscle 
    DEFINE_STATIC_STRING(AortaToMuscle);
    DEFINE_STATIC_STRING(MuscleToVenaCava);
    // Myocardium 
    DEFINE_STATIC_STRING(AortaToMyocardium);
    DEFINE_STATIC_STRING(MyocardiumToVenaCava);
    // Right Arm 
    DEFINE_STATIC_STRING(AortaToRightArm);
    DEFINE_STATIC_STRING(RightArmToVenaCava);
    // Right Kidney 
    DEFINE_STATIC_STRING(AortaToRightKidney);
    DEFINE_STATIC_STRING(RightKidneyToVenaCava);
    // Right Leg 
    DEFINE_STATIC_STRING(AortaToRightLeg);
    DEFINE_STATIC_STRING(RightLegToVenaCava);
    // Skin 
    DEFINE_STATIC_STRING(AortaToSkin);
    DEFINE_STATIC_STRING(SkinToVenaCava);
    // Small Intestine 
    DEFINE_STATIC_STRING(AortaToSmallIntestine);
    DEFINE_STATIC_STRING(SmallIntestineToLiver);
    // Splanchnic 
    DEFINE_STATIC_STRING(AortaToSplanchnic);
    DEFINE_STATIC_STRING(SplanchnicToLiver);
    // Spleen 
    DEFINE_STATIC_STRING(AortaToSpleen);
    DEFINE_STATIC_STRING(SpleenToLiver);

    // Hemorrhage and IV
    DEFINE_STATIC_STRING(VenaCavaIV);

    // Vascular To Tissue Links
    DEFINE_STATIC_STRING(BoneVascularToTissue);
    DEFINE_STATIC_STRING(BrainVascularToTissue);
    DEFINE_STATIC_STRING(FatVascularToTissue);
    DEFINE_STATIC_STRING(SmallIntestineVascularToTissue);
    DEFINE_STATIC_STRING(LargeIntestineVascularToTissue);
    DEFINE_STATIC_STRING(SplanchnicVascularToTissue);
    DEFINE_STATIC_STRING(LeftKidneyVascularToTissue);
    DEFINE_STATIC_STRING(LeftLungVascularToTissue);
    DEFINE_STATIC_STRING(LiverVascularToTissue);
    DEFINE_STATIC_STRING(MuscleVascularToTissue);
    DEFINE_STATIC_STRING(MyocardiumVascularToTissue);
    DEFINE_STATIC_STRING(RightKidneyVascularToTissue);
    DEFINE_STATIC_STRING(RightLungVascularToTissue);
    DEFINE_STATIC_STRING(SkinVascularToTissue);
    DEFINE_STATIC_STRING(SpleenVascularToTissue);

    // Renal Links
    // Reusing 'AortaToLeftKidney' to connect the Aorta To Left Renal Artery
    DEFINE_STATIC_STRING(LeftRenalArteryToAfferentArteriole);
    DEFINE_STATIC_STRING(LeftAfferentArterioleToGlomerularCapillaries);
    DEFINE_STATIC_STRING(LeftGlomerularCapillariesToEfferentArteriole);
    DEFINE_STATIC_STRING(LeftGlomerularCapillariesToBowmansCapsules);
    DEFINE_STATIC_STRING(LeftBowmansCapsulesToTubules);
    DEFINE_STATIC_STRING(LeftTubulesToPeritubularCapillaries);
    DEFINE_STATIC_STRING(LeftEfferentArterioleToPeritubularCapillaries);
    DEFINE_STATIC_STRING(LeftPeritubularCapillariesToRenalVein);
    // Reusing 'LeftKidneyToVenaCava' to connect the Left Renal Vein To Vena Cava

    // Reusing 'AortaToRightKidney' to connect the Aorta To Right Renal Artery
    DEFINE_STATIC_STRING(RightRenalArteryToAfferentArteriole);
    DEFINE_STATIC_STRING(RightAfferentArterioleToGlomerularCapillaries);
    DEFINE_STATIC_STRING(RightGlomerularCapillariesToEfferentArteriole);
    DEFINE_STATIC_STRING(RightGlomerularCapillariesToBowmansCapsules);
    DEFINE_STATIC_STRING(RightBowmansCapsulesToTubules);
    DEFINE_STATIC_STRING(RightTubulesToPeritubularCapillaries);
    DEFINE_STATIC_STRING(RightEfferentArterioleToPeritubularCapillaries);
    DEFINE_STATIC_STRING(RightPeritubularCapillariesToRenalVein);
    // Reusing 'RightKidneyToVenaCava' to connect the Right Renal Vein To Vena Cava);


    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(VenaCavaToRightHeart);
        _values.push_back(RightHeartToLeftPulmonaryArteries);
        _values.push_back(LeftPulmonaryArteriesToCapillaries);
        _values.push_back(LeftPulmonaryArteriesToVeins);
        _values.push_back(LeftPulmonaryCapillariesToVeins);
        _values.push_back(LeftPulmonaryVeinsLeak);
        _values.push_back(LeftPulmonaryVeinsToLeftHeart);
        _values.push_back(RightHeartToRightPulmonaryArteries);
        _values.push_back(RightPulmonaryArteriesToCapillaries);
        _values.push_back(RightPulmonaryArteriesToVeins);
        _values.push_back(RightPulmonaryCapillariesToVeins);
        _values.push_back(RightPulmonaryVeinsLeak);
        _values.push_back(RightPulmonaryVeinsToLeftHeart);
        _values.push_back(LeftHeartToAorta);
        _values.push_back(AortaToBone);
        _values.push_back(BoneToVenaCava);
        _values.push_back(AortaToBrain);
        _values.push_back(BrainToVenaCava);
        _values.push_back(AortaToFat);
        _values.push_back(FatToVenaCava);
        _values.push_back(AortaToLargeIntestine);
        _values.push_back(LargeIntestineToLiver);
        _values.push_back(AortaToLeftArm);
        _values.push_back(LeftArmToVenaCava);
        _values.push_back(AortaToLeftKidney);
        _values.push_back(LeftKidneyToVenaCava);
        _values.push_back(AortaToLeftLeg);
        _values.push_back(LeftLegToVenaCava);
        _values.push_back(AortaToLiver);
        _values.push_back(LiverToVenaCava);
        _values.push_back(AortaToMuscle);
        _values.push_back(MuscleToVenaCava);
        _values.push_back(AortaToMyocardium);
        _values.push_back(MyocardiumToVenaCava);
        _values.push_back(AortaToRightArm);
        _values.push_back(RightArmToVenaCava);
        _values.push_back(AortaToRightKidney);
        _values.push_back(RightKidneyToVenaCava);
        _values.push_back(AortaToRightLeg);
        _values.push_back(RightLegToVenaCava);
        _values.push_back(AortaToSkin);
        _values.push_back(SkinToVenaCava);
        _values.push_back(AortaToSmallIntestine);
        _values.push_back(SmallIntestineToLiver);
        _values.push_back(AortaToSplanchnic);
        _values.push_back(SplanchnicToLiver);
        _values.push_back(AortaToSpleen);
        _values.push_back(SpleenToLiver);

        _values.push_back(BoneVascularToTissue);
        _values.push_back(BrainVascularToTissue);
        _values.push_back(FatVascularToTissue);
        _values.push_back(SmallIntestineVascularToTissue);
        _values.push_back(LargeIntestineVascularToTissue);
        _values.push_back(SplanchnicVascularToTissue);
        _values.push_back(LeftKidneyVascularToTissue);
        _values.push_back(LeftLungVascularToTissue);
        _values.push_back(LiverVascularToTissue);
        _values.push_back(MuscleVascularToTissue);
        _values.push_back(MyocardiumVascularToTissue);
        _values.push_back(RightKidneyVascularToTissue);
        _values.push_back(RightLungVascularToTissue);
        _values.push_back(SkinVascularToTissue);
        _values.push_back(SpleenVascularToTissue);

        _values.push_back(LeftRenalArteryToAfferentArteriole);
        _values.push_back(LeftAfferentArterioleToGlomerularCapillaries);
        _values.push_back(LeftGlomerularCapillariesToEfferentArteriole);
        _values.push_back(LeftGlomerularCapillariesToBowmansCapsules);
        _values.push_back(LeftBowmansCapsulesToTubules);
        _values.push_back(LeftTubulesToPeritubularCapillaries);
        _values.push_back(LeftEfferentArterioleToPeritubularCapillaries);
        _values.push_back(LeftPeritubularCapillariesToRenalVein);
        _values.push_back(RightRenalArteryToAfferentArteriole);
        _values.push_back(RightAfferentArterioleToGlomerularCapillaries);
        _values.push_back(RightGlomerularCapillariesToEfferentArteriole);
        _values.push_back(RightGlomerularCapillariesToBowmansCapsules);
        _values.push_back(RightBowmansCapsulesToTubules);
        _values.push_back(RightTubulesToPeritubularCapillaries);
        _values.push_back(RightEfferentArterioleToPeritubularCapillaries);
        _values.push_back(RightPeritubularCapillariesToRenalVein);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class ExpandedVascularCompartment
  {
  public:
    // Cardio
    DEFINE_STATIC_STRING(Aorta);
    DEFINE_STATIC_STRING(ArterialBuffer);
    DEFINE_STATIC_STRING(Heart);
    /**/DEFINE_STATIC_STRING_EX(Myocardium, MyocardiumVasculature);
    /**/DEFINE_STATIC_STRING(LeftHeart);
    /**/DEFINE_STATIC_STRING(RightHeart);
    /**/DEFINE_STATIC_STRING(Pericardium);
    DEFINE_STATIC_STRING(VenaCava);
    DEFINE_STATIC_STRING(VenousBuffer);
    // Pulmonary
    DEFINE_STATIC_STRING(PulmonaryArteries);
    DEFINE_STATIC_STRING(PulmonaryCapillaries);
    DEFINE_STATIC_STRING(PulmonaryVeins);
    DEFINE_STATIC_STRING_EX(Lungs, LungsVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftLung, LeftLungVasculature);
    /***/DEFINE_STATIC_STRING(LeftPulmonaryArteries);
    /***/DEFINE_STATIC_STRING(LeftPulmonaryCapillaries);
    /***/DEFINE_STATIC_STRING(LeftPulmonaryVeins);
    /**/DEFINE_STATIC_STRING_EX(RightLung, RightLungVasculature);
    /***/DEFINE_STATIC_STRING(RightPulmonaryArteries);
    /***/DEFINE_STATIC_STRING(RightPulmonaryCapillaries);
    /***/DEFINE_STATIC_STRING(RightPulmonaryVeins);
    // Renal
    DEFINE_STATIC_STRING_EX(Kidneys, KidneyVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftKidney, LeftKidneyVasculature);
    /***/DEFINE_STATIC_STRING(LeftRenalArtery);
    /***/DEFINE_STATIC_STRING(LeftNephron);
    /*********/DEFINE_STATIC_STRING(LeftAfferentArteriole);
    /*********/DEFINE_STATIC_STRING(LeftGlomerularCapillaries);
    /*********/DEFINE_STATIC_STRING(LeftEfferentArteriole);
    /*********/DEFINE_STATIC_STRING(LeftPeritubularCapillaries);
    /*********/DEFINE_STATIC_STRING(LeftBowmansCapsules);
    /*********/DEFINE_STATIC_STRING(LeftTubules);
    /***/DEFINE_STATIC_STRING(LeftRenalVein);
    /**/DEFINE_STATIC_STRING_EX(RightKidney, RightKidneyVasculature);
    /***/DEFINE_STATIC_STRING(RightRenalArtery);
    /***/DEFINE_STATIC_STRING(RightNephron);
    /*********/DEFINE_STATIC_STRING(RightAfferentArteriole);
    /*********/DEFINE_STATIC_STRING(RightGlomerularCapillaries);
    /*********/DEFINE_STATIC_STRING(RightEfferentArteriole);
    /*********/DEFINE_STATIC_STRING(RightPeritubularCapillaries);
    /*********/DEFINE_STATIC_STRING(RightBowmansCapsules);
    /*********/DEFINE_STATIC_STRING(RightTubules);
    /***/DEFINE_STATIC_STRING(RightRenalVein);
    // General Organs and Periphery
    DEFINE_STATIC_STRING_EX(Bone, BoneVasculature);
    DEFINE_STATIC_STRING_EX(Head, HeadVasculature);
    /**/DEFINE_STATIC_STRING_EX(Extracranial, ExtracranialVasculature);
    /**/DEFINE_STATIC_STRING_EX(Intracranial, IntracranialVasculature);
    DEFINE_STATIC_STRING_EX(Fat, FatVasculature);
    DEFINE_STATIC_STRING_EX(Gut, GutVasculature);
    /**/DEFINE_STATIC_STRING_EX(Gut1, Gut1Vasculature);
    /**/DEFINE_STATIC_STRING_EX(Splanchnic, SplanchnicVasculature);
    /**/DEFINE_STATIC_STRING_EX(SmallIntestine, SmallIntestineVasculature);
    /**/DEFINE_STATIC_STRING_EX(LargeIntestine, LargeIntestineVasculature);
    DEFINE_STATIC_STRING_EX(LeftArm, LeftArmVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftArmArterioles, LeftArmArteriolesVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftArmBone, LeftArmBoneVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftArmFat, LeftArmFatVasculature);
    /**/DEFINE_STATIC_STRING(LeftArmMicrovasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftArmMuscle, LeftArmMuscleVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftArmSkin, LeftArmSkinVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftArmVenules, LeftArmVenulesVasculature);
    DEFINE_STATIC_STRING_EX(LeftLeg, LeftLegVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftLegArterioles, LeftLegArteriolesVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftLegBone, LeftLegBoneVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftLegFat, LeftLegFatVasculature);
    /**/DEFINE_STATIC_STRING(LeftLegMicrovasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftLegMuscle, LeftLegMuscleVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftLegSkin, LeftLegSkinVasculature);
    /**/DEFINE_STATIC_STRING_EX(LeftLegVenules, LeftLegVenulesVasculature);
    DEFINE_STATIC_STRING_EX(Liver, LiverVasculature);
    DEFINE_STATIC_STRING_EX(Muscle, MuscleVasculature);
    DEFINE_STATIC_STRING_EX(RightArm, RightArmVasculature);
    /**/DEFINE_STATIC_STRING_EX(RightArmArterioles, RightArmArteriolesVasculature);
    /**/DEFINE_STATIC_STRING_EX(RightArmBone, RightArmBoneVasculature);
    /**/DEFINE_STATIC_STRING_EX(RightArmFat, RightArmFatVasculature);
    /**/DEFINE_STATIC_STRING(RightArmMicrovasculature);
    /**/DEFINE_STATIC_STRING_EX(RightArmMuscle, RightArmMuscleVasculature);
    /**/DEFINE_STATIC_STRING_EX(RightArmSkin, RightArmSkinVasculature);
    /**/DEFINE_STATIC_STRING_EX(RightArmVenules, RightArmVenulesVasculature);
    DEFINE_STATIC_STRING_EX(RightLeg, RightLegVasculature);
    /**/DEFINE_STATIC_STRING_EX(RightLegArterioles, RightLegArteriolesVasculature);
    /**/DEFINE_STATIC_STRING_EX(RightLegBone, RightLegBoneVasculature);
    /**/DEFINE_STATIC_STRING_EX(RightLegFat, RightLegFatVasculature);
    /**/DEFINE_STATIC_STRING(RightLegMicrovasculature);
    /**/DEFINE_STATIC_STRING_EX(RightLegMuscle, RightLegMuscleVasculature);
    /**/DEFINE_STATIC_STRING_EX(RightLegSkin, RightLegSkinVasculature);
    /**/DEFINE_STATIC_STRING_EX(RightLegVenules, RightLegVenulesVasculature);
    DEFINE_STATIC_STRING_EX(Skin, SkinVasculature);
    DEFINE_STATIC_STRING_EX(Spleen, SpleenVasculature);
    DEFINE_STATIC_STRING_EX(Torso, TorsoVasculature);
    /**/DEFINE_STATIC_STRING_EX(TorsoArterioles, TorsoArteriolesVasculature);
    /**/DEFINE_STATIC_STRING_EX(TorsoBone, TorsoBoneVasculature);
    /**/DEFINE_STATIC_STRING_EX(TorsoFat, TorsoFatVasculature);
    /**/DEFINE_STATIC_STRING(TorsoMicrovasculature);
    /**/DEFINE_STATIC_STRING_EX(TorsoMuscle, TorsoMuscleVasculature);
    /**/DEFINE_STATIC_STRING_EX(TorsoSkin, TorsoSkinVasculature);
    /**/DEFINE_STATIC_STRING_EX(TorsoVenules, TorsoVenulesVasculature);
    DEFINE_STATIC_STRING(Abdomen);
    /**/DEFINE_STATIC_STRING(AbdominalCavity);

    DEFINE_STATIC_STRING(Ground);


    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Aorta);
        _values.push_back(ArterialBuffer);
        _values.push_back(Abdomen);
        _values.push_back(Heart);
        _values.push_back(Myocardium);
        _values.push_back(LeftHeart);
        _values.push_back(RightHeart);
        _values.push_back(Pericardium);
        _values.push_back(VenaCava);
        _values.push_back(VenousBuffer);
        _values.push_back(PulmonaryArteries);
        _values.push_back(PulmonaryCapillaries);
        _values.push_back(PulmonaryVeins);
        _values.push_back(Lungs);
        _values.push_back(LeftLung);
        _values.push_back(LeftPulmonaryArteries);
        _values.push_back(LeftPulmonaryCapillaries);
        _values.push_back(LeftPulmonaryVeins);
        _values.push_back(RightLung);
        _values.push_back(RightPulmonaryArteries);
        _values.push_back(RightPulmonaryCapillaries);
        _values.push_back(RightPulmonaryVeins);
        _values.push_back(Kidneys);
        _values.push_back(LeftKidney);
        _values.push_back(LeftRenalArtery);
        _values.push_back(LeftNephron);
        _values.push_back(LeftAfferentArteriole);
        _values.push_back(LeftGlomerularCapillaries);
        _values.push_back(LeftEfferentArteriole);
        _values.push_back(LeftPeritubularCapillaries);
        _values.push_back(LeftBowmansCapsules);
        _values.push_back(LeftTubules);
        _values.push_back(LeftRenalVein);
        _values.push_back(RightKidney);
        _values.push_back(RightRenalArtery);
        _values.push_back(RightNephron);
        _values.push_back(RightAfferentArteriole);
        _values.push_back(RightGlomerularCapillaries);
        _values.push_back(RightEfferentArteriole);
        _values.push_back(RightPeritubularCapillaries);
        _values.push_back(RightBowmansCapsules);
        _values.push_back(RightTubules);
        _values.push_back(RightRenalVein);
        _values.push_back(Bone);
        _values.push_back(Head);
        _values.push_back(Extracranial);
        _values.push_back(Intracranial);
        _values.push_back(Fat);
        _values.push_back(Gut);
        _values.push_back(Gut1);
        _values.push_back(Splanchnic);
        _values.push_back(SmallIntestine);
        _values.push_back(LargeIntestine);
        _values.push_back(Liver);
        _values.push_back(Spleen);
        _values.push_back(Skin);
        _values.push_back(Muscle);
        _values.push_back(LeftArm);
        _values.push_back(LeftArmArterioles);
        _values.push_back(LeftArmFat);
        _values.push_back(LeftArmMuscle);
        _values.push_back(LeftArmSkin);
        _values.push_back(LeftArmBone);
        _values.push_back(LeftArmMicrovasculature);
        _values.push_back(LeftArmVenules);
        _values.push_back(LeftLeg);
        _values.push_back(LeftLegArterioles);
        _values.push_back(LeftLegFat);
        _values.push_back(LeftLegMuscle);
        _values.push_back(LeftLegSkin);
        _values.push_back(LeftLegBone);
        _values.push_back(LeftLegMicrovasculature);
        _values.push_back(LeftLegVenules);
        _values.push_back(RightArm);
        _values.push_back(RightArmArterioles);
        _values.push_back(RightArmFat);
        _values.push_back(RightArmMuscle);
        _values.push_back(RightArmSkin);
        _values.push_back(RightArmBone);
        _values.push_back(RightArmMicrovasculature);
        _values.push_back(RightArmVenules);
        _values.push_back(RightLeg);
        _values.push_back(RightLegArterioles);
        _values.push_back(RightLegFat);
        _values.push_back(RightLegMuscle);
        _values.push_back(RightLegSkin);
        _values.push_back(RightLegBone);
        _values.push_back(RightLegMicrovasculature);
        _values.push_back(RightLegVenules);
        _values.push_back(Torso);
        _values.push_back(TorsoArterioles);
        _values.push_back(TorsoBone);
        _values.push_back(TorsoFat);
        _values.push_back(TorsoMicrovasculature);
        _values.push_back(TorsoMuscle);
        _values.push_back(TorsoSkin);
        _values.push_back(TorsoVenules);
        //_values.push_back(Ground);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class ExpandedVascularLink
  {
  public:
    // Heart and Lungs

    DEFINE_STATIC_STRING(VenousBufferToVenaCava);
    DEFINE_STATIC_STRING(VenaCavaToRightHeart);
    DEFINE_STATIC_STRING(RightHeartToLeftPulmonaryArteries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteriesToCapillaries);
    DEFINE_STATIC_STRING(LeftPulmonaryArteriesToVeins);
    DEFINE_STATIC_STRING(LeftPulmonaryCapillariesToVeins);
    DEFINE_STATIC_STRING(LeftPulmonaryVeinsToLeftHeart);
    DEFINE_STATIC_STRING(RightHeartToRightPulmonaryArteries);
    DEFINE_STATIC_STRING(RightPulmonaryArteriesToCapillaries);
    DEFINE_STATIC_STRING(RightPulmonaryArteriesToVeins);
    DEFINE_STATIC_STRING(RightPulmonaryCapillariesToVeins);
    DEFINE_STATIC_STRING(RightPulmonaryVeinsToLeftHeart);
    DEFINE_STATIC_STRING(LeftHeartToAorta);
    DEFINE_STATIC_STRING(AortaToArterialBuffer);
    // Head
    DEFINE_STATIC_STRING(AortaToIntracranial);
    DEFINE_STATIC_STRING(IntracranialToVenaCava);
    DEFINE_STATIC_STRING(AortaToExtracranial);
    DEFINE_STATIC_STRING(ExtracranialToVenaCava);
    // Left Arm
    DEFINE_STATIC_STRING(AortaToLeftArmArterioles);
    DEFINE_STATIC_STRING(LeftArmArteriolesToLeftArmBone);
    DEFINE_STATIC_STRING(LeftArmArteriolesToLeftArmFat);
    DEFINE_STATIC_STRING(LeftArmArteriolesToLeftArmMicrovasculature);
    DEFINE_STATIC_STRING(LeftArmArteriolesToLeftArmMuscle);
    DEFINE_STATIC_STRING(LeftArmArteriolesToLeftArmSkin);

    DEFINE_STATIC_STRING(LeftArmBoneToLeftArmVenules);
    DEFINE_STATIC_STRING(LeftArmFatToLeftArmVenules);
    DEFINE_STATIC_STRING(LeftArmMicrovasculatureToLeftArmVenules);
    DEFINE_STATIC_STRING(LeftArmMuscleToLeftArmVenules);
    DEFINE_STATIC_STRING(LeftArmSkinToLeftArmVenules);
    DEFINE_STATIC_STRING(LeftArmVenulesToVenaCava);

    // Left Kidney
    DEFINE_STATIC_STRING(AortaToLeftKidney);
    DEFINE_STATIC_STRING(LeftKidneyToVenaCava);
    // Left Leg
    DEFINE_STATIC_STRING(AortaToLeftLegArterioles);
    DEFINE_STATIC_STRING(LeftLegArteriolesToLeftLegBone);
    DEFINE_STATIC_STRING(LeftLegArteriolesToLeftLegFat);
    DEFINE_STATIC_STRING(LeftLegArteriolesToLeftLegMicrovasculature);
    DEFINE_STATIC_STRING(LeftLegArteriolesToLeftLegMuscle);
    DEFINE_STATIC_STRING(LeftLegArteriolesToLeftLegSkin);

    DEFINE_STATIC_STRING(LeftLegBoneToLeftLegVenules);
    DEFINE_STATIC_STRING(LeftLegFatToLeftLegVenules);
    DEFINE_STATIC_STRING(LeftLegMicrovasculatureToLeftLegVenules);
    DEFINE_STATIC_STRING(LeftLegMuscleToLeftLegVenules);
    DEFINE_STATIC_STRING(LeftLegSkinToLeftLegVenules);
    DEFINE_STATIC_STRING(LeftLegVenulesToVenaCava);
    // Gut
    DEFINE_STATIC_STRING(AortaToGut);
    //// Large Intestine
    DEFINE_STATIC_STRING(AortaToLargeIntestine);
    DEFINE_STATIC_STRING(LargeIntestineToLiver);
    //// Small Intestine
    DEFINE_STATIC_STRING(AortaToSmallIntestine);
    DEFINE_STATIC_STRING(SmallIntestineToLiver);
    //// Splanchnic
    DEFINE_STATIC_STRING(AortaToSplanchnic);
    DEFINE_STATIC_STRING(SplanchnicToLiver);
    //// Spleen
    DEFINE_STATIC_STRING(AortaToSpleen);
    DEFINE_STATIC_STRING(SpleenToLiver);
    //// Liver
    DEFINE_STATIC_STRING(AortaToLiver);
    DEFINE_STATIC_STRING(LiverToVenaCava);
    // Myocardium
    DEFINE_STATIC_STRING(AortaToMyocardium);
    DEFINE_STATIC_STRING(MyocardiumToVenaCava);
    // Right Arm
    DEFINE_STATIC_STRING(AortaToRightArmArterioles);
    DEFINE_STATIC_STRING(RightArmArteriolesToRightArmBone);
    DEFINE_STATIC_STRING(RightArmArteriolesToRightArmFat);
    DEFINE_STATIC_STRING(RightArmArteriolesToRightArmMicrovasculature);
    DEFINE_STATIC_STRING(RightArmArteriolesToRightArmMuscle);
    DEFINE_STATIC_STRING(RightArmArteriolesToRightArmSkin);

    DEFINE_STATIC_STRING(RightArmBoneToRightArmVenules);
    DEFINE_STATIC_STRING(RightArmFatToRightArmVenules);
    DEFINE_STATIC_STRING(RightArmMicrovasculatureToRightArmVenules);
    DEFINE_STATIC_STRING(RightArmMuscleToRightArmVenules);
    DEFINE_STATIC_STRING(RightArmSkinToRightArmVenules);
    DEFINE_STATIC_STRING(RightArmVenulesToVenaCava);

    // Right Kidney
    DEFINE_STATIC_STRING(AortaToRightKidney);
    DEFINE_STATIC_STRING(RightKidneyToVenaCava);
    // Right Leg
    DEFINE_STATIC_STRING(AortaToRightLegArterioles);
    DEFINE_STATIC_STRING(RightLegArteriolesToRightLegBone);
    DEFINE_STATIC_STRING(RightLegArteriolesToRightLegFat);
    DEFINE_STATIC_STRING(RightLegArteriolesToRightLegMicrovasculature);
    DEFINE_STATIC_STRING(RightLegArteriolesToRightLegMuscle);
    DEFINE_STATIC_STRING(RightLegArteriolesToRightLegSkin);

    DEFINE_STATIC_STRING(RightLegBoneToRightLegVenules);
    DEFINE_STATIC_STRING(RightLegFatToRightLegVenules);
    DEFINE_STATIC_STRING(RightLegMicrovasculatureToRightLegVenules);
    DEFINE_STATIC_STRING(RightLegMuscleToRightLegVenules);
    DEFINE_STATIC_STRING(RightLegSkinToRightLegVenules);
    DEFINE_STATIC_STRING(RightLegVenulesToVenaCava);

    // Torso
    DEFINE_STATIC_STRING(AortaToTorsoArterioles);
    DEFINE_STATIC_STRING(TorsoArteriolesToTorsoBone);
    DEFINE_STATIC_STRING(TorsoArteriolesToTorsoFat);
    DEFINE_STATIC_STRING(TorsoArteriolesToTorsoMicrovasculature);
    DEFINE_STATIC_STRING(TorsoArteriolesToTorsoMuscle);
    DEFINE_STATIC_STRING(TorsoArteriolesToTorsoSkin);

    DEFINE_STATIC_STRING(TorsoSkinToTorsoVenules);
    DEFINE_STATIC_STRING(TorsoMuscleToTorsoVenules);
    DEFINE_STATIC_STRING(TorsoMicrovasculatureToTorsoVenules);
    DEFINE_STATIC_STRING(TorsoFatToTorsoVenules);
    DEFINE_STATIC_STRING(TorsoBoneToTorsoVenules);
    DEFINE_STATIC_STRING(TorsoVenulesToVenaCava);
    // Hemorrhage and IV
    DEFINE_STATIC_STRING(VenaCavaIV);

    // Vascular To Tissue Links
    DEFINE_STATIC_STRING(TorsoBoneVascularToTissue);
    DEFINE_STATIC_STRING(LeftArmBoneVascularToTissue);
    DEFINE_STATIC_STRING(LeftLegBoneVascularToTissue);
    DEFINE_STATIC_STRING(RightArmBoneVascularToTissue);
    DEFINE_STATIC_STRING(RightLegBoneVascularToTissue);
    DEFINE_STATIC_STRING(BrainVascularToTissue);
    DEFINE_STATIC_STRING(TorsoFatVascularToTissue);
    DEFINE_STATIC_STRING(LeftArmFatVascularToTissue);
    DEFINE_STATIC_STRING(LeftLegFatVascularToTissue);
    DEFINE_STATIC_STRING(RightArmFatVascularToTissue);
    DEFINE_STATIC_STRING(RightLegFatVascularToTissue);
    DEFINE_STATIC_STRING(SmallIntestineVascularToTissue);
    DEFINE_STATIC_STRING(LargeIntestineVascularToTissue);
    DEFINE_STATIC_STRING(SplanchnicVascularToTissue);
    DEFINE_STATIC_STRING(LeftKidneyVascularToTissue);
    DEFINE_STATIC_STRING(LeftLungVascularToTissue);
    DEFINE_STATIC_STRING(LiverVascularToTissue);
    DEFINE_STATIC_STRING(TorsoMuscleVascularToTissue);
    DEFINE_STATIC_STRING(LeftArmMuscleVascularToTissue);
    DEFINE_STATIC_STRING(LeftLegMuscleVascularToTissue);
    DEFINE_STATIC_STRING(RightArmMuscleVascularToTissue);
    DEFINE_STATIC_STRING(RightLegMuscleVascularToTissue);
    DEFINE_STATIC_STRING(MyocardiumVascularToTissue);
    DEFINE_STATIC_STRING(RightKidneyVascularToTissue);
    DEFINE_STATIC_STRING(RightLungVascularToTissue);
    DEFINE_STATIC_STRING(TorsoSkinVascularToTissue);
    DEFINE_STATIC_STRING(LeftArmSkinVascularToTissue);
    DEFINE_STATIC_STRING(LeftLegSkinVascularToTissue);
    DEFINE_STATIC_STRING(RightArmSkinVascularToTissue);
    DEFINE_STATIC_STRING(RightLegSkinVascularToTissue);
    DEFINE_STATIC_STRING(SpleenVascularToTissue);

    // Renal Links
    // Reusing 'AortaToLeftKidney' to connect the Aorta To Left Renal Artery
    DEFINE_STATIC_STRING(LeftRenalArteryToAfferentArteriole);
    DEFINE_STATIC_STRING(LeftAfferentArterioleToGlomerularCapillaries);
    DEFINE_STATIC_STRING(LeftGlomerularCapillariesToEfferentArteriole);
    DEFINE_STATIC_STRING(LeftGlomerularCapillariesToBowmansCapsules);
    DEFINE_STATIC_STRING(LeftBowmansCapsulesToTubules);
    DEFINE_STATIC_STRING(LeftTubulesToPeritubularCapillaries);
    DEFINE_STATIC_STRING(LeftEfferentArterioleToPeritubularCapillaries);
    DEFINE_STATIC_STRING(LeftPeritubularCapillariesToRenalVein);
    // Reusing 'LeftKidneyToVenaCava' to connect the Left Renal Vein To Vena Cava

    // Reusing 'AortaToRightKidney' to connect the Aorta To Right Renal Artery
    DEFINE_STATIC_STRING(RightRenalArteryToAfferentArteriole);
    DEFINE_STATIC_STRING(RightAfferentArterioleToGlomerularCapillaries);
    DEFINE_STATIC_STRING(RightGlomerularCapillariesToEfferentArteriole);
    DEFINE_STATIC_STRING(RightGlomerularCapillariesToBowmansCapsules);
    DEFINE_STATIC_STRING(RightBowmansCapsulesToTubules);
    DEFINE_STATIC_STRING(RightTubulesToPeritubularCapillaries);
    DEFINE_STATIC_STRING(RightEfferentArterioleToPeritubularCapillaries);
    DEFINE_STATIC_STRING(RightPeritubularCapillariesToRenalVein);
    // Reusing 'RightKidneyToVenaCava' to connect the Right Renal Vein To Vena Cava);   


    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(VenaCavaToRightHeart);
        _values.push_back(RightHeartToLeftPulmonaryArteries);
        _values.push_back(LeftPulmonaryArteriesToCapillaries);
        _values.push_back(LeftPulmonaryArteriesToVeins);
        _values.push_back(LeftPulmonaryCapillariesToVeins);
        _values.push_back(LeftPulmonaryVeinsToLeftHeart);
        _values.push_back(RightHeartToRightPulmonaryArteries);
        _values.push_back(RightPulmonaryArteriesToCapillaries);
        _values.push_back(RightPulmonaryArteriesToVeins);
        _values.push_back(RightPulmonaryCapillariesToVeins);
        _values.push_back(RightPulmonaryVeinsToLeftHeart);
        _values.push_back(LeftHeartToAorta);
        _values.push_back(AortaToIntracranial);
        _values.push_back(IntracranialToVenaCava);
        _values.push_back(AortaToExtracranial);
        _values.push_back(ExtracranialToVenaCava);
        _values.push_back(AortaToLargeIntestine);
        _values.push_back(LargeIntestineToLiver);
        _values.push_back(LeftArmArteriolesToLeftArmBone);
        _values.push_back(LeftArmArteriolesToLeftArmFat);
        _values.push_back(LeftArmArteriolesToLeftArmMicrovasculature);
        _values.push_back(LeftArmArteriolesToLeftArmMuscle);
        _values.push_back(LeftArmArteriolesToLeftArmSkin);

        _values.push_back(AortaToLeftKidney);
        _values.push_back(LeftKidneyToVenaCava);
        _values.push_back(LeftLegArteriolesToLeftLegBone);
        _values.push_back(LeftLegArteriolesToLeftLegFat);
        _values.push_back(LeftLegArteriolesToLeftLegMicrovasculature);
        _values.push_back(LeftLegArteriolesToLeftLegMuscle);
        _values.push_back(LeftLegArteriolesToLeftLegSkin);
        _values.push_back(AortaToLiver);
        _values.push_back(LiverToVenaCava);
        _values.push_back(AortaToMyocardium);
        _values.push_back(MyocardiumToVenaCava);
        _values.push_back(RightArmArteriolesToRightArmBone);
        _values.push_back(RightArmArteriolesToRightArmFat);
        _values.push_back(RightArmArteriolesToRightArmMicrovasculature);
        _values.push_back(RightArmArteriolesToRightArmMuscle);
        _values.push_back(RightArmArteriolesToRightArmSkin);

        _values.push_back(AortaToRightKidney);
        _values.push_back(RightKidneyToVenaCava);
        _values.push_back(RightLegArteriolesToRightLegBone);
        _values.push_back(RightLegArteriolesToRightLegFat);
        _values.push_back(RightLegArteriolesToRightLegMicrovasculature);
        _values.push_back(RightLegArteriolesToRightLegMuscle);
        _values.push_back(RightLegArteriolesToRightLegSkin);



        _values.push_back(AortaToSmallIntestine);
        _values.push_back(SmallIntestineToLiver);
        _values.push_back(AortaToSplanchnic);
        _values.push_back(SplanchnicToLiver);
        _values.push_back(AortaToSpleen);
        _values.push_back(SpleenToLiver);
        _values.push_back(AortaToTorsoArterioles);
        _values.push_back(TorsoArteriolesToTorsoBone);
        _values.push_back(TorsoArteriolesToTorsoFat);
        _values.push_back(TorsoArteriolesToTorsoMicrovasculature);
        _values.push_back(TorsoArteriolesToTorsoMuscle);
        _values.push_back(TorsoArteriolesToTorsoSkin);

        _values.push_back(RightArmBoneToRightArmVenules);
        _values.push_back(RightArmFatToRightArmVenules);
        _values.push_back(RightArmMicrovasculatureToRightArmVenules);
        _values.push_back(RightArmMuscleToRightArmVenules);
        _values.push_back(RightArmSkinToRightArmVenules);
        _values.push_back(RightLegBoneToRightLegVenules);
        _values.push_back(RightLegFatToRightLegVenules);
        _values.push_back(RightLegMicrovasculatureToRightLegVenules);
        _values.push_back(RightLegMuscleToRightLegVenules);
        _values.push_back(RightLegSkinToRightLegVenules);
        _values.push_back(LeftArmBoneToLeftArmVenules);
        _values.push_back(LeftArmFatToLeftArmVenules);
        _values.push_back(LeftArmMicrovasculatureToLeftArmVenules);
        _values.push_back(LeftArmMuscleToLeftArmVenules);
        _values.push_back(LeftArmSkinToLeftArmVenules);
        _values.push_back(LeftLegBoneToLeftLegVenules);
        _values.push_back(LeftLegFatToLeftLegVenules);
        _values.push_back(LeftLegMicrovasculatureToLeftLegVenules);
        _values.push_back(LeftLegMuscleToLeftLegVenules);
        _values.push_back(LeftLegSkinToLeftLegVenules);
        _values.push_back(TorsoSkinToTorsoVenules);
        _values.push_back(TorsoMuscleToTorsoVenules);
        _values.push_back(TorsoMicrovasculatureToTorsoVenules);
        _values.push_back(TorsoFatToTorsoVenules);
        _values.push_back(TorsoBoneToTorsoVenules);
        _values.push_back(TorsoVenulesToVenaCava);


        _values.push_back(TorsoBoneVascularToTissue);
        _values.push_back(LeftArmBoneVascularToTissue);
        _values.push_back(LeftLegBoneVascularToTissue);
        _values.push_back(RightArmBoneVascularToTissue);
        _values.push_back(RightLegBoneVascularToTissue);
        _values.push_back(BrainVascularToTissue);
        _values.push_back(TorsoFatVascularToTissue);
        _values.push_back(LeftArmFatVascularToTissue);
        _values.push_back(LeftLegFatVascularToTissue);
        _values.push_back(RightArmFatVascularToTissue);
        _values.push_back(RightLegFatVascularToTissue);
        _values.push_back(SmallIntestineVascularToTissue);
        _values.push_back(LargeIntestineVascularToTissue);
        _values.push_back(SplanchnicVascularToTissue);
        _values.push_back(LeftKidneyVascularToTissue);
        _values.push_back(LeftLungVascularToTissue);
        _values.push_back(LiverVascularToTissue);
        _values.push_back(TorsoMuscleVascularToTissue);
        _values.push_back(LeftArmMuscleVascularToTissue);
        _values.push_back(LeftLegMuscleVascularToTissue);
        _values.push_back(RightArmMuscleVascularToTissue);
        _values.push_back(RightLegMuscleVascularToTissue);
        _values.push_back(MyocardiumVascularToTissue);
        _values.push_back(RightKidneyVascularToTissue);
        _values.push_back(RightLungVascularToTissue);
        _values.push_back(TorsoSkinVascularToTissue);
        _values.push_back(LeftArmSkinVascularToTissue);
        _values.push_back(LeftLegSkinVascularToTissue);
        _values.push_back(RightArmSkinVascularToTissue);
        _values.push_back(RightLegSkinVascularToTissue);
        _values.push_back(SpleenVascularToTissue);

        _values.push_back(LeftRenalArteryToAfferentArteriole);
        _values.push_back(LeftAfferentArterioleToGlomerularCapillaries);
        _values.push_back(LeftGlomerularCapillariesToEfferentArteriole);
        _values.push_back(LeftGlomerularCapillariesToBowmansCapsules);
        _values.push_back(LeftBowmansCapsulesToTubules);
        _values.push_back(LeftTubulesToPeritubularCapillaries);
        _values.push_back(LeftEfferentArterioleToPeritubularCapillaries);
        _values.push_back(LeftPeritubularCapillariesToRenalVein);
        _values.push_back(RightRenalArteryToAfferentArteriole);
        _values.push_back(RightAfferentArterioleToGlomerularCapillaries);
        _values.push_back(RightGlomerularCapillariesToEfferentArteriole);
        _values.push_back(RightGlomerularCapillariesToBowmansCapsules);
        _values.push_back(RightBowmansCapsulesToTubules);
        _values.push_back(RightTubulesToPeritubularCapillaries);
        _values.push_back(RightEfferentArterioleToPeritubularCapillaries);
        _values.push_back(RightPeritubularCapillariesToRenalVein);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };


  class UrineCompartment
  {
  public:
    DEFINE_STATIC_STRING(Ureters);
    /*****/DEFINE_STATIC_STRING(LeftUreter);
    /*****/DEFINE_STATIC_STRING(RightUreter);
    DEFINE_STATIC_STRING(Bladder);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Ureters);
        _values.push_back(LeftUreter);
        _values.push_back(RightUreter);
        _values.push_back(Bladder);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class UrineLink
  {
  public:
    DEFINE_STATIC_STRING(LeftTubulesToUreter);
    DEFINE_STATIC_STRING(LeftUreterToBladder);

    DEFINE_STATIC_STRING(RightTubulesToUreter);
    DEFINE_STATIC_STRING(RightUreterToBladder);

    DEFINE_STATIC_STRING(BladderToGround);
    DEFINE_STATIC_STRING(BladderToGroundSource);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(LeftTubulesToUreter);
        _values.push_back(LeftUreterToBladder);
        _values.push_back(RightTubulesToUreter);
        _values.push_back(RightUreterToBladder);
        _values.push_back(BladderToGround);
        _values.push_back(BladderToGroundSource);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class LymphCompartment
  {
  public:
    DEFINE_STATIC_STRING(Lymph);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Lymph);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class LymphLink
  {
  public:
    DEFINE_STATIC_STRING(BoneTissueToLymph);
    DEFINE_STATIC_STRING(BrainTissueToLymph);
    DEFINE_STATIC_STRING(FatTissueToLymph);
    DEFINE_STATIC_STRING(GutTissueToLymph);
    DEFINE_STATIC_STRING(LeftKidneyTissueToLymph);
    DEFINE_STATIC_STRING(LeftLungTissueToLymph);
    DEFINE_STATIC_STRING(LiverTissueToLymph);
    DEFINE_STATIC_STRING(MuscleTissueToLymph);
    DEFINE_STATIC_STRING(MyocardiumTissueToLymph);
    DEFINE_STATIC_STRING(RightKidneyTissueToLymph);
    DEFINE_STATIC_STRING(RightLungTissueToLymph);
    DEFINE_STATIC_STRING(SkinTissueToLymph);
    DEFINE_STATIC_STRING(SpleenTissueToLymph);

    DEFINE_STATIC_STRING(LymphToVenaCava);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(BoneTissueToLymph);
        _values.push_back(BrainTissueToLymph);
        _values.push_back(FatTissueToLymph);
        _values.push_back(GutTissueToLymph);
        _values.push_back(LeftKidneyTissueToLymph);
        _values.push_back(LeftLungTissueToLymph);
        _values.push_back(LiverTissueToLymph);
        _values.push_back(MuscleTissueToLymph);
        _values.push_back(MyocardiumTissueToLymph);
        _values.push_back(RightKidneyTissueToLymph);
        _values.push_back(RightLungTissueToLymph);
        _values.push_back(SkinTissueToLymph);
        _values.push_back(SpleenTissueToLymph);
        _values.push_back(LymphToVenaCava);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class TemperatureCompartment
  {
  public:
    DEFINE_STATIC_STRING(Active);
    DEFINE_STATIC_STRING(Ambient);
    DEFINE_STATIC_STRING(Clothing);
    DEFINE_STATIC_STRING(Enclosure);
    DEFINE_STATIC_STRING(ExternalCore);
    DEFINE_STATIC_STRING(ExternalSkin);
    DEFINE_STATIC_STRING(ExternalGround);
    DEFINE_STATIC_STRING(InternalCore);
    DEFINE_STATIC_STRING(InternalSkin);
    DEFINE_STATIC_STRING(InternalGround);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Active);
        _values.push_back(Ambient);
        _values.push_back(Clothing);
        _values.push_back(Enclosure);
        _values.push_back(ExternalCore);
        _values.push_back(ExternalSkin);
        _values.push_back(ExternalGround);
        _values.push_back(InternalCore);
        _values.push_back(InternalSkin);
        _values.push_back(InternalGround);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class TemperatureLink
  {
  public:

    DEFINE_STATIC_STRING(ActiveToClothing);
    DEFINE_STATIC_STRING(ClothingToEnclosure);
    DEFINE_STATIC_STRING(ClothingToEnvironment);
    DEFINE_STATIC_STRING(ExternalCoreToGround);
    DEFINE_STATIC_STRING(GroundToActive);
    DEFINE_STATIC_STRING(GroundToClothing);
    DEFINE_STATIC_STRING(GroundToEnclosure);
    DEFINE_STATIC_STRING(GroundToEnvironment);
    DEFINE_STATIC_STRING(ExternalSkinToGround);
    DEFINE_STATIC_STRING(ExternalSkinToClothing);
    DEFINE_STATIC_STRING(GroundToInternalCore);
    DEFINE_STATIC_STRING(InternalCoreToInternalSkin);
    DEFINE_STATIC_STRING(InternalCoreToGround);
    DEFINE_STATIC_STRING(InternalSkinToGround);
    DEFINE_STATIC_STRING(InternalCoreToExternalCore);
    DEFINE_STATIC_STRING(InternalSkinToExternalSkin);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(ActiveToClothing);
        _values.push_back(ClothingToEnclosure);
        _values.push_back(ClothingToEnvironment);
        _values.push_back(ExternalCoreToGround);
        _values.push_back(GroundToActive);
        _values.push_back(GroundToClothing);
        _values.push_back(GroundToEnclosure);
        _values.push_back(GroundToEnvironment);
        _values.push_back(ExternalSkinToGround);
        _values.push_back(ExternalSkinToClothing);
        _values.push_back(GroundToInternalCore);
        _values.push_back(InternalCoreToInternalSkin);
        _values.push_back(InternalCoreToGround);
        _values.push_back(InternalSkinToGround);
        _values.push_back(InternalCoreToExternalCore);
        _values.push_back(InternalSkinToExternalSkin);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class EnvironmentCompartment
  {
  public:
    DEFINE_STATIC_STRING(Ambient);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Ambient);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class AnesthesiaMachineCompartment
  {
  public:
    DEFINE_STATIC_STRING_EX(Connection, AnesthesiaMachineConnection);
    DEFINE_STATIC_STRING_EX(ExpiratoryLimb, AnesthesiaMachineExpiratoryLimb);
    DEFINE_STATIC_STRING_EX(GasInlet, AnesthesiaMachineGasInlet);
    DEFINE_STATIC_STRING_EX(GasSource, AnesthesiaMachineGasSource);
    DEFINE_STATIC_STRING_EX(InspiratoryLimb, AnesthesiaMachineInspiratoryLimb);
    DEFINE_STATIC_STRING_EX(ReliefValve, AnesthesiaMachineReliefValve);
    DEFINE_STATIC_STRING_EX(Scrubber, AnesthesiaMachineScrubber);
    DEFINE_STATIC_STRING_EX(Selector, AnesthesiaMachineSelector);
    DEFINE_STATIC_STRING_EX(Ventilator, AnesthesiaMachineVentilator);
    DEFINE_STATIC_STRING_EX(YPiece, AnesthesiaMachineYPiece);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Connection);
        _values.push_back(ExpiratoryLimb);
        _values.push_back(GasInlet);
        _values.push_back(GasSource);
        _values.push_back(InspiratoryLimb);
        _values.push_back(ReliefValve);
        _values.push_back(Scrubber);
        _values.push_back(Selector);
        _values.push_back(Ventilator);
        _values.push_back(YPiece);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class AnesthesiaMachineLink
  {
  public:
    DEFINE_STATIC_STRING_EX(EnvironmentToReliefValve, EnvironmentToAnesthesiaMachineReliefValve);
    DEFINE_STATIC_STRING_EX(VentilatorToSelector, AnesthesiaMachineVentilatorToSelector);
    DEFINE_STATIC_STRING_EX(SelectorToReliefValve, AnesthesiaMachineSelectorToReliefValve);
    DEFINE_STATIC_STRING_EX(SelectorToScrubber, AnesthesiaMachineSelectorToScrubber);
    DEFINE_STATIC_STRING_EX(ScrubberToGasInlet, AnesthesiaMachineScrubberToGasInlet);
    DEFINE_STATIC_STRING_EX(GasSourceToGasInlet, AnesthesiaMachineGasSourceToGasInlet);
    DEFINE_STATIC_STRING_EX(GasInletToInspiratoryLimb, AnesthesiaMachineGasInletToInspiratoryLimb);
    DEFINE_STATIC_STRING_EX(InspiratoryLimbToYPiece, AnesthesiaMachineInspiratoryLimbToYPiece);
    DEFINE_STATIC_STRING_EX(YPieceToExpiratoryLimb, AnesthesiaMachineYPieceToExpiratoryLimb);
    DEFINE_STATIC_STRING_EX(ExpiratoryLimbToSelector, AnesthesiaMachineExpiratoryLimbToSelector);
    DEFINE_STATIC_STRING_EX(YPieceToConnection, AnesthesiaMachineYPieceToConnection);
    DEFINE_STATIC_STRING_EX(ConnectionLeak, AnesthesiaMachineConnectionLeak);
    DEFINE_STATIC_STRING_EX(ConnectionToAirway, AnesthesiaMachineConnectionToAirway);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(EnvironmentToReliefValve);
        _values.push_back(VentilatorToSelector);
        _values.push_back(SelectorToReliefValve);
        _values.push_back(SelectorToScrubber);
        _values.push_back(ScrubberToGasInlet);
        _values.push_back(GasSourceToGasInlet);
        _values.push_back(GasInletToInspiratoryLimb);
        _values.push_back(InspiratoryLimbToYPiece);
        _values.push_back(YPieceToExpiratoryLimb);
        _values.push_back(ExpiratoryLimbToSelector);
        _values.push_back(YPieceToConnection);
        _values.push_back(ConnectionLeak);
        _values.push_back(ConnectionToAirway);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class BagValveMaskCompartment
  {
  public:
    DEFINE_STATIC_STRING_EX(Reservoir, BagValveMaskReservoir);
    DEFINE_STATIC_STRING_EX(Bag, BagValveMaskBag);
    DEFINE_STATIC_STRING_EX(Valve, BagValveMaskValve);
    DEFINE_STATIC_STRING_EX(Filter, BagValveMaskFilter);
    DEFINE_STATIC_STRING_EX(Connection, BagValveMaskConnection);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Reservoir);
        _values.push_back(Bag);
        _values.push_back(Valve);
        _values.push_back(Filter);
        _values.push_back(Connection);
      }
      return _values;
    }
  protected:
    static std::vector<std::string> _values;
  };

  class BagValveMaskLink
  {
  public:
    DEFINE_STATIC_STRING_EX(ReservoirToBag, BagValveMaskReservoirToBag);
    DEFINE_STATIC_STRING_EX(BagToValve, BagValveMaskBagToValve);
    DEFINE_STATIC_STRING_EX(ValveToFilter, BagValveMaskValveToFilter);
    DEFINE_STATIC_STRING_EX(FilterToConnection, BagValveMaskFilterToConnection);
    DEFINE_STATIC_STRING_EX(ConnectionToEnvironment, BagValveMaskConnectionToEnvironment);
    DEFINE_STATIC_STRING_EX(ConnectionToAirway, BagValveMaskConnectionToAirway);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(ReservoirToBag);
        _values.push_back(BagToValve);
        _values.push_back(ValveToFilter);
        _values.push_back(FilterToConnection);
        _values.push_back(ConnectionToEnvironment);
        _values.push_back(ConnectionToAirway);
      }
      return _values;
    }
  protected:
    static std::vector<std::string> _values;
  };

  class ECMOCompartment
  {
  public:
    DEFINE_STATIC_STRING_EX(Oxygenator, ECMOOxygenator);
    DEFINE_STATIC_STRING_EX(BloodSamplingPort, ECMOBloodSamplingPort);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Oxygenator);
        _values.push_back(BloodSamplingPort);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class ECMOLink
  {
  public:
    DEFINE_STATIC_STRING(OxygenatorToVasculature);
    DEFINE_STATIC_STRING(VasculatureToBloodSamplingPort);
    DEFINE_STATIC_STRING(BloodSamplingPortToOxygenator);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(OxygenatorToVasculature);
        _values.push_back(VasculatureToBloodSamplingPort);
        _values.push_back(BloodSamplingPortToOxygenator);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class InhalerCompartment
  {
  public:
    DEFINE_STATIC_STRING_EX(Mouthpiece, InhalerMouthpiece);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Mouthpiece);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class InhalerLink
  {
  public:
    DEFINE_STATIC_STRING_EX(EnvironmentToMouthpiece, EnvironmentToInhalerMouthpiece);
    DEFINE_STATIC_STRING_EX(MouthpieceToAirway, InhalerMouthpieceToAirway);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(EnvironmentToMouthpiece);
        _values.push_back(MouthpieceToAirway);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class MechanicalVentilationCompartment
  {
  public:
    DEFINE_STATIC_STRING_EX(Connection, MechanicalVentilationConnection);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(Connection);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class MechanicalVentilationLink
  {
  public:
    DEFINE_STATIC_STRING_EX(ConnectionToAirway, MechanicalVentilationConnectionToAirway);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(ConnectionToAirway);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class MechanicalVentilatorCompartment
  {
  public:
    DEFINE_STATIC_STRING(MechanicalVentilator);
    DEFINE_STATIC_STRING_EX(ExpiratoryValve, MechanicalVentilatorExpiratoryValve);
    DEFINE_STATIC_STRING_EX(InspiratoryValve, MechanicalVentilatorInspiratoryValve);
    DEFINE_STATIC_STRING_EX(ExpiratoryLimb, MechanicalVentilatorExpiratoryLimb);
    DEFINE_STATIC_STRING_EX(InspiratoryLimb, MechanicalVentilatorInspiratoryLimb);
    DEFINE_STATIC_STRING_EX(YPiece, MechanicalVentilatorYPiece);
    DEFINE_STATIC_STRING_EX(ReliefValve, MechanicalVentilatorReliefValve);
    DEFINE_STATIC_STRING_EX(Connection, MechanicalVentilatorConnection);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(MechanicalVentilator);
        _values.push_back(ExpiratoryValve);
        _values.push_back(InspiratoryValve);
        _values.push_back(ExpiratoryLimb);
        _values.push_back(InspiratoryLimb);
        _values.push_back(YPiece);
        _values.push_back(ReliefValve);
        _values.push_back(Connection);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class MechanicalVentilatorLink
  {
  public:
    DEFINE_STATIC_STRING(MechanicalVentilatorToExpiratoryValve);
    DEFINE_STATIC_STRING(MechanicalVentilatorToInspiratoryValve);
    DEFINE_STATIC_STRING_EX(ExpiratoryLimbToExpiratoryValve, MechanicalVentilatorExpiratoryLimbToExpiratoryValve);
    DEFINE_STATIC_STRING_EX(InspiratoryValveToInspiratoryLimb, MechanicalVentilatorInspiratoryValveToInspiratoryLimb);
    DEFINE_STATIC_STRING_EX(ExpiratoryLimbToYPiece, MechanicalVentilatorExpiratoryLimbToYPiece);
    DEFINE_STATIC_STRING_EX(InspiratoryLimbToYPiece, MechanicalVentilatorInspiratoryLimbToYPiece);
    DEFINE_STATIC_STRING_EX(YPieceToConnection, MechanicalVentilatorYPieceToConnection);
    DEFINE_STATIC_STRING_EX(LeakConnectionToEnvironment, MechanicalVentilatorLeakConnectionToEnvironment);
    DEFINE_STATIC_STRING_EX(ConnectionToReliefValve, MechanicalVentilatorConnectionToReliefValve);
    DEFINE_STATIC_STRING_EX(EnvironmentToReliefValve, MechanicalVentilatorEnvironmentToReliefValve);
    DEFINE_STATIC_STRING_EX(ConnectionToEnvironment, MechanicalVentilatorConnectionToEnvironment);
    DEFINE_STATIC_STRING_EX(ConnectionToAirway, MechanicalVentilatorConnectionToAirway);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(MechanicalVentilatorToExpiratoryValve);
        _values.push_back(MechanicalVentilatorToInspiratoryValve);
        _values.push_back(ExpiratoryLimbToExpiratoryValve);
        _values.push_back(InspiratoryValveToInspiratoryLimb);
        _values.push_back(ExpiratoryLimbToYPiece);
        _values.push_back(InspiratoryLimbToYPiece);
        _values.push_back(YPieceToConnection);
        _values.push_back(LeakConnectionToEnvironment);
        _values.push_back(ConnectionToReliefValve);
        _values.push_back(EnvironmentToReliefValve);
        _values.push_back(ConnectionToEnvironment);
        _values.push_back(ConnectionToAirway);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class NasalCannulaCompartment
  {
  public:
    DEFINE_STATIC_STRING(NasalCannulaOxygenSource);
    DEFINE_STATIC_STRING(NasalCannula);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(NasalCannulaOxygenSource);
        _values.push_back(NasalCannula);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class NasalCannulaLink
  {
  public:
    DEFINE_STATIC_STRING(NasalCannulaOxygenInlet);
    DEFINE_STATIC_STRING(NasalCannulaSeal);
    DEFINE_STATIC_STRING(NasalCannulaToAirway);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(NasalCannulaOxygenInlet);
        _values.push_back(NasalCannulaSeal);
        _values.push_back(NasalCannulaToAirway);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class NonRebreatherMaskCompartment
  {
  public:
    DEFINE_STATIC_STRING(NonRebreatherMaskOxygenSource);
    DEFINE_STATIC_STRING(NonRebreatherMaskPorts);
    DEFINE_STATIC_STRING(NonRebreatherMaskBag);
    DEFINE_STATIC_STRING(NonRebreatherMask);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(NonRebreatherMaskOxygenSource);
        _values.push_back(NonRebreatherMaskPorts);
        _values.push_back(NonRebreatherMaskBag);
        _values.push_back(NonRebreatherMask);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class NonRebreatherMaskLink
  {
  public:
    DEFINE_STATIC_STRING(NonRebreatherMaskOxygenInlet);
    DEFINE_STATIC_STRING(NonRebreatherMaskReservoirValve);
    DEFINE_STATIC_STRING(NonRebreatherMaskSeal);
    DEFINE_STATIC_STRING(NonRebreatherMaskExhalationValves);
    DEFINE_STATIC_STRING(NonRebreatherMaskExhalation);
    DEFINE_STATIC_STRING(NonRebreatherMaskToAirway);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(NonRebreatherMaskOxygenInlet);
        _values.push_back(NonRebreatherMaskReservoirValve);
        _values.push_back(NonRebreatherMaskSeal);
        _values.push_back(NonRebreatherMaskExhalationValves);
        _values.push_back(NonRebreatherMaskExhalation);
        _values.push_back(NonRebreatherMaskToAirway);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class SimpleMaskCompartment
  {
  public:
    DEFINE_STATIC_STRING(SimpleMaskOxygenSource);
    DEFINE_STATIC_STRING(SimpleMask);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(SimpleMaskOxygenSource);
        _values.push_back(SimpleMask);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };

  class SimpleMaskLink
  {
  public:
    DEFINE_STATIC_STRING(SimpleMaskOxygenInlet);
    DEFINE_STATIC_STRING(SimpleMaskSeal);
    DEFINE_STATIC_STRING(SimpleMaskPorts);
    DEFINE_STATIC_STRING(SimpleMaskToAirway);

    static const std::vector<std::string>& GetValues()
    {
      ScopedMutex lock;
      if (_values.empty())
      {
        _values.push_back(SimpleMaskOxygenInlet);
        _values.push_back(SimpleMaskSeal);
        _values.push_back(SimpleMaskPorts);
        _values.push_back(SimpleMaskToAirway);
      }
      return _values;
    }
    static bool HasValue(const std::string& v)
    {
      return std::find(_values.begin(), _values.end(), v) != _values.end();
    }
    static void AddValue(const std::string& v)
    {
      if (!HasValue(v))
      {
        ScopedMutex lock;
        _values.push_back(v);
      }
    }
  protected:
    static std::vector<std::string> _values;
  };
END_NAMESPACE
