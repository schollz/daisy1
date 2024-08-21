declare name "sallenKeyOnePoleLPF";
declare description "Demonstration of the Sallen-Key One Pole Low-Pass Filter";
declare author "Eric Tarr";

import("stdfaust.lib");

normFreq = hslider("freq",0.5,0,1,0.001):si.smoo;

process = ve.sallenKeyOnePoleLPF(normFreq);