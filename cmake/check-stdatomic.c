#include <stdatomic.h>
atomic_int int_var;
atomic_uintptr_t uintptr_var;
int main(void) {
    atomic_store_explicit(&int_var, 5, memory_order_relaxed);
    atomic_store_explicit(&uintptr_var, 0, memory_order_relaxed);
    int loaded_value = atomic_load_explicit(&int_var, memory_order_seq_cst);
    return 0;
}
