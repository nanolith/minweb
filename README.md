minweb simple literate programming
==================================

The minweb project provides an extremely simple literate programming system, in
which macros and text substitutions can be defined.  The system currently
supports tangling (extracting source code) and weaving (creating documentation)
from the literate file format.  mintangle performs the tangling operation, and
minweave performs the weaving operation.  A third program, called minextract can
be used to extract input to be fed to programs as key-value pairs.

Currently, minweave supports LaTeX. Eventually, support for Pandoc style
Markdown will be added as well. The reason for supporting LaTeX first is because
it is a bit more mature for book writing.  The reason for eventually supporting
Pandoc is because Pandoc style Markdown is much easier for a team to work with.

Macros
------

The literate source files consist of LaTeX documentation.  Macros can be
defined using the `<<name>>=` ... `>>@<<` block directive.  Within this
directive, the name can be further broken down into special directives, such as
the standard `*` name indicating the root of a single source file which must be
named when weaving, or `file:path` which can be used to represent a file name at
the given path.

Additional occurrences of the same macro definition will append that block to
the previous macro definitions.  This allows macro blocks such as a "global
variables" or "build rules" macro block to be woven through the documentation as
it makes sense.

Sections and Text Substitutions
-------------------------------

Text substitutions can be defined using the `%[name]%` directive, which tells
the parser to read a value from the key-value pair matching the given name. To
resolve the key-value pair matching, a section directive is used to match
outputs from an external program to text substitutions within this section
directive.  The section directive is defined using a section macro, which takes
the form of a `<<SECTION:name>>=` .. `>>@<<` block structure.  Unlike normal
macros, these are woven back into the original document as a direct splice of
whatever results from the text substitution, instead of as syntax highlighted
source code. This is useful for writing the inputs for unit tests as tables
within the documentation, so that the results of these tests can be woven back
into the documentation as a tabular view.

A special form of the text substitution directive, `%[name=value]%` can be used
to define a key-value pair that can be extracted via minextract.  The minextract
program searches for auto sections of a given name, and extracts all of the
key-value pairs defined in that auto section.

The execution flow would be to use minextract to extract all sections, then use
the key-value pairs for each section as inputs to whatever program needs to be
run to resolve these.  These are saved as `section_name.input`. The outputs of
these programs, which should also be key-value pairs, are then saved as
`section_name.output` using a user-provided build script.  When minweave runs,
it looks for an appropriately named `section_name.output` files to perform text
substitions in each section.

In the input and output files, the key-value pairs are defined as `key=value`.
The value is everything up to the newline, including additional equal signs.
Anything except a newline can be in the value.  Likewise, anything except an
equal sign or a newline can be in the key.  The reason for this specific
definition is to allow rich text to be returned by the executed program.  For
instance, when running tests, test failures could be highlighted in a certain
color or a bold face.  Depending upon the documentation language, this could
require the use of an equal sign, such as using an HTML span with style
information.

Special Directives
------------------

Two special directives, `#[include=filename]` and `#[language=value]` are
supported by both minweave and mintangle.  The include directive allows document
files to be split across multiple files.  When this directive is encountered by
any of the tools, input will be suspended from the main file and will resume in
the included file.  This support isn't exactly like a preprocessor include in C
or C++.  The include file must have valid tokens, and they can't be unbalanced.
The include path from which files are searched can be overridden with the `-I
includepath` argument for each of the tools.

The language special directive allows the default language mapping to be
overridden for the next section.  For instance:

    #[language=haskell]
    <<FILE:test.hs>>=
    module Main where
    
    <<main-function-definition>>
    
    >>@<<

Minweave Template
-----------------

When a template is specified with the `-T templatefilename` argument, minweave
will write the preamble to a special macro reference called `preamble` and the
woven document to a special macro reference called `*`.  These can be used in a
custom template file that can provide additional LaTeX options to build a custom
document.  For example:

    Begin Template
    
    <<SECTION:preamble>>=
    preamble goes here: <<preamble>>
    >>@<<
    
    <<SECTION:document>>=
    Document goes here: <<*>>
    >>@<<
    
    End Template

Using these tools
-----------------

I find that it is easiest to use these tools in a separate build directory so
that all temporary files such as section inputs / outputs or LaTeX artifacts can
be contained outside of the source directory.
