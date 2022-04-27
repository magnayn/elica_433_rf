# Test project for sending 433Mhz commands to Elica cooker hood.

I have an elica cooker hood, with a 433Mhz remote control, that I wanted to automate.

This is a test project just to bit-bang a 433Mhz RF (STX882 in my case) from an ESP8266. I'll probably integrate it into something else / tidy it up.

## Details

Looking at the pulses in a logic analyser attached to a 433Mhz RX unit, the pulses sent are... odd... that they do not seem to be in Manchester Encoded format.

Firstly, they seem to be in batches of 3: either a high-low-low, or a high-low-high (100 or 101). Secondly, the timings also seem to wander a lot - the initial high seems to be slightly longer than the following pulses; the overall 'bit' (100 or 101) seems to be ±1.008ms, which doesn't ring any bells as to a likely clock / baud rate. That said, it could be my somewhat old USB logic analyser not being entirely accurate timings-wise, or a host of other factors.

Anyway, repeating them out this way seems to work.. in that it's about as reliable as the remote control, which is to say 'not 100% reliable'. It feels like something here has been done on the cheap, so perhaps the remote control is only outputting 'good enough' data. It's possible that by tweaking the timings the reliability might be improved. I also suspect the inital batch of '100, 100, 100, 100' preamble is to somewhat bias the reciever -- but it might actually be better to send somethig closer to 50%/50% on/off to train the AGC on the other end.

# Building

Adjust main.cpp with your wifi credentials, use platformio to build it (or maybe arduino, it's pretty simple). Hit the endpoint with /light or /fan/up or /fan/down. (Sadly these are toggles, not ON or OFF or LEVEL).

