static bool array_user_size(void* array, struct Array* ra, struct Array* la,
    union Value* dest) {
  if(ra->size != 0 || la->size != 0) {
    *dest = make_c_string("expected 0 arguments");
    return false;
  }
  dest->t = INTEGER;
  dest->integer.value = ((struct Array*)array)->size;
  return true;
}

static bool array_user_update(void* array, struct Array* ra, struct Array* la,
    union Value* dest) {
  if(ra->size != 2 || la->size != 0) {
    *dest = make_c_string("expected 2 right arguments");
    return false;
  }
  if(ra->data[0].t != INTEGER) {
    *dest = make_c_string("array indexing must be done with an integer");
    return false;
  }
  if(ra->data[0].integer.value < 0)
    ra->data[0].integer.value += ((struct Array*)array)->size;
  if(ra->data[0].integer.value >= ((struct Array*)array)->size ||
      ra->data[0].integer.value < 0) {
    *dest = make_c_string("array index out of bounds!");
    return false;
  }
  ((struct Array*)array)->data[ra->data[0].integer.value] = ra->data[1];
  *dest = ra->data[1];
  return true;
}

static bool array_user_index(void* array, struct Array* ra, struct Array* la,
    union Value* dest) {
  if(ra->size != 1 || la->size != 0) {
    *dest = make_c_string("expected 1 right argument");
    return false;
  }
  if(ra->data[0].t != INTEGER) {
    *dest = make_c_string("array indexing must be done with an integer");
    return false;
  }
  if(ra->data[0].integer.value < 0)
    ra->data[0].integer.value += ((struct Array*)array)->size;
  if(ra->data[0].integer.value >= ((struct Array*)array)->size ||
      ra->data[0].integer.value < 0) {
    *dest = make_c_string("array index out of bounds!");
    return false;
  }
  *dest = ((struct Array*)array)->data[ra->data[0].integer.value];
  return true;
}

static bool array_user_append(void* array, struct Array* ra, struct Array* la,
    union Value* dest) {
  append_values(array, la->data, la->size);
  append_values(array, ra->data, ra->size);
  dest->t = NIL;
  return true;
}

static bool array_user_pop(void* array, struct Array* ra, struct Array* la,
    union Value* dest) {
  unsigned int i = ((struct Array*)array)->size;
  if(ra->size != 0 || la->size != 0) {
    *dest = make_c_string("expected 0 arguments");
    return false;
  }
  if(i == 0) {
    dest->t = NIL;
  } else {
    ((struct Array*)array)->size = --i;
    *dest = ((struct Array*)array)->data[i];
  }
  return true;
}

static bool array_user_shift(void* array, struct Array* ra, struct Array* la,
    union Value* dest) {
  if(ra->size != 0 || la->size != 0) {
    *dest = make_c_string("expected 0 arguments");
    return false;
  }
  if(((struct Array*)array)->size == 0) {
    dest->t = NIL;
  } else {
    *dest = ((struct Array*)array)->data[0];
    shift_values(array, -1);
  }
  return true;
}

static bool array_user_unshift(void* array, struct Array* ra, struct Array* la,
    union Value* dest) {
  unsigned int i = 0;
  shift_values(array, la->size + ra->size);
  for(i = 0; i < la->size; ++i)
      ((struct Array*)array)->data[i] = la->data[i];
  for(i = 0; i < ra->size; ++i)
      ((struct Array*)array)->data[i + la->size] = ra->data[i];
  dest->t = NIL;
  return true;
}

static inline void make_array_object(union Value* v, struct Array** array) {
  *array = make_array();

  make_object(v);

  set_field(v->object.data, (struct ByteArray){"u__7etype", 9},
      (union Value){.closure = (struct Closure){CLOSURE,
      ARRAY_CONSTRUCTOR_LABEL, NULL, NULL}});
  set_field(v->object.data, (struct ByteArray){"u_size", 6},
      make_external_closure(&array_user_size, *array));
  set_field(v->object.data, (struct ByteArray){"u_append", 8},
      make_external_closure(&array_user_append, *array));
  set_field(v->object.data, (struct ByteArray){"u_pop", 5},
      make_external_closure(&array_user_pop, *array));
  set_field(v->object.data, (struct ByteArray){"u_shift", 7},
      make_external_closure(&array_user_shift, *array));
  set_field(v->object.data, (struct ByteArray){"u_unshift", 9},
      make_external_closure(&array_user_unshift, *array));
  set_field(v->object.data, (struct ByteArray){"u__7eupdate", 11},
      make_external_closure(&array_user_update, *array));
  set_field(v->object.data, (struct ByteArray){"u__7eindex", 10},
      make_external_closure(&array_user_index, *array));

  seal_object(v->object.data);
}

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
      printf("%s", val->string.value.data);
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

