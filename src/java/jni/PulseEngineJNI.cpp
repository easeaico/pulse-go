/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "PulseEngineJNI.h"

#include "PulseScenario.h"
#include "engine/human_adult/whole_body/test/EngineTest.h"
#include "engine/human_adult/ventilation_mechanics/test/EngineTest.h"

extern "C"
JNIEXPORT jstring JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeGetVersion(JNIEnv * env)
{
  jstring v = env->NewStringUTF(PulseBuildInformation::Version().c_str());
  return v;
}

extern "C"
JNIEXPORT jstring JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeGetHash(JNIEnv * env)
{
  jstring h = env->NewStringUTF(PulseBuildInformation::Hash().c_str());
  return h;
}

  /////////////////////
  // TESTING SUPPORT //
  /////////////////////

extern "C"
JNIEXPORT jlong JNICALL Java_com_kitware_pulse_engine_testing_EngineUnitTestDriver_nativeAllocate(JNIEnv* /*env*/, jobject /*obj*/)
{
  auto *executor = new pulse::human_adult_whole_body::EngineTest();
  return reinterpret_cast<jlong>(executor);
}

extern "C"
JNIEXPORT void JNICALL Java_com_kitware_pulse_engine_testing_EngineUnitTestDriver_nativeDelete(JNIEnv* /*env*/, jobject /*obj*/, jlong ptr)
{
  auto *executor = reinterpret_cast<pulse::human_adult_whole_body::EngineTest*>(ptr);
  SAFE_DELETE(executor);
}

extern "C"
JNIEXPORT void JNICALL Java_com_kitware_pulse_engine_testing_EngineUnitTestDriver_nativeExecute(JNIEnv * env, jobject /*obj*/, jlong ptr, jstring test, jstring toDir)
{
  const char* testName = env->GetStringUTFChars(test, JNI_FALSE);
  const char* outputDir = env->GetStringUTFChars(toDir, JNI_FALSE);
  auto executor = reinterpret_cast<pulse::human_adult_whole_body::EngineTest*>(ptr);
  executor->RunTest(testName, outputDir);
  env->ReleaseStringUTFChars(test, testName);
  env->ReleaseStringUTFChars(toDir, outputDir);
}

  ////////////////////
  // ENGINE SUPPORT //
  ////////////////////

extern "C"
JNIEXPORT jlong JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeAllocate(JNIEnv *env, jobject obj, jstring dataDir, jint model)
{ 
  const char* dataDirStr = env->GetStringUTFChars(dataDir, JNI_FALSE);
  PulseEngineJNI *engineJNI = new PulseEngineJNI((eModelType)model, dataDirStr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;
  env->ReleaseStringUTFChars(dataDir, dataDirStr);
  
  return reinterpret_cast<jlong>(engineJNI);
}

extern "C"
JNIEXPORT void JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeDelete(JNIEnv *env, jobject obj, jlong ptr)
{
  PulseEngineJNI *engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;
  SAFE_DELETE(engineJNI);
}

extern "C"
JNIEXPORT void JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeClear(JNIEnv * env, jobject obj, jlong ptr)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;
  engineJNI->Clear();
}

//////////////////////
// SCENARIO SUPPORT //
//////////////////////

extern "C"
JNIEXPORT jboolean JNICALL Java_com_kitware_pulse_engine_PulseScenarioExec_nativeExecuteScenario(JNIEnv * env, jobject obj, jstring sceOpts, jint scenario_format)
{
  jboolean bRet;
  const char* sceOptsStr = env->GetStringUTFChars(sceOpts, JNI_FALSE);
  /*
  Thunking log messages from C++ is a problem with threaded execution on the C++ side
  The jniEnv is not stable when called from a new thread. Did not investigate, just disabled auto passing log to Java
  Logger logger;
  LoggerForwardJNI jniForward;
  jniForward.jniEnv = env;
  jniForward.jniObj = obj;
  logger.AddForward(&jniForward);
  */
  bRet = PulseEngineThunk::ExecuteScenario(sceOptsStr, (eSerializationFormat)scenario_format);
  env->ReleaseStringUTFChars(sceOpts, sceOptsStr);
  return bRet;
}

