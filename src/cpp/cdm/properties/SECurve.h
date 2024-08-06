/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/properties/SEProperty.h"
#include "cdm/properties/SESegmentConstant.h"
#include "cdm/properties/SESegmentLinear.h"
#include "cdm/properties/SESegmentParabolic.h"
#include "cdm/properties/SESegmentSigmoidal.h"

class CDM_DECL SECurve : public SEProperty, public Loggable
{
public:

  SECurve(Logger* logger=nullptr);
  virtual ~SECurve();

  void Invalidate() override;
  virtual bool IsValid() const override;
  virtual void Copy(const SECurve& src);

  virtual bool HasSegment() const;
  virtual void SortSegments();

  SESegmentConstant& AddConstantSegment();
  SESegmentLinear& AddLinearSegment();
  SESegmentParabolic& AddParabolicSegment();
  SESegmentSigmoidal& AddSigmoidalSegment();

  virtual const std::vector<SESegment*>& GetSegments();
  virtual const std::vector<const SESegment*> GetSegments() const;

  virtual std::string ToString() const;
  virtual void ToString(std::ostream& str) const;

protected:

  std::vector<SESegment*> m_Segments;
};
