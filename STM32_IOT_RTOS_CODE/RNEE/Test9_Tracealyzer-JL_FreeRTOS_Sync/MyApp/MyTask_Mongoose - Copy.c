//           ****************************************************
//          **   Processor      : STM32F107VCT6                   **
//         ***   Frequency      : 8MHz ExtClock 72MHZ CPU Clock   ***
//        ****   AUTHOR         : Mahdi Dadashi                   ****
//       *****   STM32CubeMX    : V5.40                           *****
//      ******   STM32CubeF1    : V1.8.0                          ******
//       *****   Compiler       : KEIL uVision V5.26              *****
//        ****   Instagram      : instagram.com/mahdidadashi65/   ****
//         ***   Telegram       : t.me/mahdidadashi65/            ***
//          **   Github         : github.com/mahdidadashi65/      **
//           ****************************************************
//.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''                                 ''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''      www.mahdidadashi.ir        ''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''                                 ''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._

#include "main.h"
#include "MyApp.h"
#include "MyTask_Mongoose.h"
#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "spi.h"

#include "mongoose.h"
#include "mongoose_glue.h"



TaskHandle_t xHandle_Mongoose;
   

//bool mg_random(void *buf, size_t len) {  // Use on-board RNG
//  for (size_t n = 0; n < len; n += sizeof(uint32_t)) {
//    uint32_t r=1;
//    //HAL_RNG_GenerateRandomNumber(&hrng, &r);
//    memcpy((char *) buf + n, &r, n + sizeof(r) > len ? len - n : sizeof(r));
//  }
//  return true; // TODO(): ensure successful RNG init, then return on false above
//}

//uint64_t mg_millis(void) {
//  return HAL_GetTick();
//}

//void *mg_calloc(size_t count, size_t size) {
//    size_t total = count * size;
//    void *ptr = o1heapAllocate(o1heap, total);

//    if (ptr) {
//        memset(ptr, 0, total);
//    }
//    return ptr;
//}

//void mg_free(void *ptr) {
//	o1heapFree(o1heap, ptr);
//}


typedef struct client_data
{
  int id;
  char version[3];
	char name[20];
  float voltage[4];
  float current[4];
  float temp[4];
  int soc[4];
} client_data;

#define MY_CLIENT_MAX 10
client_data my_client_data[MY_CLIENT_MAX];


void my_get_leds(struct leds *leds)
{ 
  leds->led1 = HAL_GPIO_ReadPin(LED_R_GPIO_Port,LED_R_Pin);
	leds->led2 = HAL_GPIO_ReadPin(LED_G_GPIO_Port,LED_G_Pin);
}

void my_set_leds(struct leds *leds)
{
  HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,leds->led1);
  HAL_GPIO_WritePin(LED_G_GPIO_Port,LED_G_Pin,leds->led2); 
}


void my_get_pwms(struct pwms *pwms)
{
	//pwms->pwms1=my_client_data->voltage[0];
}

void my_set_pwms(struct pwms *pwms)
{

}


void my_get_digitals(struct digitals *digitals)
{
	//pwms->pwms1=my_client_data->voltage[0];
}

void my_set_digitals(struct digitals *digitals)
{

}

void my_get_vals(struct vals *vals)
{
	//pwms->pwms1=my_client_data->voltage[0];
  
 
}

void my_set_vals(struct vals *vals)
{

}

void my_get_state(struct state *state)
{
	state->temperature01=22;
	state->temperature02=23;
}


void my_get_settings(struct settings *settings)
{	
//	printf("5settings.client_mac : %s", settings->client_mac);

//	 esp_err_t err = load_struct_settings(settings);
//	printf("6settings.client_mac : %s", settings->client_mac);

//   if (err == ESP_ERR_NVS_NOT_FOUND) {
//	   
//	   settings->int_val=10;
//	sprintf(settings->server_mac,"80:65:99:a3:1f:9c");
//	
//       //Settings.var1= 15;
//       //Settings.var2= 5;
//   } else if (err != ESP_OK) {
//       // Handle error
//   }

}

