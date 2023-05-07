#define BIT 8
#define threshold1 160
#define threshold2 160
#define thresholdLeave 720
#define thresholdTime 300

class GetPositionID
{
	private:
		int positionID;
		int val1, val2, bool1, bool2, bool1_before, bool2_before;
		int i1, i2, Time1, Time2, nowTime;
		int getData1[BIT], getData2[BIT];
		void Reset1();
		void Reset2();
		void Measure1Clock2();
		void Measure2Clock1();
    public:
		GetPositionID();
        void PhotoReflectorSetup();
        int getPositionID();
};

GetPositionID::GetPositionID() {
	this->positionID = 0;
	this->val1 = 0;
	this->val2 = 0;
	this->bool1 = 0;
	this->bool2 = 0;
	this->bool1_before = 0;
	this->bool2_before = 0;
	this->i1 = 0;
	this->i2 = 0;
	this->Time1 = 0;
	this->Time2 = 0;
	this->nowTime = 0;
}

void GetPositionID::PhotoReflectorSetup() {
	memset(getData1, '\0', BIT);
	memset(getData2, '\0', BIT);
	Reset1();
	Reset2();

	Serial.begin(115200);
}

int GetPositionID::getPositionID() {
	val1 = analogRead(A0);
	val2 = analogRead(A1);
	nowTime = millis();

	if(val1 < threshold1){bool1 = 1;}
	else{bool1 = 0;}

	if(val2 < threshold2){bool2 = 1;}
	else{bool2 = 0;}

	Measure1Clock2();
	Measure2Clock1();

	bool1_before = bool1;
	bool2_before = bool2;

	positionID = 
	Serial.print("PositionID: ");
	Serial.println(positionID);

    return positionID;
}

void GetPositionID::Reset1(){
	i1 = 0;
	for(int j = 0; j < BIT; j++){
		getData1[j] = 0;
	}	
}

void GetPositionID::Reset2(){
	i2 = 0;
	for(int j = 0; j < BIT; j++){
		getData2[j] = 0;
	}
}

void GetPositionID::Measure1Clock2(){
	if(bool2 != bool2_before){
		getData1[i1] = bool1;
		i1 += 1;
		Time1 = millis();
	}

	if(i1 >= BIT){
		positionID = 0;
		for(int j = 0; j < BIT; j++){
			positionID += getData1[BIT - j - 1] * (pow(2, BIT - j - 1) + 0.5);
		}
		sendPositionID(positionID);
		Reset1();
	}

	if(val1 > thresholdLeave){
		Reset1();
	}

	if(nowTime - Time1 > thresholdTime){
		Reset1();
	}
}

void GetPositionID::Measure2Clock1(){
	if(bool1 != bool1_before){
		getData2[i2] = bool2;
		i2 += 1;
		Time2 = millis();
	}

	if(i2 >= BIT){
		positionID = 0;
		for(int j = 0; j < BIT; j++){
			positionID += getData2[j] * (pow(2,j) + 0.5);
		}
		sendPositionID(positionID);
		Reset2();
	}

	if(val2 > thresholdLeave){
		Reset2();
	}

	if(nowTime - Time2 > thresholdTime){
		Reset2();
	}
}