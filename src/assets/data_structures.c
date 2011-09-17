struct ObjectTree {
  char* key;
  unsigned int key_size;
  union Value value;
  struct ObjectTree* left;
  struct ObjectTree* right;
};

struct ObjectData {
  bool sealed;
  struct ObjectTree* tree;
};

static inline void make_object(union Value* v) {
  v->t = OBJECT;
  v->object.data = GC_MALLOC(sizeof(struct ObjectData));
  v->object.data->sealed = false;
  v->object.data->tree = NULL;
}

static inline struct ObjectTree* new_tree_node(char* key, unsigned int key_size,
    union Value* value) {
  struct ObjectTree* t;
  t = GC_MALLOC(sizeof(struct ObjectTree));
  t->key = key;
  t->key_size = key_size;
  t->value = *value;
  t->left = NULL;
  t->right = NULL;
  return t;
}

static void _copy_object(struct ObjectTree* t1, struct ObjectTree** t2) {
  if(t1 == NULL) {
    *t2 = NULL;
    return;
  }
  *t2 = new_tree_node(t1->key, t1->key_size, &t1->value);
  _copy_object(t1->right, &(*t2)->left);
  _copy_object(t1->right, &(*t2)->right);
}

static inline bool copy_object(union Value* o1, union Value* o2) {
  if(o1->t != OBJECT) return false;
  make_object(o2);
  _copy_object(o1->object.data->tree, &o2->object.data->tree);
  return true;
}

static inline void seal_object(struct ObjectData* data) {
  data->sealed = true;
}

static bool _set_field(struct ObjectTree** tree, char* key,
    unsigned int key_size, union Value* value, bool sealed) {
  if(*tree == NULL) {
    if(sealed) return false;
    *tree = new_tree_node(key, key_size, value);
    return true;
  }
  switch (safe_strcmp(key, key_size, (*tree)->key, (*tree)->key_size)) {
    case 0:
      (*tree)->value = *value;
      return true;
    case -1:
      return _set_field(&(*tree)->left, key, key_size, value, sealed);
    default:
      return _set_field(&(*tree)->right, key, key_size, value, sealed);
  }
}

static inline bool set_field(struct ObjectData* data, char* key,
    unsigned int key_size, union Value value) {
  return _set_field(&data->tree, key, key_size, &value, data->sealed);
}

static bool _get_field(struct ObjectTree* tree, char* key,
    unsigned int key_size, union Value* value) {
  if(tree == NULL) return false;
  switch (safe_strcmp(key, key_size, tree->key, tree->key_size)) {
    case 0:
      *value = tree->value;
      return true;
    case -1:
      return _get_field(tree->left, key, key_size, value);
    default:
      return _get_field(tree->right, key, key_size, value);
  }
}

static inline bool get_field(struct ObjectData* data, char* key,
    unsigned int key_size, union Value* value) {
  return _get_field(data->tree, key, key_size, value);
}

static inline struct Array* make_array() {
  struct Array* array;
  array = GC_MALLOC(sizeof(struct Array));
  array->size = 0;
  array->highwater = MIN_ARRAY_SIZE;
  array->data = GC_MALLOC(sizeof(union Value) * MIN_ARRAY_SIZE);
  return array;
}

static inline void reserve_space(struct Array* array, unsigned int total_size) {
  unsigned int i = 0;
  if(total_size <= array->highwater) return;
  while(array->highwater < total_size) {
    unsigned int new_space = array->highwater;
    if(new_space > MAX_ARRAY_ADDITION) new_space = MAX_ARRAY_ADDITION;
    array->highwater += new_space;
  }
  union Value* new_data = GC_MALLOC(sizeof(union Value) * array->highwater);
  for(i = 0; i < array->size; ++i) new_data[i] = array->data[i];
  array->data = new_data;
}

static inline void append_values(struct Array* array, union Value* values,
    unsigned int size) {
  unsigned int i = 0;
  reserve_space(array, array->size + size);
  for(i = 0; i < size; ++i)
    array->data[array->size + i] = values[i];
  array->size += size;
}

static inline void shift_values(struct Array* array,
    signed int amount_to_right) {
  if(amount_to_right < 0) {
    signed int i = 0;
    for(i = -amount_to_right; i < array->size; ++i)
      array->data[i + amount_to_right] = array->data[i];
    array->size += amount_to_right;
    return;
  }
  if(amount_to_right == 0) return;
  reserve_space(array, array->size + amount_to_right);
  {
    unsigned int i = 0;
    for(i = array->size; i > 0; --i) {
      array->data[i - 1 + amount_to_right] = array->data[i - 1];
    }
  }
  array->size += amount_to_right;
}

static bool array_size(void* array, struct Array* ra, struct Array* la,
    union Value* dest) {
  if(ra->size != 0 || la->size != 0) {
    *dest = make_c_string("expected 0 arguments");
    return false;
  }
  dest->t = INTEGER;
  dest->integer.value = ((struct Array*)array)->size;
  return true;
}

static bool array_update(void* array, struct Array* ra, struct Array* la,
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

static bool array_index(void* array, struct Array* ra, struct Array* la,
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

static bool array_append(void* array, struct Array* ra, struct Array* la,
    union Value* dest) {
  append_values(array, la->data, la->size);
  append_values(array, ra->data, ra->size);
  dest->t = NIL;
  return true;
}

static bool array_pop(void* array, struct Array* ra, struct Array* la,
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

static bool array_shift(void* array, struct Array* ra, struct Array* la,
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

static bool array_unshift(void* array, struct Array* ra, struct Array* la,
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

  set_field(v->object.data, "u_size", 6,
      make_external_closure(&array_size, *array));
  set_field(v->object.data, "u_append", 8,
      make_external_closure(&array_append, *array));
  set_field(v->object.data, "u_pop", 5,
      make_external_closure(&array_pop, *array));
  set_field(v->object.data, "u_shift", 7,
      make_external_closure(&array_shift, *array));
  set_field(v->object.data, "u_unshift", 9,
      make_external_closure(&array_unshift, *array));
  set_field(v->object.data, "u__7eupdate", 11,
      make_external_closure(&array_update, *array));
  set_field(v->object.data, "u__7eindex", 10,
      make_external_closure(&array_index, *array));

  seal_object(v->object.data);
}
