/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/engine/SEAction.h"

// Keep enums in sync with appropriate schema/cdm/ActionEnums.proto file !!
enum class eSerialization_Mode { Save = 0, Load };
extern CDM_DECL const std::string& eSerialization_Mode_Name(eSerialization_Mode m);

class CDM_DECL SESerializeState : public SEAction
{
  friend class PBAction;//friend the serialization class
public:

  SESerializeState(Logger* logger=nullptr);
  virtual ~SESerializeState();

  static constexpr char const* Name = "Serialize State";
  std::string GetName() const override { return Name; }

  void Clear() override; //clear memory

  bool IsValid() const override;

  virtual eSerialization_Mode GetMode() const;
  virtual void SetMode(eSerialization_Mode m);

  virtual bool HasFilename() const;
  virtual std::string GetFilename() const;
  virtual void SetFilename(const std::string& filename);
  virtual void InvalidateFilename();

  const SEScalar* GetScalar(const std::string& name) override;

protected:

  std::string               m_Filename;
  eSerialization_Mode       m_Mode;
};