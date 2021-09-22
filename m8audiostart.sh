Environment="JACK_NO_AUDIO_RESERVATION=1"
LimitRTPRIO=95
LimitMEMLOCK=infinity
#export DISPLAY=:0
#sudo startx &
sleep 3
/usr/bin/jackd -R -P 95 -d alsa -d hw:M8 -r 41000 -n 3 -p 512 -S -s &
sleep 3
/usr/bin/alsa_out -j m8out -d hw:0 -r41000 &
sleep 2
/usr/bin/jack_connect system:capture_1 m8out:playback_1
/usr/bin/jack_connect system:capture_2 m8out:playback_2
sleep 3
aconnect 24:0 28:0
#sudo antimicro &
sleep 3
sudo ./m8c
