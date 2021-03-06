// Copyright (c) 2015 Gregor Klinke
// All rights reserved.

#pragma once

#include "abstract-processor.hpp"

#include "program_options/program_options.hpp"

#include <string>


namespace eyestep {

class Sosofo;
class IFormattingObject;
template <typename T>
class IFoProcessor;


class DebugProcessor : public AbstractProcessor<DebugProcessor>
{
  using Super = AbstractProcessor;

public:
  DebugProcessor() = default;
  DebugProcessor(const program_options::variables_map& args);

  std::string proc_id() const override {
    return "debug";
  }

  std::string default_output_extension() const override {
    return {};
  }

  program_options::options_description program_options() const override;

  const IFoProcessor<DebugProcessor>*
  lookup_fo_processor(const std::string& fo_classname) const override;

  void before_rendering() override;
  void render_sosofo(const Sosofo* sosofo) override;
  void render_fo(const IFormattingObject* fo) override;
};

} // ns eyestep