static inline void builtin_readln(union Value* val, union Value* exception) {
  int errsv = 0;
  val->t = STRING;
  val->string.byte_oriented = true;
  val->string.value.data = GC_MALLOC(MAX_C_STRING_SIZE);
  if(fgets(val->string.value.data, MAX_C_STRING_SIZE, stdin) != NULL) {
    val->string.value.size = strlen(val->string.value.data);
    return;
  }
  errsv = errno;
  if(feof(stdin)) {
    val->t = NIL;
    return;
  }
  strerror_r(errsv, val->string.value.data, MAX_C_STRING_SIZE);
  *exception = *val;
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
          if(val1->closure.env < val2->closure.env) return true;
          if(val1->closure.env != val2->closure.env) return false;
          return val1->closure.frame < val2->closure.frame;
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
          return safe_strcmp(val1->string.value, val2->string.value) < 0;
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
          val1->closure.env == val2->closure.env &&
          val1->closure.frame == val2->closure.frame;
    case OBJECT:
      return val1->object.data == val2->object.data;
    case STRING:
      return val1->string.byte_oriented == val2->string.byte_oriented &&
          safe_strcmp(val1->string.value, val2->string.value) == 0;
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
          value_size = val1->string.value.size + val2->string.value.size;
          value = GC_MALLOC(sizeof(char) * value_size);
          memcpy(value, val1->string.value.data, val1->string.value.size);
          memcpy(value + val1->string.value.size, val2->string.value.data,
              val2->string.value.size);
          rv->string.value.data = value;
          rv->string.value.size = value_size;
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

static inline void builtin_multiply(union Value* val1, union Value* val2,
    union Value* rv, union Value* exception) {
  switch(val1->t) {
    case INTEGER:
      switch(val2->t) {
        case INTEGER:
          rv->integer.value = val1->integer.value * val2->integer.value;
          rv->t = INTEGER;
          return;
        case FLOAT:
          rv->floating.value = val1->integer.value * val2->floating.value;
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
          *exception = make_c_string("unsupported multiplication!");
          rv->t = NIL;
          return;
      }
    case FLOAT:
      switch(val2->t) {
        case INTEGER:
          rv->floating.value = val1->floating.value * val2->integer.value;
          rv->t = FLOAT;
          return;
        case FLOAT:
          rv->floating.value = val1->floating.value * val2->floating.value;
          rv->t = FLOAT;
          return;
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported multiplication!");
          rv->t = NIL;
          return;
      }
    case STRING:
      switch(val2->t) {
        case OBJECT:
        case INTEGER:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported multiplication!");
          rv->t = NIL;
          return;
      }
    case OBJECT:
      *exception = make_c_string("TODO: unimplemented");
      rv->t = NIL;
      return;
    case BOOLEAN:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported multiplication!");
          rv->t = NIL;
          return;
      }
    case NIL:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported multiplication!");
          rv->t = NIL;
          return;
      }
    case CLOSURE:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported multiplication!");
          rv->t = NIL;
          return;
      }
    default:
      *exception = make_c_string("unsupported multiplication!");
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

