#!/bin/bash
sudo mkdir /Clock
sudo cp Clock /Clock
sudo cp main.png /Clock
sudo cp theme.css /Clock
sudo cp clock.desktop /usr/share/applications
sudo chmod 755 /Clock/Clock
sudo chmod u+x /usr/share/applications/clock.desktop
