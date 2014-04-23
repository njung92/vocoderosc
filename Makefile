fx_example:
	gcc -o bin/fx_example -Wall -Wextra src/fx.c src/fx_example.c

nbs:
	gcc -o bin/nbs -Wall -Wextra src/nonblocking_server.c src/synth.c src/saw.c src/utils.c -llo

stream:
	gcc -o bin/stream -Wall -Wextra src/stream.c src/synth.c src/saw.c src/nonblocking_server.c src/utils.c -llo -lportaudio

saw:
	gcc -o bin/saw -Wall -Wextra src/saw.c src/utils.c -llo -lm

synth:
	gcc -o bin/synth -Wall -Wextra src/synth.c src/saw.c src/utils.c -llo -lm

synthserv:
	gcc -o bin/synthserv -Wall -Wextra src/synthserv.c src/synth.c src/stream.c src/nonblocking_server.c src/utils.c src/saw.c -llo -lportaudio

at:
	gcc -o bin/at src/attenuator.c -Wall -Wextra -lm

vol:
	gcc -o bin/vol src/volumedetector.c -Wall -Wextra	

bp_test1:
	gcc -o bin/bp_test1 -Wall -Wextra src/fx.c src/bp_test1.c src/bandpass.c src/utils.c
