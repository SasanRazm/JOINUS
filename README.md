# JOINUS
Graphical user interface for the Josephson simulator programs.

*This is the test version for reporting the bugs!*

# FOREWORD
*Josephson interface utility software* (JOINUS) is a program developed by *Sasan Razmkhah* at Université Savoie Mont Blanc, France as part of COLDFLUX/SuperTools project. The first objective is to simplify the design and simulation process of analog, digital and mixed signal circuits based on Josephson junctions. The second objective is to improve the accuracy of models for simulations and the features of software tools. JOINUS uses JSIM or JoSIM simulation engines for digital circuits. It will include later more simulators such as WRSPICE, an improvement of SPICE3 , or PSCAN. JOINUS embeds a powerful built-in plotter to automatically plot output results. Other open-source plotters like GNUPlot or XMGrace are also available from the JOINUS interface.

# Ubuntu Installation
For Ubuntu, QT cannot work with clang 10+ and needs clang 8. If you are on new version of Ubuntu and get errors related to not recognizing QT core libraries, run the following commands:

`sudo apt install clang-8`

`sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-8 100`

`sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-8 100`

# LICENSE
Permission is granted to anyone to make or distribute verbatim copies of this document as received, in any medium, provided that the copyright notice and the permission notice are preserved, and that the distributor grants the recipient permission for further redistribution as permitted by this notice.
- *Linux* is a registered trademark of Linus Torvalds.
- *Windows* is a registered trademark of Microsoft Corporation.
- *macOS* is a registered trademark of Apple, Inc.
- *QCustomPlot* is the property of Emanuel Eichhammer.
All trademarks are the property of their respective owners.

The software is based upon work supported by the Office of the Director of National Intelligence (ODNI), Intelligence Advanced Research Projects Activity (IARPA), via the U.S. Army Research Office grant W911NF-17-1-0120. The views and conclusions contained herein are those of the authors and should not be interpreted as necessarily representing the official policies or endorsements, either expressed or implied, of the ODNI, IARPA, or the U.S. Government. The U.S. Government is authorized to reproduce and distribute reprints for Governmental purposes notwithstanding any copyright notation herein.
