# uNode - Arduino Library [![Badge](https://img.shields.io/badge/Read-More-blue.svg)](https://www.tlab.gr/projects/μnode-project)

> The hardware abstraction library for the uNode v1.1 board from TLab.gr

![uNode](/docs/unode-large.jpg?raw=true "uNode v1.1 Board")

## Documentation

This repository is currently being renovated. More information will be available on the README and on the Wiki page soon.

## Changelog

#### 0.7.0

* First release circulated with beta testers

#### 0.8.0

* **CHANGED** : Swapped `.undervoltageProtection` and `.serialLogLevel`
* **CHANGED** : `.serialLogLevel` enum is now `.logging` structure.
* **CHANGED** : `.undervoltageProtection` boolean is now `.undervoltageProtection` structure.
* **ADDED** : Logging baud rate can be configured via `.logging.baud`
* **ADDED** : Under-voltage protection limits can be now configured in the `.undervoltageProtection` structure. 
* **ADDED** : `.tx_sf`, `.tx_power` and `.adr` on LoRa configuration segment.

## Closed-Source Features

The following features are closed-source and they are only available on the binary release of the library:

- **WiFi OTA** for firmware flashing
- **WiFi Web UI** for fine-grained parameter configuration
- **WiFi Geo-location** for geolocating the node using WiFi access points
- **Low-power Optimizations** such as faster WiFi scanning
