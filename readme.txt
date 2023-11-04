SECL - Structured Esoteric Concatenative Language - tdwsl 2023

SECL is a simple esoteric programming language. It supports functions with
single-character names and it's sole branching structure is a while loop,
with the ability to have multiple clauses. It's stack-based, and is simple to
implement. It uses a 16-bit address space and word size, has a data stack that
cannot be directly addressed and a return stack that can, and the lower 16
bytes of the return stack is reserved as a zero-page for temporary values.
The entry point of code is 1000h, giving 4k of stack space. The data stack
can hold up to 256 words. The return stack expands positively, which is
convenient for addressing bytes on little-endian as there are only load and
store word instructions.

As stated previously, SECL goes character by character.
Reserved characters are as follows:

0-9/a-f   variable-length, push an integer literal to the stack
/         does nothing, seperate integer literals
+         add
-         subtract
&         and
|         or
^         xor
=         boolean not, i.e. 0 = 1, anything else = 0
%         shift right by
<         shift left by
#         duplicate top of stack
~         drop top of stack
*         evaluate code outside of function
?         get return stack pointer
$         set return stack pointer
(         define a function
)         terminate a function, can be used multiple times
[         begin loop
:         the while clause of a loop (can have none or multiple)
]         end loop
"         begin/end a string literal as bytes in memory
\         push following character code to the stack
,         cancel previously defined push and replace with a word in memory
'         get address of function
>         call function
!         store word
@         load word
;         start a comment, terminates at eol
.         interrupt, execute a system defined function
_         next word in memory taken as a literal

Whitespace can be used freely, but other characters will be interpreted as
instructions - non-reserved characters won't do anything, but they will occupy
space in memory.

For more context, look at the included examples.

