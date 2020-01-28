#ifndef SQLPARSER_SELECT_STATEMENT_H
#define SQLPARSER_SELECT_STATEMENT_H

#include "SQLStatement.h"
#include "Expr.h"
#include "Table.h"
#include "boost/functional/hash.hpp"

#include <iostream>


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

      size_t hash() const {
          auto hash = boost::hash_value(type);
          boost::hash_combine(hash, expr->hash());
          return hash;
      }

      void extractValues(std::vector<Expr>& result) {
            if(expr) {
                expr->extractValues(result);
            }
      }


      };

  // Description of the limit clause within a select statement.
  struct LimitDescription {
    LimitDescription(Expr* limit, Expr* offset);
    virtual ~LimitDescription();

    Expr* limit;
    Expr* offset;

    size_t hash() const {
      auto hash = limit->hash();
      boost::hash_combine(hash, offset->hash());
      return hash;
    }

      void extractValues(std::vector<Expr>& result) {
          if(limit) {
              limit->extractValues(result);
          }
          if(offset) {
              offset->extractValues(result);
          }
      }
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

      void extractValues(std::vector<Expr>& result) {
          for (auto &x: *columns) {
              x->extractValues(result);
          }
          if(having) {
              having->extractValues(result);
          }
      }
  };

  struct WithDescription {
      ~WithDescription();

      char* alias;
      SelectStatement* select;

      size_t hash() const;

      void extractValues(std::vector<Expr>& result);

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
      if (unionSelect) {
        boost::hash_combine(hash, unionSelect->hash());
      }
      for (auto const& o : *order) {
          boost::hash_combine(hash, o->hash());
      }
      for (auto const& with : *withDescriptions) {
           boost::hash_combine(hash, with->hash());
      }
      if (limit){
          boost::hash_combine(hash, limit->hash());
      }
      return hash;
    }

    void extractValues(std::vector<Expr>& result) const {
        whereClause->extractValues(result);
        if (groupBy) {
            groupBy->extractValues(result);
        }
        if (unionSelect) {
            unionSelect->extractValues(result);
        }
        if(order != nullptr) {
            for (auto const& o : *order) {
                if(!o) {
                    std::cout << "Das ist sehr dreißt" << std::endl;
                }
                o->extractValues(result);
            }
        }
        if(withDescriptions != nullptr) {
            for (auto const& with : *withDescriptions) {
                if(!with) {
                    std::cout << "Das ist sehr dreißt 2" << std::endl;
                }
                with->extractValues(result);
            }
        }

        if (limit){
            limit->extractValues(result);
        }
    }
  };

  inline size_t WithDescription::hash() const {
    auto hash = boost::hash_value(std::string(alias));
    boost::hash_combine(hash, select->hash());
    return hash;
   }

   inline void WithDescription::extractValues(std::vector<Expr>& result) {
      select->extractValues(result);
  }


   } // namespace hsql

#endif
