//
// COLOR LIGHT SENSOR DECLARE aka. colorsensor.c
//

#include "i2c.h"
#include "colorsensor.h"

void colorCalibrate(void)
{
  // Turn on LED
  digitalWrite(LEDLIGHT_PIN, HIGH);

  // Wait for ADJD reset sequence
  delay(1);

 // Sensor gain setting (Avago app note 5330) - Calibrated for primary color detection (RGB).
  // CAPs are 4bit (higher value will result in lower output)
  // Valid values from 0 - 15 decimal
  i2c_writebyte(I2C_ADDR, REG_CAP_RED, 0x0E); // normal room lightning 0x0e
  i2c_writebyte(I2C_ADDR, REG_CAP_GREEN, 0x0F); // normal room lightning 0x0f
  i2c_writebyte(I2C_ADDR, REG_CAP_BLUE, 0x06); // normal room lightning 0x06
  i2c_writebyte(I2C_ADDR, REG_CAP_CLEAR, 0x0F); // normal room lightning 0x0f
  
  // Integration time slot values are 12bit (higher value will result in higher sensor value)
  // Valid values from 0 - 4096 decimal
  // http://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html
  i2c_writebyte(I2C_ADDR, REG_INT_RED_LO, 0x2C); // normal room lightning 0x1F4
  i2c_writebyte(I2C_ADDR, REG_INT_RED_HI, 0x01);
  i2c_writebyte(I2C_ADDR, REG_INT_GREEN_LO, 0x2C); // normal room lightning 0x1F4
  i2c_writebyte(I2C_ADDR, REG_INT_GREEN_HI, 0x01);
  i2c_writebyte(I2C_ADDR, REG_INT_BLUE_LO, 0x2C); // normal room lightning 0x1F4
  i2c_writebyte(I2C_ADDR, REG_INT_BLUE_HI, 0x01);
  i2c_writebyte(I2C_ADDR, REG_INT_CLEAR_LO, 0xC8);
  i2c_writebyte(I2C_ADDR, REG_INT_CLEAR_HI, 0x00);
}

void getColors(int colors[])
{
  static char init = 0;
  unsigned int r, r_lo, r_hi, g, g_lo, g_hi, b, b_lo, b_hi, c, c_lo, c_hi;
  unsigned int v;

  if (!init) {
    // Port setup, outputs and pull-up on SDA, SCL, and LED-pin
    pinMode(LEDLIGHT_PIN, OUTPUT); pinMode(SDA_PIN, OUTPUT); pinMode(SCL_PIN, OUTPUT); 
    digitalWrite(LEDLIGHT_PIN, HIGH); digitalWrite(SDA_PIN, HIGH); digitalWrite(SCL_PIN, HIGH);
    
    // Initilize i2c and sensor caps/integration values
    i2c_init();
    colorCalibrate();
  
    init = 1;
  }

  // Turn on LED
  digitalWrite(LEDLIGHT_PIN, HIGH);

  // Wait for LEDs to light up properly
  delay(100);

  // Start color light sensing  
  i2c_writebyte(I2C_ADDR,0x00,0x01);
  
  while(i2c_readbyte(I2C_ADDR,0x00,&v) && v != 0) {
  // Waiting for a result, CTRL register bit will be set to 1 when the result is ready
  }

  // RGB color results, 10bit results
  i2c_readbyte(I2C_ADDR,REG_DATA_RED_LO,&r_lo);
  i2c_readbyte(I2C_ADDR,REG_DATA_RED_HI,&r_hi);
  r = r_lo + (r_hi<<8);
  i2c_readbyte(I2C_ADDR,REG_DATA_GREEN_LO,&g_lo);
  i2c_readbyte(I2C_ADDR,REG_DATA_GREEN_HI,&g_hi);
  g = g_lo + (g_hi<<8);
  i2c_readbyte(I2C_ADDR,REG_DATA_BLUE_LO,&b_lo);
  i2c_readbyte(I2C_ADDR,REG_DATA_BLUE_HI,&b_hi);
  b = b_lo + (b_hi<<8);
  i2c_readbyte(I2C_ADDR,REG_DATA_CLEAR_LO,&c_lo);
  i2c_readbyte(I2C_ADDR,REG_DATA_CLEAR_HI,&c_hi);
  c = c_lo + (c_hi<<8);

  // Put values into existing passed array 
  colors[0] = r;
  colors[1] = g;
  colors[2] = b;
  colors[3] = c;

  // Turn off LED
  digitalWrite(LEDLIGHT_PIN, LOW);
}

