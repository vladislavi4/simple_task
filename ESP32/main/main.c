/* UART Events Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
//===========   HW  ============
#include "driver/uart.h"
#include "esp_timer.h"
#include "driver/gpio.h"
//===========   HW  ============

//==========  serv  ============
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
//==========  serv  ============


typedef struct{
    uint8_t head[2];
    uint8_t msg_type;
    uint8_t data[8];
    uint8_t stop_byte;
}protocol_t;

enum MsgType_t{
  command = 1,
  request_rd,
  request_wr,
  ack,
};
enum command_t{
  led_on = 1,
  led_off,
};
enum request_rd_t{
  button_pressed = 1,
  button_not_pressed,
  temperature,
  humidity,
  pressure
};
typedef struct{
  uint32_t delay;
  uint32_t counter;
}task_struct_t;

typedef struct{
    union{
      uint8_t raw[6];
      struct{
        uint16_t temp;
        uint16_t humi;
        uint16_t pres;
      }sensor;
    };
}measure_t;

typedef struct {
    char    *username;
    char    *password;
} basic_auth_info_t;


static const char *TAG = "";



/**
 * This example shows how to use the UART driver to handle special UART events.
 *
 * It also reads data from UART0 directly, and echoes it to console.
 *
 * - Port: UART0
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: on
 * - Pin assignment: TxD (default), RxD (default)
 */
#define TEXT_PAGE "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"><title>Some Device</title><style>body {display: block;height: 97%;box-shadow:  0px 0px 15px 0px rgba(0, 0, 0, .45);  border-radius: 1%;}#Content-meter{position: absolute;right: 8%;text-align: end;display:inline-block;border: var(--border-main);background-color: var(--background-color-main);top:7%;padding-bottom: 20px;}#Content-meter2-data {display: inline-block;height: 20%;width: 50%;}.meterDataSmall{height:80px;width:90%;margin-left:15%;margin-right:30%;margin-top:3%;padding-top: 3%;text-align: center;font-size: 500%;font-weight: bold;border:5px solid rgb(63, 63, 63);border-radius: 10%;background-color:white;}.styled {border: 0;position:relative ;left:35%;margin-top:5%;line-height: 2.5;padding: 0 20px;font-size: 2rem;text-align: center;color: #fff;text-shadow: 1px 1px 1px #000;border-radius: 10px;background-color: rgb(0, 240, 0);background-image: linear-gradient(to top left,rgba(0, 0, 0, .2),rgba(0, 0, 0, .2) 30%,rgba(0, 0, 0, 0));box-shadow: inset 2px 2px 3px rgba(255, 255, 255, .6),inset -2px -2px 3px rgba(0, 0, 0, .6);}.styled:hover {background-color: rgba(0, 255, 0, 1);}.styled:active {box-shadow: inset -2px -2px 3px rgba(255, 255, 255, .6),inset 2px 2px 3px rgba(0, 0, 0, .6);}</style></head><body onload=\"updateValue()\"><button class=\"favorite styled\"type=\"button\" onclick=\"switch_led()\">Led</button><button class=\"favorite styled\"type=\"button\" onclick=\"switch_stm_led()\">STM32_Led</button><div id=\"Content-meter\"><div id=\"Content-meter2-data\"><div id=\"TitleSmall\">Температура, °C</div><div class=\"meterDataSmall\" id=\"textTemp\">0   </div></div><div id=\"Content-meter2-data\"><div id=\"TitleSmall\">Влажность, %</div><div class=\"meterDataSmall\" id=\"textHumi\">0   </div></div><div id=\"Content-meter2-data\"><div id=\"TitleSmall\">Давление</div><div class=\"meterDataSmall\" id=\"textPres\">0   </div></div></div><script>var LED_status = XXX;function switch_led(){var request = new XMLHttpRequest();request.open('GET','/togle_led',true);request.send();}function switch_stm_led(){var request = new XMLHttpRequest();request.open('GET','/stm_togle_led',true);request.send();}</script></body></html>"
#define HTTPD_401      "401 UNAUTHORIZED"           /*!< HTTP Response 401 */

//#define EXAMPLE_ESP_WIFI_SSID      "test_6"
//#define EXAMPLE_ESP_WIFI_PASS      "11111111"
#define ESP_AP_WIFI_SSID      "test_6"
#define ESP_AP_WIFI_PASS      "11111111"
#define ESP_STA_WIFI_SSID      "SSID"
#define ESP_STA_WIFI_PASS      "Password"
#define EXAMPLE_ESP_WIFI_CHANNEL   6
#define EXAMPLE_MAX_STA_CONN       6

