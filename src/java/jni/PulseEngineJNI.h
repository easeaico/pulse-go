/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include <jni.h>
#include <memory>
#include <iostream>
#include "PulseEngine.h"
#include "PulseEngineThunk.h"

class LoggerForwardJNI : public LoggerForward
{
public:
  LoggerForwardJNI() { Clear(); }
  virtual ~LoggerForwardJNI() { Clear(); }

  void Clear();

  void ForwardDebug(const std::string& msg) override;
  void ForwardInfo(const std::string& msg) override;
  void ForwardWarning(const std::string& msg) override;
  void ForwardError(const std::string& msg) override;
  void ForwardFatal(const std::string& msg) override;

  JNIEnv*   jniEnv;
  jobject   jniObj;
  jmethodID jniDebugMethodID;
  jmethodID jniInfoMethodID;
  jmethodID jniWarnMethodID;
  jmethodID jniErrorMethodID;
  jmethodID jniFatalMethodID;
};

class PulseEngineJNI : public PulseEngineThunk
{
public:
  PulseEngineJNI(eModelType t, const std::string& dataDir);
  virtual ~PulseEngineJNI();

  void Clear() override;

  JNIEnv* jniEnv;
  jobject jniObj;
};
