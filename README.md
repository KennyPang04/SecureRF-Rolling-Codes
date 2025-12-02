# SecureRF - Rolling Codes
Security is everywhere. It can be seen in everyday gadgets that most people would never have thought about. An example of a secure system is your car key remote. In this repository, we create a lightweight demonstration of this system and how each hardware and software plays an important role in building a secure embedded system.

In this project, we implement an old technique called the rolling code (also known as hopping codes), where a shared secret key and synchronized counter encrypts a new code on every transmit. While the receiving end would decrypt and does the comparison between expected and actual values. This algorithm is considered a bit outdated because it prevents simple replay attacks, but still can be exploited by other more sophisticated attacks.

The project is set up where the car key fob is the transmitter and the car internal system is the receiver. For more details about the project, such as design notes, software planning, testing, and other diagrams, you can check the PDF in the documentation folder!

## Built With
* ![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
* ![AES](https://img.shields.io/badge/AES%20Cryptography-4B9CD3?style=for-the-badge&logo=databricks&logoColor=white)
* ![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
* ![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
* ![VS Code](https://img.shields.io/badge/VSCode-007ACC?style=for-the-badge&logo=visualstudiocode&logoColor=white)
* ![Git](https://img.shields.io/badge/Git-F05032?style=for-the-badge&logo=git&logoColor=white)

## Getting Started

### Accomplishments

### Challenges

## Features

## What's Next?