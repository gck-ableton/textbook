// Copyright (c) 2015 Gregor Klinke
// All rights reserved.

#pragma once

#include "fo.hpp"
#include "abstract-processor.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/optional/optional.hpp>
#include <boost/program_options.hpp>

#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>

#include "hpdf.h"


namespace eyestep {

class Sosofo;
class IFormattingObject;
template <typename T>
class IFoProcessor;

namespace pdf_detail {
  enum CapsStyle {
    k_normal_caps,
    k_lower_caps,
    k_upper_caps,
    k_small_caps,
  };

  enum WrapStyle {
    k_normal_wrap,
    k_asis_wrap,
    // k_asis_wrap,
    k_no_wrap,
  };

  enum WsTreatment {
    k_preserve_ws,
    k_collapse_ws,
    k_ignore_ws,
  };

  struct PdfStyleContext {
    CapsStyle _capsstyle;
    WrapStyle _wrapstyle;
    WsTreatment _wstreatment;
  };

  enum BreakKind {
    kNoBreak,
    kBreakPageBefore,
    kBreakPageAfter,
    kSurpressNextPageBreak,
  };

  enum CropMarksKind {
    kOff,
    kCamera,
    kFrame,
  };
}

class PdfProcessor : public AbstractProcessor<PdfProcessor> {
  bool _verbose;
  boost::filesystem::ofstream _stream;
  pdf_detail::PdfStyleContext _style_ctx;

public:
  PdfProcessor();
  PdfProcessor(const boost::program_options::variables_map& args);

  std::string proc_id() const override;
  std::string default_output_extension() const override;
  boost::program_options::options_description program_options() const override;

  const IFoProcessor<PdfProcessor>*
  lookup_fo_processor(const std::string& fo_classname) const override;

  void before_rendering() override;
  void after_rendering() override;

  bool is_verbose() const;

  fo::LengthSpec paper_width() const;
  fo::LengthSpec paper_height() const;

  boost::filesystem::ofstream& stream();
  pdf_detail::PdfStyleContext& style_ctx();

  void finalize_breaks();
  void request_page_break(pdf_detail::BreakKind breakKind);

  fo::LengthSpec _current_start_margin;
  int _first_page = true;
  pdf_detail::BreakKind _break_pending = pdf_detail::kNoBreak;
  pdf_detail::CropMarksKind _cropmarks;
  // width, height, cropmarks classifier
  std::tuple<fo::LengthSpec, fo::LengthSpec, std::string> _paper_dimen;

  HPDF_Doc _pdf;
};


} // ns eyestep