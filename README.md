# ESP32 based RGB light animations

This is a project to run RGB LED light strings with a choice of animations
all controlled via a web front end.

The default operation is to create a WiFi AP called '''rgblights''' with the password '''unsecure''' which can be changed in the '''setup''' and then accessed by going to http://rgblights.local// address. If you want it to connect to a local WiFi network, then add WiFi credentials to wifipass.h file and change the '''if (false)''' in the '''setup''' function to make it start.
