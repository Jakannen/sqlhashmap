There are two starter files that you will need: TableEntry.h and
TableEntry.cpp.

Also look at the file Error_messages.txt.  There are only two
"standard" error messages for this project that will be sent to
cerr (the other errors, such as a table name that doesn't exist,
are sent to cout).  You will be shown your cerr output if:

1) It is not an INV test case (supposed to exit 1).
2) Your exit status is 1.
3) The first line of text send to cerr matches a line in that file.

If you want to add a second line, that's fine, but we won't display
it to you.  For example, this is valid:

    cerr << "Error: Unknown command line option" << endl;
    cerr << "  The argument seen was: " << choice << endl;
    exit(1);

In this case, we would show you the first line of output in the
autograder feedback.
