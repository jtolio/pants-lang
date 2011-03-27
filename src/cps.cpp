#include "cps.h"

using namespace cirth;

std::string cps::Call::format() const {
  return "TODO";
}

std::string cps::Assignment::format() const {
  return "TODO";
}

std::string cps::Variable::format() const {
  return "TODO";
}

static PTR<cps::Value> trans(const PTR<ir::Value>& val) {
  throw expectation_failure("TODO");
  return PTR<cps::Value>();
}

static PTR<cps::Assignee> trans(const PTR<ir::Assignee>& assignee) {
  throw expectation_failure("TODO");
  return PTR<cps::Assignee>();
}

void cps::transform(const std::vector<PTR<ir::Expression> >& in_ir,
    const PTR<ir::Value>& in_lastval, PTR<cps::Expression>& out_ir) {
  PTR<cps::Value> continuation = PTR<cps::Value>(
      new cps::Variable(cps::Name("halt", false, false)));
  PTR<cps::Value> exception = PTR<cps::Value>(
      new cps::Variable(cps::Name("catch", false, false)));

  PTR<cps::Call> call(new cps::Call);
  call->function = continuation;
  call->right_positional_args.push_back(cps::PositionalOutArgument(trans(
      in_lastval)));
  out_ir = call;

  for(unsigned int i = in_ir.size(); i > 0; --i) {
    ir::ReturnValue* rv(dynamic_cast<ir::ReturnValue*>(in_ir[i-1].get()));
    if(rv) {
      throw expectation_failure("TODO");
      continue;
    }
    ir::Assignment* assignment(dynamic_cast<ir::Assignment*>(in_ir[i-1].get()));
    if(assignment) {
      out_ir = PTR<cps::Assignment>(new cps::Assignment(
          trans(assignment->assignee),
          trans(assignment->value),
          assignment->mutation,
          out_ir));
      continue;
    }
    throw expectation_failure("unknown IR assignment type!");
  }
}
