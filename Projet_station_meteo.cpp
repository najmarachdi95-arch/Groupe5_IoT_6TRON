/*
 * Station météo – Groupe 5
 * Publication Température / Humidité / Pression (BME280) vers Adafruit IO
 * Connexion via Border Router 6TRON
 */

#include "mbed.h"
#include <nsapi_dns.h>
#include <MQTTClientMbedOs.h>
#include "bme280.h"

using namespace sixtron;

/* ================= CONFIG ================= */

// MQTT broker Adafruit
const char* hostname = "io.adafruit.com";
int port = 1883;

// feeds Adafruit IO
#define MQTT_TOPIC_TEMP   "moniamalek/feeds/temperature"
#define MQTT_TOPIC_HUM    "moniamalek/feeds/humidity"
#define MQTT_TOPIC_PRESS  "moniamalek/feeds/pressure"
#define MQTT_TOPIC_LED "moniamalek/feeds/led"
#define MQTT_TOPIC_ALERT "moniamalek/feeds/alert_feed"

// Intervalle MQTT
#define SYNC_INTERVAL 1
#define HUMIDITY_THRESHOLD 32.0

DigitalOut led(LED1);


I2C i2c(I2C1_SDA, I2C1_SCL);
BME280 bme(&i2c, BME280::I2CAddress::Address1);

// Network
NetworkInterface *network;
MQTTClient *client;

// Error code
nsapi_size_or_error_t rc = 0;

// Event queue
static int id_yield;
static EventQueue main_queue(32 * EVENTS_EVENT_SIZE);

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\r\n", message.qos, message.retained, message.dup, message.id);

    char payload[16];
    memcpy(payload, message.payload, message.payloadlen);
    payload[message.payloadlen] = '\0';

    printf("LED command received: %s\n", payload);

    if (strcmp(payload, "ON") == 0) {
        led = 1;
        printf("LED ON\n");
    }
    else if (strcmp(payload, "OFF") == 0) {
        led = 0;
        printf("LED OFF\n");
    }
}



static void yield()
{
    rc = client->yield(100);
    if (rc != 0) {
        printf("Yield error: %d\n", rc);
        main_queue.cancel(id_yield);
        main_queue.break_dispatch();
        system_reset();
    }
}


/* =================  METEO ================= */

static int8_t publish_meteo()
{
    bme.take_forced_measurement();

    float temperature = bme.temperature();
    float humidity    = bme.humidity();
    float pressure    = bme.pressure();

    char payload[16];
    MQTT::Message message;
    message.qos = MQTT::QOS1;
    message.retained = false;
    message.dup = false;

    /* ---- Temperature ---- */
    sprintf(payload, "%.2f", temperature);
    message.payload = payload;
    message.payloadlen = strlen(payload);
    client->publish(MQTT_TOPIC_TEMP, message);
    printf("Temperature = %.2f -C\n", temperature);

    /* ---- Humidity ---- */
    sprintf(payload, "%.2f", humidity);
    message.payloadlen = strlen(payload);
    client->publish(MQTT_TOPIC_HUM, message);
    printf("Humidity = %.2f %%\n", humidity);

    if (humidity > HUMIDITY_THRESHOLD) {
        char alert_payload[] = "ALERT";
        MQTT::Message alert_msg;
        alert_msg.qos = MQTT::QOS1;
        alert_msg.retained = true;
        alert_msg.dup = false;
        alert_msg.payload = alert_payload;
        alert_msg.payloadlen = strlen(alert_payload);

        client->publish(MQTT_TOPIC_ALERT, alert_msg);
        printf("ALERT: Humidity %.2f %% > %.2f %%\n",humidity, HUMIDITY_THRESHOLD);
    }
    else  {
        char alert_payload[] = "OK";
        MQTT::Message alert_msg;
        alert_msg.qos = MQTT::QOS1;
        alert_msg.retained = true;
        alert_msg.dup = false;
        alert_msg.payload = alert_payload;
        alert_msg.payloadlen = strlen(alert_payload);
        client->publish(MQTT_TOPIC_ALERT, alert_msg);
    }


    /* ---- Pressure ---- */
    sprintf(payload, "%.2f", pressure);
    message.payloadlen = strlen(payload);
    client->publish(MQTT_TOPIC_PRESS, message);
    printf("Pressure = %.2f Pa\n", pressure);

    return 0;
}





int main()
{
    printf("Connecting to border router...\n");

    if (!bme.initialize()) {
        printf("BME280 init failed\n");
        return 1;
    }
    bme.set_sampling();
    printf("BME280 initialized\n");

    /* Network */
    network = NetworkInterface::get_default_instance();
    if (!network) return 0;

    /* DNS via Border Router */
    nsapi_addr_t dns = {
        NSAPI_IPv6,
        { 0xfd,0x9f,0x59,0x0a,0xb1,0x58,0,0,0,0,0,0,0,0,0x00,0x01 }
    };
    nsapi_dns_add_server(dns, "LOWPAN");

    rc = network->connect();
    if (rc != 0) return rc;

    SocketAddress ip;
    network->get_ip_address(&ip);
    printf("IP: %s\n", ip.get_ip_address());

    /* TCP */
    TCPSocket socket;
    SocketAddress address;
    network->gethostbyname(hostname, &address);
    address.set_port(port);

    socket.open(network);
    socket.connect(address);

    /* MQTT */
    client = new MQTTClient(&socket);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;
    data.keepAliveInterval = 25;
    data.username.cstring = "moniamalek";
    data.password.cstring = "aio_BAMC55gWfDOsrDiBiyZ7NZE6M89R";

    client->connect(data);
    printf("Connected to Adafruit IO\n");

    client->subscribe(MQTT_TOPIC_LED, MQTT::QOS0, messageArrived);
    printf("Subscribed to LED feed\n");



    id_yield = main_queue.call_every(SYNC_INTERVAL * 1000, yield);
    main_queue.call_every(10000, publish_meteo);

    main_queue.dispatch_forever();
}
