# YAAR | SD-CARD READER

a minimal sd-card browser built on the rp2040, featuring an oled display and a morse-code-based file search system.

---

## DEMO

https://github.com/user-attachments/assets/b6927699-ecf7-4c9a-bae1-4a8d916d4dac

---

## COMPONENTS

* rp2040 dev board
* ssd1306 oled display module
* sd card breakout (spi interface)
* keyswitches

---

## FEATURES

* browse files directly from an sd card
* search files using morse code input
* simple ui rendered on ssd1306 oled display
* physical key input using keyswitches

---

## BUILD INSTRUCTIONS

### 1. setup rp2040 sdk

install the pico c/c++ sdk, then export the sdk path

---

### 2. build the project

```bash
cmake -S . -B build
cmake --build build
```

---

### 3. flash firmware

* locate the generated `.uf2` file in the `build` directory
* hold the bootsel button on the rp2040 board
* connect the board to your computer via usb
* copy the `.uf2` file to the mounted drive

---

## HOW IT WORKS

* reads files from the sd card over spi
* accepts input via keyswitches
* interprets morse code sequences to search filenames
* displays results on the oled screen

---


## LICENSE

mit license

---

