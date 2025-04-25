# Cloth Simulation

An implementation of Large Steps in Cloth Simulation (Baraff, Witkin).
Done as a final project for CSE328 at Stony Brook University.

![cloth swinging and ruffling](https://github.com/zanesterling/cloth-simulation/blob/master/resources/recording.gif?raw=true)


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

## Controls

### View controls
 | Key   | Function                          |
 |-------|-----------------------------------|
 | w / s | start rotating altitude up/down   |
 | a / d | start rotating azimuth left/right |
 | e     | stop all rotation                 |
 | q     | quit                              |

### Render settings
 | Key | Function                                          |
 |-----|---------------------------------------------------|
 | y   | toggle cloth colors on / off                      |
 | u   | toggle between wireframe and solid fill rendering |
 | t   | toggle lighting on / off                          |

### Render settings
 | Key     | Function                                        |
 |---------|-------------------------------------------------|
 | p       | toggle simulation pause                         |
 | r       | reset simulation                                |
 | o       | toggle "cuffing": end of cloth becomes narrower |
 | \[ / \] | change to the prev / next scene                 |
