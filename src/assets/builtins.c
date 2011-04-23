static inline void builtin_print(union Value val) {
  switch(val.t) {
    case INTEGER:
      printf("%lld", val.integer.value);
      break;
    case FLOAT:
      printf("%f", val.floating.value);
      break;
    case BOOLEAN:
      if(val.boolean.value) {
        printf("true");
      } else {
        printf("false");
      }
      break;
    case NIL:
      printf("null");
      break;
    case STRING:
      printf("%s", val.string.value);
      breakl

    case CLOSURE:
    case OBJECT:
    default:
      printf("<TODO: unimplemented>");
      break;
  }
}

static inline bool builtin_less_than(union Value val1, union Value val2) {
  switch(val1.t) {
    case INTEGER:
      switch(val2.t) {
        case INTEGER: return val1.integer.value < val2.integer.value;
        case FLOAT: return val1.integer.value < val2.floating.value;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case FLOAT:
      switch(val2.t) {
        case INTEGER: return val1.floating.value < val2.integer.value;
        case FLOAT: return val1.floating.value < val2.floating.value;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case BOOLEAN:
      switch(val2.t) {
        case INTEGER: return true;
        case FLOAT: return true;
        case BOOLEAN: return val1.boolean.value < val2.boolean.value;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case NIL:
      switch(val2.t) {
        case INTEGER: return true;
        case FLOAT: return true;
        case BOOLEAN: return true;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return true;
        case OBJECT: return true;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case CLOSURE:
      switch(val2.t) {
        case INTEGER: return false;
        case FLOAT: return false;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE:
          if(val1.closure.func < val2.closure.func) return true;
          if(val1.closure.func != val2.closure.func) return false;
          return val1.closure.env < val2.closure.env;
        case STRING: return false;
        case OBJECT: return false;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case STRING:
      switch(val2.t) {
        case INTEGER: return false;
        case FLOAT: return false;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING:
          if(val1.string.byte_oriented < val2.string.byte_oriented) return true;
          if(val1.string.byte_oriented != val2.string.byte_oriented)
              return false;
          return safe_strcmp(val1.string.value, val1.string.value_size,
              val2.string.value, val2.string.value_size) < 0;
        case OBJECT: return true;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    case OBJECT:
      switch(val2.t) {
        case INTEGER: return false;
        case FLOAT: return false;
        case BOOLEAN: return false;
        case NIL: return false;
        case CLOSURE: return true;
        case STRING: return false;
        case OBJECT: return val1.object.data < val2.object.data;
        default:
          printf("unknown type!\n");
          exit(1);
      }
    default:
      printf("unknown type!\n");
      exit(1);
  }
  return false;
}

static inline bool builtin_equals(union Value val1, union Value val2) {
  if(!((val1.t == INTEGER && val2.t == FLOAT) ||
       (val1.t == FLOAT && val2.t == INTEGER)) &&
     val1.t != val2.t)
    return false;
  switch(val1.t) {
    case INTEGER:
      if(val2.t == INTEGER) {
        return val1.integer.value == val2.integer.value;
      } else {
        return val1.integer.value == val2.floating.value;
      }
    case FLOAT:
      if(val2.t == INTEGER) {
        return val1.floating.value == val2.integer.value;
      } else {
        return val1.floating.value == val2.floating.value;
      }
    case BOOLEAN:
      return val1.boolean.value == val2.boolean.value;
    case NIL:
      return true;
    case CLOSURE:
      return val1.closure.func == val2.closure.func &&
          val1.closure.env == val2.closure.env;
    case OBJECT:
      return val1.object.data == val2.object.data;
    case STRING:
      return val1.string.byte_oriented == val2.string.byte_oriented &&
          safe_strcmp(val1.string.value, val1.string.value_size,
              val1.string.value, val2.string.value_size) == 0;
    default:
      printf("TODO: unimplemented\n");
      exit(1);
      return false;
  }
}

static inline union Value builtin_add(union Value val1, union Value val2) {
  union Value rv;
  rv.t = NIL;
  switch(val1.t) {
    case INTEGER:
      switch(val2.t) {
        case INTEGER:
          rv.t = INTEGER;
          rv.integer.value = val1.integer.value + val2.integer.value;
          return rv;
        case FLOAT:
          rv.t = FLOAT;
          rv.floating.value = val1.integer.value + val2.floating.value;
          return rv;
        case STRING:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case OBJECT:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("unsupported addition!\n");
          exit(1);
          return rv;
      }
    case FLOAT:
      switch(val2.t) {
        case INTEGER:
          rv.t = FLOAT;
          rv.floating.value = val1.floating.value + val2.integer.value;
          return rv;
        case FLOAT:
          rv.t = FLOAT;
          rv.floating.value = val1.floating.value + val2.floating.value;
          return rv;
        case STRING:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case OBJECT:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("unsupported addition!\n");
          exit(1);
          return rv;
      }
    case STRING:
      switch(val2.t) {
        case STRING:
          rv.t = STRING;
          if(val1.string.byte_oriented != val2.string.byte_oriented) {
            printf("TODO: unimplemented\n");
            exit(1);
          }
          rv.string.byte_oriented = val1.string.byte_oriented;
          rv.string.value_size = val1.string.value_size +
              val2.string.value_size;
          rv.string.value = GC_MALLOC(sizeof(char) * rv.string.value_size);
          memcpy(rv.string.value, val1.string.value, val1.string.value_size);
          memcpy(rv.string.value + val1.string.value, val2.string.value,
              val2.string.value_size);
          return rv;
        case OBJECT:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
      }
    case OBJECT:
      printf("TODO: unimplemented\n");
      exit(1);
      return rv;
    case BOOLEAN:
      switch(val2.t) {
        case STRING:
        case OBJECT:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("unsupported addition!\n");
          exit(1);
          return rv;
      }
    case NIL:
      switch(val2.t) {
        case STRING:
        case OBJECT:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("unsupported addition!\n");
          exit(1);
          return rv;
      }
    case CLOSURE:
      switch(val2.t) {
        case STRING:
        case OBJECT:
          printf("TODO: unimplemented\n");
          exit(1);
          return rv;
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          printf("unsupported addition!\n");
          exit(1);
          return rv;
      }
    default:
      printf("unsupported addition!\n");
      exit(1);
      return rv;
  }
}

static inline bool builtin_istrue(union Value val) {
  switch(val.t) {
    case INTEGER:
    case FLOAT:
      return val.integer.value != 0;
    case BOOLEAN:
      return val.boolean.value;
    case NIL:
      return false;
    case STRING:
    case OBJECT:
    case CLOSURE:
      return true;
    default:
      printf("unimplemented!\n");
      exit(1);
  }
}