//////////////////
// START ENGINE //
//////////////////

extern "C"
JNIEXPORT jboolean JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeSerializeFromFile(JNIEnv *env, jobject obj, jlong ptr, jstring stateFilename, jstring dataRequests, jint dataRequestsFormat)
{
  PulseEngineJNI *engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  const char* fnStr = env->GetStringUTFChars(stateFilename, JNI_FALSE);
  const char* dmStr = nullptr;
  if(dataRequests != nullptr)
    dmStr = env->GetStringUTFChars(dataRequests, JNI_FALSE);
  jboolean bRet = engineJNI->SerializeFromFile(fnStr, dmStr, (eSerializationFormat)dataRequestsFormat);

  env->ReleaseStringUTFChars(stateFilename, fnStr);
  if(dmStr != nullptr)
    env->ReleaseStringUTFChars(dataRequests, dmStr);
  return bRet;
}
extern "C"
JNIEXPORT jboolean JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeSerializeToFile(JNIEnv *env, jobject obj, jlong ptr, jstring stateFilename)
{
  jboolean bRet;
  PulseEngineJNI *engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env; 
  engineJNI->jniObj = obj;

  const char* fnStr = env->GetStringUTFChars(stateFilename, JNI_FALSE);
  bRet = engineJNI->SerializeToFile(fnStr);

  env->ReleaseStringUTFChars(stateFilename, fnStr);
  return bRet;
}

extern "C"
JNIEXPORT jboolean JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeSerializeFromString(JNIEnv * env, jobject obj, jlong ptr, jstring state, jstring dataRequests, jint format)
{
  jboolean bRet;
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  const char* dmStr = nullptr;
  if (dataRequests != nullptr)
    dmStr = env->GetStringUTFChars(dataRequests, JNI_FALSE);
  const char* sStr = env->GetStringUTFChars(state, JNI_FALSE);
  
  bRet = engineJNI->SerializeFromString(sStr, dmStr, (eSerializationFormat)format);

  env->ReleaseStringUTFChars(state, sStr);
  if (dmStr != nullptr)
    env->ReleaseStringUTFChars(dataRequests, dmStr);
  return bRet;
}
extern "C"
JNIEXPORT jstring JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeSerializeToString(JNIEnv * env, jobject obj, jlong ptr, jint format)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  std::string out = engineJNI->SerializeToString((eSerializationFormat)format);
  jstring state = env->NewStringUTF(out.c_str());
  return state;
}

extern "C"
JNIEXPORT jboolean JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeInitializeEngine(JNIEnv *env, jobject obj, jlong ptr, jstring patient_configuration, jstring dataRequests, jint format)
{
  bool bRet;
  PulseEngineJNI *engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  if (patient_configuration == nullptr)
    return false;
  const char* pcStr = env->GetStringUTFChars(patient_configuration, JNI_FALSE);

  const char* drStr = nullptr;
  if (dataRequests != nullptr)
    drStr = env->GetStringUTFChars(dataRequests, JNI_FALSE);

  bRet = engineJNI->InitializeEngine(pcStr, drStr==nullptr?"":drStr, (eSerializationFormat)format);

  env->ReleaseStringUTFChars(patient_configuration, pcStr);
  if(drStr != nullptr)
    env->ReleaseStringUTFChars(dataRequests, drStr);

  return bRet;
}

extern "C"
JNIEXPORT jstring JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeGetInitialPatient(JNIEnv * env, jobject obj, jlong ptr, jint format)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  std::string stream = engineJNI->GetInitialPatient((eSerializationFormat)format);
  jstring patient = env->NewStringUTF(stream.c_str());
  return patient;
}

