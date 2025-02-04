#pragma once

#include "sql/expr/expression.h"
#include "sql/parser/parse_defs.h"
#include "storage/table/table.h"
class JoinStmt {
public:
  Table *&table() { return table_; }
  std::unique_ptr<JoinStmt> &sub_join() { return sub_join_; }
  std::unique_ptr<ConjunctionExpr> &condition() { return condition_; }

  static RC create(Db *db, JoinSqlNode *sql_node, JoinStmt *&stmt, std::vector<Table *> &tables,
                   std::unordered_map<std::string, Table *> &table_map);

private:
  Table *table_ = nullptr;
  std::unique_ptr<JoinStmt> sub_join_;
  std::unique_ptr<ConjunctionExpr> condition_;
};