void my_set_settings(struct settings *settings)
{
//	printf("1settings.client_mac : %s", settings->client_mac);
//	    esp_err_t err = save_struct_settings(*settings);
//    if (err != ESP_OK) {
//        // Handle error
//    }
}

void my_get_network_settings(struct network_settings *network_settings)
{	

//	 esp_err_t err = load_struct_network_settings(network_settings);

//   if (err == ESP_ERR_NVS_NOT_FOUND) {
//	   
//	  // settings->int_val=10;

//   } else if (err != ESP_OK) {
//       // Handle error
//   }

}
void my_set_network_settings(struct network_settings *network_settings)
{
//	    esp_err_t err = save_struct_network_settings(*network_settings);
//    if (err != ESP_OK) {
//        // Handle error
//    }
}


static void ws_200(struct mg_connection *c) {
  mg_ws_printf(c, WEBSOCKET_OP_TEXT, "{%m: %llu}", MG_ESC("time"), mg_now());
    mg_ws_printf(c, WEBSOCKET_OP_TEXT, "{%m: %llu}", MG_ESC("voltage"), mg_now());

}



static void ws_500(struct mg_connection *c) 
{
    mg_ws_printf(c, WEBSOCKET_OP_TEXT, "{%m: %d}", MG_ESC("led"), 1);

    char val_name[20];
    char val_val[20];

    for(int i=1;i<MY_CLIENT_MAX;i++)
    {
      for(int n=1;n<4;n++)
      {
        sprintf(val_name,"voltage%d%d",i,n);
        mg_ws_printf(c, WEBSOCKET_OP_TEXT, "{%m: %d}", MG_ESC(val_name), my_client_data[i].voltage[n]);
      }
    }

    for(int i=1;i<MY_CLIENT_MAX;i++)
    {
      for(int n=1;n<4;n++)
      {
        sprintf(val_name,"current%d%d",i,n);
        mg_ws_printf(c, WEBSOCKET_OP_TEXT, "{%m: %d}", MG_ESC(val_name), my_client_data[i].current[n]);
      }
    }

    for(int i=1;i<MY_CLIENT_MAX;i++)
    {
      for(int n=1;n<4;n++)
      {
        sprintf(val_name,"temperature%d%d",i,n);
        mg_ws_printf(c, WEBSOCKET_OP_TEXT, "{%m: %d}", MG_ESC(val_name), my_client_data[i].temp[n]);
      }
    } 
  		
    for(int i=1;i<MY_CLIENT_MAX;i++)
    {
    sprintf(val_name,"soc%d",i);
    mg_ws_printf(c, WEBSOCKET_OP_TEXT, "{%m: %d}", MG_ESC(val_name), my_client_data[i].soc);
    }

    for(int i=1;i<MY_CLIENT_MAX;i++)
    {
    sprintf(val_name,"version%d",i);
    sprintf(val_val,"%d.%d.%d",my_client_data[i].version[0],my_client_data[i].version[1],my_client_data[i].version[2]);
    mg_ws_printf(c, WEBSOCKET_OP_TEXT, "{%m: %d}", MG_ESC(val_name), val_val);
    }
}



unsigned char hex_digit( char ch )
{
    if(       ( '0' <= ch ) && ( ch <= '9' ) ) 
        { ch -= '0'; }
    else
    {
        if(   ( 'a' <= ch ) && ( ch <= 'f' ) ) 
            { ch += 10 - 'a'; }
        else
        {
            if(( 'A' <= ch ) && ( ch <= 'F' ) ) 
                { ch += 10 - 'A';}
            else                                
                { ch = 16; }
        }
    }
    return ch;
}
  
int convert_string_mac_to_array(char *mac_str , unsigned char* mac)
{
	
	uint8_t  idx=0;
	for(idx = 0; idx < 6 ; ++idx )
	{
		mac[idx]  = hex_digit( mac_str[     3 * idx ] ) << 4;
		mac[idx] |= hex_digit( mac_str[ 1 + 3 * idx ] );
	}
	//Input is actually 3*6 bytes with \0.
	return 0;
}