#define EX_UART_NUM UART_NUM_2
//UART_NUM_0
#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

#define LED       5

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

//static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

static QueueHandle_t uart_queue;
static httpd_handle_t server = NULL;

task_struct_t send_cmd_task_time = {0};
protocol_t      protocol_data   = {.head[0] = 0xAA, .head[1] = 0x55, .stop_byte = 0x55};
protocol_t      recived_data    = {0};
measure_t       measure         = {0};

float           temp            = 0;
float           humi            = 0;
float           pres            = 0;

volatile char str_wb_pg[2248] = {TEXT_PAGE};

const uint8_t   protocol_head[2] = {0xAA, 0x55};

volatile uint32_t time_tick;
volatile uint8_t uart_rxflag;
volatile uint8_t led_state = 0;
volatile uint8_t stm_led_state = 0;
uint8_t rxBuf[64] ={0};
char* pointer_meas_str;

static void periodic_timer_callback(void* arg)
{
    time_tick++;
}

//*************************************************    HTTP INIT  **********************************************************
static char *http_auth_basic(const char *username, const char *password)
{
    int out;
    char *user_info = NULL;
    char *digest = NULL;
    size_t n = 0;
    asprintf(&user_info, "%s:%s", username, password);
    if (!user_info) {
        ESP_LOGE(TAG, "No enough memory for user information");
        return NULL;
    }
    esp_crypto_base64_encode(NULL, 0, &n, (const unsigned char *)user_info, strlen(user_info));

    /* 6: The length of the "Basic " string
     * n: Number of bytes for a base64 encode format
     * 1: Number of bytes for a reserved which be used to fill zero
    */
    digest = calloc(1, 6 + n + 1);
    if (digest) {
        strcpy(digest, "Basic ");
        esp_crypto_base64_encode((unsigned char *)digest + 6, n, (size_t *)&out, (const unsigned char *)user_info, strlen(user_info));
    }
    free(user_info);
    return digest;
}

/* An HTTP GET handler */
static esp_err_t basic_auth_get_handler(httpd_req_t *req)
{
    char *buf = NULL;
    size_t buf_len = 0;
    basic_auth_info_t *basic_auth_info = req->user_ctx;

    buf_len = httpd_req_get_hdr_value_len(req, "Authorization") + 1;
    if (buf_len > 1) {
        buf = calloc(1, buf_len);
        if (!buf) {
            ESP_LOGE(TAG, "No enough memory for basic authorization");
            return ESP_ERR_NO_MEM;
        }

        if (httpd_req_get_hdr_value_str(req, "Authorization", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Authorization: %s", buf);
        } else {
            ESP_LOGE(TAG, "No auth value received");
        }

        char *auth_credentials = http_auth_basic(basic_auth_info->username, basic_auth_info->password);
        if (!auth_credentials) {
            ESP_LOGE(TAG, "No enough memory for basic authorization credentials");
            free(buf);
            return ESP_ERR_NO_MEM;
        }

        if (strncmp(auth_credentials, buf, buf_len)) {
            ESP_LOGE(TAG, "Not authenticated");
            httpd_resp_set_status(req, HTTPD_401);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");
            httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Hello\"");
            httpd_resp_send(req, NULL, 0);
        } else {
            ESP_LOGI(TAG, "Authenticated!");
            char *basic_auth_resp = NULL;
            httpd_resp_set_status(req, HTTPD_200);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");
            asprintf(&basic_auth_resp, "{\"authenticated\": true,\"user\": \"%s\"}", basic_auth_info->username);
            if (!basic_auth_resp) {
                ESP_LOGE(TAG, "No enough memory for basic authorization response");
                free(auth_credentials);
                free(buf);
                return ESP_ERR_NO_MEM;
            }
            httpd_resp_send(req, basic_auth_resp, strlen(basic_auth_resp));
            free(basic_auth_resp);
        }
        free(auth_credentials);
        free(buf);
    } else {
        ESP_LOGE(TAG, "No auth header received");
        httpd_resp_set_status(req, HTTPD_401);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Connection", "keep-alive");
        httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"Hello\"");
        httpd_resp_send(req, NULL, 0);
    }

    return ESP_OK;
}

static httpd_uri_t basic_auth = {
    .uri       = "/basic_auth",
    .method    = HTTP_GET,
    .handler   = basic_auth_get_handler,
};

