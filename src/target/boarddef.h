typedef enum {INVERTED_RX, RX, GPS, OSD} usart_usage ;




typedef struct
{
	unsigned char rxGPIO;
	unsigned char syncGPIO;

	usart_usage usart_type; //verify this turns into a byte, and if not switch it to them
} usart_pinout;

typedef struct
{
	usart_pinout usart_ports[8];




} target_pinout;