int save_struct_settings(struct settings settings)
{
//    nvs_handle_t my_handle;
//    esp_err_t err;

//printf("2settings.client_mac : %s", settings.client_mac);

//    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
//    if (err != ESP_OK) return err;

//    err = nvs_set_blob(my_handle, "settings", &settings, sizeof(struct settings));
//    if (err != ESP_OK) return err;

//    err = nvs_commit(my_handle);
//    if (err != ESP_OK) return err;

//    nvs_close(my_handle);
//	printf("save_struct settings ok");
//    return ESP_OK;
  
  return 0;
}

int save_struct_network_settings(struct network_settings network_settings)
{
//    nvs_handle_t my_handle;
//    esp_err_t err;

//    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
//    if (err != ESP_OK) return err;

//    err = nvs_set_blob(my_handle, "network", &network_settings, sizeof(struct network_settings));
//    if (err != ESP_OK) return err;

//    err = nvs_commit(my_handle);
//    if (err != ESP_OK) return err;

//    nvs_close(my_handle);
//	printf("save_struct network_settings ok");
//    return ESP_OK;
  
  return 0;
}




int load_struct_settings(struct settings *settings)
{
//    nvs_handle_t my_handle;
//    esp_err_t err;

//    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
//    if (err != ESP_OK) return err;

//    size_t required_size = sizeof(struct settings);
//		printf("settings required_size : %d", required_size);
////printf("3settings.client_mac : %s", settings->client_mac);
//    err = nvs_get_blob(my_handle, "settings", settings, &required_size);
//	//printf("4settings.client_mac : %s", settings->client_mac);
//    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

//    nvs_close(my_handle);
//		printf("load_struct_settings ok");

//    return ESP_OK;
  
   return 0;
}



int load_struct_network_settings(struct network_settings *network_settings)
{
//    nvs_handle_t my_handle;
//    esp_err_t err;

//    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
//    if (err != ESP_OK) return err;

//    size_t required_size = sizeof(struct network_settings);
//	printf("network_settings required_size : %d", required_size);
//    err = nvs_get_blob(my_handle, "network", network_settings, &required_size);
//    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

//    nvs_close(my_handle);
//		printf("load_struct_network_settings ok");

//    return ESP_OK;
  
  return 0;
}

		
		
void init_settings(void)
{
//    esp_err_t err = nvs_flash_init();
//    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//        ESP_ERROR_CHECK(nvs_flash_erase());
//        err = nvs_flash_init();
//    }
//    ESP_ERROR_CHECK( err );

//    //Settings_t Settings = {0};
//    err = load_struct_settings(&my_settings);
//    if (err == ESP_ERR_NVS_NOT_FOUND) 
//	{
//			printf("load_struct settings err set to default vals");

//		my_settings.is_server = true;
//		
//        //Settings.var1= 15;
//        //Settings.var2= 5;
//    } else if (err != ESP_OK) {
//        // Handle error
//    }

//        err = load_struct_network_settings(&my_network_settings);
//    if (err == ESP_ERR_NVS_NOT_FOUND) 
//	{
//			printf("load_struct my_network_settings err set to default vals");

//		//my_settings.is_server = true;

//    } else if (err != ESP_OK) {
//        // Handle error
//    }

}

bool my_check_reboot(void)
{
  return true;
}
void my_start_reboot(struct mg_str params) 
{
  //MG_DEBUG(("Passed parameters: [%.*s]", params.len, params.buf));
 
}


//**************************** udp

// client resources
static struct c_res_s {
  int i;
  struct mg_connection *c;
} c_res;

static const char *udp_server_listen_url = "udp://localhost:50001";   // Listening address
static const char *tcp_server_listen_url = "tcp://localhost:50003";   // Listening address
static const char *tcp_client_url        = "tcp://192.168.10.20:50004";   // 

