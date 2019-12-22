/**
 * Divides the terminal into 4 sections
 *
 * NONE:
 * If not modifying anything
 *
 * TOP:
 * - 1 line
 * - Right Aligned: <IP ADDRESS> | <PORT>
 * - Left Aligned: <error message if any>
 *
 * MIDDLE:
 * - Rest of lines (after compensating for TOP, MESSAGE, and COMMAND section
 *
 * BOTTOM:
 * - 2 lines
 *
 */
enum Section {NONE, TOP, MIDDLE, BOTTOM};

void initialize_display_lock();

void *update_screen();