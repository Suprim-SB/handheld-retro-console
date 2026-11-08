# HANDHELD RETRO CONSOLE
A diy handheld retro console that is built to be simple, cheap and small. It includes classic retro games that you can play to spend your time!
<img width="987" height="868" alt="image" src="https://github.com/user-attachments/assets/269e4eaf-958d-4553-8293-6915fe6f3a2e" />
<img width="1038" height="874" alt="image" src="https://github.com/user-attachments/assets/0f7331a1-5604-4e29-8a73-bfa1d3ea6337" />



# WHAT IS IT?
A handheld console is a game device meant to fit in the palm of your hands!. Although they are not as powerful as traditional consoles, they are quite nice to help you pass your free time instead of just doomscrolling. And with how everyone larps "anti-brainrot edcs", this project could fit right in :> This handheld console packs enough punch to handle classic retro games like snake, Tetris, brick breaker, Pacman and so on. It includes a 1.3 inch monochrome oled screen to view your games. Its intended feature is supposed to be a compact device you can carry with you anywhere. It also includes a rechargable battery so no hassle of replacing batteries everytime the battery dies.
<br>
<br>_This build of the console includes Snake, Tetris, Pacman, Brickbreaker and a Traffic racer (More games can be added on your own)._
# WHY?
This project was made to be the project for the week 5 of Hack Club half life. It is my first time creating a project trying to learn on my own without following a tutorial word for word. It helped me understand Kicad and project making more.

# WHAT YOU NEED
Despite the project sounding complicated its not that complicated. Since this is a simplified version of the project with a oled instead of tft and less buttons and no speakers (to fit budget and size constraints), this does require a long list of expensive materials.
MATERIALS YOU NEED:
1. ESP32-C3 Supermini (The main microprocessor)
2. 1.3" Oled SH1106 (The display for the game)
3. 6 Tactile Push Buttons (6mmx6mm) (The four directional input and two action buttons)
4. 220Ω Ohm Resistor
5. Protected TP4506 charging module (The battery charger)
6. 6x Push button caps
7. Slide Switch (to turn on and off the console)
8. A 3.7v LiPo Battery pouch (The battery; for this build I was only able to find a 1100MaH battery, but you can use one with higher capacity)

# How the parts fit:
The ESP32-C3 supermini is the main brains of the project. It is what controls the display, loads the games, and takes your inputs. The 1.3" Oled is the display. It shows you the boot animation, the game menu, and displays the games while you play them. The buzzer plays different frequencies depending on what is happening to make the console more polished and fun to use. The tactile input switches are to select the game you want to play from the menu and to control the game. The battery and charging modules are just a part of the module that powers the console.

# Project Status
Designing Phase is complete. The physical board is yet to be made, tested , and shipped.
The PCB design, firmware, Schematics, Gerber Files and the BOM are uploaded here.
<br>_Further changes to the modes, features and designs could be added later_

# Project Photos:
PCB:
<br><img width="906" height="819" alt="image" src="https://github.com/user-attachments/assets/578a5e15-a5c8-4692-a8ec-a5b108a31b00" />
<br><img width="898" height="819" alt="image" src="https://github.com/user-attachments/assets/8062367e-150d-4f98-9020-2693ef7cda8a" />

<br>Schematic:
<br><img width="545" height="374" alt="image" src="https://github.com/user-attachments/assets/3999a16e-a1f2-42a0-8c06-f0fc1aef9cf0" />


## Bill of Materials (BOM)

| Item | Qty | Supplier | Cost/Item | Total Cost | Link |
|---|---:|---|---:|---:|---|
| ESP32-C3-Supermini | 1 | Ekaithau | $4.65 | $4.65 | [Link](https://ekaithau.com/products/esp32-c3-mini/) |
| 1.3" OLED | 1 | Himalayan Solutions | $4.00 | $4.00 | [Link](https://himalayansolution.com/product/1-3-inch-oled-display) |
| Buzzer | 1 | Himalayan Solutions | $0.20 | $0.20 | [Link](https://himalayansolution.com/product/buzzer) |
| LiPo Battery 3.7V | 1 | Giga Nepal | $3.66 | $3.66 | [Link](https://www.giganepal.com/product/3-7v-1100mah-li-po-rechargeable-battery-original-type/) |
| TP4056 Module | 1 | Himalayan Solutions | $0.33 | $0.33 | [Link](https://himalayansolution.com/product/type-c-5v-1a-tp4056-1s-lithium-battery-charger) |
| Push Buttons (5 pack) | 2 | Giga Nepal | $0.66 | $1.32 | [Link](https://www.giganepal.com/product/push-button-switch/) |
| Slide Switch | 1 | Himalayan Solutions | $0.10 | $0.10 | [Link](https://himalayansolution.com/product/slide-switch-small) |
| PCB Printing | 1 | JLCPCB | $2.00 | $2.00 | [Link](https://cart.jlcpcb.com/shopcart/cart/) |
| Passive Components | 3 | Himalayan Solutions | $0.22 | $0.66 | [Link](https://himalayansolution.com/product/capacitor-solid-state-100uf-35v) |
| Estimated Shipping | 1 | - | - | $12.87 | - |
| **Total** | | | | **$29.79** | |
<br> *The photos of carts are uploaded below*


<img width="959" height="538" alt="1" src="https://github.com/user-attachments/assets/48ea1b12-be4f-43aa-a2b7-3698d0fd1bb8" />
<img width="959" height="539" alt="2" src="https://github.com/user-attachments/assets/cd4084d7-aa62-4527-aa81-e23a98c8b917" />
<img width="926" height="485" alt="3" src="https://github.com/user-attachments/assets/63d566e6-8aea-4108-bd74-db77ae6f592b" />
<img width="928" height="484" alt="4" src="https://github.com/user-attachments/assets/04a7ef7e-e02d-442b-a6e9-bdd3739ada28" />
<br> *Ignore the shipping prices, I am getting them physically*