// CLIENT event handler
static void cfn(struct mg_connection *c, int ev, void *ev_data) {
  int *i = &((struct c_res_s *) c->fn_data)->i;
  if (ev == MG_EV_OPEN) {
    MG_INFO(("CLIENT has been initialized"));
  } else if (ev == MG_EV_CONNECT) {
    MG_INFO(("CLIENT connected"));
    if (mg_url_is_ssl(tcp_client_url)) {
      struct mg_tls_opts opts = {.ca = mg_unpacked("/certs/ss_ca.pem"),
                                 .cert = mg_unpacked("/certs/ss_client.pem"),
                                 .key = mg_unpacked("/certs/ss_client.pem")};
      mg_tls_init(c, &opts);
    }
    *i = 1;  // do something
  } else if (ev == MG_EV_READ) {
    struct mg_iobuf *r = &c->recv;
    MG_INFO(("CLIENT got data: %.*s", r->len, r->buf));
    r->len = 0;  // Tell Mongoose we've consumed data
  } else if (ev == MG_EV_CLOSE) {
    MG_INFO(("CLIENT disconnected"));
    // signal we are done
    ((struct c_res_s *) c->fn_data)->c = NULL;
  } else if (ev == MG_EV_ERROR) {
    MG_INFO(("CLIENT error: %s", (char *) ev_data));
  } else if (ev == MG_EV_POLL && *i != 0) {
    switch ((*i)++) {
      case 50:  // 50 x 100ms = 5s
        mg_send(c, "Hi, Server \r\n", 13);
        MG_INFO(("CLIENT sent data"));
        break;
      case 100:  // another 5s
        // send any possible outstanding data and close the connection
        c->is_draining = 1;
        break;
    }
  }
}


// SERVER event handler
static void udp_server_fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_OPEN && c->is_listening == 1) 
  {
    MG_INFO(("UDP SERVER is listening"));
  } 
  else if (ev == MG_EV_ACCEPT) 
  {
    MG_INFO(("UDP SERVER accepted a connection"));
//    if (mg_url_is_ssl(s_listen_url)) 
//    {
//      struct mg_tls_opts opts = {.ca = mg_unpacked("/certs/ss_ca.pem"),
//                                 .cert = mg_unpacked("/certs/ss_server.pem"),
//                                 .key = mg_unpacked("/certs/ss_server.pem")};
//      mg_tls_init(c, &opts);
//    }
  } 
  else if (ev == MG_EV_READ) 
  {
    struct mg_iobuf *r = &c->recv;
    MG_INFO(("UDP SERVER got data: %.*s", r->len, r->buf));
    mg_send(c, r->buf, r->len);  // echo it back
    r->len = 0;                  // Tell Mongoose we've consumed data
  } 
  else if (ev == MG_EV_CLOSE) 
  {
    MG_INFO(("UDP SERVER disconnected"));
  } 
  else if (ev == MG_EV_ERROR) 
  {
    MG_INFO(("UDP SERVER error: %s", (char *) ev_data));
  }
}


static void tcp_server_fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_OPEN && c->is_listening == 1) 
  {
    MG_INFO(("TCP SERVER is listening"));
  } 
  else if (ev == MG_EV_ACCEPT) 
  {
    MG_INFO(("TCP SERVER accepted a connection"));
//    if (mg_url_is_ssl(s_listen_url)) 
//    {
//      struct mg_tls_opts opts = {.ca = mg_unpacked("/certs/ss_ca.pem"),
//                                 .cert = mg_unpacked("/certs/ss_server.pem"),
//                                 .key = mg_unpacked("/certs/ss_server.pem")};
//      mg_tls_init(c, &opts);
//    }
  } 
  else if (ev == MG_EV_READ) 
  {
    struct mg_iobuf *r = &c->recv;
    MG_INFO(("TCP SERVER got data: %.*s", r->len, r->buf));
    mg_send(c, r->buf, r->len);  // echo it back
    r->len = 0;                  // Tell Mongoose we've consumed data
  } 
  else if (ev == MG_EV_CLOSE) 
  {
    MG_INFO(("TCP SERVER disconnected"));
  } 
  else if (ev == MG_EV_ERROR) 
  {
    MG_INFO(("TCP SERVER error: %s", (char *) ev_data));
  }
}