static void httpd_register_basic_auth(httpd_handle_t server)
{
    basic_auth_info_t *basic_auth_info = calloc(1, sizeof(basic_auth_info_t));
    if (basic_auth_info) {
        basic_auth_info->username = "User_Name";
        basic_auth_info->password = "Password";

        basic_auth.user_ctx = basic_auth_info;
        httpd_register_uri_handler(server, &basic_auth);
    }
}

/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-2: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-1: %s", buf);
        }
        free(buf);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query1=%s", param);
            }
            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query3=%s", param);
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query2=%s", param);
            }
        }
        free(buf);
    }

    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

static const httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Hello World!"
};

/* An HTTP GET handler */
static esp_err_t main_page_get_handler(httpd_req_t *req)
{
    httpd_resp_send(req, str_wb_pg, strlen(str_wb_pg));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "main page was load");
    }
    return ESP_OK;
}

static const httpd_uri_t main_page = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = main_page_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t echo = {
    .uri       = "/echo",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = NULL
};
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/echo", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

/* An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 */
static esp_err_t ctrl_put_handler(httpd_req_t *req)
{
    char buf;
    int ret;

    if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    if (buf == '0') {
        /* URI handlers can be unregistered using the uri string */
        ESP_LOGI(TAG, "Unregistering /hello and /echo URIs");
        httpd_unregister_uri(req->handle, "/hello");
        httpd_unregister_uri(req->handle, "/echo");
        /* Register the custom error handler */
        httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    else {
        ESP_LOGI(TAG, "Registering /hello and /echo URIs");
        httpd_register_uri_handler(req->handle, &hello);
        httpd_register_uri_handler(req->handle, &echo);
        /* Unregister custom error handler */
        httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, NULL);
    }

    /* Respond with empty body */
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t ctrl = {
    .uri       = "/ctrl",
    .method    = HTTP_PUT,
    .handler   = ctrl_put_handler,
    .user_ctx  = NULL
};

/* An HTTP GET handler */
static esp_err_t web_led_handler(httpd_req_t *req)
{
    if (led_state) {
        led_state = 0;
    } else {
        led_state = 1;
    }
    gpio_set_level(LED, led_state);
    httpd_resp_send(req, " ", 1);
    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "led_handler");
    }
    return ESP_OK;
}

static const httpd_uri_t togle_led = {
    .uri       = "/togle_led",
    .method    = HTTP_GET,
    .handler   = web_led_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

/* An HTTP GET handler */
static esp_err_t web__stm_led_handler(httpd_req_t *req)
{
    if (stm_led_state) {
        stm_led_state = 0;
    } else {
        stm_led_state = 1;
    }
    httpd_resp_send(req, " ", 1);
    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "stm_led_handler");
    }
    return ESP_OK;
}

static const httpd_uri_t stm_togle_led = {
    .uri       = "/stm_togle_led",
    .method    = HTTP_GET,
    .handler   = web__stm_led_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &main_page);
        httpd_register_uri_handler(server, &togle_led);
        httpd_register_uri_handler(server, &stm_togle_led);
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &ctrl);
        #if CONFIG_EXAMPLE_BASIC_AUTH
        httpd_register_basic_auth(server);
        #endif
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

static void web_page_init(void){
    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
   /* Start the server for the first time */
    pointer_meas_str = strstr(str_wb_pg, ">0   <");

    server = start_webserver();
}

//*************************************************    HTTP INIT  **********************************************************


//**************************************************    SET HW  ************************************************************

