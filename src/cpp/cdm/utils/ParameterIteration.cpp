/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
#include "cdm/utils/ParameterIteration.h"

template <typename T>
ParameterIteration<T>::ParameterIteration()
{
  m_SliceIdx = 0;
}

template <typename T>
ParameterIteration<T>::ParameterIteration(T min, T max, T step)
{
  SetMinMaxStep(min, max, step);
}

template <typename T>
ParameterIteration<T>::ParameterIteration(std::initializer_list<T> values, int sliceIdx)
{
  SetValues(values, sliceIdx);
}

template <typename T>
void ParameterIteration<T>::SetMinMaxStep(T min, T max, T stepSize)
{
  m_Values.clear();
  int idx;
  if (min == max)
    m_Values.push_back(max);
  else
  {
    int n = (int)((max - min) / stepSize);
    for (idx = 0; idx <= n; ++idx)
    {
      m_Values.push_back(min + stepSize * idx);
    }
  }
}

template <typename T>
void ParameterIteration<T>::SetValues(const std::vector<T>& values, int sliceIdx)
{
  m_Values.clear();
  for (T v : values)
    m_Values.push_back(v);
  m_SliceIdx = sliceIdx;
}

template <typename T>
void ParameterIteration<T>::SetValues(std::initializer_list<T> values, int sliceIdx)
{
  std::vector<T> v(values);
  this->SetValues(v, sliceIdx);
}

template class ParameterIteration<size_t>;
template class ParameterIteration<double>;
