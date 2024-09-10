/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
class SESubstanceManager;

enum class eMergeType { Append = 0, Replace };
extern CDM_DECL const std::string& eMergeType_Name(eMergeType m);

enum class eAppliedRespiratoryCycle { Expiratory=0, Inspiratory, Instantaneous };
extern CDM_DECL const std::string& eAppliedRespiratoryCycle_Name(eAppliedRespiratoryCycle m);

class CDM_DECL SEAction : public Loggable
{
  friend class PBAction;//friend the serialization class
public:

  SEAction(Logger* logger);
  virtual ~SEAction();

  SEAction(const SEAction&) = delete;
  SEAction& operator= (const SEAction&) = delete;

  virtual std::string GetName() const = 0;
  static constexpr char const* ActionType = "Action";
  virtual std::string GetActionType() const { return ActionType; }

  virtual void Clear();
  static SEAction* Copy(const SEAction&, const SESubstanceManager&);

  virtual bool SerializeToString(std::string& dst, eSerializationFormat fmt) const;
  static SEAction* SerializeFromString(const std::string, eSerializationFormat fmt, const SESubstanceManager&);

  /** Test if the action has all data it needs */
  virtual bool IsValid() const { return true; }

  /** Actions can be turned off or on with various data combinations
  *  This method will encapsulate that logic in a single function */
  virtual void Activate() { m_Active = true; }
  virtual bool IsActive() const { return !m_Active ? false : IsValid(); }
  virtual void Deactivate() { m_Active = false; }

  virtual std::string GetComment() const;
  virtual void SetComment(const std::string& comment);
  virtual bool HasComment() const;
  virtual void InvalidateComment();

  virtual const SEScalar* GetScalar(const std::string& name)=0;

  static std::string PrettyPrint(const std::string& str);

  virtual std::string ToJSON() const;
  virtual std::string ToString() const;

protected:
  bool                  m_Active = true;
  std::string           m_Comment;
};

inline std::ostream& operator<< (std::ostream& out, const SEAction& a)
{
  out << a.ToString();
  return out;
}
