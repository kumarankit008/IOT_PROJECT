#include <pgmspace.h>

#define SECRET

const char ssid[] = "ankit";
const char pass[] = "PASSWORD";

#define THINGNAME "Smart_Irrigation"

int8_t TIME_ZONE = -5; //NYC(USA): -5 UTC
//#define USE_SUMMER_TIME_DST  //uncomment to use DST

const char MQTT_HOST[] = "API_KEY";

// Obtain First CA certificate for Amazon AWS
// https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html#server-authentication

//KEYS CREATED WHILE MAKING A NEW THING IN AWS

// Copy contents from CA certificate here ▼
static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
CA_CERT_KEY
-----END CERTIFICATE-----


)EOF";

// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
CLIENT_CERT_KEY
-----END CERTIFICATE-----


)KEY";

// Copy contents from  XXXXXXXX-private.pem.key here ▼
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
YOUR_PRIVATE_KEY
-----END RSA PRIVATE KEY-----
)KEY";
