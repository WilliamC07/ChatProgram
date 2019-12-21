/**
 * Divides the terminal into 4 sections
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
enum Section {TOP, MIDDLE, BOTTOM};

void *update_screen();