extern "C"
JNIEXPORT jstring JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeGetConditions(JNIEnv * env, jobject obj, jlong ptr, jint format)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  std::string stream = engineJNI->GetConditions((eSerializationFormat)format);
  jstring conditionManager = env->NewStringUTF(stream.c_str());
  return conditionManager;
}

extern "C"
JNIEXPORT jstring JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeGetAssessment(JNIEnv * env, jobject obj, jlong ptr, jint type, jint format)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  std::string stream = engineJNI->GetPatientAssessment(type,(eSerializationFormat)format);
  jstring assessment = env->NewStringUTF(stream.c_str());
  return assessment;
}


extern "C"
JNIEXPORT void JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeSetLogFilename(JNIEnv * env, jobject obj, jlong ptr, jstring logFilename)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;
  const char* logF = env->GetStringUTFChars(logFilename, JNI_FALSE);
  engineJNI->SetLogFilename(logF);
  env->ReleaseStringUTFChars(logFilename, logF);
}
extern "C"
JNIEXPORT void JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeKeepLogMessages(JNIEnv* env, jobject obj, jlong ptr, jboolean b)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;
  engineJNI->KeepLogMessages(b);
}
extern "C"
JNIEXPORT jstring JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativePullLogMessages(JNIEnv* env, jobject obj, jlong ptr, jint format)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  std::string out = engineJNI->PullLogMessages((eSerializationFormat)format);
  jstring events = env->NewStringUTF(out.c_str());
  return events;
}

extern "C"
JNIEXPORT void JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeKeepEventChanges(JNIEnv * env, jobject obj, jlong ptr, jboolean b)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;
  engineJNI->KeepEventChanges(b);
}
extern "C"
JNIEXPORT jstring JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativePullEvents(JNIEnv * env, jobject obj, jlong ptr, jint format)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  std::string out = engineJNI->PullEvents((eSerializationFormat)format);
  jstring events = env->NewStringUTF(out.c_str());
  return events;
}
extern "C"
JNIEXPORT jstring JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativePullActiveEvents(JNIEnv * env, jobject obj, jlong ptr, jint format)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  std::string out = engineJNI->PullActiveEvents((eSerializationFormat)format);
  jstring events = env->NewStringUTF(out.c_str());
  return events;
}

extern "C"
JNIEXPORT jboolean JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeProcessActions(JNIEnv * env, jobject obj, jlong ptr, jstring actions, jint format)
{
  if (actions == nullptr)
    return true;
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  const char* aStr = env->GetStringUTFChars(actions, JNI_FALSE);
  jboolean bRet = engineJNI->ProcessActions(aStr, (eSerializationFormat)format);
  env->ReleaseStringUTFChars(actions, aStr);
  return bRet;
}

extern "C"
JNIEXPORT jstring JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativePullActiveActions(JNIEnv * env, jobject obj, jlong ptr, jint format)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  std::string stream = engineJNI->PullActiveActions((eSerializationFormat)format);
  jstring actions = env->NewStringUTF(stream.c_str());
  return actions;
}

extern "C"
JNIEXPORT jdouble JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeGetTimeStep(JNIEnv * env, jobject obj, jlong ptr, jstring unit)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  const char* uStr = "s";
  if(unit != nullptr)
    uStr = env->GetStringUTFChars(unit, JNI_FALSE);
  jdouble ts = engineJNI->GetTimeStep(uStr);
  env->ReleaseStringUTFChars(unit, uStr);
  return ts;
}

extern "C"
JNIEXPORT bool JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativeAdvanceTimeStep(JNIEnv *env, jobject obj, jlong ptr)
{
  PulseEngineJNI *engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  return engineJNI->AdvanceTimeStep();
}

