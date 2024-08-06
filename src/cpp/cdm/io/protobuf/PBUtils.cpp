/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "cdm/CommonDefs.h"
PUSH_PROTO_WARNINGS
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>
POP_PROTO_WARNINGS
#include "cdm/io/protobuf/PBUtils.h"
#include "cdm/utils/FileUtils.h"

void PBUtils::LogError(const std::string& method, const std::string& err, Logger* logger)
{
  if (logger != nullptr)
    logger->Warning("Protobuf could not "+method+": " + err);
  else
  {
    std::cerr << "Protobuf could not "<< method << std::endl;
    std::cerr << err << std::endl;
  }
}

bool PBUtils::SerializeFromFile(const std::string& filename, google::protobuf::Message& dst, Logger* logger)
{
  if (!FileExists(filename))
    return false;
  if (IsJSONFile(filename))
  {
    std::string content;
    if (!ReadFile(filename, content))
      return false;
    if (!SerializeFromString(content, dst, eSerializationFormat::JSON, logger))
      return false;
    return true;
  }
  std::ifstream input(filename, std::ios::binary);
  if (!input.is_open())
    return false;
  bool b = dst.ParseFromIstream(&input);
  if (!b)
  {

  }
  input.close();
  return b;
}

bool PBUtils::SerializeToFile(const google::protobuf::Message& src, const std::string& filename, Logger* logger)
{
  if (IsJSONFile(filename))
  {
    std::string content;
    if (!SerializeToString(src, content, eSerializationFormat::JSON, logger))
      return false;
    return WriteFile(content, filename);
  }
  if (!CreateFilePath(filename))
    return false;
  std::ofstream output(filename, std::ios::binary);
  bool b = src.SerializeToOstream(&output);
  if (!b)
  {

  }
  output.close();
  return b;
}


bool PBUtils::SerializeFromString(const std::string& src, google::protobuf::Message& dst, eSerializationFormat m, Logger* logger)
{
  bool ret = true;
  if (m == eSerializationFormat::JSON || m == eSerializationFormat::VERBOSE_JSON)
  {
    google::protobuf::util::JsonParseOptions opts;
    auto stat = google::protobuf::util::JsonStringToMessage(src, &dst, opts);
    if (!stat.ok())
    {
      if (logger != nullptr)
        logger->Warning("Protobuf could not SerializeFromString: " + stat.ToString());
      else
        std::cerr << stat.ToString() << std::endl;
      ret = false;
      // Old code to check if the string is in the old google text format
      // We generally do not support this, but if there is some weird reason
      // Here is how you would do that
      // logger->Info("Checking to see if string is in google text format...");
      // ret = google::protobuf::TextFormat::ParseFromString(src, &dst);
    }
  }
  else if (m == eSerializationFormat::TEXT)
  {
    google::protobuf::TextFormat::ParseFromString(src, &dst);
  }
  else
  {
    ret = dst.ParseFromString(src);
  }
  return ret;
}

bool PBUtils::SerializeToString(const google::protobuf::Message& src, std::string& output, eSerializationFormat m, Logger* logger)
{
  if (m == eSerializationFormat::JSON || m == eSerializationFormat::VERBOSE_JSON)
  {
    google::protobuf::util::JsonPrintOptions opts;
    opts.add_whitespace = true;
    opts.preserve_proto_field_names = true;
    if (m == eSerializationFormat::VERBOSE_JSON)
      opts.always_print_fields_with_no_presence = true;
    auto status = google::protobuf::util::MessageToJsonString(src, &output, opts);
    if (!status.ok())
    {
      LogError("SerializeFromString", status.ToString(), logger);
      return false;
    }
  }
  else if (m == eSerializationFormat::TEXT)
  {
    if (!google::protobuf::TextFormat::PrintToString(src, &output))
    {
      LogError("SerializeFromString", src.InitializationErrorString(), logger);
      return false;
    }
  }
  else
  {
    if (!src.SerializeToString(&output))
    {
      LogError("SerializeFromString", src.InitializationErrorString(), logger);
      return false;
    }
  }
  return true;
}
