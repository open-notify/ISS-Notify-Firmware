/**
 * Total number of commands for allocation and looping.
 * Increment this if you add a command!!!
 */
#define N_CMDS 13

// All commands have to have signature of void NAME(void);
void ack(void);
void say_ms(void);
void set_color(void);
void say_time(void);
void set_passes(void);
void dump_mem(void);
void set_clock(void);
void wb_red(void);
void wb_green(void);
void wb_blue(void);
void set_wb(void);
void show_color(void);
void toggle_rainbow(void);

/**
 * Array of commands
 * A command (c) can only be a single ascii char. The resp is a pointer to the command function
 */
static const cmd COMMANDS[N_CMDS] = { {c: 'a', resp: ack},
                                      {c: 'm', resp: say_ms},
                                      {c: 'c', resp: set_color},
                                      {c: 't', resp: say_time},
                                      {c: 'u', resp: set_passes},
                                      {c: 'd', resp: dump_mem},
                                      {c: 'T', resp: set_clock},
                                      {c: 'r', resp: wb_red},
                                      {c: 'g', resp: wb_green},
                                      {c: 'b', resp: wb_blue},
                                      {c: 'W', resp: set_wb},
                                      {c: 'l', resp: show_color},
                                      {c: 'x', resp: toggle_rainbow},
							   		};