extern "C"
JNIEXPORT jdoubleArray JNICALL Java_com_kitware_pulse_engine_PulseEngine_nativePullData(JNIEnv * env, jobject obj, jlong ptr)
{
  PulseEngineJNI* engineJNI = reinterpret_cast<PulseEngineJNI*>(ptr);
  engineJNI->jniEnv = env;
  engineJNI->jniObj = obj;

  jdoubleArray jData;
  double* data = engineJNI->PullDataPtr();
  jsize len = (jsize)engineJNI->DataLength();
  jData = env->NewDoubleArray(len);
  env->SetDoubleArrayRegion(jData, 0, len, data);
  return jData;
}

PulseEngineJNI::PulseEngineJNI(eModelType t, const std::string& dataDir) : PulseEngineThunk(t,dataDir)
{
  Clear();
}

PulseEngineJNI::~PulseEngineJNI()
{
  Clear();
}

void PulseEngineJNI::Clear()
{
  jniEnv = nullptr;
  jniObj = nullptr;
  PulseEngineThunk::Clear();
}

void LoggerForwardJNI::Clear()
{
  jniEnv=nullptr;
  jniObj=nullptr;
  jniDebugMethodID=nullptr;
  jniInfoMethodID=nullptr;
  jniWarnMethodID=nullptr;
  jniErrorMethodID=nullptr;
  jniFatalMethodID=nullptr;
}

void LoggerForwardJNI::ForwardDebug(const std::string& msg)
{
  if (jniEnv != nullptr && jniObj != nullptr)
  {
    jstring m = jniEnv->NewStringUTF(msg.c_str());
    if (jniDebugMethodID == nullptr)
      jniDebugMethodID = jniEnv->GetMethodID(jniEnv->GetObjectClass(jniObj), "handleDebug", "(Ljava/lang/String;)V");
    jniEnv->CallVoidMethod(jniObj, jniDebugMethodID, m);
  }
}

void LoggerForwardJNI::ForwardInfo(const std::string& msg)
{
  if (jniEnv != nullptr && jniObj != nullptr)
  {
    jstring m = jniEnv->NewStringUTF(msg.c_str());
    if (jniInfoMethodID == nullptr)
      jniInfoMethodID = jniEnv->GetMethodID(jniEnv->GetObjectClass(jniObj), "handleInfo", "(Ljava/lang/String;)V");
    jniEnv->CallVoidMethod(jniObj, jniInfoMethodID, m);
  }
}

void LoggerForwardJNI::ForwardWarning(const std::string& msg)
{
  if (jniEnv != nullptr && jniObj != nullptr)
  {
    jstring m = jniEnv->NewStringUTF(msg.c_str());
    if (jniWarnMethodID == nullptr)
      jniWarnMethodID = jniEnv->GetMethodID(jniEnv->GetObjectClass(jniObj), "handleWarning", "(Ljava/lang/String;)V");
    jniEnv->CallVoidMethod(jniObj, jniWarnMethodID, m);
  }
}

void LoggerForwardJNI::ForwardError(const std::string& msg)
{
  if (jniEnv != nullptr && jniObj != nullptr)
  {
    jstring m = jniEnv->NewStringUTF(msg.c_str());
    if (jniErrorMethodID == nullptr)
      jniErrorMethodID = jniEnv->GetMethodID(jniEnv->GetObjectClass(jniObj), "handleError", "(Ljava/lang/String;)V");
    jniEnv->CallVoidMethod(jniObj, jniErrorMethodID, m);
  }
}

void LoggerForwardJNI::ForwardFatal(const std::string& msg)
{
  if (jniEnv != nullptr && jniObj != nullptr)
  {
    jstring m = jniEnv->NewStringUTF(msg.c_str());
    if (jniFatalMethodID == nullptr)
      jniFatalMethodID = jniEnv->GetMethodID(jniEnv->GetObjectClass(jniObj), "handleFatal", "(Ljava/lang/String;)V");
    jniEnv->CallVoidMethod(jniObj, jniFatalMethodID, m);
  }
}
