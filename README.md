# ScoreCore
ScoreCore is a modular LED scoreboard system based on an ESP8266, featuring custom-designed driver boards and segment panels. Open design, non-commercial license.


# The idea💡

The Idea of ScoreCore was born after three electronics trainees from germany decided to design and build a modular, low cost scoreboard for a local football club.
The goal (pun intended) was to design a scoreboard system that wouldn't break the bank, be easy to use and modular so everything could be easily switched out for a easy repair or extended if that was needed.

  ## The first version
  The first version was put together with simple materials we either had laying arround or could by online.
  It was based arround an Arduino Mega together with a custom 3,3V in 12V out LED Driver to drive the LED and an ESP8266 paired with a RTC for the UI
  and time keeping.

  The Hardware was made out of wood and PVC panels with LED strips attached as the seven-segment-displays.
  Over time we noticed that this would work for the first version but if we wanted to build this project again, we would need to make the design both
  simpler and more efficient. 
  
  That's when we started

  ## Version Two

  The goals for V2 were simple. Make the design simpler, more efficient and maybe even cheaper than the first version.
  V2 started with a bunch of designs. First modular drivers, than plug-in ones but ultimately we setteled on a version that was mounted directly to
  the 7 segment displays, connects via an XT90(2+2) connector and communicates to the controller over I²C.
  Same for the Controller. First there was one that connected over RJ45 Connectors, then one where the drivers could plug in but ultimately we decided
  on one that took a XT90(2+2) for input power and more XT90(2+2) for the outputs.
  
  These changes kept the modularity of the system, but shrunk down the sice drasticly.🥳





---

## 📜 License

ScoreCore is released under a mixed license model:

### Hardware (PCB, schematics, mechanical design)
Licensed under **Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)**.

You are free to:
- Share and redistribute the design files
- Modify and improve the hardware
- Build the system for personal and non-commercial use

Under the following conditions:
- Attribution must be given to the original author
- Commercial use is not permitted without written permission
- Derivative works must be licensed under the same terms

License text: https://creativecommons.org/licenses/by-nc-sa/4.0/

---

### Firmware / Software
Licensed under the **MIT License** for non-commercial use.

Commercial use of the firmware requires written permission from the author.

---

### Documentation, images and media
Licensed under **CC BY-NC-SA 4.0**.

---

## Commercial Licensing

Commercial use of any part of this project (hardware, firmware or complete systems) is not permitted under the default license.

If you are interested in producing, selling or integrating ScoreCore into a commercial product, please contact:

📧 dev@justricity.de

---

Copyright © 2026 Justricity