#define UUID ((uint8_t *) UID_BASE)  // Unique 96-bit chip ID. TRM 39.1

// Helper macro for MAC generation
#define GENERATE_LOCALLY_ADMINISTERED_MAC()                        \
  {                                                                \
    2, UUID[0] ^ UUID[1], UUID[2] ^ UUID[3], UUID[4] ^ UUID[5],    \
        UUID[6] ^ UUID[7] ^ UUID[8], UUID[9] ^ UUID[10] ^ UUID[11] \
  }
  
  
  static void timer_fn(void *arg) {
  struct mg_tcpip_if *ifp = arg;                         // And show
  const char *names[] = {"down", "up", "req", "ready"};  // network stats
  MG_INFO(("Ethernet: %s, IP: %M, rx:%u, tx:%u, dr:%u, er:%u",
           names[ifp->state], mg_print_ip4, &ifp->ip, ifp->nrecv, ifp->nsent,
           ifp->ndrop, ifp->nerr));
}
  

// Timer function - recreate client connection if it is closed
static void timer_fn2(void *arg) {
  struct mg_mgr *mgr = (struct mg_mgr *) arg;
  if (c_res.c == NULL) {
    c_res.i = 0;
    c_res.c = mg_connect(mgr, tcp_client_url, cfn, &c_res);
    MG_INFO(("CLIENT %s", c_res.c ? "connecting" : "failed"));
  }
}


#define HTTP_SERVER_URL "http://0.0.0.0:8080"


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


// Event handler for a server (accepted) connection
static void http_server_cb(struct mg_connection *c, int ev, void *ev_data) 
{
  if (ev == MG_EV_HTTP_MSG) 
  {  
      struct mg_http_message *hm = ev_data;
      if (mg_match(hm->uri, mg_str("/test"), NULL)) 
      {
          mg_http_reply(c, 200, "", index_page);
      }
      else
      {
        mg_http_reply(c, 200, "", "Hello from Mongoose!\n");
      }
  }
}


//***************************

#ifdef __W5500
  
void spi_begin(void *spi) 
{
	(void) spi;
	HAL_GPIO_WritePin(WIZ_CS_GPIO_Port, WIZ_CS_Pin, GPIO_PIN_RESET);

}

void spi_end(void *spi) 
{
	(void) spi;
	HAL_GPIO_WritePin(WIZ_CS_GPIO_Port, WIZ_CS_Pin, GPIO_PIN_SET);
}

uint8_t spi_txn(void *spi, uint8_t byte) 
{
  (void) spi;
	uint8_t rbyte = 0;
	HAL_SPI_TransmitReceive(&hspi1, &byte, &rbyte, 1, 100);	
	return rbyte;
}
#endif


//void MG_TCPIP_DRIVER_INIT(struct mg_mgr *mgr)
//{
//    #ifdef __RMII
//  struct mg_tcpip_driver_stm32f_data driver_data = {.phy_addr = 1};
//  struct mg_tcpip_if mif = {.mac = GENERATE_LOCALLY_ADMINISTERED_MAC(),
//                            // Uncomment below for static configuration:
//                             .ip = mg_htonl(MG_U32(192, 168, 10, 10)),
//                             .mask = mg_htonl(MG_U32(255, 255, 255, 0)),
//                             .gw = mg_htonl(MG_U32(192, 168, 10, 1)),
//                             .driver = &mg_tcpip_driver_stm32f,
//                             .driver_data = &driver_data};
//  #endif
//  
//  #ifdef __W5500
//  
//  
//    static struct mg_tcpip_spi spi_driver_data = {NULL, spi_begin, spi_end, spi_txn};     
//     struct mg_tcpip_if mif = {.mac = GENERATE_LOCALLY_ADMINISTERED_MAC(),
//                             .ip = mg_htonl(MG_U32(192, 168, 10, 10)),
//                             .mask = mg_htonl(MG_U32(255, 255, 255, 0)),
//                             .gw = mg_htonl(MG_U32(192, 168, 10, 1)),
//                             .driver = &mg_tcpip_driver_w5500,
//                             .driver_data = &spi_driver_data};  
//    
//                            
//  #endif   
//}


