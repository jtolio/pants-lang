enum TreeTag {
  NODE,
  LEAF
};

struct ObjectTree {
  char* key;
  unsigned int key_size;
  union Value value;
  union ObjectTree* left;
  union ObjectTree* right;
};

struct ObjectData {
  bool sealed;
  struct ObjectTree* tree;
};

static inline union Value make_object() {
  union Value v;
  v.t = OBJECT;
  v.object.data = GC_MALLOC(sizeof(ObjectData));
  v.object.data.sealed = false;
  v.object.data.tree = NULL;
  return v;
}

static inline void seal_object(struct ObjectData* data) {
  data->sealed = true;
}

static inline struct ObjectTree* new_tree_node(char* key, unsigned int key_size,
    union Value* value) {
  struct ObjectTree* t;
  t = GC_MALLOC(sizeof(ObjectTree));
  t->key = key;
  t->key_size = key_size;
  t->value = *value;
  t->left = NULL;
  t->right = NULL;
}

static inline bool set_field(struct ObjectTree* tree, char* key,
    unsigned int key_size, union Value* value, bool sealed) {
  switch (safe_strcmp(key, key_size, tree->key, tree->key_size)) {
    case 0:
      tree->value = *value;
      return true;
    case -1:
      if(tree->left != NULL)
        return set_field(tree->left, key, key_size, value, sealed);
      if(sealed) return false;
      tree->left = new_tree_node(key, key_size, value);
      return true;
    default:
      if(tree->right != NULL)
        return set_field(tree->right, key, key_size, value, sealed);
      if(sealed) return false;
      tree->right = new_tree_node(key, key_size, value);
      return true;
  }
}

static inline bool set_field(struct ObjectData* data, char* key,
    unsigned int key_size, union Value* value) {
  if(data->tree != NULL)
    return set_field(data->tree, key, key_size, value, data->sealed);
  if(data->sealed) return false;
  data->tree = new_tree_node(key, key_size, value);
  return true;
}

static inline bool get_field(struct ObjectTree* tree, char* key,
    unsigned int key_size, union Value* value) {
  int cmp;
  if(tree == NULL) return false;
  switch (safe_strcmp(key, key_size, tree->key, tree->key_size)) {
    case 0:
      *value = tree->value;
      return true;
    case -1:
      return get_field(tree->left, key, key_size, value);
    default:
      return get_field(tree->right, key, key_size, value);
  }
}

static inline bool get_field(struct ObjectData* data, char* key,
    unsigned int key_size, union Value* value) {
  return get_field(data->tree, key, key_size, value);
}
