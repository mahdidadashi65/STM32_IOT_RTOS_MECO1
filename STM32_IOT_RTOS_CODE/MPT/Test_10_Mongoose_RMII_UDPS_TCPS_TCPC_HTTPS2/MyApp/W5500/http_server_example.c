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


const char index_page1[] =
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

const char index_page2[] =
"<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset=\"utf-8\">"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
"<title>STM32 W5500</title>"
//"<link href=\"https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap\" rel=\"stylesheet\">"
"<style>"
":root{"
"  --glass: rgba(255,255,255,0.045);"
"  --glass2: rgba(255,255,255,0.07);"
"  --border: rgba(255,255,255,0.08);"
"  --text: rgba(255,255,255,0.88);"
"  --muted: rgba(255,255,255,0.55);"
"}"
"* { box-sizing: border-box; }"
"html,body{"
"  height:100%;"
"  margin:0;"
"  font-family: Inter, system-ui, sans-serif;"
"  color: var(--text);"
"}"
"body{"
"  background:"
"    radial-gradient(900px 600px at 20% 10%, rgba(120,120,255,0.12), transparent 60%),"
"    radial-gradient(900px 600px at 80% 90%, rgba(255,120,200,0.10), transparent 60%),"
"    linear-gradient(135deg, #0b1020, #0e1628);"
"}"
".wrap{"
"  height:100%;"
"  display:grid;"
"  place-items:center;"
"  padding:24px;"
"}"
".card{"
"  width:min(880px,100%);"
"  display:grid;"
"  grid-template-columns: 1.2fr 0.8fr;"
"  gap:16px;"
"  padding:18px;"
"  border-radius:24px;"
"  background: linear-gradient(180deg,var(--glass2),var(--glass));"
"  border:1px solid var(--border);"
"  backdrop-filter: blur(10px);"
"}"
"@media(max-width:820px){"
"  .card{grid-template-columns:1fr;}"
"}"
".panel{"
"  padding:18px;"
"  border-radius:18px;"
"  background: rgba(255,255,255,0.04);"
"  border:1px solid var(--border);"
"}"
".title{"
"  font-size:.85rem;"
"  letter-spacing:.08em;"
"  text-transform:uppercase;"
"  color:var(--muted);"
"}"
".temp{"
"  margin-top:10px;"
"  font-size:clamp(4.2rem,9vw,6.8rem);"
"  font-weight:700;"
"  opacity:.92;"
"}"
".chip{"
"  display:inline-block;"
"  margin-top:12px;"
"  padding:8px 12px;"
"  border-radius:999px;"
"  font-weight:600;"
"  background: rgba(255,255,255,0.05);"
"  border:1px solid var(--border);"
"  opacity:.85;"
"}"
".time{"
"  margin-top:14px;"
"  font-size:clamp(2.1rem,5vw,3rem);"
"  font-weight:600;"
"  opacity:.85;"
"}"
".date{"
"  margin-top:8px;"
"  font-size:1rem;"
"  color:var(--muted);"
"}"
".kv{"
"  display:flex;"
"  justify-content:space-between;"
"  padding:10px 12px;"
"  margin-top:10px;"
"  border-radius:14px;"
"  background: rgba(255,255,255,0.04);"
"  border:1px solid var(--border);"
"}"
".k{ color:var(--muted); }"
".v{ font-weight:600; opacity:.9; }"
".footer{"
"  margin-top:14px;"
"  font-size:.9rem;"
"  color:var(--muted);"
"}"
"</style>"
"</head>"
"<body>"
"<div class=\"wrap\">"
"  <div class=\"card\">"
"    <div class=\"panel\">"
"      <div class=\"title\">Indoor climate</div>"
"      <div class=\"temp\" id=\"temp\">--.- °C</div>"
"      <div class=\"chip\" id=\"press\">----.- hPa</div>"
"      <div class=\"time\" id=\"time\">--:--:--</div>"
"      <div class=\"date\" id=\"date\"></div>"
"      <div class=\"footer\">mydev.local • STM32</div>"
"    </div>"
"    <div class=\"panel\">"
"      <div class=\"title\">Details</div>"
"      <div class=\"kv\"><div class=\"k\">Temperature</div><div class=\"v\" id=\"t2\"></div></div>"
"      <div class=\"kv\"><div class=\"k\">Pressure</div><div class=\"v\" id=\"p2\"></div></div>"
"      <div class=\"kv\"><div class=\"k\">Time</div><div class=\"v\" id=\"time2\"></div></div>"
"      <div class=\"kv\"><div class=\"k\">Date</div><div class=\"v\" id=\"date2\"></div></div>"
"    </div>"
"  </div>"
"</div>"
"<script>"
//"async function update(){"
//"  try{"
//"    const t = await (await fetch('/temp')).text();"
//"    const p = await (await fetch('/press')).text();"
//"    const ti = await (await fetch('/time')).text();"
//"    const d = await (await fetch('/date')).text();"
//"    temp.textContent = t;"
//"    press.textContent = p;"
//"    time.textContent = ti;"
//"    date.textContent = d;"
//"    t2.textContent = t;"
//"    p2.textContent = p;"
//"    time2.textContent = ti;"
//"    date2.textContent = d;"
//"  }catch(e){}"
//"}"
"function update(){"
" fetch('sensor.cgi')"
" .then(r=>r.json())"
" .then(data=>{"
//" document.getElementById('temp').innerHTML = data.temp;"
//" document.getElementById('hum').innerHTML = data.hum;"
//" document.getElementById('pres').innerHTML = data.pres;"
"    temp.textContent = data.temp;"
"    press.textContent = data.pres;"
"    time.textContent = data.time;"
"    date.textContent = data.date;"
"    t2.textContent = data.temp;"
"    p2.textContent = data.pres;"
"    time2.textContent = data.time;"
"    date2.textContent = data.date;"
" });"
"}"
"update();"
"setInterval(update,1000);"
"</script>"
"</body>"
"</html>";



// HTTP Server Setup
void httpServer_setup(void)
{
    httpServer_init(tx_buf, rx_buf, HTTP_MAX_SOCK, sockList);

    reg_httpServer_cbfunc(NVIC_SystemReset, NULL);

    /* Register Static pages */
    reg_httpServer_webContent((uint8_t*)"index.html", (uint8_t*)index_page1);
    reg_httpServer_webContent((uint8_t*)"climate.html", (uint8_t*)index_page2);
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
                "{\"temp\": %.2f, \"hum\": %.2f, \"pres\": %.2f, \"time\": \"%s\", \"date\": \"%s\"}",
				Temperature, Humidity, Pressure,"11:22:33","2026/01/11");

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