void vTask_Mongoose1( void * pvParameters )
{
  Log("Task vTask_Mongoose Started\r\n");
  
  HAL_GPIO_WritePin(WIZ_RST_GPIO_Port, WIZ_RST_Pin, GPIO_PIN_RESET);
  vTaskDelay(pdMS_TO_TICKS(50)); 
  HAL_GPIO_WritePin(WIZ_RST_GPIO_Port, WIZ_RST_Pin, GPIO_PIN_SET);
  vTaskDelay(pdMS_TO_TICKS(200)); 
  

  struct mg_mgr mgr;  // Event manager
  struct mg_connection *tcp_server;
  struct mg_connection *udp_server;


  mg_mgr_init(&mgr);       // Initialize event manager
  //mg_log_set_fn(mg_pfn_t fn, void *param);
  mg_log_set(MG_LL_NONE);  // Set log level
  
//  mongoose_init();
//  mg_log_set(0);
  
  #ifdef __RMII
  struct mg_tcpip_driver_stm32f_data driver_data = {.phy_addr = 1};
  struct mg_tcpip_if mif = {.mac = GENERATE_LOCALLY_ADMINISTERED_MAC(),
                            // Uncomment below for static configuration:
                             .ip = mg_htonl(MG_U32(192, 168, 10, 10)),
                             .mask = mg_htonl(MG_U32(255, 255, 255, 0)),
                             .gw = mg_htonl(MG_U32(192, 168, 10, 1)),
                             .driver = &mg_tcpip_driver_stm32f,
                             .driver_data = &driver_data};
  #endif
  
  #ifdef __W5500
  
  
    static struct mg_tcpip_spi spi_driver_data = {NULL, spi_begin, spi_end, spi_txn};     
    struct mg_tcpip_if mif = {.mac = GENERATE_LOCALLY_ADMINISTERED_MAC(),
                            // Uncomment below for static configuration:
                             .ip = mg_htonl(MG_U32(192, 168, 10, 10)),
                             .mask = mg_htonl(MG_U32(255, 255, 255, 0)),
                             .gw = mg_htonl(MG_U32(192, 168, 10, 1)),
                             .driver = &mg_tcpip_driver_w5500,
                             .driver_data = &spi_driver_data};  
    
  #endif     
   //MG_SET_MAC_ADDRESS(mif.mac);
                        
                             
  mg_tcpip_init(&mgr, &mif);
  mg_timer_add(&mgr, 1000, MG_TIMER_REPEAT, timer_fn, &mif);

  MG_INFO(("MAC: %M. Waiting for IP...", mg_print_mac, mif.mac));
  while (mif.state != MG_TCPIP_STATE_READY) 
  {
    mg_mgr_poll(&mgr, 10);
  }
  
  
  tcp_server = mg_listen(&mgr, tcp_server_listen_url, tcp_server_fn, NULL);  // Create server connection
  if (tcp_server == NULL) 
  {
    MG_INFO(("TCP SERVER cant' open a connection"));
    vTaskDelete(NULL);
  }
  
  udp_server = mg_listen(&mgr, udp_server_listen_url, udp_server_fn, NULL);  // Create server connection
  if (udp_server == NULL) 
  {
    MG_INFO(("UDP SERVER cant' open a connection"));
    vTaskDelete(NULL);
  }
  
    mg_timer_add(&mgr, 10000, MG_TIMER_REPEAT | MG_TIMER_RUN_NOW, timer_fn2, &mgr);

   
  mg_http_listen(&mgr, HTTP_SERVER_URL, http_server_cb, &mgr);    // Listening server
  
  //*************************
  //  mongoose_init();

  //0 - Disable logging
  //1 - Log errors only
  //2 - Log errors and info messages
  //3 - Log errors, info and debug messages
  //4 - Log everything
  //mg_log_set(0);
//  mongoose_set_http_handlers("action", my_check_reboot, my_start_reboot);
//  mongoose_set_http_handlers("leds", my_get_leds, my_set_leds);
//  mongoose_set_http_handlers("pwms", my_get_pwms, my_set_pwms);
//  mongoose_set_http_handlers("digitals", my_get_digitals, my_set_digitals);
//  mongoose_set_http_handlers("vals", my_get_vals, my_set_vals);
//  mongoose_set_http_handlers("state", my_get_state, NULL);
//  mongoose_set_http_handlers("settings", my_get_settings, my_set_settings);
//  mongoose_set_http_handlers("network_settings", my_get_network_settings, my_set_network_settings);

//  mongoose_add_ws_handler(200, ws_200);
//  mongoose_add_ws_handler(500, ws_500);
  //*************************
  
  for(;;)
  {
    mg_mgr_poll(&mgr, 100);  // Infinite event loop, blocks for upto 100ms unless there is network activity 
    //vTaskDelay(pdMS_TO_TICKS(1)); 

    //mongoose_poll();    
  }
  mg_mgr_free(&mgr);         // Free resources
  
}

