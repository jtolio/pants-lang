#include "../src/assets/header.c"
#include "../src/assets/data_structures.c"
#include "../src/assets/builtins.c"

#define assert(bool) \
  if(!bool) { \
    printf("failure on line %d\n", __LINE__); \
    return 1; \
  }

void dump_tree(struct ObjectTree* tree) {
  printf("tree: pointer: %p\n", tree);
  printf(".     key: %s\n", tree->key.data);
  printf(".     key_size: %d\n", tree->key.size);
  printf(".     integer_value: %lld\n", tree->value.integer.value);
  printf(".     left: %p\n", tree->left);
  printf(".     right: %p\n", tree->right);
  printf(".     parent: %p\n", tree->parent);
  if(tree->left != NULL) dump_tree(tree->left);
  if(tree->right != NULL) dump_tree(tree->right);
}

void dump_object(struct ObjectData* object) {
  printf("object: pointer: %p\n", object);
  printf(".       sealed: %s\n", object->sealed ? "yes" : "no");
  printf(".       tree-head: %p\n", object->tree);
  dump_tree(object->tree);
}

int main(int argc, char** argv) {
  struct ObjectData object;
  struct ObjectIterator it;
  union Value value;

  initialize_object(&object);
  initialize_object_iterator(&it, &object);

  assert(object_iterator_complete(&it));

  value.t = INTEGER;
  value.integer.value = 42;
  assert(set_field(&object, (struct ByteArray){"jtfield", 7}, value));

  initialize_object_iterator(&it, &object);
  assert(!object_iterator_complete(&it));
  assert(object_iterator_current_node(&it)->value.t == INTEGER);
  assert(object_iterator_current_node(&it)->value.integer.value == 42);
  object_iterator_step(&it);
  assert(object_iterator_complete(&it));

  value.t = INTEGER;
  value.integer.value = 6141;
  assert(set_field(&object, (struct ByteArray){"field1", 6}, value));
  value.t = INTEGER;
  value.integer.value = 14677;
  assert(set_field(&object, (struct ByteArray){"field3", 6}, value));
  value.t = INTEGER;
  value.integer.value = 46131;
  assert(set_field(&object, (struct ByteArray){"field2", 6}, value));

  // dump_object(&object);

  initialize_object_iterator(&it, &object);
  assert(!object_iterator_complete(&it));
  assert(object_iterator_current_node(&it)->value.t == INTEGER);
  assert(object_iterator_current_node(&it)->value.integer.value == 6141);
  object_iterator_step(&it);
  assert(!object_iterator_complete(&it));
  assert(object_iterator_current_node(&it)->value.t == INTEGER);
  assert(object_iterator_current_node(&it)->value.integer.value == 46131);
  object_iterator_step(&it);
  assert(!object_iterator_complete(&it));
  assert(object_iterator_current_node(&it)->value.t == INTEGER);
  assert(object_iterator_current_node(&it)->value.integer.value == 14677);
  object_iterator_step(&it);
  assert(!object_iterator_complete(&it));
  assert(object_iterator_current_node(&it)->value.t == INTEGER);
  assert(object_iterator_current_node(&it)->value.integer.value == 42);
  object_iterator_step(&it);
  assert(object_iterator_complete(&it));

  return 0;
}
