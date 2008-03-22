/* NG-SPICE -- An electrical circuit simulator
 * $Id$
 *
 * Copyright (c) 1990 University of California
 * Copyright (c) 2000 Arno W. Peters
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation without fee, and without a written agreement is
 * hereby granted, provided that the above copyright notice, this
 * paragraph and the following three paragraphs appear in all copies.
 *
 * This software program and documentation are copyrighted by their
 * authors. The software program and documentation are supplied "as
 * is", without any accompanying services from the authors. The
 * authors do not warrant that the operation of the program will be
 * uninterrupted or error-free. The end-user understands that the
 * program was developed for research purposes and is advised not to
 * rely exclusively on the program for any reason.
 * 
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE TO ANY PARTY FOR DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
 * DOCUMENTATION, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. THE AUTHORS SPECIFICALLY DISCLAIMS ANY
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE
 * SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE AUTHORS
 * HAVE NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS. */

/* Table of available commands.  Note that they're sorted so that the
 * commands that appear in the spiceinit file are at the top.  */

#include <ngspice.h>
#include <ftedefs.h>
#include <cpdefs.h>

#include "ftehelp.h"
#include "commands.h"

#include "com_ahelp.h"
#include "com_asciiplot.h"
#include "com_compose.h"
#include "com_display.h"
#include "com_hardcopy.h"
#include "com_help.h"
#include "com_let.h"
#include "com_plot.h"
#include "com_setscale.h"
#include "com_xgraph.h"
#include "com_gnuplot.h"
#include "com_state.h"
#include "com_chdir.h"
#include "com_echo.h"
#include "com_rehash.h"
#include "com_shell.h"
#include "com_shift.h"
#include "com_unset.h"
#include "fourier.h"

#ifdef EXPERIMENTAL_CODE
#include "com_option.h"
#include "spiceif.h" /* for com_loadsnap() and com_savesnap() */
#endif

#include "com_dl.h"

#ifdef XSPICE
/* gtri - begin - wbk - add include files */
#include "evtproto.h"
/* gtri - end - wbk - add include files */
#endif

/* FIXME: Integrate spcp_coms and nutcp_coms into one variable. */


/* Bool fields:     stringargs, spiceonly, major */

