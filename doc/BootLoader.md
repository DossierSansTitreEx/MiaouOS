# BootLoader

## Design Decisions

The bootloader is designed to perform the following operations:

 * Disable interrupts
 * Reset the computer to sane defaults (IP: 0000:7c00, among other things).
 * Immediately relocate to 0000:0600
 * Set up basic operations, like a stack and segments
 * Parse the MBR headers
 * Load the VBR to 0000:7c00
 * Jump to the VBR
 * Setup protected mode, with flat memory, no paging
 * Enter pmode
 * Go back to real mode with the pmode segments (to disable the memory barrier)
 * Use the BIOS to record boot informations
 * Use the BIOS to load the master block
 * Parse the filesystem to load the kernel at a fixed address
 * Once the kernel is loaded, go back to protected mode
 * Setup long mode paging
 * Page enough memory for the kernel
 * Enter long mode
 * Load the kernel to it's final address
 * Fix the boot info to take into account the kernel pages
 * Jump into the kernel
