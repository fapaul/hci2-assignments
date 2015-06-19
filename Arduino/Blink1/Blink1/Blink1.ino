#define PORT 12
#define DURATION 200


void setup() {
	pinMode(PORT, OUTPUT);
}

char* code = "...---...";

void blink(int duration) {
	digitalWrite(PORT, HIGH);
	delay(duration);
	digitalWrite(PORT, LOW);
	delay(duration);
}


void loop() {
	for (char* c = code; *c != 0; c++) {
		if (*c == '.') {
			blink(DURATION / 2);
		}
		else {
			blink(DURATION);
		}
	}

	delay(1000);
}
