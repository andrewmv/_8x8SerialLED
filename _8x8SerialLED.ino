/* 
 *  8x8 Hack
 *  Drive an 8x8 bycolor LED backpack soldered creatively directly onto the Teensy LC
 */

int led = 13; //Board's build in LED

//Row/col pin definitions (Layout guess #1)
int row[] = {3, 6, 9, 12, 22, 19, 16, 13};
int red[] = {2, 5, 8, 11, 23, 20, 17, 14};
int grn[] = {1, 4, 7, 10, 26, 21, 18, 15};

const int RED = 1;
const int GREEN = 2;
const int YELLOW = 3;

const byte CMD_RED_ROW0 = 0x48;
const byte CMD_RED_ROW7 = 0x4f;

const byte CMD_RED_COL0 = 0x58;
const byte CMD_RED_COL7 = 0x5f;

const byte CMD_GRN_ROW0 = 0x28;
const byte CMD_GRN_ROW7 = 0x2f;

const byte CMD_GRN_COL0 = 0x38;
const byte CMD_GRN_COL7 = 0x3f;

const byte CMD_SCR_DOWN = 0x40;
const byte CMD_SCR_UP = 0x41;

const byte READY = 0x27;

const byte BIT[] = {
  B10000000,
  B01000000,
  B00100000,
  B00010000,
  B00001000,
  B00000100,
  B00000010,
  B00000001
};

const byte HEART[] = {
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  B00000000
};

byte data_red[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

byte data_green[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

int offset = 0;

byte cmd = READY;

void setup() {
//Configure for common anode
  for (int i = 0; i < 8; i++) {
    //rows
    pinMode(row[i], OUTPUT);
    digitalWrite(row[i], LOW);
    //cols
    pinMode(red[i], OUTPUT);
    digitalWrite(red[i], HIGH);
    pinMode(grn[i], OUTPUT);
    digitalWrite(grn[i], HIGH);
    // initialize serial communication:
    Serial.begin(9600);
  }
}

void flash(int x, int y, int color) {
  digitalWrite(row[x], HIGH);
  if (color & RED) { (digitalWrite(red[y], LOW)); }
  if (color & GREEN) { (digitalWrite(grn[y], LOW)); }
  //delay(50);
  digitalWrite(row[x], LOW);
  digitalWrite(red[y], HIGH);
  digitalWrite(grn[y], HIGH);
}

void scroll(byte glob[]) {
  for (int i = 0; i < 9; i++) {
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        if ((glob[x + i] >> y) % 2) {
          flash(x, y, RED);
        }
      }
    }
    delay(50);
  }
}

void writeRow(int color, byte row, byte data) {
  if (color == RED) {
    data_red[row] = data;
  } else if (color == GREEN) {
    data_green[row] = data;
  }
}

/* TODO: Implement this
 *  
 */
void writeCol(int color, byte col, byte data) {
//  byte col_mask = B10000000 >> col;
//  if (color == RED) {
//    for (int i = 0; i < 8; i++) {
//      data_red[i] = (data_red[i] && (data ~ col_mask));
//    }
//  }
}

/* Command Parser
 */
void getData() {
    byte incomingByte;
    while (Serial.available() > 0) {
      // read the oldest byte in the serial buffer:
      incomingByte = Serial.read();
      if (cmd == READY) {
        cmd = incomingByte;
        break;
      } else if ((cmd >= CMD_RED_ROW0) and (cmd <= CMD_RED_ROW7)) { //CMD_RED_ROW
        Serial.print("RED ROW WRITE OK\n");
        writeRow(RED, cmd - CMD_RED_ROW0, incomingByte);
      } else if ((cmd >= CMD_GRN_ROW0) and (cmd <= CMD_GRN_ROW7)) { //CMD_GRN_ROW
        writeRow(GREEN, cmd - CMD_GRN_ROW0, incomingByte);
        Serial.print("GREEN ROW WRITE OK\n");
      } else if ((cmd >= CMD_RED_COL0) and (cmd <= CMD_RED_COL7)) { //CMD_RED_COL
        writeCol(RED, cmd - CMD_RED_COL0, incomingByte);
        Serial.print("RED COL WRITE OK\n");
      } else if ((cmd >= CMD_GRN_COL0) and (cmd <= CMD_GRN_COL7)) { //CMD_GRN_COL
        writeCol(GREEN, cmd - CMD_GRN_COL0, incomingByte);
        Serial.print("GREEN COL WRITE OK\n");
      } else if (cmd == CMD_SCR_DOWN) { 
        offset += 1;
        if (offset > 7) {
          offset = 0;
        }
      } else if (cmd == CMD_SCR_UP) {
        offset -= 1;
        if (offset < 0) {
          offset = 7;
        }
      } else { //Command byte was invalid
        Serial.print("\nBad Command\n");
      }
      cmd = READY;
    }
}

int wrap(int x) {
  if (x > 7) {
    return x - 8;
  } else {
    return x;
  }
}

void loop() {
  //Heart!
//  for (int y = 0; y < 8; y++) {
//    for (int x = 0; x < 8; x++) {
//      if ((HEART[x] >> y) % 2) {
//        flash(x, y, RED);
//      }
//    }
//  }

  //Diag test
//  for (int i = 0; i < 8; i++) {
//    flash(i, i, RED);
//    flash(i, 7-i, GREEN);
//  }

//Display buffer
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (data_red[y] & BIT[wrap(x+offset)]) {
        flash(x, y, RED);
      }
      if (data_green[y] & BIT[wrap(x+offset)]) {
        flash(x, y, GREEN);
      }
    }
  }
  getData();
  
//Simple scan
/*  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      flash(x, y, YELLOW);
    }
  }
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      flash(x, y, GREEN);
    }
  }
*/  
}
