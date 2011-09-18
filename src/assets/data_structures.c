struct ObjectTree {
  struct ByteArray key;
  union Value value;
  struct ObjectTree* left;
  struct ObjectTree* right;
  struct ObjectTree* parent;
};

struct ObjectData {
  bool sealed;
  struct ObjectTree* tree;
  unsigned int size;
};

static inline void initialize_object(struct ObjectData* data) {
  data->sealed = false;
  data->tree = NULL;
  data->size = 0;
}

static inline void make_object(union Value* v) {
  v->t = OBJECT;
  v->object.data = GC_MALLOC(sizeof(struct ObjectData));
  initialize_object(v->object.data);
}

static inline struct ObjectTree* new_tree_node(struct ByteArray key,
    union Value* value, struct ObjectTree* parent) {
  struct ObjectTree* t;
  t = GC_MALLOC(sizeof(struct ObjectTree));
  t->key = key;
  t->value = *value;
  t->left = NULL;
  t->right = NULL;
  t->parent = parent;
  return t;
}

static void _copy_object(struct ObjectTree* t1, struct ObjectTree** t2,
    struct ObjectTree* t2_parent) {
  if(t1 == NULL) {
    *t2 = NULL;
    return;
  }
  *t2 = new_tree_node(t1->key, &t1->value, t2_parent);
  _copy_object(t1->right, &(*t2)->left, *t2);
  _copy_object(t1->right, &(*t2)->right, *t2);
}

static inline bool copy_object(union Value* o1, union Value* o2) {
  if(o1->t != OBJECT) return false;
  make_object(o2);
  _copy_object(o1->object.data->tree, &o2->object.data->tree, NULL);
  o2->object.data->size = o1->object.data->size;
  return true;
}

static inline void seal_object(struct ObjectData* data) {
  data->sealed = true;
}

static bool _set_field(struct ObjectTree** tree, struct ByteArray key,
    union Value* value, struct ObjectData* data, struct ObjectTree* parent) {
  if(*tree == NULL) {
    if(data->sealed) return false;
    *tree = new_tree_node(key, value, parent);
    ++(data->size);
    return true;
  }
  switch (safe_strcmp(key, (*tree)->key)) {
    case 0:
      (*tree)->value = *value;
      return true;
    case -1:
      return _set_field(&(*tree)->left, key, value, data, *tree);
    default:
      return _set_field(&(*tree)->right, key, value, data, *tree);
  }
}

static inline bool set_field(struct ObjectData* data, struct ByteArray key,
    union Value value) {
  return _set_field(&data->tree, key, &value, data, NULL);
}

static bool _get_field(struct ObjectTree* tree, struct ByteArray key,
    union Value* value) {
  if(tree == NULL) return false;
  switch (safe_strcmp(key, tree->key)) {
    case 0:
      *value = tree->value;
      return true;
    case -1:
      return _get_field(tree->left, key, value);
    default:
      return _get_field(tree->right, key, value);
  }
}

static inline bool get_field(struct ObjectData* data, struct ByteArray key,
    union Value* value) {
  return _get_field(data->tree, key, value);
}

struct ObjectIterator {
  struct ObjectTree* current_node;
  bool complete;
};

static inline struct ObjectTree* object_iterator_current_node(
    struct ObjectIterator* it) {
  return it->current_node;
}

static inline bool object_iterator_complete(struct ObjectIterator* it) {
  return it->complete;
}

static inline void object_iterator_step(struct ObjectIterator* it) {
  // if you're on a node, you can assume both of your subtrees are complete.
  // so, the next node of a given node is defined thusly:
  //  if your parent is null you're done.
  //  if you are a left descendant of your parent, then the next node is your
  //  parent's right child's leftmost descendant, assuming there is one.
  //  otherwise, your parent is the next node.

  if(it->current_node == NULL || it->current_node->parent == NULL) {
    it->complete = true;
    return;
  }
  if(it->current_node == it->current_node->parent->right ||
      it->current_node->parent->right == NULL) {
    it->current_node = it->current_node->parent;
    return;
  }
  it->current_node = it->current_node->parent->right;
  while(it->current_node->left != NULL || it->current_node->right != NULL) {
    it->current_node = it->current_node->left != NULL ?
        it->current_node->left : it->current_node->right;
  }
}

static inline void initialize_object_iterator(struct ObjectIterator* it,
    struct ObjectData* data) {
  if(data->tree == NULL) {
    it->complete = true;
    return;
  }
  it->current_node = data->tree;
  it->complete = false;
  while(it->current_node->left != NULL || it->current_node->right != NULL) {
    it->current_node = it->current_node->left != NULL ?
        it->current_node->left : it->current_node->right;
  }
}

static inline void initialize_array(struct Array* array) {
  array->size = 0;
  array->highwater = MIN_ARRAY_SIZE;
  array->data = GC_MALLOC(sizeof(union Value) * MIN_ARRAY_SIZE);
}

static inline struct Array* make_array() {
  struct Array* array;
  array = GC_MALLOC(sizeof(struct Array));
  initialize_array(array);
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

static inline unsigned int binary_search(struct ByteArray key,
    struct ByteArray* key_array, unsigned int key_count) {
  // TODO
  unsigned int i = 0;
  for(; i < key_count; ++i) {
    if(safe_strcmp(key, key_array[i]) == 0) {
      return i;
    }
  }
  return key_count;
}
