[![GitHubBuild](https://github.com/bkthomps/GeneticMultiplexer/workflows/build/badge.svg)](https://github.com/bkthomps/GeneticMultiplexer)
[![Language](https://img.shields.io/badge/language-C++17-orange.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/bkthomps/GeneticMultiplexer/blob/master/LICENSE)

# GeneticMultiplexer

Uses genetic programming to compute the decision tree of a multiplexer.

## Setup

To compile, run `make` and then you can run `./gen_mux <address_pins>` where `address_pins` is the
number of address pins you would like the multiplexer to contain.

## What is a multiplexer?

A multiplexer is a circuit component that contains data pins, address pins, and an output pin. All
of these pins are binary values.

The data pins are inputs, and the output is the value of one of the data pins, but it depends on
the values of the address pins. The address pins map to the data pins, meaning that a multiplexer
with two address pins has four data pins, and a multiplexer with three address pins has eight
data pins.

![Three Address Multiplexer](images/3_address_mux.png "Three Address Multiplexer")
