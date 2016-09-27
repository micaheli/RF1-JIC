typedef enum {INVERTED_RX, RX, GPS, OSD} usart_usage ;



//uint16_t pins;

typedef struct
{
	uint16_t rxGPIO;
	unsigned char rxPIN;

	uint16_t txGPIO;
	unsigned char txPIN;

	usart_usage usart_type; //verify this turns into a byte, and if not switch it to them
} usart_pinout;

typedef struct
{
	usart_pinout usart_ports[8];




} target_pinout;
