#include "optimize.h"

using namespace cirth;

void cirth::optimize::ir(std::vector<PTR<ir::Expression> >& ir) {
  // TODO: here, we want to group as many definitions together as we can
}

class CallCompactorEV : public cps::ExpressionVisitor {
public:
  CallCompactorEV(unsigned int* count) : m_count(count) {}
  void visit(cps::VariableMutation* mut) { mut->next_expression->accept(this); }
  void visit(cps::ObjectMutation* mut) { mut->next_expression->accept(this); }
  void visit(cps::Call* call);
private:
  unsigned int* m_count;
};

void CallCompactorEV::visit(cps::Call* outer_call) {
  for(unsigned int i = 0; i < outer_call->right_positional_args.size(); ++i) {
    cps::Callable* arg(dynamic_cast<cps::Callable*>(
        outer_call->right_positional_args[i].get()));
    if(arg) arg->expression->accept(this);
  }
  if(outer_call->continuation.get()) {
    cps::Callable* cont(dynamic_cast<cps::Callable*>(
        outer_call->continuation.get()));
    if(cont) cont->expression->accept(this);
  }
  cps::Callable* outer_cont(dynamic_cast<cps::Callable*>(
      outer_call->callable.get()));
  if(!outer_cont) return;
  outer_cont->expression->accept(this);

  cps::Call* inner_call(dynamic_cast<cps::Call*>(
      outer_cont->expression.get()));
  if(!inner_call) return;
  cps::Callable* inner_cont(dynamic_cast<cps::Callable*>(
      inner_call->callable.get()));
  if(!inner_cont) return;

#define CHECK(c) \
  if(c->left_positional_args.size() > 0 || !!c->left_arbitrary_arg || \
      c->right_optional_args.size() > 0 || !!c->right_arbitrary_arg || \
      !!c->right_keyword_arg) \
    return;

#define CHECK_CALL(c) \
  CHECK(c) \
  if(c->hidden_object_optional_args.size() > 0 || c->continuation.get()) \
    return;

#define CHECK_CONT(c) \
  CHECK(c) \
  if(c->function) return;

  // TODO: don't bail on all these cases here.
  //   not really high priority because the translation pipeline doesn't
  //   generate many.

  CHECK_CALL(outer_call)    CHECK_CONT(outer_cont)
  CHECK_CALL(inner_call)    CHECK_CONT(outer_cont)

#undef CHECK_CALL
#undef CHECK_CONT
#undef CHECK

  if(outer_call->right_positional_args.size() !=
      outer_cont->right_positional_args.size())
    throw expectation_failure("mismatched argument count");
  if(inner_call->right_positional_args.size() !=
      inner_cont->right_positional_args.size())
    throw expectation_failure("mismatched argument count");

  PTR<cps::Call> new_call(new cps::Call);
  PTR<cps::Callable> new_cont(new cps::Callable(false));

  *new_call = *inner_call;
  *new_cont = *inner_cont;
  new_call->callable = new_cont;

  std::set<cps::Name> inner_call_free_names;
  inner_call->free_names(inner_call_free_names);
  for(unsigned int i = 0; i < outer_cont->right_positional_args.size(); ++i) {
    if(inner_call_free_names.find(outer_cont->right_positional_args[i]) ==
        inner_call_free_names.end())
      continue;
    cps::Name name(outer_cont->right_positional_args[i]);
    PTR<cps::Value> outer_val(outer_call->right_positional_args[i]);

    bool can_add_easily = true;
    for(unsigned int j = 0; j < inner_call->right_positional_args.size(); ++j) {
      std::set<cps::Name> arg_free_names;
      inner_call->right_positional_args[j]->free_names(arg_free_names);
      if(arg_free_names.find(name) != arg_free_names.end()) {
        can_add_easily = false;
        break;
      }
    }

    if(!can_add_easily) {
      // TODO: don't bail here. there's a number of strategies we could take
      //  to still work here.
      return;
    }

    if(can_add_easily) {
      new_call->right_positional_args.push_back(outer_val);
      new_cont->right_positional_args.push_back(name);
    }

  }

  *outer_call = *new_call;
  ++*m_count;
}

//#include <iostream>

void cirth::optimize::cps(PTR<cps::Expression>& cps) {
  unsigned int compaction_count = 0;
  CallCompactorEV call_compactor(&compaction_count);
  cps->accept(&call_compactor);
//  std::cerr << compaction_count << " compactions" << std::endl;
}