ItemColor determineColor(double detectedhsv[])
{
  int i, j, maxhits = 0;
  ItemColor itemcolor = purple; // default unknown color is set to purple bin

  // Calibrated values from the actual items (M&Ms or Skittles)
/*
// Bright light:
double calibratedhsv[6][2][2][3] = {
  {
    { // M&M Red
      {8.02,0.31,0.52}, // mean
      {2.18,0.07,0.09}, // stddev
    }, 
    { // Skittles Red
      {1.16,0.23,0.95}, // mean
      {1.34,0.11,0.04}, // stddev
    },
  },
  {
    { // M&M Green
      {115.39,0.19,0.51}, // mean
      {10.92,0.02,0.09}, // stddev
    }, 
    { // Skittles Green
      {40.74,0.17,0.92}, // mean
      {17.01,0.04,0.04}, // stddev
    },
  },
  {
    { // M&M Blue
      {226.75,0.39,0.61}, // mean
      {3.24,0.08,0.14}, // stddev
    }, 
    { // unknown
      {0.00,0.00,0.00}, // mean
      {0.00,0.00,0.00}, // stddev
    },
  },
  {
    { // M&M Orange
      {26.5,0.41,0.65}, // mean
      {2.69,0.07,0.15}, // stddev
    }, 
    { // Skittles Orange
      {18.53,0.32,1}, // mean
      {3.43,0.08,0.05}, // stddev
    },
  },
  {
    { // M&M Yellow
      {37.1,0.33,0.66}, // mean
      {5.75,0.16,0.15}, // stddev
    }, 
    { // Skittles Yellow
      {36.76,0.29,0.72}, // mean
      {6.14,0.05,0.07}, // stddev
    },
  },
  {
    { // unknown
      {0.00,0.00,0.00}, // mean
      {0.00,0.00,0.00}, // stddev
    },
    { // unknown
      {0.00,0.00,0.00}, // mean
      {0.00,0.00,0.00}, // stddev
    },
  },
};
*/

// Regular light
double calibratedhsv[6][2][2][3] = {
  {
    { // M&M Red
      {8.02,0.31,0.52}, // mean
      {2.18,0.07,0.09}, // stddev
    }, 
    { // Skittles Red
      {7.59,0.29,0.52}, // mean
      {2.34,0.09,0.06}, // stddev
    },
  },
  {
    { // M&M Green
      {115.39,0.19,0.51}, // mean
      {10.92,0.02,0.09}, // stddev
    }, 
    { // Skittles Green
      {93.69,0.22,0.53}, // mean
      {3.43,0.06,0.06}, // stddev
    },
  },
  {
    { // M&M Blue
      {226.75,0.39,0.61}, // mean
      {3.24,0.08,0.14}, // stddev
    }, 
    { // unknown
      {0.00,0.00,0.00}, // mean
      {0.00,0.00,0.00}, // stddev
    },
  },
  {
    { // M&M Orange
      {26.5,0.41,0.65}, // mean
      {2.69,0.07,0.15}, // stddev
    }, 
    { // Skittles Orange
      {29.5,0.4,0.69}, // mean
      {1.97,0.09,0.13}, // stddev
    },
  },
  {
    { // M&M Yellow
      {61.1,0.33,0.66}, // mean
      {5.75,0.16,0.15}, // stddev
    }, 
    { // Skittles Yellow
      {62.32,0.31,0.76}, // mean
      {2.71,0.07,0.16}, // stddev
    },
  },
  {
    { // unknown
      {0.00,0.00,0.00}, // mean
      {0.00,0.00,0.00}, // stddev
    },
    { // unknown
      {0.00,0.00,0.00}, // mean
      {0.00,0.00,0.00}, // stddev
    },
  },
};

  // Find color - based on 2 times standard derivation of the RGB values for 95% coverage
  for(i = 0; i < 7; i++)
  {
    for(j = 1; j < 2; j++) // element 0 m&m values, 1 skittles values
    {
      if ((calibratedhsv[i][j][0][0]-colordeterminestddev*calibratedhsv[i][j][1][0] < detectedhsv[0]) && (calibratedhsv[i][j][0][0]+colordeterminestddev*calibratedhsv[i][j][1][0] > detectedhsv[0]) // hue
        && (calibratedhsv[i][j][0][1]-colordeterminestddev*calibratedhsv[i][j][1][1] < detectedhsv[1]) && (calibratedhsv[i][j][0][1]+colordeterminestddev*calibratedhsv[i][j][1][1] > detectedhsv[1]) // saturation
        //&& (calibratedhsv[i][j][0][2]-colordeterminestddev*calibratedhsv[i][j][1][2] < detectedhsv[2]) && (calibratedhsv[i][j][0][2]+colordeterminestddev*calibratedhsv[i][j][1][2] > detectedhsv[2]) // black key
       ){
        itemcolor = (ItemColor)i;
        i = 6; // exit
        j = 2;
      }
    }
  }
  return itemcolor;
}

void rgbToHSV(double r, double g, double b, double hsv[]) {
  double computedH, computedS, computedV;
 double minRGB, maxRGB;
 double d = 0, h = 0;

  r = r/1023; g = g/1023; b = b/1023;
  minRGB = min(r,min(g,b));
  maxRGB = max(r,max(g,b));

  if (minRGB == maxRGB) {
    // Black-gray-white
    computedV = minRGB;
    hsv[0] = 0; hsv[1] = 0; hsv[2] = computedV;
  } else { 
    // Colors other than black-gray-white:
    d = (r == minRGB) ? g-b : ((b == minRGB) ? r-g : b-r);
    h = (r == minRGB) ? 3 : ((b == minRGB) ? 1 : 5);
  
    computedH = 60*(h - (d/(maxRGB - minRGB)));
    computedS = (maxRGB - minRGB)/maxRGB;
    computedV = maxRGB;
    
    hsv[0] = computedH; hsv[1] = computedS; hsv[2] = computedV;
  }
}

