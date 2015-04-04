// Copyright (c) 2015 Gregor Klinke
// All rights reserved.

#pragma once

#include "nodes.hpp"

#include <boost/filesystem.hpp>
#include <boost/optional/optional.hpp>

#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>


namespace eyestep {

namespace fs = boost::filesystem;

class Node;
class Grove;

namespace textbook {

  class TagSpec;
  class DocSpec;

  class ParseException : public std::exception {
    std::string _msg;

  public:
    ParseException(const std::string& srcpos, const std::string& msg)
      : _msg(srcpos + ": " + msg)
    {
    }

    const char* what() const noexcept override { return _msg.c_str(); }
  };


  class GroveBuilder {
    std::string _current_tag;
    Node* _current_node;
    Node* _root_node;
    std::list<std::tuple<std::string, Node*>> _node_stack;

  public:
    GroveBuilder(Node* rootnd);

    void add_node(Node* nd);
    void push_node(const std::string& tag, Node* nd);
    void pop_node(const std::string& tag);
    Node* root_node();
  };


  class Stream {
    std::vector<char> _unread_buffer;
    size_t _unread_nc;
    char _current_c;
    size_t _nc;
    size_t _line_no;
    fs::path _fpath;
    std::string _data;

  public:
    Stream(boost::optional<std::string> data, boost::optional<fs::path> path,
           size_t start_line_no = 0);
    char current_c() const;
    size_t line_no() const;
    fs::path fpath() const;
    std::string srcpos() const;
    char read();
    char unread(const std::string& buf);
    bool eof() const;
  };


  using VariableEnv = std::map<std::string, std::string>;
  using Catalog = std::unordered_map<std::string, std::unique_ptr<DocSpec>>;

  class Parser {
    bool _verbose;
    std::vector<fs::path> _catalog_path;
    std::shared_ptr<Stream> _stream;
    std::list<std::shared_ptr<Stream>> _stream_stack;
    std::stringstream _text;
    eyestep::Grove& _grove;
    GroveBuilder& _grovebuilder;
    VariableEnv& _vars;
    Catalog& _catalog;
    DocSpec* _docspec;
    std::string _doctype;
    Node* _current_p_nd;
    bool _is_mixed_content;
    std::list<bool> _is_mixed_content_stack;

  public:
    Parser(eyestep::Grove& grove, GroveBuilder& grovebuilder,
           VariableEnv& _vars, Catalog& catalog, DocSpec* docspec,
           const std::vector<fs::path>& catalog_path,
           bool is_mixed_content = false, bool is_verbose = false);

    std::string doctype() const;

    Node* parse_file(const fs::path& fpath);
    Node* parse_string(const std::string& buf, size_t start_line_no = 0);
    Node* parse_stream(std::shared_ptr<Stream> stream);

  private:
    using Args = std::vector<std::string>;
    using AttrsAndId = std::tuple<Nodes, boost::optional<std::string>>;

    void push_stream(std::shared_ptr<Stream> stream);
    void pop_stream();
    bool eof();
    char nextc();
    char unread(const std::string& buf);
    char currentc();
    size_t lineno();
    std::string srcpos();

    void push_p_nd();
    void push_text();
    void finish_paragraph();
    std::string skip_ws();
    void skip_until_eol();
    std::string parse_symbol();
    Args parse_args();
    AttrsAndId args2nl(const Args& args, const TagSpec& tagspec,
                       size_t lineno_at_start);
    void parse_at_form();
    void parse_at();
    void parse_openbrace();
    void parse_closebrace();
    void parse_varlookup();
    void parse_endtag();
    void parse_let();
    fs::path path_for_include(const fs::path& fpath);
    void parse_include();
    void parse_tag_with_params(const std::string& tag);
    void parse_tag();

    void set_docspec_by_root_tag(const std::string& tag);
    void check_tag_args(const Args& args, const TagSpec& tagspec);

    void end_tag(const std::string& endtag);
  };

} // ns textbook
} // ns eyestep