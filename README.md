# FSM - interpreter of SDL (Specification and Description Language) processes

Create an executable finite state machine using the [SDL language](https://en.wikipedia.org/wiki/Specification_and_Description_Language) and integrate it
into your code.


## Features

- C++ implementation
- interpretation of SDL processes
- call of user defined functions
- generation of SDL/GR diagrams

## Requirements

- C++11 compiler
- boost system, date_time
- own libraries: anyvalue utils scheduler make_tools

## Building an example


``` bash
git clone https://github.com/trodevel/anyvalue.git
git clone https://github.com/trodevel/make_tools.git
git clone https://github.com/trodevel/scheduler.git
git clone https://github.com/trodevel/utils.git
git clone https://github.com/trodevel/fsm.git
cd fsm
export BOOST_PATH=...your_boost_directory...
make
```

## Example

TBD

## Generation of SDL/GR diagrams

For generation of SDL/GR diagrams the following software is required:

- graphviz
- ps2pdf
- [sdl.ps](https://github.com/trodevel/Ruby-Graphviz/blob/master/examples/sdlshapes/sdl.ps)

## Contact info

I'll appreciate your feedback. Please feel free to contact me at trodevel@gmail.com. Thanks!

## TODOs

- implement loader and parser of SDL/PR files

