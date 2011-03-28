#include "cps.h"

using namespace cirth;

std::string cps::Call::format() const {
  return "TODO";
}

std::string cps::Mutation::format() const {
  return "TODO";
}

std::string cps::Variable::format() const {
  return "TODO";
}

std::string cps::UserFunction::format() const {
  return "TODO";
}

std::string cps::Continuation::format() const {
  return "TODO";
}

std::string cps::SubExpression::format() const {
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
    ir::Mutation* mutation(dynamic_cast<ir::Mutation*>(in_ir[i-1].get()));
    if(mutation) {
      out_ir = PTR<cps::Mutation>(new cps::Mutation(
          trans(mutation->assignee),
          trans(mutation->value),
          out_ir));
      continue;
    }
    ir::ReturnValue* rv(dynamic_cast<ir::ReturnValue*>(in_ir[i-1].get()));
    if(rv) {
      PTR<cps::Call> call(new cps::Call);
      out_ir = call;
      call->function = trans(rv->term->function);
      call->left_positional_args.reserve(rv->term->left_positional_args.size());
      for(unsigned int i = 0; i < rv->term->left_positional_args.size(); ++i) {
        call->left_positional_args.push_back(cps::PositionalOutArgument(trans(
            rv->term->left_positional_args[i].variable)));
      }
      if(!!rv->term->left_arbitrary_arg) {
        call->left_arbitrary_arg = cps::ArbitraryOutArgument(trans(
            rv->term->left_arbitrary_arg.get().variable));
      }
      call->right_positional_args.reserve(
          rv->term->right_positional_args.size());
      for(unsigned int i = 0; i < rv->term->right_positional_args.size(); ++i) {
        call->right_positional_args.push_back(cps::PositionalOutArgument(trans(
            rv->term->right_positional_args[i].variable)));
      }
      call->right_optional_args.reserve(rv->term->right_optional_args.size());
      for(unsigned int i = 0; i < rv->term->right_optional_args.size(); ++i) {
        call->right_optional_args.push_back(cps::OptionalOutArgument(
            rv->term->right_optional_args[i].key,
            trans(rv->term->right_optional_args[i].variable)));
      }
      if(!!rv->term->right_arbitrary_arg) {
        call->right_arbitrary_arg = cps::ArbitraryOutArgument(trans(
            rv->term->right_arbitrary_arg.get().variable));
      }
      if(!!rv->term->right_keyword_arg) {
        call->right_keyword_arg = cps::KeywordOutArgument(trans(
            rv->term->right_keyword_arg.get().variable));
      }
      call->scoped_optional_args.reserve(rv->term->scoped_optional_args.size());
      for(unsigned int i = 0; i < rv->term->scoped_optional_args.size(); ++i) {
        call->scoped_optional_args.push_back(cps::OptionalOutArgument(
            rv->term->scoped_optional_args[i].key,
            trans(rv->term->scoped_optional_args[i].variable)));
      }
      if(!!rv->term->scoped_keyword_arg) {
        call->scoped_keyword_arg = cps::KeywordOutArgument(trans(
            rv->term->scoped_keyword_arg.get().variable));
      }
      PTR<cps::Continuation> continuation(new cps::Continuation);
      continuation->expression = out_ir;
      throw expectation_failure("TODO");
//      continuation->left_positional_args.push_back(cps::PositionalInArgument(
//          rv->assignee
//       ));
      call->continuation = continuation;
      continue;
    }
    ir::Definition* definition(dynamic_cast<ir::Definition*>(in_ir[i-1].get()));
    if(definition) {
      throw expectation_failure("TODO");
      continue;
    }
   throw expectation_failure("unknown IR assignment type!");
  }
}
