# dmon-duino
:hash: Arduino interface for Realtime D-MON

***
### Intro

This project is developed as an Undergraduate Final Year Project and is in eternal beta mode.

There are three parts to this project:

1. [dmon-app](https://github.com/Zyten/dmon-app)
2. dmon-duino - This repository
3. [dmon-web](https://github.com/Zyten/dmon-web)

###### Project Page: [zyten.github.io/dmon-web](http://zyten.github.io/dmon-web)

### Synopsis

This repository hosts the code used by the Arduino interface in Realtime D-MON. The Arduino sketch reads the sensor values and transmits them to a hosted PHP script while the PHP script, which is also kept in this repository for clarity, transit the sensor values to ThingSpeak to be made accessible to the other interfaces (Android and Web).

### License

![MIT license](https://img.shields.io/npm/l/express.svg)
