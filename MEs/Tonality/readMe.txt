Hi all,

here's a very reduced version of Tonality3, which should be suitable for your first test.

For using it, you need the instrument samples we have for Piano and Vibraphone. In case you don't have them on the machine you're working on, you have to download them from Mega. Robert has access to it, or maybe you too, Tobias.
You can find them in:
"MC2 software/MC2,5/Tonality2/Samples/
The folders you need, are called "OldBlackGrand" and "Vibraphone".
Just download and copy them into the "Samples" folder, found in the main directory of the patch I just sent.

After that, just start the main Pd file "ME_tester.pd". It was written in Pd 0.48, but should also work with whatever version you have.
Some basic infos about the relevant OSC stuff can be found on the crappy interface. If you're not sure, if your data streams are received like expected, there's a small checkbox labeled with "Print TM messages" at the bottom of the patch, which enables printout in Pd's console. All data received on port 6160 is visible as written text then. Otherwise there are also the small faders and buttons on the GUI, monitoring the incoming data.

If the connection properly works and the soundfiles are in the right place, you only have to hit the "Play/Stop" toggle and turn up the "Main Volume" on the right. Depending on the soundcard of your machine, you might have to make small adjustments in Pd's "Audio Settings" in the "Media" menu. But that's all I think.

I hope I didn't forget anything, since I have trouble concentrating right now. But well, if you have any problems with the patch, just let me know.

Best regards
Ives