void vTask_Mongoose2( void * pvParameters )
{
  Log("Task vTask_Mongoose Started\r\n");
  
  HAL_GPIO_WritePin(WIZ_RST_GPIO_Port, WIZ_RST_Pin, GPIO_PIN_RESET);
  vTaskDelay(pdMS_TO_TICKS(50)); 
  HAL_GPIO_WritePin(WIZ_RST_GPIO_Port, WIZ_RST_Pin, GPIO_PIN_SET);
  vTaskDelay(pdMS_TO_TICKS(200)); 
   
  mongoose_init();
  mg_log_set(0);
  
  
  uint8_t ip_add[4]={192, 168, 10, 12};
  memcpy(&g_mgr.ifp->ip, ip_add, 4);
 
  mongoose_set_http_handlers("action", my_check_reboot, my_start_reboot);
  mongoose_set_http_handlers("leds", my_get_leds, my_set_leds);
  mongoose_set_http_handlers("pwms", my_get_pwms, my_set_pwms);
  mongoose_set_http_handlers("digitals", my_get_digitals, my_set_digitals);
  mongoose_set_http_handlers("vals", my_get_vals, my_set_vals);
  mongoose_set_http_handlers("state", my_get_state, NULL);
  mongoose_set_http_handlers("settings", my_get_settings, my_set_settings);
  mongoose_set_http_handlers("network_settings", my_get_network_settings, my_set_network_settings);

  mongoose_add_ws_handler(200, ws_200);
  mongoose_add_ws_handler(500, ws_500);
  
 
  //*************************
  
  for(;;)
  {
    mongoose_poll(); 
    vTaskDelay(pdMS_TO_TICKS(1));     
  }  
}

