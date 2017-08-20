#include "Adafruit_ST7735.h"

#define TFT_DC     15
#define TFT_CS     12
#define TFT_RST    0
#define TFT_CLK    13
#define TFT_MOSI   14

class QRcode
{
	private:
		Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);
		void render(int x, int y, int color);

	public:
		
		QRcode();
		
		void setSize(int qsize);
		void init();
		void debug();
		void setStatus(bool status);
		bool getStatus();
		void screenwhite();
		void create(String message);
};
