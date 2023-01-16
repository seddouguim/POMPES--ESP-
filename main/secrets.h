#ifndef SRC_SECRETS
#define SRC_SECRETS

#include <avr/pgmspace.h>

// AMAZON ROOT CA1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAITFu/joOTe83iaDkVpbudh1+1icMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMjExMDkxNDI1
MjZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDyRyaOf059EggJYIXV
kSuARSbNkxWjyhwenO2d5umwZhQFAKmiQSC/rpnRwdPuoxrjCWvapilUwizXMDHh
EUEBbvH/uXxqk16xI62/XYKUPTdLgDZGjyFEWgH5/w7xBHchag9ymZAGC2YkrIyB
4Wm3Qc+GKhwzdOyWh5u2N5HoZ8vJIGOhDZrmIurV6zNmRWO4YOoQCu6BRtzpMJ1A
aysFzmr61135aAYBUC77u+nl5KJAuUeu4OcY/YcDv3xhOfHy/ybkihgzMjLjQVG+
jy+abbXXr28n8jy/Dgoildtn+vJ/dAx5n3eyG+2j/BWXQGiBPmKuZZ7Xnl86GilD
yh21AgMBAAGjYDBeMB8GA1UdIwQYMBaAFOuU/ra+KhZgqxj5cMbk+cR5gIEbMB0G
A1UdDgQWBBSg46Hih4Mif4i7I2LO/6Riga2skDAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAjGMUdK62W3KNpc/ViGmD4fyC
Mzxbw3kH149d3QsaImx3UR8z7P4S3NyYDGQx9KB0wWfByFDxSUOLXJoWPiG33usp
d8zkVASnopQogNw+QvAueYNX4u9Ta7/qZTiFuunbBSjcP2tHDPaLIIgaBOK8x5pk
9F199aK/gd+lxjuwy4gkkUU23IjNlld9tqNX3LFUM6hT3HIbsZjNjlZe/xdQa/Nz
QEGZSveKPQd6cBGEEJ/37ApiYUAP2vXgtkqPp749h85cSUWr0YCX1DIPbn3bE7FH
ePRRZKi7Af0Ub+HuahSZZ/gOgZXdV55xZhCWEJcLfMWeGFeHfQF31LPu1PwhPQ==
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA8kcmjn9OfRIICWCF1ZErgEUmzZMVo8ocHpztnebpsGYUBQCp
okEgv66Z0cHT7qMa4wlr2qYpVMIs1zAx4RFBAW7x/7l8apNesSOtv12ClD03S4A2
Ro8hRFoB+f8O8QR3IWoPcpmQBgtmJKyMgeFpt0HPhiocM3TsloebtjeR6GfLySBj
oQ2a5iLq1eszZkVjuGDqEArugUbc6TCdQGsrBc5q+tdd+WgGAVAu+7vp5eSiQLlH
ruDnGP2HA798YTnx8v8m5IoYMzIy40FRvo8vmm21169vJ/I8vw4KIpXbZ/ryf3QM
eZ93shvto/wVl0BogT5irmWe155fOhopQ8odtQIDAQABAoIBABdbQJ5QK5LnIBs3
UO+r1RPCluEOovFtVJSm8Eler3yv+pIY5EGeIbgY7v9HvlStKLRmfh1QE3/2UDhW
jxhxf2DR/0Z17Uag+mn9VIy0SRhdZm1C6PiYZ86dFr+xFRGrKojwHpkfsMDEbI5w
jAMhZf2ivIzkll9nkD9ax2ojpU9QMlE7qhJe4swD8Z6kYaYP1tC9Qnq/CPO2ZRAR
IFJLh/GZF9O9azk5M3ux9sze1iscniAHwVn0Z22B3e16AGTWlHvY6yE0C2jbplCJ
MtJKodQdfRulBDcOxycCnKKHaJpcyeO7n4BpgNUOQ1bkKNcRYNy+ZM+tE0z+VSWU
bvUPagECgYEA/KGluR4/0EjivQErTj7dUVdqUmlil8n8HPZDvFwHH0+jNA5waELH
YTA2WRcsWEN+HYy8qmYtl79bpU3vXuVLFPLpuntPmK/PDWi6uDz+N2V1HMl/Dvfs
16RmbRKtALHUJ3lnqqeFM4z/VGGtH7pg6ixFpPFeuItB3kqWO0RR08ECgYEA9YIp
aa3L4Vl5AG0FQIwTrOU29+ydAW7zhWuqKxDORvxjEr8h4EsSjhb6dCB2fGeI6NcU
yfKlzTTJ20D1ZeX5BUJQ8nGHIrAT1+rdVi8XaECrE2CZkbLwUQPkfNf9zCJIAjCI
PVogDO9rg5M0eMVc8ZA9pEoehfDiSs45wgO/9vUCgYEA5KiSzUkZzTYLM7M5NSsl
Oph6fXkxad3COZyUlopdT/PO7jEbgOLgf5EZv+1/j6wXHDvkD/9HyQ/ErYwq2qOD
H8T11HoEA1kteR+P0W4zUoScSZ//RtcUIeKFarf6qqRKOChkskvJ/mziNcfq4Mrb
KVlE+BQX/cTdq4urCp29yoECgYB9EUPbjCfyssvLhb5JNZLIXzRzdFw/YLA55bEY
o0USMHygDjJJurTHIbmQ8wCpO20pgHgcsLn6Icw7H4pj0JXdMf1epKr85soVEpaS
0waPm3ILOwjAinGWM+9wEfA+z6/zPZoJ9yGgtU8GBjWS2KgSfzKHwSJvhVkiVpBh
39spCQKBgQColUUIBjYnkxYV/hFXcAYlABJ6ZXBjhFA5EgC1qaQ3AKtOIZXc0wOm
CynbJDGwXIEq00LEgzGAruk04uyRlO3UVG416mrVnF38XgUNMfuXrN+iJaTzmjbU
chH1o4zya8gR+UKryYTmzRnksfIICK/7YOoHccjvldOGWoijaqFqYw==
-----END RSA PRIVATE KEY-----
)KEY";

#endif /* SRC_SECRETS */
