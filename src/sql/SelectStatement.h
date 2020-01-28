#ifndef SQLPARSER_SELECT_STATEMENT_H
#define SQLPARSER_SELECT_STATEMENT_H

#include "SQLStatement.h"
#include "Expr.h"
#include "Table.h"
#include "boost/functional/hash.hpp"


namespace hsql {
  enum OrderType {
    kOrderAsc,
    kOrderDesc
  };


  // Description of the order by clause within a select statement.
  struct OrderDescription {
    OrderDescription(OrderType type, Expr* expr);
    virtual ~OrderDescription();

    OrderType type;
    Expr* expr;
  };

  // Description of the limit clause within a select statement.
  struct LimitDescription {
    LimitDescription(Expr* limit, Expr* offset);
    virtual ~LimitDescription();

    Expr* limit;
    Expr* offset;
  };

  // Description of the group-by clause within a select statement.
  struct GroupByDescription {
    GroupByDescription();
    virtual ~GroupByDescription();

    std::vector<Expr*>* columns;
    Expr* having;

    size_t hash() const {
      auto hash = having->hash();
      for (auto &x: *columns) {
        boost::hash_combine(hash, x->hash());
      }
      return hash;
    }
  };

  struct WithDescription {
      ~WithDescription();

      char* alias;
      SelectStatement* select;
  };

  // Representation of a full SQL select statement.
  // TODO: add union_order and union_limit.
  struct SelectStatement : SQLStatement {
    SelectStatement();
    virtual ~SelectStatement();

    TableRef* fromTable;
    bool selectDistinct;
    std::vector<Expr*>* selectList;
    Expr* whereClause;
    GroupByDescription* groupBy;

    SelectStatement* unionSelect;
    std::vector<OrderDescription*>* order;
    std::vector<WithDescription*>* withDescriptions;
    LimitDescription* limit;

    size_t hash() const {
      auto hash = boost::hash_value(std::string(fromTable->name));
      boost::hash_combine(hash, selectDistinct);
      boost::hash_combine(hash, whereClause->hash());
      if (groupBy) {
        boost::hash_combine(hash, groupBy->hash());
      }
      /*if (unionSelect) {
        boost::hash_combine(hash, groupBy->hash());
      }
      for ()
      if (WithDescriptions)*/
      return hash;
    }
  };


} // namespace hsql

#endif
