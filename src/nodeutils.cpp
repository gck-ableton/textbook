// Copyright (c) 2015 Gregor Klinke
// All rights reserved.

#include "nodeutils.hpp"
#include "nodeclass.hpp"
#include "nodelist.hpp"
#include "nodes.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>


namespace eyestep {

//------------------------------------------------------------------------------

TraverseRecursion node_traverse(const Node* root, const TraverseNodeVisitor& functor,
                                int depth) {
  auto rec = functor(root, depth);

  if (rec == TraverseRecursion::k_recurse) {
    const auto& nodes = root->property<Nodes>(CommonProps::k_children);

    for (const auto& nd : nodes) {
      auto rec2 = node_traverse(nd, functor, depth + 1);
      if (rec2 == TraverseRecursion::k_break) {
        return rec2;
      }
    }
  }

  return rec;
}


namespace {
  std::ostream& escape(std::ostream& os, const std::string& str) {
    for (const auto c : str) {
      switch (c) {
      case '\n':
        os << "\\n";
        break;
      case '\r':
        os << "\\r";
        break;
      case '\t':
        os << "\\t";
        break;
      case '"':
        os << "\\\"";
        break;
      default:
        os << c;
      }
    }

    return os;
  }


  class JsPrinter
  {
    std::ostream& _os;
    bool _do_indent;

  public:
    JsPrinter(std::ostream& os, bool do_indent)
      : _os(os)
      , _do_indent(do_indent) {}

    const JsPrinter& newln() const {
      if (_do_indent) {
        _os << std::endl;
      }
      return *this;
    }

    const JsPrinter& indent(int depth, int ofs) const {
      if (_do_indent) {
        _os << std::string(depth * 4 + ofs * 2, ' ');
      }
      return *this;
    }

    const JsPrinter& sep() const {
      _os << ",";
      return newln();
    }

    const JsPrinter& open_obj() const {
      _os << "{";
      if (_do_indent) {
        _os << " ";
      }
      return *this;
    }

    const JsPrinter& close_obj() const {
      _os << "}";
      return *this;
    }

    const JsPrinter& open_array() const {
      _os << "[";
      return newln();
    }

    const JsPrinter& close_array() const {
      _os << "]";
      return *this;
    }

    const JsPrinter& empty_array() const {
      _os << "[]";
      return *this;
    }

    const JsPrinter& print_attrnm(const std::string& nm) const {
      _os << "\"" << nm << "\":";
      if (_do_indent) {
        _os << " ";
      }
      return *this;
    }

    const JsPrinter& print_value(const std::string& val) const {
      _os << "\"";
      escape(_os, val) << "\"";
      return *this;
    }

    const JsPrinter& print_value(int val) const {
      _os << val;
      return *this;
    }
  };


  void serialize(const JsPrinter& pp, const Node* nd, int depth) {
    struct SerializeVisitor
    {
      using return_type = void;

      JsPrinter _pp;
      int _depth;

      SerializeVisitor(const JsPrinter& pp, int depth)
        : _pp(pp)
        , _depth(depth) {}

      void operator()(const Undefined&) {}

      void operator()(const int& val) {
        _pp.print_value(val);
      }

      void operator()(const std::string& val) {
        _pp.print_value(val);
      }

      void operator()(const Node* nd) {
        _pp.newln();
        serialize(_pp, nd, _depth);
      }

      void operator()(const Nodes& nl) {
        if (!nl.empty()) {
          _pp.open_array();
          auto first = true;
          for (auto* nd : nl) {
            if (!first) {
              _pp.sep();
            }
            else {
              first = false;
            }
            serialize(_pp, nd, _depth);
          }
          _pp.newln().indent(_depth, -1).close_array();
        }
        else
          _pp.empty_array();
      }
    };

    pp.indent(depth, 0).open_obj().print_attrnm("type").print_value(nd->classname());

    if (nd->node_class() == element_class_definition()) {
      pp.sep().indent(depth, 1).print_attrnm("gi").print_value(nd->gi());
    }

    for (const auto& prop : nd->properties()) {
      if (prop.first != CommonProps::k_gi && prop.first != CommonProps::k_parent &&
          prop.first != CommonProps::k_auto_id) {
        pp.sep().indent(depth, 1).print_attrnm(prop.first);

        apply(SerializeVisitor(pp, depth + 1), prop.second);
      }
    }

    pp.newln().indent(depth, 0).close_obj();

    if (depth == 0) {
      pp.newln();
    }
  }

} // ns anon


void serialize(std::ostream& os, const Node* nd, bool pretty_printing, int depth) {
  auto pp = JsPrinter{os, pretty_printing};
  serialize(pp, nd, depth);
}


//------------------------------------------------------------------------------

std::string node_data(const Node* base_nd) {
  std::stringstream ss;

  node_traverse(base_nd,
                [&ss](const Node* nd, int /*depth*/) {
                  ss << nd->property<std::string>(CommonProps::k_data);

                  auto key = nd->property<std::string>(CommonProps::k_data_attr);
                  if (auto named_attr = nd->attribute(key)) {
                    ss << node_data(named_attr);
                  }
                  return TraverseRecursion::k_recurse;
                },
                0);

  return ss.str();
}


ConstNodes elements_with_id(const Grove* grove, const std::string& id) {
  auto nl_result = ConstNodes{};

  for (const auto& nd : grove->nodes()) {
    if (nd.get()) {
      if (effective_id(*nd.get()) == id) {
        nl_result.emplace_back(nd.get());
      }
    }
  }

  return nl_result;
}


Node* desc_element(const Node* nd) {
  using namespace std;

  const auto& nodes = nd->property<Nodes>(CommonProps::k_children);

  auto i_desc = find_if(begin(nodes), end(nodes), [&](const Node* child) {
    return child->gi() == CommonProps::k_desc;
  });
  return i_desc != end(nodes) ? *i_desc : nullptr;
}

} // ns eyestep
