#include <EasyTransfer.h>
//create object
EasyTransfer ET; 

struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  unsigned int UIN_1;
  unsigned int IIN_1;
  unsigned int UOUT_1;
  int BVLR_1;
  int OVT_1;
  int NOC_1;
  int UNDV_1;
  int TAMB_1;
  
  unsigned int UIN_2;
  unsigned int IIN_2;
  unsigned int UOUT_2;
  int BVLR_2;
  int OVT_2;
  int NOC_2;
  int UNDV_2;
  int TAMB_2;
  unsigned int UIN_3;
  unsigned int IIN_3;
  unsigned int UOUT_3;
  int BVLR_3;
  int OVT_3;
  int NOC_3;
  int UNDV_3;
  int TAMB_3;
  unsigned int UIN_4;
  unsigned int IIN_4;
  unsigned int UOUT_4;
  int BVLR_4;
  int OVT_4;
  int NOC_4;
  int UNDV_4;
  int TAMB_4;
  
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE mydata;

//Variables globales MPPTS

  unsigned int UIN_1;
  unsigned int IIN_1;
  unsigned int UOUT_1;
  int BVLR_1;
  int OVT_1;
  int NOC_1;
  int UNDV_1;
  int TAMB_1;
  unsigned int UIN_2;
  unsigned int IIN_2;
  unsigned int UOUT_2;
  int BVLR_2;
  int OVT_2;
  int NOC_2;
  int UNDV_2;
  int TAMB_2;
  unsigned int UIN_3;
  unsigned int IIN_3;
  unsigned int UOUT_3;
  int BVLR_3;
  int OVT_3;
  int NOC_3;
  int UNDV_3;
  int TAMB_3;
  unsigned int UIN_4;
  unsigned int IIN_4;
  unsigned int UOUT_4;
  int BVLR_4;
  int OVT_4;
  int NOC_4;
  int UNDV_4;
  int TAMB_4;
  
void setup(){
  Serial.begin(9600);
  Serial3.begin(9600);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc. 
  ET.begin(details(mydata), &Serial3);
  
}

void loop()
{
  //check and see if a data packet has come in. 
  if(ET.receiveData()){
    //this is how you access the variables. [name of the group].[variable name]
    //since we have data, we will blink it out.
    
    UIN_1 = mydata.UIN_1;
    IIN_1 = mydata.IIN_1;
    UOUT_1 = mydata.UOUT_1;
    BVLR_1 = mydata.BVLR_1;
    OVT_1 = mydata.OVT_1;
    NOC_1 = mydata.NOC_1;
    UNDV_1 = mydata.UNDV_1;
    TAMB_1 = mydata.TAMB_1;

    UIN_2 = mydata.UIN_2;
    IIN_2 = mydata.IIN_2;
    UOUT_2 = mydata.UOUT_2;
    BVLR_2 = mydata.BVLR_2;
    OVT_2 = mydata.OVT_2;
    NOC_2 = mydata.NOC_2;
    UNDV_2 = mydata.UNDV_2;
    TAMB_2 = mydata.TAMB_2;

    UIN_3 = mydata.UIN_3;
    IIN_3 = mydata.IIN_3;
    UOUT_3 = mydata.UOUT_3;
    BVLR_3 = mydata.BVLR_3;
    OVT_3 = mydata.OVT_3;
    NOC_3 = mydata.NOC_3;
    UNDV_3 = mydata.UNDV_3;
    TAMB_3 = mydata.TAMB_3;

    UIN_4 = mydata.UIN_4;
    IIN_4 = mydata.IIN_4;
    UOUT_4 = mydata.UOUT_4;
    BVLR_4 = mydata.BVLR_4;
    OVT_4 = mydata.OVT_4;
    NOC_4 = mydata.NOC_4;
    UNDV_4 = mydata.UNDV_4;
    TAMB_4 = mydata.TAMB_4;

    Serial.print("UIN_1: ");Serial.println(mydata.UIN_1);
    Serial.print("IIN_1: ");Serial.println(mydata.IIN_1);
    Serial.print("UOUT_1: ");Serial.println(mydata.UOUT_1);
    Serial.print("BVLR_1: ");Serial.println(mydata.BVLR_1);
    Serial.print("OVT_1: ");Serial.println(mydata.OVT_1);
    Serial.print("NOC_1: ");Serial.println(mydata.NOC_1);
    Serial.print("UNDV_1: ");Serial.println(mydata.UNDV_1);
    Serial.print("TAMB_1: ");Serial.println(mydata.TAMB_1);

    Serial.print("UIN_2: ");Serial.println(mydata.UIN_2);
    Serial.print("IIN_2: ");Serial.println(mydata.IIN_2);
    Serial.print("UOUT_2: ");Serial.println(mydata.UOUT_2);
    Serial.print("BVLR_2: ");Serial.println(mydata.BVLR_2);
    Serial.print("OVT_2: ");Serial.println(mydata.OVT_2);
    Serial.print("NOC_2: ");Serial.println(mydata.NOC_2);
    Serial.print("UNDV_2: ");Serial.println(mydata.UNDV_2);
    Serial.print("TAMB_2: ");Serial.println(mydata.TAMB_2);
  }

}
