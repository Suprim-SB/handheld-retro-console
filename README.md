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


#Bill of Materials:
This bill is mostly fit for the country from where I am building this (Nepal), if if it isn't possible to buy these items from where you are, you could resort to international vendors.
| Item | Link | Price (USD) |
|---|---|---:|
| Seeed Studio XIAO ESP32-S3 | [Daraz Nepal](https://www.daraz.com.np/products/i1546190932-s12368458639.html) | $9.56 |
| 0.96-inch OLED Display SSD1306 128×64 | [Giga Nepal](https://www.giganepal.com/product/0-96-inch-oled-display/) | $3.87 |
| DHT11 Temperature and Humidity Sensor | [Giga Nepal](https://www.giganepal.com/product/temperature-humidity-sensor-dht-11/) | $1.93 |
| GY-521 MPU6050 Accelerometer and Gyroscope | [Giga Nepal](https://www.giganepal.com/product/mpu6050-3-axis-accelerometer-gyroscope-sensor/) | $3.22 |
| DIY DSA Keycap (×2) | [Daraz Nepal](https://www.daraz.com.np/mechanical-keyboard-accessories-diy-dsa-keycap-multiple-color-for-mx-switches-keyboard-key-cap-durable-not-engraving-blank-key-cover-i169794078-s1186614213.html) | $2.60 |
| HMX Taro Ball Linear Switch (×2) | [ErgoKeys Nepal](https://ergokeysnepal.com/product/hmx-taro-ball-linear-switch/) | $0.84 |
| **Total** | | **$22.03** _(Without Shipping)_|

_Prices may fluctuate_
<br>_PCB prices (JLCPCB)
<br> PCB price = $10.30
<br> Delivery fee (the resonably priced one takes long 18-24 days) = $15.33 OR a faster one at ~35$ for shipping
<br> Total = 25.63 (but with a coupon for new user it becomes 15.33)

<br>So pcb + other parts = $37.36 (Over the 30$ grant)
<br> So i would fund the rest with my own budget


<br><img width="1313" height="698" alt="image" src="https://github.com/user-attachments/assets/76f4387f-3865-43fc-ae94-438269b520c5" />


<img width="1209" height="508" alt="image" src="https://github.com/user-attachments/assets/b8042103-e81b-497b-a573-3017fd8e17cf" />
<img width="1253" height="495" alt="image" src="https://github.com/user-attachments/assets/7a65ec73-e1d3-4444-a52a-7d95d7134755" />
<img width="1313" height="693" alt="image" src="https://github.com/user-attachments/assets/09e8c3e1-4651-4751-873e-6ea7fa75f65b" />