static void sys_tick_init(void){
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000));//1ms
}
//==================    init UART  ==============================
static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(uart_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            switch(event.type) {
                case UART_DATA:
                    uart_read_bytes(EX_UART_NUM, rxBuf, event.size, portMAX_DELAY);
                    uart_rxflag = 1;
                    uart_flush_input(EX_UART_NUM);
                    //ESP_LOGI(TAG, "%s", rxBuf);
                    break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(uart_queue);
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(uart_queue);
                    break;
                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break");
                    break;
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");
                    break;
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");
                    break;
                case UART_PATTERN_DET:
                    uart_get_buffered_data_len(EX_UART_NUM, &buffered_size);
                    int pos = uart_pattern_pop_pos(EX_UART_NUM);
                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                    if (pos == -1) {
                        uart_flush_input(EX_UART_NUM);
                    } else {
                        uart_read_bytes(EX_UART_NUM, rxBuf, pos, 100 / portTICK_PERIOD_MS);
                        uint8_t pat[PATTERN_CHR_NUM + 1];
                        memset(pat, 0, sizeof(pat));
                        uart_read_bytes(EX_UART_NUM, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                        ESP_LOGI(TAG, "read data: %s", rxBuf);
                        ESP_LOGI(TAG, "read pat : %s", pat);
                    }
                    break;
                //Others
                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
        //ESP_LOGI(TAG, "time tick is - %d", time_tick);
    }
    //free(dtmp);
    //dtmp = NULL;
    vTaskDelete(NULL);
}

//==================    set UART  ==============================
static void uart2_init(void){
    uart_config_t uart_config = {
        //.baud_rate = 115200,
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(EX_UART_NUM, 128 * 2, 128 * 2, 20, &uart_queue, 0);
    uart_param_config(EX_UART_NUM, &uart_config);
    esp_log_level_set(TAG, ESP_LOG_INFO);
    uart_set_pin(EX_UART_NUM, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_enable_pattern_det_baud_intr(EX_UART_NUM, '+', PATTERN_CHR_NUM, 9, 0, 0);
    uart_pattern_queue_reset(EX_UART_NUM, 20);
}
//==================    set WIFI  ==============================
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}
static void wifi_init_softap(void)
{
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_AP_WIFI_SSID,
            .ssid_len = strlen(ESP_AP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = ESP_AP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(ESP_AP_WIFI_SSID) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             ESP_AP_WIFI_SSID, ESP_AP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 10) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            //xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        //xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init_sta(void)
{
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_STA_WIFI_SSID,
            .password = ESP_STA_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}

//==================    set GPIO  ==============================
static void configure_led(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}
//**************************************************    SET HW  ************************************************************


static void msg_parse(uint8_t* data){
    if(strstr((const char*)data, (const char*)protocol_head)){
        //ESP_LOGI(TAG, "%s",data);
        memcpy((uint8_t*)&recived_data, data, 12);
        //ESP_LOGI(TAG, "%s",(char *)&recived_data);
        if(recived_data.msg_type == ack){
            if (strstr((char*)&recived_data.data, "ok")){
                //ESP_LOGI(TAG, "ack OK\r\n");
            }
            if (recived_data.data[0] == led_on){
                //ESP_LOGI(TAG, "LED was turned on\r\n");
            }
            if (recived_data.data[0] == led_off){
                //ESP_LOGI(TAG, "LED was turned off\r\n");
            }
        }
        else if(recived_data.msg_type == request_rd){
            memcpy(measure.raw, &recived_data.data[1], 6);
            temp = (float)measure.sensor.temp/100.0;
            humi = (float)measure.sensor.humi/100.0;
            char tmp_bf[6] ={0};
            sprintf(tmp_bf, "%.2f", temp);
            memcpy(pointer_meas_str+1,tmp_bf, 4);
            sprintf(tmp_bf, "%.2f", humi);
            memcpy(pointer_meas_str+137,tmp_bf, 4);
            sprintf(tmp_bf, "%d", measure.sensor.pres);
            memcpy(pointer_meas_str+268,tmp_bf, 4);
            //ESP_LOGI(TAG, "%s", str_wb_pg);
            //ESP_LOGI(TAG, "\r\n pointer on poss - %d \r\n", pointer_meas_str - str_wb_pg);
        }
        memset((uint8_t*)&recived_data, 0, sizeof(recived_data));
    }
    else{
       ESP_LOGI(TAG, "non exspected data\r\n");
    }
}


void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    sys_tick_init();
    uart2_init();
    configure_led();
    send_cmd_task_time.delay = 500;
    //Create a task to handler UART event from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_softap();
    //wifi_init_sta();
    web_page_init();

    while(1)
    {
        if(time_tick - send_cmd_task_time.counter >= send_cmd_task_time.delay){
            static uint8_t tx_type = 1;
            switch(tx_type){

                case 0:
                protocol_data.msg_type = command;
                if(stm_led_state){
                    protocol_data.data[0] = (uint8_t)led_off;
                }
                else{
                    protocol_data.data[0] = (uint8_t)led_on;
                }

                tx_type = 1;
                break;

                case 1:
                protocol_data.msg_type = request_rd;
                tx_type = 0;
                break;
            }
            uart_write_bytes(EX_UART_NUM, &protocol_data, sizeof(protocol_data));
            send_cmd_task_time.counter = time_tick;
        }
        if(uart_rxflag){
            msg_parse(&rxBuf[0]);
            uart_rxflag = 0;
            memset(rxBuf, 0, sizeof(rxBuf));
        }
        
    }
}
