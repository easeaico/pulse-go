/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
class SESubstanceManager;
class CCompoundUnit;
class SEDataRequestManager;
#include "cdm/engine/SEDecimalFormat.h"

enum class eDataRequest_Category {
  Patient = 0, Physiology, Environment, Action,
  GasCompartment, LiquidCompartment, ThermalCompartment, TissueCompartment,
  Substance, AnesthesiaMachine, BagValveMask, ECG, ECMO, Inhaler, MechanicalVentilator
};
extern CDM_DECL const std::string& eDataRequest_Category_Name(eDataRequest_Category m);

class CDM_DECL SEDataRequest : public SEDecimalFormat
{
  friend class PBEngine;//friend the serialization class
  friend class SEDataRequestManager;
  friend class SEDataRequestScalar;
  friend class SEEngineTracker;
protected:
  SEDataRequest(const SEDataRequest& dr);
  SEDataRequest(eDataRequest_Category category, const SEDecimalFormat* dfault = nullptr);
public:

  virtual ~SEDataRequest();

  void Clear() override; //clear memory
  virtual bool IsValid();
  virtual void Copy(const SEDataRequest& src);

  virtual size_t HashCode() const;

  // The Request Category
  virtual eDataRequest_Category GetCategory() const;

  // OPTIONAL The Action Name holding the property
protected:
  virtual void SetActionName(const std::string& name);
  virtual void InvalidateActionName();
public:
  virtual std::string GetActionName() const;
  virtual bool HasActionName() const;

  // OPTIONAL The Compartment Name holding the property
protected:
  virtual void SetCompartmentName(const std::string& name);
  virtual void InvalidateCompartmentName();
public:
  virtual std::string GetCompartmentName() const;
  virtual bool HasCompartmentName() const;

  // OPTIONAL The Substance Name holding the property
protected:
  virtual void SetSubstanceName(const std::string& name);
  virtual void InvalidateSubstanceName();
public:
  virtual std::string GetSubstanceName() const;
  virtual bool HasSubstanceName() const;

  // The System Property Name
protected:
  virtual void SetPropertyName(const std::string& name);
  virtual void InvalidatePropertyName();
public:
  virtual std::string GetPropertyName() const;
  virtual bool HasPropertyName() const;

  // The Requested Unit String
protected:
  virtual void SetRequestedUnit(const std::string& unit);
  virtual void InvalidateRequestedUnit();
public:
  virtual std::string GetRequestedUnit() const;
  virtual bool HasRequestedUnit() const;

  // The Unit that data will be retrieved in
  // If the Requested unit is present, this should be set to that unit
  // I cannot do this automatically, since I don't know what the quantity of the property is, so some other logic needs to complete this logic
  // If the Requested unit is NOT present, this should be set to whatever the unit of the System Property is set as
protected:
  virtual void SetUnit(const CCompoundUnit& unit);
  virtual void InvalidateUnit();
public:
  virtual const CCompoundUnit* GetUnit() const;
  virtual bool HasUnit() const;

  virtual std::string ToString() const;
  virtual std::string GetHeaderName() const;

protected:

  eDataRequest_Category          m_Category;
  std::string                    m_ActionName;
  std::string                    m_CompartmentName;
  std::string                    m_SubstanceName;
  std::string                    m_PropertyName;
  std::string                    m_RequestedUnit;
  const CCompoundUnit*           m_Unit;
  mutable std::string            m_Header;
};