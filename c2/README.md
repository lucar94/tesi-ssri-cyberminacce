# C2 - Command and Control

## This project is for educational purposes only.

### How it works

This is a simple C2 server written in Java using Spring Boot. To run it, use the command: `mvn spring-boot:run`.

The server simulates a remote C2 environment, with a database where victim information is stored. This project is used in conjunction with ransomware and MiTB (Man-in-the-Browser) attack simulations.

To access the database, a console is available at: `http://localhost:8080/h2-console`. Credentials can be found in the `application.yml` file.