raspigreen
==========

Raspberry Pi Greenhouse Climate Control

This project is a for-fun project made with the intention of controlling the climate in a small indoor greenhouse.

The control of the climate will be acheived by reading sensor measurements and controlling actuators and printing information of the status on a 1602LCD display and uploading the measurements to google docs speadsheet. This requires a series of drivers to be developed:

- Funduino 1602 I2C Driver
- DHT11 Driver
- BH1750 I2C Light intensity sensor
- Digital power amplifier for water pumps and vent fans
