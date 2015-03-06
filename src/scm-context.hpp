// Copyright (c) 2015 Gregor Klinke
// All rights reserved.

#pragma once

#include <boost/filesystem.hpp>

#include <memory>
#include <vector>


namespace eyestep {

class Node;
class Sosofo;

class ISchemeContext {
public:
  virtual ~ISchemeContext(){};

  virtual void
  initialize(const std::vector<boost::filesystem::path>& modulePaths) = 0;
  virtual bool loadModuleFile(const boost::filesystem::path& scriptFile) = 0;
  virtual bool loadScript(const boost::filesystem::path& scriptFile) = 0;

  virtual std::unique_ptr<Sosofo> processRootNode(const Node* rootNode) = 0;
};

std::unique_ptr<ISchemeContext> createSchemeContext();

} // ns eyestep
