/*
 * http_server_example.c
 *
 *  Created on: Nov 15, 2025
 *      Author: controllerstech
 */

#include "main.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "httpServer/httpServer.h"
#include "httpServer/httpParser.h"
#include "httpServer/httpUtil.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define HTTP_MAX_SOCK 2
uint8_t sockList[] = {0, 1};

uint8_t rx_buf[1024];
uint8_t tx_buf[1024];


// --- LED Control Function ---
extern void led_Control(uint8_t state);
extern void pwm_Set(int val);
extern void getBME (float temp, float pres, float hum);


const char index_page[] =
"<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='UTF-8'>"
"<title>W5500 Control Panel</title>"
"<style>"
"body{font-family:Arial;background:#eef;padding:20px;}"
".card{background:#fff;padding:20px;border-radius:10px;margin:15px 0;box-shadow:0 0 10px #aaa;}"
"button{padding:10px 20px;margin:5px;}"
"</style>"
"</head>"
"<body>"

"<h2>W5500 Web Control Panel</h2>"

/*-------------------- LED Control ------------------------*/
"<div class='card'>"
"<h3>LED Control</h3>"
"<button onclick=\"setLED('on')\">LED ON</button>"
"<button onclick=\"setLED('off')\">LED OFF</button>"
"<div id='ledStatus' style='margin-top:10px;font-weight:bold;'>Status: Unknown</div>"
"</div>"

"<script>"
"function setLED(state){"
" fetch('led.cgi',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},"
" body:'led='+state})"
" .then(r=>r.text())"
" .then(t=>{document.getElementById('ledStatus').innerHTML='Status: '+t;});"
"}"
"</script>"

/*-------------------- PWM Control ------------------------*/
"<div class='card'>"
"<h3>PWM Control</h3>"
"PWM Value: <span id='pwmVal'>128</span><br>"
"<input type='range' id='pwmSlider' min='0' max='255' value='128' "
"oninput='pwmVal.innerHTML=this.value; sendPWM(this.value);'>"
"</div>"

"<script>"
"function sendPWM(v){"
" fetch('pwm.cgi',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},"
" body:'pwm='+v})"
" .then(r=>r.text())"
"}"
"</script>"

/*-------------------- Sensor Data ------------------------*/
"<div class='card'>"
"<h3>Sensor Data</h3>"
"Temperature: <span id='temp'>0</span> °C<br>"
"Humidity: <span id='hum'>0</span> %<br>"
"Pressure: <span id='pres'>0</span> hPa<br>"
"</div>"

"<script>"
"function updateSensors(){"
" fetch('sensor.cgi')"
" .then(r=>r.json())"
" .then(data=>{"
" document.getElementById('temp').innerHTML = data.temp;"
" document.getElementById('hum').innerHTML = data.hum;"
" document.getElementById('pres').innerHTML = data.pres;"
" });"
"}"
"setInterval(updateSensors, 5000);"
"updateSensors();"
"</script>"

"</body>"
"</html>";





// HTTP Server Setup
void httpServer_setup(void)
{
    httpServer_init(tx_buf, rx_buf, HTTP_MAX_SOCK, sockList);

    reg_httpServer_cbfunc(NVIC_SystemReset, NULL);

    /* Register Static pages */
    reg_httpServer_webContent((uint8_t*)"index.html", (uint8_t*)index_page);
}

// Call this inside the while loop
void httpServerRun (void)
{
    for (int sn = 0; sn < HTTP_MAX_SOCK; sn++){
    	httpServer_run(sn);
    }
}

int count  = 0;
uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len)
{
	if (strstr((char*)uri_name, "sensor.cgi"))
    {

		float Temperature=0, Pressure=0, Humidity=0;
//		getBME(Temperature, Pressure, Humidity);

	    count = count + 13;
		Temperature = (float)count/3;
		Pressure = (float)count/5;
		Humidity = (float)count/7;

        sprintf((char*)buf,
                "{\"temp\": %.2f, \"hum\": %.2f, \"pres\": %.2f}",
				Temperature, Humidity, Pressure);

        *len = strlen((char*)buf);
        return 1;
    }

    return 0;
}

uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len)
{
	if(strcmp((char*)uri_name, "led.cgi") == 0)
	{
	    // Extract POST data: "led=on" OR "led=off"
	    char* p = strstr((char*)uri, "led=");
	    if(p)
	    {
	        p += 4; // skip "led="

	        if(strncmp(p, "on", 2) == 0)
	        {
	            led_Control(1);
	            *len = sprintf((char*)buf, "ON");
	        }
	        else if(strncmp(p, "off", 3) == 0)
	        {
	            led_Control(0);
	            *len = sprintf((char*)buf, "OFF");
	        }
	        else
	        {
	            *len = sprintf((char*)buf, "UNKNOWN");
	        }

	        return 1;
	    }
	}


	if(strcmp((char*)uri_name, "pwm.cgi") == 0)
	{
	    // Extract POST data such as "pwm=128"
		char* value = strstr((char *)uri, "pwm=");
	    if(value)
	    {
	        int val = atoi(value + 4); // skip "pwm="
	        pwm_Set(val);
	    }

	    sprintf((char*)buf, "Updated");
	    *len = strlen((char*)buf);
	    return 1;
	}

    return 0;
}
