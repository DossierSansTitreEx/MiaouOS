# Address Space

Here is the address space of fromOS (from the kernel point of view).

| from               | to                     | size      | description                                          |
|--------------------|------------------------|-----------|------------------------------------------------------|
| 0x0                | 0x7fffffff             | 1 GiB     | Identity map of the first gibibyte (boot only)       |
| 0x0                | 0x7fffffffff           | 512 GiB   | Userland (userland only)                             |
| 0xffffffff80000000 | 0xffffffffffffffff     | 512 GiB   | Kernel                                               |
| 0xff
