# Cloth Simulation

An implementation of Large Steps in Cloth Simulation (Baraff, Witkin).
Done as a final project for CSE328 at Stony Brook University.

## Setup

### MacOS
To build the project, you will first need to install
[Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)
and [freeglut](https://freeglut.sourceforge.net/).

```shell
$ brew install eigen freeglut
```

Next update the `PLATFORM_INCLUDES` in the `Makefile` to point to the include directories for your versions of each library.

## Run

```shell
$ make run
```

Click and drag left and right to adjust the cuffing of the cloth.