static inline void builtin_subtract(union Value* val1, union Value* val2,
    union Value* rv, union Value* exception) {
  switch(val1->t) {
    case INTEGER:
      switch(val2->t) {
        case INTEGER:
          rv->integer.value = val1->integer.value - val2->integer.value;
          rv->t = INTEGER;
          return;
        case FLOAT:
          rv->floating.value = val1->integer.value - val2->floating.value;
          rv->t = FLOAT;
          return;
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported subtraction!");
          rv->t = NIL;
          return;
      }
    case FLOAT:
      switch(val2->t) {
        case INTEGER:
          rv->floating.value = val1->floating.value - val2->integer.value;
          rv->t = FLOAT;
          return;
        case FLOAT:
          rv->floating.value = val1->floating.value - val2->floating.value;
          rv->t = FLOAT;
          return;
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported subtraction!");
          rv->t = NIL;
          return;
      }
    case STRING:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case INTEGER:
        case STRING:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported subtraction!");
          rv->t = NIL;
          return;
      }
    case OBJECT:
      *exception = make_c_string("TODO: unimplemented");
      rv->t = NIL;
      return;
    case BOOLEAN:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported subtraction!");
          rv->t = NIL;
          return;
      }
    case NIL:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported subtraction!");
          rv->t = NIL;
          return;
      }
    case CLOSURE:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported subtraction!");
          rv->t = NIL;
          return;
      }
    default:
      *exception = make_c_string("unsupported subtraction!");
      rv->t = NIL;
      return;
  }
}

static inline void builtin_divide(union Value* val1, union Value* val2,
    union Value* rv, union Value* exception) {
  switch(val1->t) {
    case INTEGER:
      switch(val2->t) {
        case INTEGER:
          if(val1->integer.value % val2->integer.value == 0) {
            rv->integer.value = val1->integer.value / val2->integer.value;
            rv->t = INTEGER;
            return;
          }
          rv->floating.value = ((double)val1->integer.value) /
              val2->integer.value;
          rv->t = FLOAT;
          return;
        case FLOAT:
          rv->floating.value = val1->integer.value / val2->floating.value;
          rv->t = FLOAT;
          return;
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported division!");
          rv->t = NIL;
          return;
      }
    case FLOAT:
      switch(val2->t) {
        case INTEGER:
          rv->floating.value = val1->floating.value / val2->integer.value;
          rv->t = FLOAT;
          return;
        case FLOAT:
          rv->floating.value = val1->floating.value / val2->floating.value;
          rv->t = FLOAT;
          return;
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported division!");
          rv->t = NIL;
          return;
      }
    case STRING:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case INTEGER:
        case STRING:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported division!");
          rv->t = NIL;
          return;
      }
    case OBJECT:
      *exception = make_c_string("TODO: unimplemented");
      rv->t = NIL;
      return;
    case BOOLEAN:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported division!");
          rv->t = NIL;
          return;
      }
    case NIL:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported division!");
          rv->t = NIL;
          return;
      }
    case CLOSURE:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported division!");
          rv->t = NIL;
          return;
      }
    default:
      *exception = make_c_string("unsupported division!");
      rv->t = NIL;
      return;
  }
}

static inline void builtin_modulo(union Value* val1, union Value* val2,
    union Value* rv, union Value* exception) {
  switch(val1->t) {
    case INTEGER:
      switch(val2->t) {
        case INTEGER:
          rv->integer.value = val1->integer.value % val2->integer.value;
          rv->t = INTEGER;
          return;
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case FLOAT:
        case STRING:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported modulo!");
          rv->t = NIL;
          return;
      }
    case FLOAT:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case INTEGER:
        case FLOAT:
        case STRING:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported modulo!");
          rv->t = NIL;
          return;
      }
    case STRING:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case INTEGER:
        case STRING:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported modulo!");
          rv->t = NIL;
          return;
      }
    case OBJECT:
      *exception = make_c_string("TODO: unimplemented");
      rv->t = NIL;
      return;
    case BOOLEAN:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported modulo!");
          rv->t = NIL;
          return;
      }
    case NIL:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported modulo!");
          rv->t = NIL;
          return;
      }
    case CLOSURE:
      switch(val2->t) {
        case OBJECT:
          *exception = make_c_string("TODO: unimplemented");
          rv->t = NIL;
          return;
        case STRING:
        case INTEGER:
        case FLOAT:
        case BOOLEAN:
        case NIL:
        case CLOSURE:
        default:
          *exception = make_c_string("unsupported modulo!");
          rv->t = NIL;
          return;
      }
    default:
      *exception = make_c_string("unsupported modulo!");
      rv->t = NIL;
      return;
  }
}
