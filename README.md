# <img src="html/img/favicon.png" width="32" height="32" alt="ESP32AsyncBuffer" style="vertical-align: top" /> ESP32AsyncBuffer Advanced Example 

This example shows an advanced single page app using ESP32AsyncBuffer and
demonstrating a simple API Explorer with websocket commands and data streams.
It is also shows use of large `css` file and using a javascript framework like `PreactJS`.

## Generate Sources
You will first need to run the node script to generate the static HTML files.
```shell
# From your project root
node ~/Documents/Arduino/libraries/ESP32AsyncBuffer/GenerateSources.js
```

Now you can build and upload this example.


## Run

Once the firmware is flashed to the esp32 you should see an Access Point 
named `AsyncBufferAdvanced-xxx`. When you connect to it a Captive Portal
will open and allow you to setup your own network credentials.

Have a look at:
* `html/index.html` to modify the API Tester app or write your own.
* `html/captive_portal.html` to modify the captive portal greeting.
