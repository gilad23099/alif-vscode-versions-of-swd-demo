# VSCode Getting Started Template
A simple Blinky project to setup and test VSCode and GNU tools based development environment.

The default main branch is set to support Gen 2 Ensemble devices. "gen1" branch must be used
for older Gen 1 devices.

To build the template for a supported board other than the DevKit, you may have to download
the Board Library submodule. From your local clone, do the following:
1. %submodule init%
2. %submodule update%
3. Update the board.h file to pick the right variant of the board

Please refer to the [Getting Started Guide](https://alifsemi.com/download/AUGD0012) for more details.