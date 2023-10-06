/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai 2023/6/27
//

#pragma once

#include "sql/parser/date.h"
#include <compare>
#include <limits>
#include <memory>
#include <set>
#include <string>
#include <vector>

const int INVALID_COMPARE = std::numeric_limits<int>::min();
/**
 * @brief 属性的类型
 * 
 */
enum AttrType {
  UNDEFINED,
  CHARS,    ///< 字符串类型
  INTS,     ///< 整数类型(4字节)
  DATES,    ///< 日期类型(4字节)
  FLOATS,   ///< 浮点数类型(4字节)
  NULLS,    ///< NULL
  LISTS,    ///< 多行数据
  BOOLEANS, ///< boolean类型，当前不是由parser解析出来的，是程序内部使用的
};

int attr_type_to_size(AttrType type);
const char *attr_type_to_string(AttrType type);
AttrType attr_type_from_string(const char *s);

class ValueList;

/**
 * @brief 属性的值
 * 
 */
class Value {
public:
  Value() = default;

  Value(AttrType attr_type, char *data, int length = 4) : attr_type_(attr_type) { this->set_data(data, length); }

  explicit Value(int val);
  explicit Value(float val);
  explicit Value(bool val);
  explicit Value(const char *s, int len = 0);
  explicit Value(Date date);
  explicit Value(std::set<ValueList> &list);

  Value(const Value &other) = default;
  Value &operator=(const Value &other) = default;

  void set_type(AttrType type) { this->attr_type_ = type; }
  void set_data(char *data, int length);
  void set_data(const char *data, int length) { this->set_data(const_cast<char *>(data), length); }
  void set_int(int val);
  void set_float(float val);
  void set_boolean(bool val);
  void set_string(const char *s, int len = 0);
  void set_date(Date date);
  void set_value(const Value &value);
  void set_null();
  void set_list(const std::set<ValueList> &list);

  std::string to_string() const;

  int compare(const Value &other) const;

  const char *data() const;
  int length() const { return length_; }

  AttrType attr_type() const { return attr_type_; }

  std::strong_ordering operator<=>(const Value &value) const;

public:
  /**
   * 获取对应的值
   * 如果当前的类型与期望获取的类型不符，就会执行转换操作
   */
  int get_int() const;
  float get_float() const;
  std::string get_string() const;
  bool get_boolean() const;
  Date get_date() const;
  std::shared_ptr<std::set<ValueList>> get_list() const;
  char* get_fiexed_string() const;

public:
  /**
   * 语法分析层面是否可以将from转换为to
   */
  static bool convert(AttrType from, AttrType to, Value &value);

  static bool check_value(const Value &v);

private:
  AttrType attr_type_ = UNDEFINED;
  int length_ = 0;

  union {
    int int_value_;
    float float_value_;
    bool bool_value_;
    Date date_value_;
    char str_value_[4];
  } num_value_;
  std::string str_value_;
  std::shared_ptr<std::set<ValueList>> list_value_;
};

AttrType AttrTypeCompare(AttrType a, AttrType b);

class ValueList {
public:
  ValueList(const Value &v) : list_(1, v) {}
  std::vector<Value> &get_list() { return list_; }
  const std::vector<Value> &get_list() const { return list_; }

  std::string to_string() const;

  std::strong_ordering operator<=>(const ValueList &other) const;

private:
  std::vector<Value> list_;
};
