# Normalized Compression Distance

## What is this?

The NCD (Normalized Compression Distance) algorithm lets you calculate the
similarities between a set of files so you can plot them as a hierarchical
cluster. The resulting image lets you easily detect relationships in your
data.

## For example?

Say, you have a set of DNA sequences. Coincidentally there is one in
[this directory](https://github.com/MKleusberg/ncd/tree/master/data/dna). In
an ideal world you could now analyse the information stored in these sequences
and decide about which species are related on this basis. Unfortunately though,
it's not possible to calculate this so-called information distance. However,
there is an approximation, the normalised compression distance which estimates
the information distance by using a compression algorithm on the files. You
might want to read the [Wikipedia article](https://en.wikipedia.org/wiki/Normalized_compression_distance)
for more details.

This is what this program does. The result is a text file containing the
similarities in a matrix, a huge pile of numbers. This data can then be
plotted using a tool like R. If you don't know R, it might be best to start
by having a look at a graphical frontend like [RStudio](http://www.rstudio.org).

This should give you an image like this one:
![DNA example](https://github.com/MKleusberg/ncd/raw/master/example.png "DNA example")

## Is it any good?

[Yes.](https://news.ycombinator.com/item?id=3067434)

## How do you build it?

* Setup build environment. I'm using LLVM/Clang but g++ works just as fine
* Install dependencies (LZMA library and Boost filesystem library)
* make all
* make run

## How can I run run it?

    $ ./cluster input-dir output-file

This reads *all* files in the specified input directory and performs an NCD
calculation on them. The result are written to the specified output file
which is overwritten without warning if it already exists.

You can then use the included plot.R file to plot the NCD matrix.

## Any prebuilt executables available?

No.

## More questions?

Feel free to just open a new issue.
