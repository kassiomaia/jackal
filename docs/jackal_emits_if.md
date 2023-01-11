## Emits IF

# Example 1

```
  if (a == b) {
    puts "Hello word"
  }
```

# Compiled 1

LOAD a      `load variable a to the stack`
LOAD b      `load variable b to the stack`
OP   EQ     `compare both and add the result to the stack`
JNE IFEB    `jump to the block of code when the value on the stack is > 0`
...         `list of block instructions`
IFEB        `end of block`

# Example 2

```
  if (a == b) || (a != b) {
  }
```

# Compiled 2

LOAD a          `stack => [a]`
LOAD b          `stack => [a, b]`
OP   EQ         `stack => [r0]`
LOAD a          `stack => [r0, a]`
LOAD b          `stack => [r0, a, b]`
OP   NEQ        `stack => [r0, r1]`
OP   OR         `stack => [r2]`
JNE  IFEB       `stack => []`
...
IFEB            `stack => []`
