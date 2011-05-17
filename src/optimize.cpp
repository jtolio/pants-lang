#include "optimize.h"

using namespace cirth;

void cirth::optimize::ir(std::vector<PTR<ir::Expression> >& ir) {
  // TODO: here, we want to group as many definitions together as we can
}

class CallCompactorEV : public cps::ExpressionVisitor {
public:
  void visit(cps::VariableMutation* mut) { mut->next_expression->accept(this); }
  void visit(cps::ObjectMutation* mut) { mut->next_expression->accept(this); }
  void visit(cps::Call* call);
};

void CallCompactorEV::visit(cps::Call* call) {
  for(unsigned int i = 0; i < call->right_positional_args.size(); ++i) {
    cps::Callable* arg(dynamic_cast<cps::Callable*>(
        call->right_positional_args[i].get()));
    if(arg) arg->expression->accept(this);
  }
  if(call->continuation.get()) {
    cps::Callable* cont(dynamic_cast<cps::Callable*>(call->continuation.get()));
    if(cont) cont->expression->accept(this);
  }
  cps::Callable* call_cont(dynamic_cast<cps::Callable*>(call->callable.get()));
  if(!call_cont) return;
  call_cont->expression->accept(this);

  cps::Call* call_cont_call(dynamic_cast<cps::Call*>(
      call_cont->expression.get()));
  if(!call_cont_call) return;
  cps::Callable* call_cont_call_cont(dynamic_cast<cps::Callable*>(
      call_cont_call->callable.get()));
  if(!call_cont_call_cont) return;

  if(call_cont->function || call_cont_call_cont->function) {
    // TODO: don't bail here, actually think through the cases of what happens
    //  to CONTINUATION and HIDDEN_OBJECT.
    //  not really high priority though because currently the translation
    //  pipeline causes functions to be named before they are called, so we need
    //  to do some variable resolution before this matters.
    return;
  }

  // TODO: mash the two calls together here.
}

void cirth::optimize::cps(PTR<cps::Expression>& cps) {
  CallCompactorEV call_compactor;
  cps->accept(&call_compactor);
}
