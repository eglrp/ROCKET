# rocket
Real-time Orbit determination and ClocK Estimation toolkit 

The ROCKET is short for the Real-time Oribt determination, ClocK Estimation 
Tookit, which is based on GPSTK. 

In order to compute satelite orbits, clock and upds in real-time, the 
structure of the GPSTK is re-designed and pulled out as a independent
software.

The software abosorbs the merits of several softwares, including
GPSTK, GAMIT, BNC and RTKLIB.

Further plan of the ROCKET software includes:

(1) one kalman filter for orbit, clock and upd estimation
(2) real-time processing 
(3) four-constellation GNSS satellite orbit determination.
...
