int val;
int main(void) {
    __atomic_store_n(&val, 1, __ATOMIC_SEQ_CST);
    (void)__atomic_load_n(&val, __ATOMIC_SEQ_CST);
    return 0;
}