struct comm spcp_coms[] = {
    { "let", com_let, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      arg_let,
      "varname = expr : Assign vector variables." } ,
    { "reshape", com_reshape, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      arg_let,
      "vector ... [ shape ] : change the dimensions of a vector." } ,
    { "define", com_define, FALSE, FALSE, TRUE,
      { 010000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[[func (args)] stuff] : Define a user-definable function." } ,    
    { "set", com_set, FALSE, FALSE, TRUE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 0, LOTS,
      arg_set,
      "[option] [option = value] ... : Set a variable." } ,

#ifdef EXPERIMENTAL_CODE
/* PN support for altering options in interactive mode */    
    { "option", com_option, FALSE, TRUE, TRUE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 0, LOTS,
      arg_set,
      "[option] [option = value] ... : Set a simulator option." } ,
    { "savesnap", com_savesnap, FALSE, FALSE, TRUE,
      { 1, 040000, 040000, 040000 }, E_DEFHMASK, 1, 1,
      (void (*)()) NULL,
      "file : Save a snapshot." } ,
    { "loadsnap", com_loadsnap, FALSE, FALSE, TRUE,
      { 1, 040000, 040000, 040000 }, E_DEFHMASK, 2, 2,
      (void (*)()) NULL,
      "file : Load a snapshot." } ,
#endif    
    
    { "alias", com_alias, FALSE, FALSE, FALSE,
      { 02, 04, 04, 04 }, E_ADVANCED, 0, LOTS,
      (void (*)()) NULL,
      "[[word] alias] : Define an alias." } ,
    { "deftype", com_dftype, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 3, LOTS,
      (void (*)()) NULL,
      "spec name pat ... : Redefine vector and plot types.\n" } ,
    { "plot", com_plot, FALSE, FALSE, TRUE,
      { 041000, 041000, 041000, 041000 }, E_BEGINNING | E_HASPLOTS, 1, LOTS,
      arg_plot,
      "expr ... [vs expr] [xl xlo xhi] [yl ylo yhi] : Plot things." },
    { "display", com_display, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 0, LOTS,
      arg_display,
      ": Display vector status." } ,
    { "destroy", com_destroy, FALSE, FALSE, FALSE,
      { 0400, 0400, 0400, 0400 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[plotname] ... : Throw away all the data in the plot." } ,
    { "setplot", com_splot, FALSE, FALSE, TRUE,
      { 0400, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[plotname] : Change the current working plot." } ,
    { "setcirc", com_scirc, FALSE, TRUE, FALSE,
      { 04, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[circuit name] : Change the current circuit." } ,
    { "setscale", com_setscale, FALSE, FALSE, FALSE,
      { 040000, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[vecname] : Change default scale of current working plot." } ,
    { "transpose", com_transpose, FALSE, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "varname ... : Perform matrix transposition on multi-D vectors." } ,
    { "xgraph", com_xgraph, FALSE, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "file plotargs : Send plot to Xgraph-11." } ,
    { "gnuplot", com_gnuplot, FALSE, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "file plotargs : Send plot to gnuplot." } ,
    { "hardcopy", com_hardcopy, FALSE, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "file plotargs : Produce hardcopy plots." } ,
    { "asciiplot", com_asciiplot, FALSE, FALSE, TRUE,
      { 041000, 041000, 041000, 041000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "plotargs : Produce ascii plots." } ,
    { "write", com_write, FALSE, FALSE, TRUE,
      { 1, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "file expr ... : Write data to a file." } ,
    { "compose", com_compose, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 2, LOTS,
      (void (*)()) NULL,
      "var parm=val ... : Compose a vector." } ,
    { "unlet", com_unlet, FALSE, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "varname ... : Undefine vectors." } ,
    { "print", com_print, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      arg_print,
      "[col] expr ... : Print vector values." } ,
#ifdef XSPICE
/* gtri - begin - wbk - add event print command */
    { "eprint", EVTprint, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      (void (*)()) NULL,
      "node node ... : Print event values." } ,
/* gtri - end - wbk - add event print command */
    { "codemodel", com_codemodel, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      (void (*)()) NULL,
      "library library ... : Loads the opus librarys." } ,
#endif
#ifdef DEVLIB
    { "use", com_use, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      (void (*)()) NULL,
      "library library ... : Loads the device librarys." } ,
#endif
    { "load", com_load, FALSE, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_BEGINNING | E_NOPLOTS, 1, LOTS,
      arg_load,
      "file ... : Load in data." } ,
    { "cross", com_cross, FALSE, FALSE, TRUE,
      { 040000, 0, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      (void (*)()) NULL,
      "vecname number [ vector ... ] : Make a vector in a strange way." } ,
    { "undefine", com_undefine, FALSE, FALSE, FALSE,
      { 010000, 010000, 010000, 010000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[func ...] : Undefine a user-definable function." } ,
    { "op", com_op, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.op line args] : Determine the operating point of the circuit." } ,
    { "tf", com_tf, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.tran line args] : Do a transient analysis." } ,
    { "tran", com_tran, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.tran line args] : Do a transient analysis." } ,
    { "ac", com_ac, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.ac line args] : Do an ac analysis." } ,
    { "dc", com_dc, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.dc line args] : Do a dc analysis." } ,
    { "pz", com_pz, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.pz line args] : Do a pole / zero analysis." } ,
    { "sens", com_sens, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.sens line args] : Do a sensitivity analysis." } ,
    { "disto", com_disto, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.disto line args] : Do an distortion analysis." } ,
    { "noise", com_noise, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.noise line args] : Do a noise analysis." } ,
    { "listing", com_listing, FALSE, TRUE, TRUE,
      { 0100, 0100, 0100, 0100 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[logical] [physical] [deck] : Print the current circuit." } ,
    { "edit", com_edit, FALSE, TRUE, TRUE,
      { 1, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[filename] : Edit a spice deck and then load it in." } ,
    { "dump", com_dump, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Print a dump of the current circuit." } ,
    { "fourier", com_fourier, FALSE, FALSE, TRUE,
      { 0, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "fund_freq vector ... : Do a fourier analysis of some data." } ,
    { "spec", com_spec, FALSE, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 4, LOTS,
      (void (*)()) NULL,
      "start_freq stop_freq step_freq vector ... : Create a frequency domain plot." } ,
    { "show", com_show, FALSE, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "devices ... : parameters ... : Print out device summary." } ,
    { "showmod", com_showmod, FALSE, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "models ... : parameters ... : Print out model summary." } ,
    { "alter", com_alter, FALSE, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "devspecs : parmname value : Alter device parameters." } ,
    { "altermod", com_altermod, FALSE, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "devspecs : parmname value : Alter model parameters." } ,
    { "resume", com_resume, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Continue after a stop." } ,
    { "state", com_state, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "(unimplemented) : Print the state of the circuit." },
    { "stop", com_stop, FALSE, TRUE, FALSE,
      { 04200, 04200, 04200, 04200 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[stop args] : Set a breakpoint." } ,
    { "trace", com_trce, FALSE, TRUE, FALSE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[all] [node ...] : Trace a node." } ,
    { "save", com_save, FALSE, TRUE, FALSE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[all] [node ...] : Save a spice output." } ,
    { "iplot", com_iplot, FALSE, TRUE, TRUE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[all] [node ...] : Incrementally plot a node." } ,
    { "status", com_sttus, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Print the current breakpoints and traces." } ,
    { "delete", com_delete, FALSE, TRUE, FALSE,
      { 020, 020, 020, 020 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[all] [break number ...] : Delete breakpoints and traces." } ,
    { "step", com_step, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[number] : Iterate number times, or one." } ,
    { "reset", com_rset, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Terminate a simulation after a breakpoint (formerly 'end')." } ,
    { "run", com_run, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[rawfile] : Run the simulation as specified in the input file." } ,
    { "aspice", com_aspice, FALSE, FALSE, FALSE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 1, 2,
      (void (*)()) NULL,
      "file [outfile] : Run a spice job asynchronously." } ,
    { "jobs", com_jobs, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Report on asynchronous spice jobs." } ,
    { "rspice", com_rspice, FALSE, FALSE, FALSE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[input file] : Run a spice job remotely." } ,
    { "bug", com_bug, FALSE, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Report a %s bug." } ,
    { "where", com_where, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Print last non-converging node or device" } ,
    { "newhelp", com_ahelp, FALSE, FALSE, TRUE,
      { 010, 010, 010, 010 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[command name] ... : help." },
    { "tutorial", com_ghelp, FALSE, FALSE, TRUE,
      { 023010, 023010, 023010, 023010 }, E_BEGINNING, 0, LOTS,
      (void (*)()) NULL,
      "[subject] ... : Hierarchical documentation browser." } ,
    { "help", com_ghelp, FALSE, FALSE, TRUE,
      { 023010, 023010, 023010, 023010 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[subject] ... : Hierarchical documentation browser." } ,
    { "oldhelp", com_help, FALSE, FALSE, TRUE,
      { 010, 010, 010, 010 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[command name] ... : Print help." } ,
    { "quit", com_quit, FALSE, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_BEGINNING, 0, 0,
      (void (*)()) NULL,
      ": Quit %s." } ,
    { "source", com_source, FALSE, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "file : Source a %s file." } ,
    { "shift", com_shift, FALSE, FALSE, FALSE,
      { 020000, 0, 0, 0 }, E_DEFHMASK, 0, 2,
      (void (*)()) NULL,
      "[var] [number] : Shift argv or the named list var to the left." } ,
    { "unset", com_unset, FALSE, FALSE, FALSE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "varname ... : Unset a variable." } ,
    { "unalias", com_unalias, FALSE, FALSE, FALSE,
      { 02, 02, 02, 02 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "word ... : Undefine an alias." } ,
    { "history", com_history, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 2,
      (void (*)()) NULL,
      "[-r] [number] : Print command history." } ,
    { "echo", com_echo, FALSE, FALSE, FALSE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[stuff ...] : Print stuff." } ,
    { "shell", com_shell, FALSE, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[args] : Fork a shell, or execute the command." } ,
    { "rusage", com_rusage, FALSE, FALSE, FALSE,
      { 02000, 02000, 02000, 02000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[resource ...] : Print current resource usage." } ,
    { "cd", com_chdir, FALSE, FALSE, FALSE,
      { 1, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[directory] : Change working directory." } ,
    { "version", com_version, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[number] : Print the version number." } ,
    { "diff", com_diff, FALSE, FALSE, FALSE,
      { 0400, 0400, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "plotname plotname [vec ...] : 'diff' two plots." } ,
    { "rehash", com_rehash, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Rebuild the unix command database." } ,
    { "while", NULL, FALSE, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "condition : Execute while the condition is TRUE." } ,
    { "repeat", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0}, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[number] : Repeat number times, or forever." } ,
    { "dowhile", NULL, FALSE, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "condition : Execute while the condition is TRUE." } ,
    { "foreach", NULL, FALSE, FALSE, FALSE,
      { 0, 040000, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      (void (*)()) NULL,
      "variable value ... : Do once for each value." } ,
    { "if", NULL, FALSE, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "condition : Execute if the condition is TRUE." } ,
    { "else", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Goes with if." } ,
    { "end", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": End a block." } ,
    { "break", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Break out of a block." } ,
    { "continue", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Continue a loop." } ,
    { "label", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 1, 1,
      (void (*)()) NULL,
      "word : Create someplace to go to." } ,
    { "goto", NULL, FALSE, FALSE, FALSE,
      { 0100000, 0, 0, 0 }, E_DEFHMASK, 1, 1,
      (void (*)()) NULL,
      "word : Go to a label." } ,
    { "cdump", com_cdump, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Dump the current control structures." } ,
    { "settype", com_stype, FALSE, FALSE, FALSE,
      { 0200000, 040000, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      (void (*)()) NULL,
      "type vec ... : Change the type of a vector." } ,
    { "strcmp", com_strcmp, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 3, 3,
      (void (*)()) NULL,
      "varname s1 s2 : Set $varname to strcmp(s1, s2)." } ,
    { "linearize", com_linearize, FALSE, TRUE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      " [ vec ... ] : Convert plot into one with linear scale." } ,
    { 0, NULL, FALSE, FALSE, FALSE, { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      NULL }
};


/* Bool fields:     stringargs, spiceonly, major */
struct comm nutcp_coms[] = {
    { "let", com_let, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      arg_let,
      "varname = expr : Assign vector variables." } ,
    { "reshape", com_reshape, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      arg_let,
      "vector ... [ shape ] : change the dimensions of a vector." } ,
    { "define", com_define, FALSE, FALSE, TRUE,
      { 010000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[[func (args)] stuff] : Define a user-definable function." } ,
    { "set", com_set, FALSE, FALSE, TRUE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 0, LOTS,
      arg_set,
      "[option] [option = value] ... : Set a variable." } ,

#ifdef EXPERIMENTAL_CODE
/* PN support for altering options in interactive mode */    
    { "option", com_option, FALSE, TRUE, TRUE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 0, LOTS,
      arg_set,
      "[option] [option = value] ... : Set a simulator option." } ,
#endif    
     
    { "alias", com_alias, FALSE, FALSE, FALSE,
      { 02, 04, 04, 04 }, E_ADVANCED, 0, LOTS,
      (void (*)()) NULL,
      "[[word] alias] : Define an alias." } ,
    { "deftype", com_dftype, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 3, LOTS,
      (void (*)()) NULL,
      "spec name pat ... : Redefine vector and plot types.\n" } ,
    { "plot", com_plot, FALSE, FALSE, TRUE,
      { 041000, 041000, 041000, 041000 }, E_BEGINNING | E_HASPLOTS, 1, LOTS,
      arg_plot,
      "expr ... [vs expr] [xl xlo xhi] [yl ylo yhi] : Plot things." },
    { "display", com_display, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 0, LOTS,
      arg_display,
      ": Display vector status." } ,
    { "destroy", com_destroy, FALSE, FALSE, FALSE,
      { 0400, 0400, 0400, 0400 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[plotname] ... : Throw away all the data in the plot." } ,
    { "setplot", com_splot, FALSE, FALSE, TRUE,
      { 0400, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[plotname] : Change the current working plot." } ,
    { "setcirc", NULL, FALSE, TRUE, FALSE,
      { 04, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[circuit name] : Change the current circuit." } ,
    { "setscale", com_setscale, FALSE, FALSE, FALSE,
      { 040000, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[vecname] : Change default scale of current working plot." } ,
    { "transpose", com_transpose, FALSE, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "varname ... : Perform matrix transposition on multi-D vectors." } ,
    { "xgraph", com_xgraph, FALSE, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "file plotargs : Send plot to Xgraph-11." } ,
    { "gnuplot", com_gnuplot, FALSE, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "file plotargs : Send plot to gnuplot." } ,
    { "hardcopy", com_hardcopy, FALSE, FALSE, TRUE,
      { 1, 041000, 041000, 041000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "file plotargs : Produce hardcopy plots." } ,
    { "asciiplot", com_asciiplot, FALSE, FALSE, TRUE,
      { 041000, 041000, 041000, 041000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "plotargs : Produce ascii plots." } ,
    { "write", com_write, FALSE, FALSE, TRUE,
      { 1, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "file expr ... : Write data to a file." } ,
    { "compose", com_compose, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 2, LOTS,
      (void (*)()) NULL,
      "var parm=val ... : Compose a vector." } ,
    { "unlet", com_unlet, FALSE, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "varname ... : Undefine vectors." } ,
    { "print", com_print, FALSE, FALSE, TRUE,
      { 040000, 040000, 040000, 040000 }, E_BEGINNING, 1, LOTS,
      arg_print,
      "[col] expr ... : Print vector values." } ,
    { "load", com_load, FALSE, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_BEGINNING | E_NOPLOTS, 1, LOTS,
      arg_load,
      "file ... : Load in data." } ,
    { "cross", com_cross, FALSE, FALSE, TRUE,
      { 040000, 0, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      (void (*)()) NULL,
      "vecname number [ vector ... ] : Make a vector in a strange way." } ,
    { "undefine", com_undefine, FALSE, FALSE, FALSE,
      { 010000, 010000, 010000, 010000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[func ...] : Undefine a user-definable function." } ,
    { "op", NULL, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.op line args] : Determine the operating point of the circuit." } ,
    { "tran", NULL, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.tran line args] : Do a transient analysis." } ,
    { "ac", NULL, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.ac line args] : Do an ac analysis." } ,
    { "dc", NULL, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.dc line args] : Do a dc analysis." } ,
    { "pz", NULL, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.pz line args] : Do a pole / zero analysis." } ,
    { "sens", NULL, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.ac line args] : Do a sensitivity analysis." } ,
    { "disto", NULL, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.disto line args] : Do an distortion analysis." } ,
    { "noise", NULL, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[.noise line args] : Do a noise analysis." } ,
    { "listing", NULL, FALSE, TRUE, TRUE,
      { 0100, 0100, 0100, 0100 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[logical] [physical] [deck] : Print the current circuit." } ,
    { "edit", NULL, FALSE, TRUE, TRUE,
      { 1, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[filename] : Edit a spice deck and then load it in." } ,
    { "dump", NULL, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Print a dump of the current circuit." } ,
    { "fourier", com_fourier, FALSE, FALSE, TRUE,
      { 0, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "fund_freq vector ... : Do a fourier analysis of some data." } ,
    { "show", NULL, FALSE, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "devspecs ... : parmspecs ... : Print out device parameters." } ,
    { "alter", NULL, FALSE, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 3, LOTS,
      (void (*)()) NULL,
      "devspecs : parmname value : Alter device parameters." } ,
    { "altermod", NULL, FALSE, TRUE, FALSE,
      { 040, 040, 040, 040 }, E_DEFHMASK, 3, LOTS,
      (void (*)()) NULL,
      "devspecs : parmname value : Alter model parameters." } ,
    { "resume", NULL, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Continue after a stop." } ,
    { "state", NULL, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "(unimplemented) : Print the state of the circuit." },
    { "stop", NULL, FALSE, TRUE, FALSE,
      { 04200, 04200, 04200, 04200 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[stop args] : Set a breakpoint." } ,
    { "trace", NULL, FALSE, TRUE, FALSE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[all] [node ...] : Trace a node." } ,
    { "save", NULL, FALSE, TRUE, FALSE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[all] [node ...] : Save a spice output." } ,
    { "iplot", NULL, FALSE, TRUE, TRUE,
      { 0200, 0200, 0200, 0200 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[all] [node ...] : Incrementally plot a node." } ,
    { "status", NULL, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Print the current breakpoints and traces." } ,
    { "delete", NULL, FALSE, TRUE, FALSE,
      { 020, 020, 020, 020 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[all] [break number ...] : Delete breakpoints and traces." } ,
    { "step", NULL, FALSE, TRUE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[number] : Iterate number times, or one." } ,
    { "reset", NULL, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Terminate a simulation after a breakpoint (formerly 'end')." } ,
    { "run", NULL, FALSE, TRUE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[rawfile] : Run the simulation as specified in the input file." } ,
    { "bug", com_bug, FALSE, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Report a %s bug." } ,
    { "newhelp", com_ahelp, FALSE, FALSE, TRUE,
      { 010, 010, 010, 010 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[command name] ... : help." },
    { "tutorial", com_ghelp, FALSE, FALSE, TRUE,
      { 023010, 023010, 023010, 023010 }, E_BEGINNING, 0, LOTS,
      (void (*)()) NULL,
      "[subject] ... : Hierarchical documentation browser." } ,
    { "help", com_ghelp, FALSE, FALSE, TRUE,
      { 023010, 023010, 023010, 023010 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[subject] ... : Hierarchical documentation browser." } ,
    { "oldhelp", com_help, FALSE, FALSE, TRUE,
      { 010, 010, 010, 010 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[command name] ... : Print help." } ,
    { "quit", com_quit, FALSE, FALSE, TRUE,
      { 0, 0, 0, 0 }, E_BEGINNING, 0, 0,
      (void (*)()) NULL,
      ": Quit %s." } ,
    { "source", nutcom_source, FALSE, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "file : Source a %s file." } ,
    { "shift", com_shift, FALSE, FALSE, FALSE,
      { 020000, 0, 0, 0 }, E_DEFHMASK, 0, 2,
      (void (*)()) NULL,
      "[var] [number] : Shift argv or the named list var to the left." } ,
    { "unset", com_unset, FALSE, FALSE, FALSE,
      { 020000, 020000, 020000, 020000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "varname ... : Unset a variable." } ,
    { "unalias", com_unalias, FALSE, FALSE, FALSE,
      { 02, 02, 02, 02 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "word ... : Undefine an alias." } ,
    { "history", com_history, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 2,
      (void (*)()) NULL,
      "[-r] [number] : Print command history." } ,
    { "echo", com_echo, FALSE, FALSE, FALSE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[stuff ...] : Print stuff." } ,
    { "shell", com_shell, FALSE, FALSE, TRUE,
      { 1, 1, 1, 1 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[args] : Fork a shell, or execute the command." } ,
    { "rusage", com_rusage, FALSE, FALSE, FALSE,
      { 02000, 02000, 02000, 02000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[resource ...] : Print current resource usage." } ,
    { "cd", com_chdir, FALSE, FALSE, FALSE,
      { 1, 0, 0, 0 }, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[directory] : Change working directory." } ,
    { "version", com_version, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "[number] : Print the version number." } ,
    { "diff", com_diff, FALSE, FALSE, FALSE,
      { 0400, 0400, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      "plotname plotname [vec ...] : 'diff' two plots." } ,
    { "rehash", com_rehash, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Rebuild the unix command database." } ,
    { "while", NULL, FALSE, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "condition : Execute while the condition is TRUE." } ,
    { "repeat", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0}, E_DEFHMASK, 0, 1,
      (void (*)()) NULL,
      "[number] : Repeat number times, or forever." } ,
    { "dowhile", NULL, FALSE, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "condition : Execute while the condition is TRUE." } ,
    { "foreach", NULL, FALSE, FALSE, FALSE,
      { 0, 040000, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      (void (*)()) NULL,
      "variable value ... : Do once for each value." } ,
    { "if", NULL, FALSE, FALSE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 1, LOTS,
      (void (*)()) NULL,
      "condition : Execute if the condition is TRUE." } ,
    { "else", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Goes with if." } ,
    { "end", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": End a block." } ,
    { "break", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Break out of a block." } ,
    { "continue", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Continue a loop." } ,
    { "label", NULL, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 1, 1,
      (void (*)()) NULL,
      "word : Create someplace to go to." } ,
    { "goto", NULL, FALSE, FALSE, FALSE,
      { 0100000, 0, 0, 0 }, E_DEFHMASK, 1, 1,
      (void (*)()) NULL,
      "word : Go to a label." } ,
    { "cdump", com_cdump, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 0, 0,
      (void (*)()) NULL,
      ": Dump the current control structures." } ,
    { "settype", com_stype, FALSE, FALSE, FALSE,
      { 0200000, 040000, 040000, 040000 }, E_DEFHMASK, 2, LOTS,
      (void (*)()) NULL,
      "type vec ... : Change the type of a vector." } ,
    { "strcmp", com_strcmp, FALSE, FALSE, FALSE,
      { 0, 0, 0, 0 }, E_DEFHMASK, 3, 3,
      (void (*)()) NULL,
      "varname s1 s2 : Set $varname to strcmp(s1, s2)." } ,
    { "linearize", NULL, FALSE, TRUE, FALSE,
      { 040000, 040000, 040000, 040000 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      " [ vec ... ] : Convert plot into one with linear scale." } ,

    { 0, NULL, FALSE, FALSE, FALSE, { 0, 0, 0, 0 }, E_DEFHMASK, 0, LOTS,
      (void (*)()) NULL,
      NULL }

} ;
