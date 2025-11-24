/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "cdm/blackbox/electrical/SEElectricalBlackBox.h"
#include "cdm/blackbox/fluid/SEGasBlackBox.h"
#include "cdm/blackbox/fluid/SELiquidBlackBox.h"
#include "cdm/blackbox/thermal/SEThermalBlackBox.h"

class CDM_DECL SEBlackBoxManager : public Loggable
{
  friend class CommonDataModelTest;
public:
  SEBlackBoxManager(Logger* logger);
  virtual ~SEBlackBoxManager();

  virtual bool HasElectricalBlackBox(const std::string& name) const;
  virtual bool HasElectricalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName) const;
  virtual bool HasGasBlackBox(const std::string& name) const;
  virtual bool HasGasBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName) const;
  virtual bool HasLiquidBlackBox(const std::string& name) const;
  virtual bool HasLiquidBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName) const;
  virtual bool HasThermalBlackBox(const std::string& name) const;
  virtual bool HasThermalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName) const;

  virtual SEElectricalBlackBox* GetElectricalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, std::string name="");
  virtual SEGasBlackBox* GetGasBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, std::string name = "");
  virtual SELiquidBlackBox* GetLiquidBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, std::string name = "");
  virtual SEThermalBlackBox* GetThermalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, std::string name = "");

  static std::string GetBlackBoxName(const std::string& srcCmptName, const std::string& tgtCmptName);

protected:
  virtual SEElectricalBlackBox* CreateElectricalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, const std::string& name);
  virtual SEGasBlackBox* CreateGasBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, const std::string& name);
  virtual SELiquidBlackBox* CreateLiquidBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, const std::string& name);
  virtual SEThermalBlackBox* CreateThermalBlackBox(const std::string& srcCmptName, const std::string& tgtCmptName, const std::string& name);

  template<typename tBlackBox> tBlackBox* CreateBlackBox(const std::string& name, std::map<std::string, tBlackBox*>& ledger);
  
  // Map circuit compartments in a separate function, to support unit tests
  template<typename tBlackBox, typename tCompartment, typename tLink, typename tNode, typename tPath>
  bool MapBlackBox(tBlackBox& bb, tLink& src2bbLink, tLink& bb2tgtLink)
  {
    if (bb.HasCompartment())
      Warning("Blackbox " + bb.GetName() + " already has a compartment mapped, only 1 black box can be between 2 compartments");

    tCompartment& midCmpt = src2bbLink.GetTargetCompartment();
    tCompartment& srcCmpt = src2bbLink.GetSourceCompartment();
    tCompartment& tgtCmpt = bb2tgtLink.GetTargetCompartment();
    bb.SetCompartment(&midCmpt);
    bb.SetSourceCompartment(&srcCmpt);
    bb.SetSourceLink(&src2bbLink);
    bb.SetTargetCompartment(&tgtCmpt);
    bb.SetTargetLink(&bb2tgtLink);

    tPath& srcPath = *src2bbLink.GetPath();
    tPath& tgtPath = *bb2tgtLink.GetPath();

    return MapBlackBox<tBlackBox, tNode, tPath>(bb, srcPath, tgtPath);
  }

  // Map circuit components in a separate function, to support unit tests
  template<typename tBlackBox, typename tNode, typename tPath>
  bool MapBlackBox(tBlackBox& bb, tPath& srcPath, tPath& tgtPath)
  {
    if (bb.HasMiddleNode())
      Warning("Blackbox " + bb.GetName() + " already has nodes mapped, only 1 blackbox can be between nodes");

    tNode* midNode=nullptr;
    tNode* srcNode=nullptr;
    tNode* tgtNode=nullptr;

    //Middle node is the shared one
    if (&srcPath.GetSourceNode() == &tgtPath.GetSourceNode())
    {
      midNode = &srcPath.GetSourceNode();
      srcNode = &srcPath.GetTargetNode();
      tgtNode = &tgtPath.GetTargetNode();
    }
    else if (&srcPath.GetSourceNode() == &tgtPath.GetTargetNode())
    {
      midNode = &srcPath.GetSourceNode();
      srcNode = &srcPath.GetTargetNode();
      tgtNode = &tgtPath.GetSourceNode();
    }
    else if (&srcPath.GetTargetNode() == &tgtPath.GetSourceNode())
    {
      midNode = &srcPath.GetTargetNode();
      srcNode = &srcPath.GetSourceNode();
      tgtNode = &tgtPath.GetTargetNode();
    }
    else if (&srcPath.GetTargetNode() == &tgtPath.GetTargetNode())
    {
      midNode = &srcPath.GetTargetNode();
      srcNode = &srcPath.GetSourceNode();
      tgtNode = &tgtPath.GetSourceNode();
    }
    else
    {
      Fatal("Blackbox nodes/paths incorrectly setup.");
      return false;
    }

    midNode->SetPartOfBlackBox(true);
    midNode->SetBlackBoxSourceTargetNodes(*srcNode, *tgtNode);
    midNode->SetBlackBoxSourceTargetPaths(srcPath, tgtPath);
    srcNode->SetPartOfBlackBox(true);
    tgtNode->SetPartOfBlackBox(true);
    srcPath.SetPartOfBlackBox(true);
    tgtPath.SetPartOfBlackBox(true);

    bb.SetMiddleNode(midNode);
    bb.SetSourceNode(srcNode);
    bb.SetSourcePath(&srcPath);
    bb.SetTargetNode(tgtNode);
    bb.SetTargetPath(&tgtPath);

    return true;
  }
  std::map<std::string, SEElectricalBlackBox*> m_ElectricalBoxes;
  std::map<std::string, SEGasBlackBox*> m_GasBoxes;
  std::map<std::string, SELiquidBlackBox*> m_LiquidBoxes;
  std::map<std::string, SEThermalBlackBox*> m_ThermalBoxes;
};
