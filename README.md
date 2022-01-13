# gnuplot-bitmap

Small experiment looking at displaying a black and white image with gnuplot.

## Usage

Prerequisites:

- A C compiler. clang is the default, but it also works with GCC; just change `CC` in the `Makefile`.
- GNU make
- gnuplot

Steps:

- Clone this repository with `--recurse-submodules` in order to fetch the STB header.
	- If you already cloned without that option, run `git submodule update --init --recursive` to fetch it after the fact.
- Compile:

```sh
$ make
```

- Now run `./gnuplot-bitmap` with whichever options you like.

```
usage: ./gnuplot-bitmap -i infile -o outfile [-Idh] [-t threshold] [-a alpha_threshold]
    -i infile:          image to use as input. most common formats are supported.
    -o outfile:         PDF output file.
    -t threshold:       (default 128) pixels with grayscale values below (default) or
                        above (with -I) this are plotted. 0-255.

    -a alpha_threshold: (default 128) pixels with alpha values below this are not
                        plotted, no matter their grayscale value. 0-255.

    -I:                 plot pixels above threshold instead of below.
    -d:                 print a data file to stdout instead of plotting anything.
                        outfile is not required, but may be used to send data to a file
                        instead of stdout. each line is of the form 'x y'. y coordinates
                        are negative.

    -h:                 display this help and exit.
```