void vTask_Mongoose3( void * pvParameters )
{
  struct mg_connection *tcp_server;
  struct mg_connection *udp_server;
  
  Log("Task vTask_Mongoose Started\r\n");
  
  HAL_GPIO_WritePin(WIZ_RST_GPIO_Port, WIZ_RST_Pin, GPIO_PIN_RESET);
  vTaskDelay(pdMS_TO_TICKS(50)); 
  HAL_GPIO_WritePin(WIZ_RST_GPIO_Port, WIZ_RST_Pin, GPIO_PIN_SET);
  vTaskDelay(pdMS_TO_TICKS(200)); 
   
  mongoose_init();
  mg_log_set(0);
  
   
 uint8_t ip_add[4]={192, 168, 10, 13};
 memcpy(&g_mgr.ifp->ip, ip_add, 4);   
 //MG_SET_MAC_ADDRESS(mif.mac);
                        
                            
  mg_timer_add(&g_mgr, 1000, MG_TIMER_REPEAT, timer_fn, g_mgr.ifp);

  MG_INFO(("MAC: %M. Waiting for IP...", mg_print_mac, g_mgr.ifp->mac));
  while (g_mgr.ifp->state != MG_TCPIP_STATE_READY) 
  {
    mg_mgr_poll(&g_mgr, 10);
  }
  
  
  tcp_server = mg_listen(&g_mgr, tcp_server_listen_url, tcp_server_fn, NULL);  // Create server connection
  if (tcp_server == NULL) 
  {
    MG_INFO(("TCP SERVER cant' open a connection"));
    vTaskDelete(NULL);
  }
  
  udp_server = mg_listen(&g_mgr, udp_server_listen_url, udp_server_fn, NULL);  // Create server connection
  if (udp_server == NULL) 
  {
    MG_INFO(("UDP SERVER cant' open a connection"));
    vTaskDelete(NULL);
  }
  
  mg_timer_add(&g_mgr, 10000, MG_TIMER_REPEAT | MG_TIMER_RUN_NOW, timer_fn2, &g_mgr);
  
  mg_http_listen(&g_mgr, HTTP_SERVER_URL, http_server_cb, &g_mgr);    // Listening server
  

  

  mongoose_set_http_handlers("action", my_check_reboot, my_start_reboot);
  mongoose_set_http_handlers("leds", my_get_leds, my_set_leds);
  mongoose_set_http_handlers("pwms", my_get_pwms, my_set_pwms);
  mongoose_set_http_handlers("digitals", my_get_digitals, my_set_digitals);
  mongoose_set_http_handlers("vals", my_get_vals, my_set_vals);
  mongoose_set_http_handlers("state", my_get_state, NULL);
  mongoose_set_http_handlers("settings", my_get_settings, my_set_settings);
  mongoose_set_http_handlers("network_settings", my_get_network_settings, my_set_network_settings);

  mongoose_add_ws_handler(200, ws_200);
  mongoose_add_ws_handler(500, ws_500);
  //*************************
  
  for(;;)
  {
    mongoose_poll(); 
    vTaskDelay(pdMS_TO_TICKS(1));     
  }  
}

void start_task_mongoose(void)
{		   
  Log("Mongoose Task Start\r\n");
  
  
    my_client_data[0].voltage[1]=10;
    my_client_data[0].voltage[2]=20;
    my_client_data[0].soc[1]=30;

    my_client_data[1].voltage[1]=11;
    my_client_data[1].voltage[2]=21;
    my_client_data[1].soc[1]=31;
  
  
    /* Create the task. */
    if( xTaskCreate(
    vTask_Mongoose3, /* Pointer to the function that implements the task. */
    "task_mg", /* Text name given to the task. */
    2048, /* The size of the stack that should be created for the task.
    This is defined in words, not bytes. */
    (void*) 0,/* A reference to xParameters is used as the task parameter.
    This is cast to a void * to prevent compiler warnings. */
    0, /* The priority to assign to the newly created task. */
    &xHandle_Mongoose /* The handle to the task being created will be placed in
    xHandle. */
    ) != pdPASS )
    {
    /* The task could not be created as there was insufficient heap memory remaining. If
    heap_1.c, heap_2.c or heap_4.c are included in the project then this situation can be
    trapped using the vApplicationMallocFailedHook() callback (or ‘hook’) function, and the
    amount of FreeRTOS heap memory that remains unallocated can be queried using the
    xPortGetFreeHeapSize() API function.*/
           
      Log("Task1 Mongoose Create Err\r\n");
    }
    else
    {
      Log("Task1 Mongoose Created\r\n");
    }

    Log("MyApp END\r\n");
}
