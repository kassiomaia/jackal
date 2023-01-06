#ifndef JACKAL_VM_H
#define JACKAL_VM_H

jkl_word_t jkl_vm_init(jkl_vm_t *vm);
jkl_word_t jkl_vm_load(jkl_vm_t *vm, jkl_program_t *program);
jkl_word_t jkl_vm_run(jkl_vm_t *vm);

#endif