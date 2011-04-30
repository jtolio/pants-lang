static inline void builtin_print(union Value* val, union Value* exception) {
  switch(val->t) {
    case INTEGER:
      printf("%lld", val->integer.value);
      break;
    case FLOAT:
      printf("%f", val->floating.value);
      break;
    case BOOLEAN:
      if(val->boolean.value) {
        printf("true");
      } else {
        printf("false");
      }
      break;
    case NIL:
      printf("null");
      break;
    case STRING:
      printf("%s", val->string.value);
      break;

    case CLOSURE:
      printf("<TODO: closure>");
      break;
    case OBJECT:
      printf("<TODO: object>");
      break;
    default:
      *exception = make_c_string("unknown type!");
      break;
  }
}

static inline bool builtin_less_than(union Value* val1, union Value* val2,
    union Value* exception) {
  switch(val1->t) {
    case INTEGER:
      switch(val2->t) {
        case INTEGER: return val1->integer.value < val2->integer.value;
        case FLOAT: return val1->integer.value < val2->floating.value;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          *exception = make_c_string("unknown type!");
          return false;
      }
    case FLOAT:
      switch(val2->t) {
        case INTEGER: return val1->floating.value < val2->integer.value;
        case FLOAT: return val1->floating.value < val2->floating.value;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          *exception = make_c_string("unknown type!");
          return false;
      }
    case BOOLEAN:
      switch(val2->t) {
        case INTEGER: return true;
        case FLOAT: return true;
        case BOOLEAN: return val1->boolean.value < val2->boolean.value;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          *exception = make_c_string("unknown type!");
          return false;
      }
    case NIL:
      switch(val2->t) {
        case INTEGER: return true;
        case FLOAT: return true;
        case BOOLEAN: return true;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          *exception = make_c_string("unknown type!");
          return false;
      }
    case CLOSURE:
      switch(val2->t) {
        case INTEGER: return false;
        case FLOAT: return false;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE:
          if(val1->closure.func < val2->closure.func) return true;
          if(val1->closure.func != val2->closure.func) return false;
          return val1->closure.env < val2->closure.env;
        case STRING: return false;
        case OBJECT: return false;
        default:
          *exception = make_c_string("unknown type!");
          return false;
      }
    case STRING:
      switch(val2->t) {
        case INTEGER: return false;
        case FLOAT: return false;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING:
          if(val1->string.byte_oriented < val2->string.byte_oriented)
              return true;
          if(val1->string.byte_oriented != val2->string.byte_oriented)
              return false;
          return safe_strcmp(val1->string.value, val1->string.value_size,
              val2->string.value, val2->string.value_size) < 0;
        case OBJECT: return true;
        default:
          *exception = make_c_string("unknown type!");
          return false;
      }
    case OBJECT:
      switch(val2->t) {
        case INTEGER: return false;
        case FLOAT: return false;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return false;
        case OBJECT: return val1->object.data < val2->object.data;
        default:
          *exception = make_c_string("unknown type!");
          return false;
      }
    default:
      *exception = make_c_string("unknown type!");
      return false;
  }
  return false;
}

static inline bool builtin_equals(union Value* val1, union Value* val2,
    union Value* exception) {
  if(!((val1->t == INTEGER && val2->t == FLOAT) ||
       (val1->t == FLOAT && val2->t == INTEGER)) &&
     val1->t != val2->t)
    return false;
  switch(val1->t) {
    case INTEGER:
      if(val2->t == INTEGER) {
        return val1->integer.value == val2->integer.value;
      } else {
        return val1->integer.value == val2->floating.value;
      }
    case FLOAT:
      if(val2->t == INTEGER) {
        return val1->floating.value == val2->integer.value;
      } else {
        return val1->floating.value == val2->floating.value;
      }
    case BOOLEAN:
      return val1->boolean.value == val2->boolean.value;
    case NIL:
      return true;
    case CLOSURE:
      return val1->closure.func == val2->closure.func &&
          val1->closure.env == val2->closure.env;
    case OBJECT:
      return val1->object.data == val2->object.data;
    case STRING:
      return val1->string.byte_oriented == val2->string.byte_oriented &&
          safe_strcmp(val1->string.value, val1->string.value_size,
              val1->string.value, val2->string.value_size) == 0;
    default:
      *exception = make_c_string("TODO: unimplemented");
      return false;
  }
}

static inline void builtin_add(union Value* val1, union Value* val2,
    union Value* rv, union Value* exception) {
  switch(val1->t) {
    case INTEGER:
      switch(val2->t) {
        case INTEGER:
          rv->integer.value = val1->integer.value + val2->integer.value;
          rv->t = INTEGER;
          return;
        case FLOAT:
          rv->floating.value = val1->integer.value + val2->floating.value;
          rv->t = FLOAT;
          return;
        case STRING:
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported addition!");
          rv->t = NIL;
          return;
      }
    case FLOAT:
      switch(val2->t) {
        case INTEGER:
          rv->floating.value = val1->floating.value + val2->integer.value;
          rv->t = FLOAT;
          return;
        case FLOAT:
          rv->floating.value = val1->floating.value + val2->floating.value;
          rv->t = FLOAT;
          return;
        case STRING:
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported addition!");
          rv->t = NIL;
          return;
      }
    case STRING:
      switch(val2->t) {
        case STRING:
          if(val1->string.byte_oriented != val2->string.byte_oriented) {
            *exception = make_c_string("TODO: unimplemented");
            rv->t = NIL;
            return;
          }
          unsigned int value_size;
          char* value;
          value_size = val1->string.value_size + val2->string.value_size;
          value = GC_MALLOC(sizeof(char) * value_size);
          memcpy(value, val1->string.value, val1->string.value_size);
          memcpy(value + val1->string.value_size, val2->string.value,
              val2->string.value_size);
          rv->string.value = value;
          rv->string.value_size = value_size;
          rv->t = STRING;
          rv->string.byte_oriented = val1->string.byte_oriented;
          return;
        case OBJECT:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
      }
    case OBJECT:
      *exception = make_c_string("TODO: unimplemented");
      rv->t = NIL;
      return;
    case BOOLEAN:
      switch(val2->t) {
        case STRING:
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported addition!");
          rv->t = NIL;
          return;
      }
    case NIL:
      switch(val2->t) {
        case STRING:
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported addition!");
          rv->t = NIL;
          return;
      }
    case CLOSURE:
      switch(val2->t) {
        case STRING:
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported addition!");
          rv->t = NIL;
          return;
      }
    default:
      *exception = make_c_string("unsupported addition!");
      rv->t = NIL;
      return;
  }
}

static inline bool builtin_istrue(union Value* val, union Value* exception) {
  switch(val->t) {
    case INTEGER:
    case FLOAT:
      return val->integer.value != 0;
    case BOOLEAN:
      return val->boolean.value;
    case NIL:
      return false;
    case STRING:
    case OBJECT:
    case CLOSURE:
      return true;
    default:
      *exception = make_c_string("unimplemented!");
      return false;
  }
}
